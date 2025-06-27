#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pacientes.h"
#include "fila.h"
#include "hashtable.h"
#include "triagem.h"
#include "pilha.h"

static void menu_principal(void);
static void cadastrar_paciente_ui(void);
static void triagem_ui(void);
static void atender_proximo_ui(void);
static void fila_ui(void);
static void relatorio_ui(void);
static void prontuario_ui(void);
static void listar_cpfs_ui(void);

int main(void) {
    /* 1. estruturas centrais ------------------------------------ */
    init_hash();                 /* tabela-hash vazia           */
    carregar_pacientes();        /* popula a hash a partir do .dat */
    reconstruir_historicos();    /* opcional – recria pilha de consultas */
    init_fila();                 /* filas de prioridade vazias  */

    /* 2. loop de menu ------------------------------------------- */

    int opc;
    while (1) {
        menu_principal();
        if (scanf("%d", &opc) != 1) {
            puts("Entrada invalida");
            while (getchar()!='\n');
            continue;
        }
        while (getchar()!='\n'); // limpar buffer
        switch (opc) {
            case 1: cadastrar_paciente_ui(); break;
            case 2: listar_cpfs_ui();        break;
            case 3: triagem_ui();            break;
            case 4: atender_proximo_ui();    break;
            case 5: fila_ui();               break;
            case 6: relatorio_ui();          break;
            case 7: prontuario_ui();         break;
            case 0: puts("Saindo..."); destruir_hash(); return 0;
            default: puts("Opção invalida");
        }
    }
}

static void menu_principal(void) {
    puts("\n===== SISTEMA DE TRIAGEM HOSPITALAR =====");
    puts("1. Cadastrar paciente");
    puts("2. Listar CPFs cadastrados"); 
    puts("3. Triar paciente (inserir na fila)");
    puts("4. Atender proximo paciente");
    puts("5. Visualizar fila de espera");
    puts("6. Gerar relatorio");
    puts("7. Ver prontuario de paciente");
    puts("0. Sair");
    printf("Escolha: ");
}

static void cadastrar_paciente_ui(void)
{
    char nome[100], cpf_input[32];
    int idade;

    printf("Nome: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Idade: ");
    scanf("%d", &idade);
    while (getchar() != '\n');        /* limpa o buffer */

    printf("CPF (somente digitos): ");
    fgets(cpf_input, sizeof(cpf_input), stdin);
    cpf_input[strcspn(cpf_input, "\n")] = '\0';

    /* cria o struct usando cpf_input */
    Paciente *p = criar_paciente(nome, idade, cpf_input);
    if (!p) {
        puts("CPF invalido ou ja cadastrado.");
        return;
    }

    inserir_hash(p);          /* coloca na tabela de busca */
    persistir_paciente(p);    /* grava imediatamente no arquivo */

    puts("Paciente cadastrado com sucesso.");
}

static void triagem_ui(void) {
    char cpf_input[32];
    printf("Informe o CPF do paciente a ser triado: "); fgets(cpf_input, sizeof(cpf_input), stdin); cpf_input[strcspn(cpf_input,"\n")]='\0';
    Paciente* p = buscar_hash(cpf_input);
    if (!p) { puts("Paciente nao encontrado."); return; }
    int prio = classificar_paciente_ui();
    p->prioridade = prio;
    p->chegada = time(NULL);
    enqueue_paciente(p, prio);
    puts("Paciente enfileirado com sucesso.");
}

static void atender_proximo_ui(void) {
    Paciente* p = dequeue_proximo();
    if (!p) { puts("Nenhum paciente aguardando."); return; }
    time_t agora = time(NULL);
    double espera = difftime(agora, p->chegada)/60.0; // minutos
    printf("Atendendo %s (CPF: %s) | Espera: %.1f min | Cor: %s\n",
           p->nome, p->cpf, espera, cor_string(p->prioridade));

    // registrar consulta (simplificado)
    char motivo[64];
    printf("Motivo/Resumo da consulta: "); fgets(motivo, sizeof(motivo), stdin); motivo[strcspn(motivo,"\n")]='\0';

    criar_pasta_consulta(p, motivo); // definida em pilha.c
}

static void fila_ui(void) {
    exibir_fila();
}

static void relatorio_ui(void) {
    gerar_relatorio(); // definida em fila.c ou módulo de estatísticas
}

static void prontuario_ui(void) {
    char cpf_input[32];
    printf("CPF do paciente: "); fgets(cpf_input, sizeof(cpf_input), stdin); cpf_input[strcspn(cpf_input,"\n")]='\0';
    Paciente* p = buscar_hash(cpf_input);
    if (!p) { puts("Paciente nao encontrado."); return; }
    listar_e_abrir_exames(p); // definida em pilha.c
}

static void print_pac(Paciente *pac, void *ctx) {
    (void)ctx;
    printf("%s | %s\n", pac->cpf, pac->nome);
}

static void listar_cpfs_ui(void)
{
    puts("\n== CPFs cadastrados ==");
    hash_foreach(print_pac, NULL);           /* percorre a tabela */
    puts("(copie e cole conforme necessario)\n");
}