#include "fila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PRIOR 6 // usaremos índices 1..5

typedef struct NoFila {
    Paciente* pac;
    struct NoFila* prox;
} NoFila;

static NoFila* inicio[MAX_PRIOR];
static NoFila* fim[MAX_PRIOR];

long atendidos[6]={0};
long triados[6]={0};
static double soma_espera[6]={0};

void init_fila(void) {
    memset(inicio,0,sizeof(inicio));
    memset(fim,0,sizeof(fim));
}

void enqueue_paciente(Paciente* p, int prioridade) {
    if (prioridade<1||prioridade>5) return;
    NoFila* n = malloc(sizeof(NoFila));
    n->pac = p;
    n->prox = NULL;
    if (!inicio[prioridade]) inicio[prioridade]=n; else fim[prioridade]->prox=n;
    fim[prioridade]=n;
    triados[prioridade]++;
}

Paciente* dequeue_proximo(void) {
    for (int pr=1; pr<=5; ++pr) {
        if (inicio[pr]) {
            NoFila* n = inicio[pr];
            inicio[pr]=n->prox;
            if (!inicio[pr]) fim[pr]=NULL;
            // estatísticas
            time_t agora=time(NULL);
            double espera=difftime(agora, n->pac->chegada)/60.0;
            soma_espera[pr]+=espera;
            atendidos[pr]++;
            Paciente* p = n->pac;
            free(n);
            return p;
        }
    }
    return NULL;
}

void exibir_fila(void) {
    puts("\n--- Fila de Espera ---");
    for (int pr=1; pr<=5; ++pr) {
        printf("Prioridade %d (%s):", pr, cor_string(pr));
        if (!inicio[pr]) { puts(" (vazia)"); continue; }
        NoFila* it=inicio[pr];
        while (it) {
            printf(" %s", it->pac->nome);
            it=it->prox;
            if (it) printf(",");
        }
        puts("");
    }
}

void gerar_relatorio(void) {
    puts("\n===== RELATORIO DE ATENDIMENTOS =====");
    for (int pr=1; pr<=5; ++pr) {
        printf("%s - Triados: %ld | Atendidos: %ld", cor_string(pr), triados[pr], atendidos[pr]);
        if (atendidos[pr]) {
            printf(" | Media espera: %.1f min", soma_espera[pr]/atendidos[pr]);
        }
        puts("");
    }
    puts("====================================\n");
}

const char* cor_string(int prioridade) {
    static const char* cores[]={"","Vermelho","Laranja","Amarelo","Verde","Azul"};
    return (prioridade>=1&&prioridade<=5)?cores[prioridade]:"?";
}