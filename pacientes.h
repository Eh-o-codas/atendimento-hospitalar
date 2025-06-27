#ifndef PACIENTES_H
#define PACIENTES_H
#include <time.h>

#define TAM_CPF 12 // 11 dígitos + null

typedef struct Consulta Consulta;

typedef struct Paciente {
    char nome[100];
    int idade;
    char cpf[TAM_CPF];
    int prioridade;       // 1..5 ou 0= não triado
    time_t chegada;       // timestamp da triagem
    Consulta* historico;  // pilha de consultas
} Paciente;

Paciente* criar_paciente(const char* nome, int idade, const char* cpf_raw);
void destruir_paciente(Paciente* p);
void mostrar_paciente(const Paciente* p);
void carregar_pacientes(void);
void persistir_paciente(const Paciente *p);
void reconstruir_historicos(void);

#endif