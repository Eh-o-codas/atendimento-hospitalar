#include "triagem.h"
#include <stdio.h>
int classificar_paciente_ui(void) {
    int pr;
    puts("Classifique o paciente pelo nivel de risco:");
    puts("1 - Vermelho (emergencia)");
    puts("2 - Laranja (muito urgente)");
    puts("3 - Amarelo (urgente)");
    puts("4 - Verde (pouco urgente)");
    puts("5 - Azul (nao urgente)");
    printf("Escolha: ");
    if (scanf("%d", &pr)!=1 || pr<1 || pr>5) { while (getchar()!='\n'); return 3; }
    while (getchar()!='\n');
    return pr;
}