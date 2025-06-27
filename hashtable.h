#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "pacientes.h"

#define HASH_SIZE 97

void init_hash(void);
void inserir_hash(Paciente *p);
Paciente* buscar_hash(const char *cpf_raw);
void destruir_hash(void);

/* iterador – apenas protótipo aqui */
typedef void (*PacienteVisitor)(Paciente *p, void *ctx);
void hash_foreach(PacienteVisitor fn, void *ctx);

#endif