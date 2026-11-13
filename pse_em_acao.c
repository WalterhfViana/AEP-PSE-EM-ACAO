/*
 * PSE em Acao - Sistema de Planejamento e Acompanhamento de Acoes
 * de Saude na Escola
 *
 * AEP - Engenharia de Software / UniCesumar - 2o semestre de 2026
 * Integrantes: Gabriel Dias Ramalho, Walter Viana, Pedro Henrique Bordini
 *
 * O sistema trabalha apenas com dados ficticios e coletivos.
 * Nao armazena nomes, diagnosticos ou qualquer dado individual de estudantes.
 *
 * Compilar: gcc -Wall -Wextra -o pse_em_acao pse_em_acao.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef LIMITE_ACOES
#define LIMITE_ACOES 100
#endif
#define TAM_TEXTO 60
#define TAM_DATA 11

#define PLANEJADA 1
#define REALIZADA 2
#define CANCELADA 3

typedef struct {
    int codigo;
    char escola[TAM_TEXTO];
    char tema[TAM_TEXTO];
    char data_prevista[TAM_DATA];
    char publico_alvo[TAM_TEXTO];
    char responsavel[TAM_TEXTO];
    int qtd_prevista;
    int qtd_efetiva;
    int situacao;
} Acao;

Acao acoes[LIMITE_ACOES];
int quantidade_acoes = 0;

/* ---------- Funcoes de entrada e validacao ---------- */

void remover_quebra_linha(char *texto) {
    texto[strcspn(texto, "\r\n")] = '\0';
}

int texto_vazio(const char *texto) {
    int i;
    for (i = 0; texto[i] != '\0'; i++) {
        if (!isspace((unsigned char) texto[i])) {
            return 0;
        }
    }
    return 1;
}

/* Le um texto obrigatorio: repete enquanto o campo estiver vazio. */
void ler_texto(const char *mensagem, char *destino, int tamanho) {
    char linha[256];

    while (1) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\nEntrada encerrada.\n");
            exit(0);
        }
        /* descarta o restante se o usuario digitou mais que o buffer */
        if (strchr(linha, '\n') == NULL) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
        }
        remover_quebra_linha(linha);

        if (texto_vazio(linha)) {
            printf("  [ERRO] Campo obrigatorio. Digite um valor.\n");
        } else if ((int) strlen(linha) >= tamanho) {
            printf("  [ERRO] Texto muito longo (maximo %d caracteres).\n", tamanho - 1);
        } else {
            strcpy(destino, linha);
            return;
        }
    }
}

/* Le um numero inteiro entre minimo e maximo, repetindo ate ser valido. */
int ler_inteiro(const char *mensagem, int minimo, int maximo) {
    char linha[64];
    char *fim;
    long valor;

    while (1) {
        ler_texto(mensagem, linha, sizeof(linha));
        valor = strtol(linha, &fim, 10);

        while (isspace((unsigned char) *fim)) {
            fim++;
        }
        if (*fim != '\0') {
            printf("  [ERRO] Digite apenas numeros inteiros.\n");
        } else if (valor < minimo || valor > maximo) {
            printf("  [ERRO] Valor deve estar entre %d e %d.\n", minimo, maximo);
        } else {
            return (int) valor;
        }
    }
}

/* Verifica o formato DD/MM/AAAA e se dia e mes sao possiveis. */
int data_valida(const char *data) {
    int dia, mes, ano;
    int dias_no_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') {
        return 0;
    }
    if (sscanf(data, "%2d/%2d/%4d", &dia, &mes, &ano) != 3) {
        return 0;
    }
    if (ano < 2000 || ano > 2100 || mes < 1 || mes > 12) {
        return 0;
    }
    if ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0) {
        dias_no_mes[1] = 29;
    }
    return dia >= 1 && dia <= dias_no_mes[mes - 1];
}

void ler_data(const char *mensagem, char *destino) {
    char linha[TAM_TEXTO];

    while (1) {
        ler_texto(mensagem, linha, sizeof(linha));
        if (data_valida(linha)) {
            strcpy(destino, linha);
            return;
        }
        printf("  [ERRO] Data invalida. Use o formato DD/MM/AAAA.\n");
    }
}

int ler_situacao(void) {
    printf("  1 - Planejada\n  2 - Realizada\n  3 - Cancelada\n");
    return ler_inteiro("Situacao: ", PLANEJADA, CANCELADA);
}

/* ---------- Funcoes auxiliares ---------- */

const char *nome_situacao(int situacao) {
    switch (situacao) {
        case PLANEJADA: return "Planejada";
        case REALIZADA: return "Realizada";
        case CANCELADA: return "Cancelada";
        default:        return "Desconhecida";
    }
}

/* Retorna a posicao da acao no vetor ou -1 se nao existir. */
int buscar_por_codigo(int codigo) {
    int i;
    for (i = 0; i < quantidade_acoes; i++) {
        if (acoes[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

/* Verifica se 'trecho' aparece dentro de 'texto', ignorando maiusculas. */
int contem_texto(const char *texto, const char *trecho) {
    int i, j;
    for (i = 0; texto[i] != '\0'; i++) {
        for (j = 0; trecho[j] != '\0'; j++) {
            if (texto[i + j] == '\0' ||
                tolower((unsigned char) texto[i + j]) != tolower((unsigned char) trecho[j])) {
                break;
            }
        }
        if (trecho[j] == '\0') {
            return 1;
        }
    }
    return 0;
}

void exibir_acao(const Acao *acao) {
    printf("----------------------------------------------------------\n");
    printf("Codigo ............: %d\n", acao->codigo);
    printf("Escola ............: %s\n", acao->escola);
    printf("Tema ..............: %s\n", acao->tema);
    printf("Data prevista .....: %s\n", acao->data_prevista);
    printf("Publico-alvo ......: %s\n", acao->publico_alvo);
    printf("Responsavel .......: %s\n", acao->responsavel);
    printf("Participantes prev.: %d\n", acao->qtd_prevista);
    if (acao->situacao == REALIZADA) {
        printf("Participantes efet.: %d\n", acao->qtd_efetiva);
    }
    printf("Situacao ..........: %s\n", nome_situacao(acao->situacao));
}

void pausar(void) {
    char linha[8];
    printf("\nPressione ENTER para voltar ao menu...");
    if (fgets(linha, sizeof(linha), stdin) == NULL) {
        exit(0);
    }
}

/* ---------- Funcionalidades do sistema ---------- */

/* RF01 / RF05 / RF07 - Cadastrar acao */
void cadastrar_acao(void) {
    Acao nova;
    int codigo;

    printf("\n===== CADASTRAR ACAO =====\n");

    if (quantidade_acoes >= LIMITE_ACOES) {
        printf("[ERRO] Limite de %d acoes atingido. Nao e possivel cadastrar.\n", LIMITE_ACOES);
        return;
    }

    codigo = ler_inteiro("Codigo da acao: ", 1, 999999);
    while (buscar_por_codigo(codigo) != -1) {
        printf("  [ERRO] Codigo %d ja cadastrado.\n", codigo);
        codigo = ler_inteiro("Codigo da acao: ", 1, 999999);
    }
    nova.codigo = codigo;

    ler_texto("Escola: ", nova.escola, TAM_TEXTO);
    ler_texto("Tema (ex.: alimentacao saudavel, saude bucal): ", nova.tema, TAM_TEXTO);
    ler_data("Data prevista (DD/MM/AAAA): ", nova.data_prevista);
    ler_texto("Publico-alvo (ex.: 6o ano): ", nova.publico_alvo, TAM_TEXTO);
    ler_texto("Responsavel (equipe ou setor): ", nova.responsavel, TAM_TEXTO);
    nova.qtd_prevista = ler_inteiro("Quantidade prevista de participantes: ", 0, 100000);

    nova.situacao = ler_situacao();
    nova.qtd_efetiva = 0;
    if (nova.situacao == REALIZADA) {
        nova.qtd_efetiva = ler_inteiro("Quantidade efetiva de participantes: ", 0, 100000);
    }

    acoes[quantidade_acoes] = nova;
    quantidade_acoes++;

    printf("\n[OK] Acao %d cadastrada com sucesso! (%d/%d)\n",
           nova.codigo, quantidade_acoes, LIMITE_ACOES);
}

/* RF02 - Listar acoes */
void listar_acoes(void) {
    int i;

    printf("\n===== ACOES CADASTRADAS =====\n");

    if (quantidade_acoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    printf("%-7s %-28s %-22s %-10s %-6s %-6s %-10s\n",
           "Codigo", "Escola", "Tema", "Data", "Prev.", "Efet.", "Situacao");
    printf("------------------------------------------------------------------------------------------\n");
    for (i = 0; i < quantidade_acoes; i++) {
        printf("%-7d %-28.28s %-22.22s %-10s %-6d ",
               acoes[i].codigo, acoes[i].escola, acoes[i].tema,
               acoes[i].data_prevista, acoes[i].qtd_prevista);
        if (acoes[i].situacao == REALIZADA) {
            printf("%-6d ", acoes[i].qtd_efetiva);
        } else {
            printf("%-6s ", "-");
        }
        printf("%-10s\n", nome_situacao(acoes[i].situacao));
    }
    printf("\nTotal: %d acao(oes).\n", quantidade_acoes);
}

/* RF03 - Pesquisar acoes por codigo, escola ou tema */
void pesquisar_acoes(void) {
    int tipo, codigo, posicao, i, encontradas = 0;
    char termo[TAM_TEXTO];

    printf("\n===== PESQUISAR ACOES =====\n");

    if (quantidade_acoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    printf("  1 - Por codigo\n  2 - Por escola\n  3 - Por tema\n");
    tipo = ler_inteiro("Pesquisar por: ", 1, 3);

    if (tipo == 1) {
        codigo = ler_inteiro("Codigo: ", 1, 999999);
        posicao = buscar_por_codigo(codigo);
        if (posicao == -1) {
            printf("[AVISO] Nenhuma acao com o codigo %d.\n", codigo);
        } else {
            exibir_acao(&acoes[posicao]);
        }
        return;
    }

    ler_texto("Digite o termo de busca: ", termo, TAM_TEXTO);
    for (i = 0; i < quantidade_acoes; i++) {
        if ((tipo == 2 && contem_texto(acoes[i].escola, termo)) ||
            (tipo == 3 && contem_texto(acoes[i].tema, termo))) {
            exibir_acao(&acoes[i]);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("[AVISO] Nenhuma acao encontrada para \"%s\".\n", termo);
    } else {
        printf("----------------------------------------------------------\n");
        printf("%d acao(oes) encontrada(s).\n", encontradas);
    }
}

/* RF04 / RF05 - Atualizar situacao e registrar participantes */
void atualizar_situacao(void) {
    int codigo, posicao, nova_situacao;

    printf("\n===== ATUALIZAR SITUACAO =====\n");

    if (quantidade_acoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    codigo = ler_inteiro("Codigo da acao: ", 1, 999999);
    posicao = buscar_por_codigo(codigo);
    if (posicao == -1) {
        printf("[ERRO] Acao com codigo %d nao encontrada.\n", codigo);
        return;
    }

    exibir_acao(&acoes[posicao]);
    printf("\nNova situacao:\n");
    nova_situacao = ler_situacao();

    if (nova_situacao == REALIZADA) {
        acoes[posicao].qtd_efetiva =
            ler_inteiro("Quantidade efetiva de participantes: ", 0, 100000);
    } else {
        acoes[posicao].qtd_efetiva = 0;
    }
    acoes[posicao].situacao = nova_situacao;

    printf("\n[OK] Acao %d atualizada para \"%s\".\n",
           codigo, nome_situacao(nova_situacao));
}

/* RF06 - Gerar resumo geral */
void gerar_resumo(void) {
    int i;
    int planejadas = 0, realizadas = 0, canceladas = 0;
    int total_previsto = 0, previsto_realizadas = 0, total_participantes = 0;
    float percentual = 0.0f;

    printf("\n===== RESUMO GERAL =====\n");

    if (quantidade_acoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    for (i = 0; i < quantidade_acoes; i++) {
        total_previsto += acoes[i].qtd_prevista;

        if (acoes[i].situacao == PLANEJADA) {
            planejadas++;
        } else if (acoes[i].situacao == REALIZADA) {
            realizadas++;
            previsto_realizadas += acoes[i].qtd_prevista;
            total_participantes += acoes[i].qtd_efetiva;
        } else if (acoes[i].situacao == CANCELADA) {
            canceladas++;
        }
    }

    /* percentual calculado sobre o previsto das acoes que ja aconteceram */
    if (previsto_realizadas > 0) {
        percentual = (float) total_participantes / previsto_realizadas * 100.0f;
    }

    printf("Total de acoes ...................: %d\n", quantidade_acoes);
    printf("  Planejadas .....................: %d\n", planejadas);
    printf("  Realizadas .....................: %d\n", realizadas);
    printf("  Canceladas .....................: %d\n", canceladas);
    printf("Participantes previstos (todas) ..: %d\n", total_previsto);
    printf("Participantes previstos (realiz.) : %d\n", previsto_realizadas);
    printf("Total de participantes efetivos ..: %d\n", total_participantes);
    printf("Percentual de participacao .......: %.1f%%\n", percentual);
}

/* ---------- Menu principal ---------- */

void exibir_menu(void) {
    printf("\n==========================================================\n");
    printf("        PSE EM ACAO - Acoes de Saude na Escola\n");
    printf("==========================================================\n");
    printf("  1 - Cadastrar acao\n");
    printf("  2 - Listar acoes\n");
    printf("  3 - Pesquisar acoes\n");
    printf("  4 - Atualizar situacao\n");
    printf("  5 - Gerar resumo\n");
    printf("  0 - Encerrar\n");
    printf("----------------------------------------------------------\n");
    printf("Acoes cadastradas: %d/%d\n", quantidade_acoes, LIMITE_ACOES);
}

int ler_opcao_menu(void) {
    char linha[64];
    char *fim;
    long valor;

    ler_texto("Escolha uma opcao: ", linha, sizeof(linha));
    valor = strtol(linha, &fim, 10);
    while (isspace((unsigned char) *fim)) {
        fim++;
    }
    if (*fim != '\0') {
        return -1;
    }
    return (int) valor;
}

int main(void) {
    int opcao;

    do {
        exibir_menu();
        opcao = ler_opcao_menu();

        switch (opcao) {
            case 1: cadastrar_acao();     break;
            case 2: listar_acoes();       break;
            case 3: pesquisar_acoes();    break;
            case 4: atualizar_situacao(); break;
            case 5: gerar_resumo();       break;
            case 0: printf("\nSistema encerrado. Ate logo!\n"); break;
            default:
                printf("\n[ERRO] Opcao invalida. Escolha um numero de 0 a 5.\n");
        }

        if (opcao != 0) {
            pausar();
        }
    } while (opcao != 0);

    return 0;
}
