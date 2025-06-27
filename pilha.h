#ifndef PILHA_H
#define PILHA_H
#include "pacientes.h"

typedef struct Consulta {
    char pasta[128];          /* caminho ex.: pacientes/00001111222/Tosse_27-06-2025 */
    char data[16];            /* “27-06-2025”  – preenchido ao criar                */
    char titulo[64];          /* “Tosse” (primeira palavra do motivo)               */
    struct Consulta *prox;
} Consulta;

typedef struct Consulta Consulta;

void pushConsulta(Paciente* p, const char* pasta);
void criar_pasta_consulta(Paciente* p, const char* motivo);
void listar_e_abrir_exames(Paciente* p);

#endif