/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 726573
 * Aluno: Michele Argolo Carvalho
 * ========================================================================== */
 
/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
 
/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
 
#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
 
 
#define POS_OCUPADA     "[%d] Ocupado: %s\n"
#define POS_LIVRE         "[%d] Livre\n"
#define POS_REMOVIDA     "[%d] Removido\n"
 
/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2
 
/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA                              "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM                          "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO                         "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO                        "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO                          "**********************EXCLUIR*********************\n"
 
#define SUCESSO                   "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                      "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA         "ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO         "Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"
 
 
/* Registro do jogo */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;
 
/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
    int estado;
    char pk[TAM_PRIMARY_KEY];
    int rrn;
} Chave;
 
/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
} Hashtable;
 
/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;
 
 
 
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
 
/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();
 
/* Exibe o jogo */
int exibir_registro(int rrn);
 
/*Função de Hash*/
short hash(const char* chave, int tam);
 
 
/*Auxiliar para a função de hash*/
short f(char x);
 
/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);
 
/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);
 
 
/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
Produto recuperar_registro(int rrn);
void criar_tabela(Hashtable *tabela, int tam);
void ler_entrada(Produto *novo);
void insere_arquivo(Produto novo);
int buscaTabelaHash(Hashtable *tabela, char chave[]);
void imprimir_tabela(Hashtable tabela);
int verificaDesconto(char desconto[]);
int removeHashTable(Hashtable *tabela, char chave[]);
int inserir_hashtable(Hashtable *tabela, char chave[], int pos, int eharquivo);
 
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main()
{
    /* Arquivo */
    int carregarArquivo = 0;
    scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
    if (carregarArquivo)
        carregar_arquivo();
 
    /* Tabela Hash */
    int tam;
    scanf("%d", &tam);
    tam = prox_primo(tam);
 
    Hashtable tabela;
    criar_tabela(&tabela, tam);
    if (carregarArquivo)
        carregar_tabela(&tabela);
 
    /* Execução do programa */
    int opcao = 0;
    while(opcao != 6) {
        scanf("%d%*c", &opcao);
        switch(opcao)
        {
            case 1:
                cadastrar(&tabela);
                break;
            case 2:
                printf(INICIO_ALTERACAO);
                if(alterar(tabela))
                    printf(SUCESSO);
                else
                    printf(FALHA);
                break;
            case 3:
                printf(INICIO_BUSCA);
                buscar(tabela);
                break;
            case 4:
                printf(INICIO_EXCLUSAO);
                if(    remover(&tabela))
                    printf(SUCESSO);
                else
                    printf(FALHA);
                break;
            case 5:
                printf(INICIO_LISTAGEM);
                imprimir_tabela(tabela);
                break;
            case 6:
                liberar_tabela(&tabela);
                break;
            case 10:
                printf(INICIO_ARQUIVO);
                printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
                break;
            default:
                printf(OPCAO_INVALIDA);
                break;
        }
    }
    return 0;
}
 
/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */
 
/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
    scanf("%[^\n]\n", ARQUIVO);
}
 
/*Auxiliar para a função de hash*/
short f(char x)
{
    return (x < 59) ? x - 48 : x - 54;
}
 
/* Exibe o jogo */
int exibir_registro(int rrn)
{
    if(rrn<0)
        return 0;
    float preco;
    int desconto;
    Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
    printf("%s\n", j.pk);
    printf("%s\n", j.nome);
    printf("%s\n", j.marca);
    printf("%s\n", j.data);
    printf("%s\n", j.ano);
    sscanf(j.desconto,"%d",&desconto);
    sscanf(j.preco,"%f",&preco);
    preco = preco *  (100-desconto);
    preco = ((int) preco)/ (float) 100 ;
    printf("%07.2f\n",  preco);
    strncpy(categorias, j.categoria, strlen(j.categoria));
    categorias[strlen(j.categoria)] = '\0';
 
    for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
        printf("%s ", cat);
    printf("\n");
    return 1;
}
 
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.pk,p);
    p = strtok(NULL,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);
    //gerarChave(&j);
    return j;
}
 
int  prox_primo(int a){
    int achou=0, naoehprimo=0, i;
 
    while(!achou){
        for(i = 2; i <= a/2; i++){
            if(a % i == 0){
                naoehprimo = 1;
                break;
            }
        }
 
        if(naoehprimo){
            naoehprimo = 0;
            a++;
        } else
            achou=1;
    }
 
    return a;
}
 
void criar_tabela(Hashtable *tabela, int tam){
    int i=0;
    // Aloca o espaço para a tabela hash
    tabela->tam = tam;
    tabela->v = (Chave *)malloc(sizeof(Chave) * tam);
    while(i < tabela->tam){
        tabela->v[i].rrn = -1;
        tabela->v[i].estado = LIVRE;
 
        i++;
    }
    nregistros = 0;
}
 
 
void carregar_tabela(Hashtable* tabela){
    char *percorreArquivo = ARQUIVO;
    char chave[TAM_PRIMARY_KEY] = "\0";
    char *percorreArquivoProx;
    nregistros = strlen(ARQUIVO)/TAM_REGISTRO;
    for(int i=0; i<nregistros; i++) {
        sscanf(percorreArquivo, "%[^@]@", chave);
        percorreArquivoProx = percorreArquivo + 1;
        if (*percorreArquivo == '*' && *percorreArquivoProx == '|')
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        else {
            inserir_hashtable(tabela, chave, i, 1);
            percorreArquivo = percorreArquivo + TAM_REGISTRO;
        }
    }
}
 
int inserir_hashtable(Hashtable *tabela, char chave[], int pos, int eharquivo){
    int espaco = hash(chave, tabela->tam);   // Procura posiçao pra ver se está livre
    int ncolisoes = 0;
    int primeiroEspaco = espaco;
 
    if(tabela->v[espaco].estado == OCUPADO) {   // Posição está ocupada, então procura o próximo espaço disponivel
        espaco++;
        ncolisoes++;
        if(espaco == tabela->tam)
            espaco = 0;
        while (espaco < tabela->tam) {
            if (tabela->v[espaco].estado != OCUPADO) { // pode inserir
                break;
            } else {
                if(primeiroEspaco == espaco) { // já deu a volta na tabela e não achou
                    return 0;
                }
 
                if (espaco == tabela->tam-1) {
                    espaco = 0;
                    ncolisoes++;
                } else {
                    espaco++;
                    ncolisoes++;
                }
            }
        }
    }
 
    // Insere normalmente
    tabela->v[espaco].estado = OCUPADO;
    tabela->v[espaco].rrn = pos;
    strcpy(tabela->v[espaco].pk, chave);
    if(!eharquivo)
        printf(REGISTRO_INSERIDO, chave, ncolisoes);
    return 1;
}
 
void cadastrar(Hashtable* tabela){
    Produto novo;
    int buscaHash = -1;
 
    ler_entrada(&novo);  // Lê as entradas específicas e coloca as informações na strct novo
 
    if (nregistros != 0) {   // Faz uma busca pra ver se a pk já ta na tabela
        buscaHash = buscaTabelaHash(tabela, novo.pk);
    }
 
    if (buscaHash != -1){
        printf(ERRO_PK_REPETIDA, novo.pk);
        return;
    } else { // Agora precisa conferir se a tabela ta cheia
        int insere = inserir_hashtable(tabela, novo.pk, nregistros, 0);
 
        if(insere == 1){ // Caso em que pode inserir
            insere_arquivo(novo);     // Insere no arquivo de dados
            nregistros = nregistros + 1;
        } else {
            printf(ERRO_TABELA_CHEIA);
        }
    }
}
 
void ler_entrada(Produto *novo){
    // Lê do usuários as informações de um registro
 
    scanf("%51[^\n]%*c", novo->nome);
    scanf("%51[^\n]%*c", novo->marca);
    scanf("%11[^\n]%*c", novo->data);
    scanf("%3[^\n]%*c", novo->ano);
    scanf("%8[^\n]%*c", novo->preco);
    scanf("%4[^\n]%*c", novo->desconto);
    scanf("%51[^\n]%*c", novo->categoria);
 
    //Gera chave
    novo->pk[0] = toupper(novo->nome[0]);
    novo->pk[1] = toupper(novo->nome[1]);
    novo->pk[2] = toupper(novo->marca[0]);
    novo->pk[3] = toupper(novo->marca[1]);
    novo->pk[4] = novo->data[0];
    novo->pk[5] = novo->data[1];
    novo->pk[6] = novo->data[3];
    novo->pk[7] = novo->data[4];
    novo->pk[8] = novo->ano[0];
    novo->pk[9] = novo->ano[1];
    novo->pk[10] = '\0';
 
}
 
 
void insere_arquivo(Produto novo){
    char *registro = ARQUIVO + nregistros * TAM_REGISTRO;
    char str[193] = "\0";
    int i = 0;
    // Insere em str todas as informações digitadas separadas pelo delimitador "@"
 
    sprintf(str, "%s", novo.pk);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.nome);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.marca);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.data);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.ano);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.preco);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.desconto);
    sprintf(str + strlen(str), "%s", "@");
    sprintf(str + strlen(str), "%s", novo.categoria);
    sprintf(str + strlen(str), "%s", "@");
 
    for(i =  strlen(str); i < TAM_REGISTRO; i++)
        str[i] = '#';
 
    str[i] = '\0';
 
    sprintf(registro, "%s" , str);
}
 
// Função de hash
short hash(const char* chave, int tam){
    return ((f(chave[0]) + 2 * f(chave[1]) + 3 * f(chave[2]) + 4 * f(chave[3]) + 5 * f(chave[4]) + 6 * f(chave[5]) + 7 * f(chave[6]) + 8 * f(chave[7])) % tam);
}
 
int buscaTabelaHash(Hashtable *tabela, char chave[]){
    int espaco = hash(chave, tabela->tam);
    int primeiroEspaco = espaco;
 
    if(tabela->v[espaco].estado == OCUPADO && strcmp(tabela->v[espaco].pk, chave) == 0) { // achou
        return tabela->v[espaco].rrn;
    } else {
        espaco++;
        if(espaco == tabela->tam)
            espaco = 0;
        while (espaco < tabela->tam) {
            if(tabela->v[espaco].estado == OCUPADO && strcmp(tabela->v[espaco].pk, chave) == 0)
                return tabela->v[espaco].rrn;
            else {
                if(primeiroEspaco == espaco) { // já deu a volta na tabela e não achou
                    return -1;
                }
 
                if (espaco == tabela->tam-1)
                    espaco = 0;
                else
                    espaco++;
            }
        }
    }
 
    return -1;
}
 
void buscar(Hashtable tabela){
    char chave[TAM_PRIMARY_KEY] = "\0";
    scanf("%11[^\n]%*c", chave);
 
    int espaco = hash(chave, tabela.tam);
    int primeiroEspaco = espaco;
    if(tabela.v[espaco].estado == OCUPADO && strcmp(tabela.v[espaco].pk, chave) == 0) { // achou
        exibir_registro(tabela.v[espaco].rrn);
    } else {
        while (espaco < tabela.tam) {
            if (tabela.v[espaco].estado == LIVRE || tabela.v[espaco].estado == REMOVIDO || (tabela.v[espaco].estado == OCUPADO && strcmp(tabela.v[espaco].pk, chave) != 0)) {
                espaco++;
                if (espaco == tabela.tam)
                    espaco = 0;
            } else {
                exibir_registro(tabela.v[espaco].rrn);
                return;
            }
 
            if(primeiroEspaco == espaco) { // já deu a volta na tabela e não achou
                printf(REGISTRO_N_ENCONTRADO);
                return;
            }
        }
    }
}
 
void imprimir_tabela(Hashtable tabela){
    for(int i=0; i<tabela.tam; i++){
        if(tabela.v[i].estado == OCUPADO)
            printf(POS_OCUPADA, i, tabela.v[i].pk);
        else if(tabela.v[i].estado == LIVRE)
            printf(POS_LIVRE, i);
        else
            printf(POS_REMOVIDA, i);
    }
}
 
int alterar(Hashtable tabela){
    char *pArquivo;
    char chave[TAM_PRIMARY_KEY];
    char desconto[TAM_DESCONTO] = "\0", teste[20];
    Produto recupera;
    float precoNovo;
 
    scanf("%11[^\n]%*c", chave);
    // Busca a chave primária digitada na tabela hash
    int buscaArquivo = buscaTabelaHash(&tabela, chave);
 
    if(buscaArquivo == -1){
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    do {
        scanf("%[^\n]%*c", teste);
    } while (verificaDesconto(teste) == 0);
 
    strcpy(desconto, teste);
    desconto[3] = '\0';
    pArquivo = ARQUIVO + (TAM_REGISTRO * buscaArquivo);
    recupera = recuperar_registro(buscaArquivo);
 
    pArquivo = pArquivo + strlen(recupera.pk) + strlen(recupera.nome) + strlen(recupera.marca) + strlen(recupera.data) + strlen(recupera.ano) + strlen(recupera.preco) + 6;
    *pArquivo = desconto[0];
    pArquivo++;
    *pArquivo = desconto[1];
    pArquivo++;
    *pArquivo = desconto[2];
 
    return 1;
 
}
 
int verificaDesconto(char desconto[]){
    if(((atof(desconto) > 100) || atof(desconto) < 0) || strlen(desconto) != 3){
        printf(CAMPO_INVALIDO);
        return 0;
    }
 
    else{
        if(strlen(desconto) == 3){
            for(int i = 0; i<strlen(desconto); i++){
                if(desconto[i] < 48 || desconto[i] > 57) {
                    printf(CAMPO_INVALIDO);
                    return 0;
                }
            }
        }
    }
    return 1;
}
 
int remover(Hashtable* tabela){
    char chave[TAM_PRIMARY_KEY];
    scanf("%11[^\n]%*c", chave);
 
    int buscaArquivo = removeHashTable(tabela, chave);
 
    if(buscaArquivo == -1) {
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    char *p = ARQUIVO + TAM_REGISTRO * buscaArquivo;
    *p = '*';
    p = p + 1;
    *p = '|';
 
    return 1;
}
 
int removeHashTable(Hashtable *tabela, char chave[]){
    int espaco = hash(chave, tabela->tam);
    int primeiroEspaco = espaco, rrn;
 
 
    if(tabela->v[espaco].estado == OCUPADO && strcmp(tabela->v[espaco].pk, chave) == 0) { // achou
        tabela->v[espaco].estado = REMOVIDO;
        rrn = tabela->v[espaco].rrn;
        tabela->v[espaco].rrn = -1;
        return rrn;
    } else {
        while (espaco < tabela->tam) {
            if ((tabela->v[espaco].estado == OCUPADO && strcmp(tabela->v[espaco].pk, chave) != 0) || tabela->v[espaco].estado == LIVRE || tabela->v[espaco].estado == REMOVIDO) {
                espaco++;
                if (espaco == tabela->tam)
                    espaco = 0;
            } else {
                tabela->v[espaco].estado = REMOVIDO;
                rrn = tabela->v[espaco].rrn;
                tabela->v[espaco].rrn = -1;
                return rrn;
            }
 
            if(primeiroEspaco == espaco) { // já deu a volta na tabela e não achou
                return -1;
            }
        }
    }
 
    return -1;
}
 
void liberar_tabela(Hashtable* tabela){
    free(tabela->v);
    tabela->v = NULL;
}
