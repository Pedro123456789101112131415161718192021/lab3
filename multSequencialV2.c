/* Programa sequencial que multiplica duas matrizes recebidas de arquivos binario
 * Entrada: Nome dos arquivos binarios e o nome do arquivo de saída
 * Saída: Resultado da multiplicação das matrizes de entrada em forma de arquivo binário
 * */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Funcao que realiza a multiplicacao de duas matrizes armazenando em uma nova matriz
void multmatriz(int x, int n, int m, float matriza[], float matrizb[], float matrizc[]){
    int i, j, k;
    for(i = 0; i < n; i++){
        for(j = 0; j < m; j++){
            for(k = 0; k < x; k++){
                matrizc[i*m+j] += matriza[i*x+k] * matrizb[k*m+j];
            }
        }
    }
}

int main(int argc, char*argv[]){
    float *matriz1, *matriz2; //Matrizes vindas dos arquivos
    float *matriz3; //Matriz que armazenara a multiplicacao
    int linhas1, colunas1, linhas2, colunas2; //Número de linhas e colunas de cada matriz
    long long int tam1, tam2, tam3; //Número de elementos de cada matriz

    FILE * descritorArquivo1; //***Descritores dos
    FILE * descritorArquivo2; //   arquivos de entrada***
    FILE * descritorArquivo3; //***Descritor arquivo de saida***

    size_t ret; //retorno de leitura de arquivo

    clock_t inicio, fim, iniciom, fimm; //Variaveis para verificacao de tempo de execucao
    double tempo, tempom;

    inicio = clock();//inicia marcacao de tempo

    //Verifica se todos os argumentos de entrada estão presentes
    if(argc < 4){
        printf("Digite: %s <Arquivo1> <Arquivo2> <ArquivoDeSaida>\n", argv[0]);
        return 1;
    }

    //Abre o arquivo binario 1
    descritorArquivo1 = fopen(argv[1], "rb");
    if(!descritorArquivo1){
        fprintf(stderr, "Erro ao abrir arquivo\n");
        return 2;
    }

    //leitura das dimensões arquivo 1
    ret = fread(&linhas1, sizeof(int), 1, descritorArquivo1);
    if(!ret){
        fprintf(stderr, "Erro ao ler dimensao\n");
        return 3;
    }
    ret = fread(&colunas1, sizeof(int), 1, descritorArquivo1);
    if(!ret){
        fprintf(stderr, "Erro ao ler dimensao\n");
        return 3;
    }

    //Abre o arquivo binario 2
    descritorArquivo2 = fopen(argv[2], "rb");
    if(!descritorArquivo2){
        fprintf(stderr, "Erro ao abrir arquivo\n");
        return 2;
    }

    //leitura das dimensões arquivo 2
    ret = fread(&linhas2, sizeof(int), 1, descritorArquivo2);
    if(!ret){
        fprintf(stderr, "Erro ao ler dimensao\n");
        return 3;
    }
    ret = fread(&colunas2, sizeof(int), 1, descritorArquivo2);
    if(!ret){
        fprintf(stderr, "Erro ao ler dimensao\n");
        return 3;
    }

    //Caso o no de colunas da primeira matriz for diferente do no de linhas da segunda
    //a multiplicacao nao pode ser efetuada
    if(colunas1 != linhas2){
        fprintf(stderr,"O numero de colunas da primeira matriz deve ser igual ao de linhas da segunda\n");
        return 4;
    }

    //Define o no de elementos de cada matriz
    tam1 = linhas1 * colunas1;
    tam2 = linhas2 * colunas2;
    tam3 = linhas1 * colunas2;


    //Aloca memória para as matrizes
    matriz1 = (float*) malloc(sizeof(float) * tam1);
    if(!matriz1){
        fprintf(stderr, "Erro ao alocar memoria\n");
        return 5;
    }
    matriz2 = (float*) malloc(sizeof(float) * tam2);
    if(!matriz2){
        fprintf(stderr, "Erro ao alocar memoria\n");
        return 5;
    }
    matriz3 = (float*) malloc(sizeof(float) * tam3);
    if(!matriz3){
        fprintf(stderr, "Erro ao alocar memoria\n");
        return 5;
    }

    //Carrega as matrizes dos arquivos
    ret = fread(matriz1, sizeof(float), tam1, descritorArquivo1);
    if(ret < tam1){
        fprintf(stderr, "Erro ao carregar matriz\n");
        return 6;
    }
    ret = fread(matriz2, sizeof(float), tam2, descritorArquivo2);
    if(ret < tam2){
        fprintf(stderr, "Erro ao carregar matriz\n");
        return 6;
    }


    //define os elementos iniciais de m3 = 0
    for(int i = 0; i < tam3; i++){
        matriz3[i] = 0;
    }


    fim = clock();//finaliza marcacao de tempo
    tempo =((double)(fim-inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execucao da parte 1(verificacao de entradas, abertura de arquivos, alocacao memoria, etc): %f segundos\n", tempo);


    //realiza a funcao de multiplicacao
    iniciom = clock();//inicia marcacao de tempo
    multmatriz(colunas1, linhas1, colunas2, matriz1, matriz2, matriz3);
    fimm = clock();//finaliza marcacao de tempo
    tempom = ((double)(fimm-iniciom)) / CLOCKS_PER_SEC;
    printf("Tempo de execução da parte 2 (funcao de multiplicacao): %f segundos\n", tempom);


    inicio = clock();//inicia marcacao de tempo
    //inicia escrita binaria no arquivo
    descritorArquivo3 = fopen(argv[3], "wb");
    if(!descritorArquivo3){
        fprintf(stderr, "Erro ao abrir arquivo\n");
        return 2;
    }

    //escreve as dimensoes da nova matriz
    ret = fwrite(&linhas1, sizeof(int), 1, descritorArquivo3);
    if(!ret){
        fprintf(stderr, "Erro ao escrever dimensao\n");
        return 7;
    }
    ret = fwrite(&colunas2, sizeof(int), 1, descritorArquivo3);
    if(!ret){
        fprintf(stderr, "Erro ao escrever dimensao\n");
        return 7;
    }
    //escreve os elementos
    ret = fwrite(matriz3, sizeof(float), tam3, descritorArquivo3);
    if(ret<tam3){
        fprintf(stderr, "Erro ao escrever arquivo\n");
        return 7;
    }

    //Finalizas os descritores de arquivo e libera memoria alocada
    fclose(descritorArquivo1);
    fclose(descritorArquivo2);
    fclose(descritorArquivo3);
    free(matriz1);
    free(matriz2);
    free(matriz3);

    fim = clock();//finaliza marcacao de tempo
    tempo =((double)(fim-inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execucao da parte 3(escrever dimensoes e elementos no arquivo binario, liberar memoria, etc): %f segundos\n", tempo);

    return 0;
}
