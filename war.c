// ============================================================================
//         PROJETO WAR ESTRUTURADO - NIVEL AVENTUREIRO
// ============================================================================
// Nivel Aventureiro: Batalhas Estrategicas
// - Alocacao dinamica com calloc
// - Ponteiros
// - Simulacao de dados com rand()
// - Laco interativo de ataque
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Constantes Globais ---
#define NUM_TERRITORIOS 5
#define MAX_NOME        50
#define MAX_COR         20

// --- Estrutura de Dados ---
typedef struct {
    char nome[MAX_NOME];
    char corExercito[MAX_COR];
    int  numTropas;
} Territorio;

// --- Prototipos das Funcoes ---
Territorio *alocarMapa(void);
void        cadastrarTerritorios(Territorio *mapa);
void        exibirMapa(const Territorio *mapa);
void        simularAtaque(Territorio *mapa, int idAtacante, int idDefensor);
void        faseDeAtaque(Territorio *mapa);
void        liberarMemoria(Territorio *mapa);
void        limparBufferEntrada(void);

// ============================================================================
// FUNCAO PRINCIPAL
// ============================================================================
int main(void) {
    srand((unsigned int)time(NULL));

    // --- Alocacao dinamica ---
    Territorio *mapa = alocarMapa();
    if (mapa == NULL) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        return 1;
    }

    // --- Cadastro dos territorios ---
    cadastrarTerritorios(mapa);

    // --- Exibe mapa inicial ---
    printf("\n========== MAPA INICIAL ==========\n");
    exibirMapa(mapa);

    // --- Fase de ataque ---
    faseDeAtaque(mapa);

    // --- Exibe mapa final ---
    printf("\n========== MAPA FINAL ==========\n");
    exibirMapa(mapa);

    // --- Libera memoria ---
    liberarMemoria(mapa);

    printf("\nObrigado por jogar WAR Aventureiro!\n");
    return 0;
}

// ============================================================================
// IMPLEMENTACAO DAS FUNCOES
// ============================================================================

// Aloca dinamicamente o vetor de territorios com calloc.
// Retorna ponteiro para o vetor ou NULL em caso de falha.
Territorio *alocarMapa(void) {
    Territorio *mapa = (Territorio *)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

// Cadastra os dados de cada territorio via terminal (fgets e scanf).
void cadastrarTerritorios(Territorio *mapa) {
    printf("===== CADASTRO DE TERRITORIOS =====\n");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        printf("\n--- Territorio %d ---\n", i + 1);

        printf("Nome: ");
        limparBufferEntrada();
        fgets(mapa[i].nome, MAX_NOME, stdin);
        // Remove o '\n' que fgets captura
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor do Exercito: ");
        fgets(mapa[i].corExercito, MAX_COR, stdin);
        mapa[i].corExercito[strcspn(mapa[i].corExercito, "\n")] = '\0';

        printf("Numero de Tropas: ");
        scanf("%d", &mapa[i].numTropas);
    }
}

// Exibe o estado atual do mapa em formato de tabela.
// Usa 'const' pois apenas le os dados.
void exibirMapa(const Territorio *mapa) {
    printf("\n%-5s %-20s %-15s %s\n", "ID", "Nome", "Cor Exercito", "Tropas");
    printf("------------------------------------------------------\n");
    for (int i = 0; i < NUM_TERRITORIOS; i++) {
        if (mapa[i].numTropas > 0) {
            printf("[%d]   %-20s %-15s %d\n",
                   i + 1,
                   mapa[i].nome,
                   mapa[i].corExercito,
                   mapa[i].numTropas);
        } else {
            printf("[%d]   %-20s %-15s CONQUISTADO\n",
                   i + 1,
                   mapa[i].nome,
                   mapa[i].corExercito);
        }
    }
    printf("------------------------------------------------------\n");
}

// Simula uma batalha entre dois territorios usando dados (rand).
// Regras:
//   - Atacante vence (dado_ataque >= dado_defesa) -> defensor perde 1 tropa
//   - Se defensor zerar tropas -> territorio e conquistado (muda de cor)
//   - Empates favorecem o atacante
void simularAtaque(Territorio *mapa, int idAtacante, int idDefensor) {
    Territorio *atacante = &mapa[idAtacante];
    Territorio *defensor = &mapa[idDefensor];

    // Validacoes
    if (atacante->numTropas <= 1) {
        printf("\n[ERRO] O territorio atacante precisa de pelo menos 2 tropas para atacar!\n");
        return;
    }
    if (defensor->numTropas <= 0) {
        printf("\n[ERRO] O territorio defensor ja foi conquistado!\n");
        return;
    }
    if (strcmp(atacante->corExercito, defensor->corExercito) == 0) {
        printf("\n[ERRO] Voce nao pode atacar seu proprio territorio!\n");
        return;
    }

    // Rolagem dos dados (1 a 6)
    int dadoAtaque  = (rand() % 6) + 1;
    int dadoDefesa  = (rand() % 6) + 1;

    printf("\n========== BATALHA ==========\n");
    printf("  Atacante: %s (%s) - %d tropas\n",
           atacante->nome, atacante->corExercito, atacante->numTropas);
    printf("  Defensor: %s (%s) - %d tropas\n",
           defensor->nome, defensor->corExercito, defensor->numTropas);
    printf("  Dado de Ataque : %d\n", dadoAtaque);
    printf("  Dado de Defesa : %d\n", dadoDefesa);

    // Empate ou vitoria do atacante -> defensor perde tropa
    if (dadoAtaque >= dadoDefesa) {
        defensor->numTropas--;
        printf("  RESULTADO: Ataque bem-sucedido! %s perde 1 tropa.\n", defensor->nome);

        // Conquista do territorio
        if (defensor->numTropas <= 0) {
            printf("  *** TERRITORIO CONQUISTADO! ***\n");
            printf("  %s agora pertence ao exercito %s!\n",
                   defensor->nome, atacante->corExercito);
            strncpy(defensor->corExercito, atacante->corExercito, MAX_COR - 1);
            defensor->corExercito[MAX_COR - 1] = '\0';
            // Move 1 tropa do atacante para o territorio conquistado
            defensor->numTropas = 1;
            atacante->numTropas--;
        }
    } else {
        printf("  RESULTADO: Defesa bem-sucedida! %s resistiu ao ataque.\n", defensor->nome);
    }
    printf("=============================\n");
}

// Gerencia o laco interativo de ataque.
// Permite ao jogador escolher atacante e defensor repetidamente.
void faseDeAtaque(Territorio *mapa) {
    int continuar = 1;
    int atacante, defensor;

    printf("\n===== FASE DE ATAQUE =====\n");
    printf("Digite 0 para encerrar os ataques.\n");

    while (continuar) {
        exibirMapa(mapa);

        printf("\nEscolha o territorio ATACANTE (1 a %d) ou 0 para sair: ", NUM_TERRITORIOS);
        scanf("%d", &atacante);

        if (atacante == 0) {
            continuar = 0;
            break;
        }

        // Valida intervalo
        if (atacante < 1 || atacante > NUM_TERRITORIOS) {
            printf("[ERRO] Escolha invalida. Tente novamente.\n");
            limparBufferEntrada();
            continue;
        }

        printf("Escolha o territorio DEFENSOR (1 a %d): ", NUM_TERRITORIOS);
        scanf("%d", &defensor);

        if (defensor < 1 || defensor > NUM_TERRITORIOS) {
            printf("[ERRO] Escolha invalida. Tente novamente.\n");
            limparBufferEntrada();
            continue;
        }

        if (atacante == defensor) {
            printf("[ERRO] O atacante e o defensor nao podem ser o mesmo territorio!\n");
            continue;
        }

        // Indices internos comecam em 0
        simularAtaque(mapa, atacante - 1, defensor - 1);
    }
}

// Libera a memoria alocada para o mapa.
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

// Limpa o buffer de entrada do teclado para evitar leituras incorretas.
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}