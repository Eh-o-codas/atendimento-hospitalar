#include <windows.h>
#include <sys/stat.h>
#include "pacientes.h"
#include "pilha.h"
#include "hashtable.h"
#include "fila.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
  #include <io.h>        /* _commit */
#else
  #include <unistd.h>    /* fsync   */
  #include <dirent.h>    /* para reconstruir_historicos() */
#endif
static int  dir_eh_valido(const WIN32_FIND_DATAA *fd);
static void reconst_cb   (Paciente *p, void *ctx);

static int sanitizar_cpf(const char* entrada, char* saida) {
    int j=0;
    for (int i=0; entrada[i] && j<11; ++i) {
        if (isdigit((unsigned char)entrada[i])) {
            saida[j++]=entrada[i];
        }
    }
    saida[j]='\0';
    return j==11;
}

Paciente* criar_paciente(const char* nome, int idade, const char* cpf_raw) {
    char cpf[TAM_CPF];
    if (!sanitizar_cpf(cpf_raw, cpf)) return NULL; // cpf inválido
    // verificação duplicidade será feita em inserir_hash
    Paciente* p = malloc(sizeof(Paciente));
    if (!p) return NULL;
    strncpy(p->nome, nome, sizeof(p->nome)); p->nome[sizeof(p->nome)-1]='\0';
    p->idade = idade;
    strcpy(p->cpf, cpf);
    p->prioridade = 0;
    p->chegada = 0;
    p->historico = NULL;
    return p;
}

void destruir_paciente(Paciente* p) {
    // liberar pilha de histórico
    Consulta* c = p->historico;
    while (c) {
        Consulta* tmp = c->prox;
        free(c);
        c = tmp;
    }
    free(p);
}

void mostrar_paciente(const Paciente* p) {
    printf("%s | Idade: %d | CPF: %s\n", p->nome, p->idade, p->cpf);
}

void persistir_paciente(const Paciente *p)
{
    FILE *f = fopen("pacientes.dat", "a");   /* append-text */
    if (!f) { perror("pacientes.dat"); return; }

    /* nome|idade|cpf\n  –  use | como separador seguro */
    fprintf(f, "%s|%d|%s\n", p->nome, p->idade, p->cpf);

    fflush(f);          /* garante que foi ao buffer kernel */
#ifdef _WIN32           /* força flush físico em NTFS/FAT */
    _commit(_fileno(f));
#else                   /* POSIX */
    fsync(fileno(f));
#endif
    fclose(f);
}

void carregar_pacientes(void)
{
    FILE *f = fopen("pacientes.dat", "r");
    if (!f) return;                 /* primeira execução */

    char linha[256];
    while (fgets(linha, sizeof(linha), f)) {
        Paciente *p = malloc(sizeof(Paciente));
        if (!p) { fclose(f); return; }

        /* nome|idade|cpf */
        if (sscanf(linha, "%99[^|]|%d|%11s",
                   p->nome, &p->idade, p->cpf) == 3)
        {
            p->prioridade = 0;
            p->chegada    = 0;
            p->historico  = NULL;

            inserir_hash(p);   /* coloca na hash           */
            ++triados[0];      /* conta no relatorio (nivel 0 = nao triado) */
        } else {
            free(p);           /* linha corrompida */
        }
    }
    fclose(f);
}

void reconstruir_historicos(void)
{
    hash_foreach(reconst_cb, NULL);
}

/* ----- callback chamada para cada paciente ----- */
static void reconst_cb(Paciente *p, void *ctx)
{
    (void)ctx;                                   /* evita warning */

    /* percorre todas as sub-pastas de pacientes\<cpf>\* */
    char mask[300];
    snprintf(mask, sizeof(mask), "pacientes\\%s\\*", p->cpf);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(mask, &fd);
    if (h == INVALID_HANDLE_VALUE) return;

    do {
        if (!dir_eh_valido(&fd)) continue;

        /* monta caminho COMPLETO: pacientes\<cpf>\NOME_DA_PASTA */
        char dir_completo[300];
        snprintf(dir_completo, sizeof(dir_completo),
                 "pacientes\\%s\\%s", p->cpf, fd.cFileName);

        pushConsulta(p, dir_completo);   /* agora aponta para a pasta certa */
        ++atendidos[p->prioridade];      /* estatística opcional            */

    } while (FindNextFileA(h, &fd));
    FindClose(h);
}

static int dir_eh_valido(const WIN32_FIND_DATAA *fd)
{
    return (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
           strcmp(fd->cFileName, ".")  != 0 &&
           strcmp(fd->cFileName, "..") != 0;
}