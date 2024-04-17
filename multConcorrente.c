/* Programa concorrente multiplica duas matrizes recebidas de arquivos binario
 * Entrada: Nome dos arquivos binarios e o nome do arquivo de saída
 * Saída: Resultado da multiplicação das matrizes de entrada em forma de arquivo binário
 * */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

//Struct utilizada para passar os parametros para as threads
typedef struct{
    int x, m, i, j, id, rep;
    float *m1, *m2, *m3;
} argT;


//***Funcao executada pelas threads
//   cada thread calculara n linhas da matriz***
void * multmatriz(void * arg){
    argT *args = (argT *) arg;
    int i = args->i, j, k, l;
    for(j = i; j < i + args->rep; j++){
        for(l = 0; l < args->m; l++){
            for(k = 0; k < args->x; k++){
                args->m3[j*args->m+l] += args->m1[j*args->x+k] * args->m2[k*args->m+l];
            }
        }
    }
    return NULL;
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

    int nthreads;//variavel que carrega o no de threads

    inicio = clock();//inicia marcacao de tempo

    //Verifica se todos os argumentos de entrada estão presentes
    if(argc < 5){
        fprintf(stderr, "Digite: %s <Arquivo1> <Arquivo2> <ArquivoDeSaida> <numeroDeThreads>\n", argv[0]);
        return 1;
    }

    //Verifica se o no de threads é valido
    nthreads=atol(argv[4]);
    if(nthreads < 1){
        fprintf(stderr, "No de threads menor que 1\n");
        return -1;
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



    iniciom = clock();//inicia marcacao de tempo

    argT args[nthreads];//struct que carrega parametros das threads
    pthread_t tid_sistema[nthreads];//Aloca memoria para criacao das threads


    int rep = linhas1 / nthreads;//no de linhas da matriz que cada thread processara
    int resto = linhas1% nthreads;


    for(int i = 0; i<nthreads; i++){ //realiza a multiplicacao d
        args[i].id = i;
        args[i].rep = rep + (i < resto ? 1 : 0); //***caso o no de linhas n seja divisível pelo no de threads
        args[i].i = (i == 0 ? 0 : args[i-1].i + args[i-1].rep);
        args[i].x = colunas1;
        args[i].m = colunas2;
        args[i].m1 = matriz1;
        args[i].m2 = matriz2;
        args[i].m3 = matriz3;
        if(pthread_create(&tid_sistema[i], NULL, multmatriz, (void*) &args[i])){
            fprintf(stderr, "Falha ao criar thread\n");
            return -2;
        }
    }

    //Aguarda todas as threads terminarem para seguir com o programa
    for (int a=0; a<nthreads; a++) {
        if (pthread_join(tid_sistema[a], NULL)) {
            printf("Erro pthread_join()\n");
            return -3;
        }
    }


    fimm = clock();//finaliza marcacao de tempo
    tempom = ((double)(fimm-iniciom)) / CLOCKS_PER_SEC;
    printf("Tempo de execução da parte 2/concorrente (funcao de multiplicacao): %f segundos\n", tempom);


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
