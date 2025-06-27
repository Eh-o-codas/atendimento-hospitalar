/* hashtable.c ----------------------------------------------------------- */
#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* CONSTANTES E TIPOS PRIVADOS ----------------------------------------- */
typedef struct NodoHash {
    Paciente *pac;
    struct NodoHash *prox;
} NodoHash;

static NodoHash *tabela[HASH_SIZE] = { NULL };

/* FUNÇÕES AUXILIARES --------------------------------------------------- */
static int sanitizar_cpf(const char *in, char *out)
{
    int j = 0;
    for (int i = 0; in[i] && j < 11; ++i)
        if (isdigit((unsigned char)in[i]))
            out[j++] = in[i];
    out[j] = '\0';
    return j == 11;
}

static unsigned hash_cpf(const char *cpf)
{
    unsigned long v = 0;
    for (int i = 0; i < 11; ++i)
        v = v * 31 + (cpf[i] - '0');
    return v % HASH_SIZE;
}

/* API PUBLICA ---------------------------------------------------------- */
void init_hash(void)
{
    for (int i = 0; i < HASH_SIZE; ++i)
        tabela[i] = NULL;
}

void inserir_hash(Paciente *p)
{
    unsigned h = hash_cpf(p->cpf);
    NodoHash *n = malloc(sizeof(NodoHash));
    if (!n) return;
    n->pac  = p;
    n->prox = tabela[h];
    tabela[h] = n;
}

Paciente *buscar_hash(const char *cpf_raw)
{
    char cpf[12];
    if (!sanitizar_cpf(cpf_raw, cpf)) return NULL;

    unsigned h = hash_cpf(cpf);
    for (NodoHash *n = tabela[h]; n; n = n->prox)
        if (strcmp(n->pac->cpf, cpf) == 0)
            return n->pac;
    return NULL;
}

void destruir_hash(void)
{
    for (int i = 0; i < HASH_SIZE; ++i) {
        NodoHash *n = tabela[i];
        while (n) {
            NodoHash *tmp = n->prox;
            destruir_paciente(n->pac);
            free(n);
            n = tmp;
        }
        tabela[i] = NULL;
    }
}

/* ITERADOR ---------------------------------------------------- */
void hash_foreach(PacienteVisitor fn, void *ctx)
{
    for (int i = 0; i < HASH_SIZE; ++i)
        for (NodoHash *n = tabela[i]; n; n = n->prox)
            fn(n->pac, ctx);
}
