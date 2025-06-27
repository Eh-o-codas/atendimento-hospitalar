#include "pilha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32          //  ← ADICIONE
  #include <direct.h>  //  ← ADICIONE (_mkdir)
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #define MKDIR(path) mkdir(path, 0755)
#endif
#include <dirent.h>
#include <time.h>

static void limpaBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) /* descarta resto da linha */;
}


void pushConsulta(Paciente *p, const char *dir)
{
    Consulta *c = malloc(sizeof(Consulta));
    if (!c) return;

    /* caminho completo */
    strncpy(c->pasta, dir, sizeof(c->pasta));

    /* extrai “titulo” e “data” a partir do nome da pasta  .../Titulo_Data */
    const char *base = strrchr(dir, '/');
    if (!base) base = strrchr(dir, '\\');
    base = base ? base + 1 : dir;

    const char *u = strrchr(base, '_');
    if (u) {
        size_t len = u - base;
        strncpy(c->titulo, base,
                len >= sizeof(c->titulo) ? sizeof(c->titulo) - 1 : len);
        c->titulo[len] = '\0';
        strncpy(c->data, u + 1, sizeof(c->data));
    } else {
        strncpy(c->titulo, base, sizeof(c->titulo));
        c->data[0] = '\0';
    }

    c->prox        = p->historico;
    p->historico   = c;
}


/* static void gerar_nome_pasta(char* dst, size_t sz, const char* cpf, const char* motivo) {
    time_t t=time(NULL);
    struct tm *tm=localtime(&t);
    char data[32];
    strftime(data,sizeof(data),"%d-%m-%Y",tm);
    snprintf(dst, sz, "pacientes/%s/%s_%s", cpf, motivo, data);
} */

void criar_pasta_consulta(Paciente *p, const char *motivo)
{
    char data[16];
    time_t t = time(NULL);
    strftime(data, sizeof(data), "%d-%m-%Y", localtime(&t));

    char titulo[64];
    sscanf(motivo, "%63s", titulo);          /* primeira palavra p/ exibir */

    char dir[160];
    snprintf(dir, sizeof(dir), "pacientes/%s/%s_%s", p->cpf, titulo, data);

    MKDIR("pacientes");
    char pacientedir[64];
    snprintf(pacientedir, sizeof(pacientedir), "pacientes/%s", p->cpf);
    MKDIR(pacientedir);
    MKDIR(dir);

    /* cria atendimento.txt */
    char file[200];
    snprintf(file, sizeof(file), "%s/atendimento.txt", dir);
    FILE *f = fopen(file, "w");
    if (f) {
        fprintf(f,
        "<Anamnese>\n%s\n</Anamnese>\n"
        "<Alertas>\n\n</Alertas>\n"
        "<Resultados>\n\n<FimResultados>\n",
        motivo);
        fclose(f);
    }
    pushConsulta(p, dir);
}

static void listar_arquivos(const char* dir, char nomes[][64], int* n) {
    DIR* d=opendir(dir);
    if (!d) { *n=0; return; }
    struct dirent* ent;
    *n=0;
    while ((ent = readdir(d)) != NULL && *n < 64) {
        if (ent->d_name[0] == '.') continue;          // ignora . e ..
        const char *ext = strrchr(ent->d_name, '.');  // checa extensão
        if (ext && strcmp(ext, ".txt") == 0) {
            strncpy(nomes[*n], ent->d_name, 64);
            nomes[*n][63] = '\0';
            (*n)++;
        }
    }
    closedir(d);
    // ordena
    for (int i=0;i<*n-1;++i)
      for (int j=i+1;j<*n;++j)
        if (strcmp(nomes[i],nomes[j])>0) { char tmp[64]; strcpy(tmp,nomes[i]); strcpy(nomes[i],nomes[j]); strcpy(nomes[j],tmp);}    
}

/* ------------- pilha.c (substitua função inteira) ------------------- */
static void mostrar_bloco(FILE *f, const char *tagIni, const char *tagFim)
{
    char line[256];
    int on = 0;
    while (fgets(line,sizeof(line),f)) {
        if (strstr(line, tagIni)) { on = 1; continue; }
        if (strstr(line, tagFim)) break;
        if (on) printf("%s", line);
    }
    rewind(f);                 /* volta p/ reutilizar no outro bloco */
}

void listar_e_abrir_exames(Paciente *p)
{
    if (!p || !p->historico) { puts("Nenhuma consulta."); return; }

    for (Consulta *c = p->historico; c; c = c->prox)
    {
        for (;;) {
            printf("\nPaciente %s | Consulta: %s - %s\n",
                   p->nome, c->titulo, c->data);

            char nomes[64][64]; int n=0;
            listar_arquivos(c->pasta, nomes, &n);

            puts("1. Ver ANAMNESE + ALERTAS");
            for (int i=0;i<n;++i) printf("%d. %s\n", i+2, nomes[i]);
            printf("%d. -> Proxima consulta na Pilha\n", n+2);
            puts("0. Voltar ao menu principal");
            printf("Escolha: ");

            int opt; if (scanf("%d",&opt)!=1){ limpaBuffer(); continue; }
            limpaBuffer();

            if (opt==0) return;

            /******** 1. ANAMNESE + ALERTAS ********/
            if (opt==1) {
                char atd[256]; snprintf(atd,sizeof(atd),"%s/atendimento.txt",c->pasta);
                FILE *f=fopen(atd,"r"); if (!f){puts("Sem atendimento.txt");continue;}
                puts("\n== ANAMNESE ==");
                mostrar_bloco(f,"<Anamnese>","</Anamnese>");
                puts("\n== ALERTAS AUTOMATIZADOS ==");
                mostrar_bloco(f,"<Alertas>","</Alertas>");
                fclose(f);
                puts("\n(Enter p/ voltar)"); getchar(); continue;
            }

            /******** arquivos de exame ********/
            if (opt>=2 && opt<n+2) {
                char path[260]; snprintf(path,sizeof(path),"%s/%s",c->pasta,nomes[opt-2]);
                FILE *fx=fopen(path,"r"); if(!fx){perror("Erro");continue;}
                puts("\n--- Resultados ---");
                mostrar_bloco(fx,"<Resultados>","<FimResultados>");
                puts("------------------\n(Enter)"); fclose(fx); getchar(); continue;
            }

            /******** próxima consulta ********/
            if (opt==n+2) {
                if (!c->prox) { puts("Pilha de consultas finalizada."); return; }
                break;                               /* sai do for interno → vai p/ c = c->prox */
            }

            puts("Opcao invalida.");
        }
    }
}

/* ------------------------------------------------------------------- */



/********************* triagem.h ***************************/
#ifndef TRIAGEM_H
#define TRIAGEM_H
int classificar_paciente_ui(void);
#endif