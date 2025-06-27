#ifndef FILA_H
#define FILA_H
#include "pacientes.h"

extern long triados[6];      /* definidos em fila.c */
extern long atendidos[6];

void init_fila(void);
void enqueue_paciente(Paciente *p, int prio);
Paciente* dequeue_proximo(void);
void exibir_fila(void);
void gerar_relatorio(void);
const char* cor_string(int prio);
#endif
