/*
 Versão do compilador: gcc (SUSE Linux) 4.8.5
 Descricao: Jogo em grid 2D (20x20) com jogador principal e personagens secundarios, com suas respectivas funcoes
 (aleatorias ou nao), e sendo o objetivo principal o jogador conseguir o maior numero de pontos antes do tempo
 limite se esgotar. O jogador tera opcao de alterar as opcoes de jogo em configuracoes, ver as instrucoes e, por fim,
 ranking com os dez primeiros colocados.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*#include <termios.h>*/
#include <unistd.h>
#include <time.h>


int TAMX;
int TAMY;
int MAXFRUIT;
int MAXBARR;
int MAXINIM;


#define MAXBOMBA 20 /*O valor maximo de bombas 'Q' eh 20.*/
#define MAXPTS 30 /*Valor maximo de pontos.*/

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

int numJogadas = 40; /*Quantidade de jogadas permitidas por jogo.*/


/*Structs para guardar os Tipos dos Personagens, Tempo e Tabuleiro (Grid 2D).*/


typedef struct Inimigo{
    int x, y;
}inimigo;

typedef struct Jogador{
    int pontos;
    int x, y;
}joga;
typedef struct Fruta{
    int x, y;
    int tempo;
}fruit;

typedef struct Bomba{
    int x, y;
    int tempo;
}bomba;

typedef struct Barreira{
    int x, y;
    int tempo;
}barreira;

typedef struct tabuleiro{
    char tabuleiro[40][40];
}tabu;

typedef struct{
    char nome[11];
}ranking;


/*Matriz do tabuleiro*/
tabu* iniciaTabuleiro(){
    int i, j;

    tabu *t1 = (tabu*)malloc(sizeof(tabu)); /*Reserva especifica de memoria que depende do ponteiro 'tabu'.*/

    /*Insere pontos (.) no mapa do jogo enquanto o incremento for menor que o tamanho do espaco grid.*/
    for(i=0; i<TAMX; i++){
        for(j=0 ;j<TAMY ;j++){
            t1->tabuleiro[i][j] = '.';
        }
    }
    /*Retorna para a funcao principal os pontos em formato matricial.*/
    return t1;
}

/* Printa tabuleiro enquanto o jogador estiver dentro do tempo estipulado e com o numero de jogadas nao nulo.*/
void printTabuleiro(tabu* t){
    printf("Jogadas restantes: %d\n", numJogadas);
    int i, j;

    /* A iteracao deve ser menor que o tamanho definido previamente em #define.*/
    for(i=0; i<TAMX; i++){
        printf("\n\t\t");
        for(j=0 ;j<TAMY ;j++){
            printf("%c", t->tabuleiro[i][j]);
        }
    }
    printf("\n");
}


/*Inicia jogador no mapa*/
joga* iniciaJogador(){
    joga * nj = (joga*)malloc(sizeof(joga));
    nj->pontos = 0;
    nj->x = TAMX - 1;
    nj->y=2;

    return nj;
}

/*Inicia inimigo no mapa*/
inimigo* iniciaInimigo(){
    int rx, ry;

    inimigo* n = (inimigo*)malloc(sizeof(inimigo)); /*Reserva especifica de memoria dependendo do tamanho do ponteiro inimigo.*/
    rx = (rand() %TAMX-1); /*Random inimigo em x depende do tamanho final do grid.*/
    ry = (rand() %TAMY-1); /*Random inimigo em y depende do tamanho final do grid.*/

    n->x = rx; /*Posicoes aleatorias na horizontal.*/
    n->y = ry; /*Posicoes aleatorias na vertical.*/

    return n;
}

bomba* iniciaBomba(){
    bomba *n = (bomba*)malloc(sizeof(bomba)); /*Reserva especifica de memoria que depende do tamanho do ponteiro bomba.*/

    /*Bomba aparece aleatoriamente*/
    if(rand()%1 == 0){
        n->x= rand()%(TAMX-1);
        n->y= rand()%(TAMY -1);
        n->tempo = (rand() % 10);
        /*O tempo tambem e alternado, de acordo com o random.*/
    }
    return n;
}

/*Coloca o personagem 'B' no plano, e funciona como uma barreira que por onde passa, deixa rastro.*/
barreira* iniciaBarreira(){

    /*Barreira aparece aleatoriamente*/
    barreira* n = (barreira*)malloc(sizeof(barreira)); /*Reserva especifica de memoria. Depende do tamanho do ponteiro barreira.*/
    if(rand()%8 ==0){
        n->x= rand()%(TAMX-1);
        n->y= rand()%(TAMY -1);
        n->tempo = (rand() % 10);
    }
    return n;
}

/*Inicia Fruta 'O' no mapa*/
fruit* iniciaPonto(){
    fruit* n = (fruit*)malloc(sizeof(fruit)); /*Reserva especifica de memoria que depende do tamanho do ponteiro 'fruit'.*/

    /*Define (aleatoriamente) quando a "fruta" aparece.*/
    if(rand()%4 == 0){
        n->x= (rand()%(TAMX-1));
        n->y= (rand()%(TAMY-1));
        n->tempo = (rand()%10);
    }

    return n;
}

/*Move inimigo no plano*/
void moveInim(inimigo* inim[MAXINIM], int num_inim){
    int r, i;

    i=0;
    while(i != num_inim){ /*Enquanto o numero de inimigos for diferente de 0...*/
        /*Movimento aleatorio (0-3=4 (25%)).*/
        r = (rand() %3);
        switch(r){
            case 0:
                if((inim[i]->x-1)== 0)
                    break;
                inim[i]->x--; /*Inimigo se move na horizontal*/
                break;
            case 1:
                if(inim[i]->y -1 ==0)
                    break;
                else
                    inim[i]->y--; /*Inimigo se move na vertical*/
                break;
            case 2:
                if(inim[i]->x +1 == TAMX)
                    break;
                else
                    inim[i]->x++; /*Inimigo se move na horizontal*/
                break;
            case 3:
                if((inim[i]->y+1)== TAMY)
                    break;
                else
                    inim[i]->y++; /*Inimigo se move na vertical*/
                break;
        }
        i++;
    }
}

/*Ordena as frutas/ponto extra.*/
void ordenaPilha_fruit(fruit* fruta[MAXFRUIT],int *n_fruit){
    int i, j, ls;
    fruit * aux;
    /*Nova fruta*/
    int new_n_fruit = *n_fruit;

    ls = *n_fruit;
    for (j = 0; j < *n_fruit ; j++) {
        for (i = 0; i < ls; i++) {
            if(fruta[i]->tempo <= 0){
                aux = fruta[i]; /*Auxiliar p/guardar fruta*/
                fruta[i] = fruta[i+1];
                fruta[i+1] = aux;
                new_n_fruit--;
            }/*Quando o jogador pega fruta*/
        }
        ls--;
    }
    *n_fruit =  new_n_fruit; /*Nova fruta*/

}

int insereElementos(tabu* t, joga * j, inimigo* inim[MAXINIM], int num_inim, fruit* fruta[MAXFRUIT], int *num_fruit, bomba * b[MAXBOMBA], int * n_bomba, int *ptr_pont, barreira* barreira[MAXINIM], int n_barreira){
    int x, y;
    int i;
    /*int r;*/


    /*Insere as bombas no mapa*/
    i= 0;
    int tempoBomba;
    while(i != *n_bomba){
        x = b[i]->x;
        y = b[i]->y;

        t->tabuleiro[x][y] = 'Q'; /*Coloca o caractere 'Q' no grid quando houver uma bomba.*/


        tempoBomba = b[i]->tempo;
        b[i]->tempo = tempoBomba -1;

        /*Confere se a bomba explodiu, colocando # ao redor do caractere 'Q'.*/
        if(b[i]->tempo == 0){ /*Tempo de explosao da bomba*/
            t->tabuleiro[x+1][y] = '#';
            t->tabuleiro[x-1][y] = '#';
            t->tabuleiro[x][y+1] = '#';
            t->tabuleiro[x][y-1] = '#';
            t->tabuleiro[x+1][y+1] = '#';
            t->tabuleiro[x-1][y-1] = '#';
            t->tabuleiro[x+1][y-1] = '#';
            t->tabuleiro[x-1][y+1] = '#';

            /*Jogador morre caso esteja dentro dessa area estipulada.*/
            if( ((x-3 < j->x) && (j->x < x+3)) && ((y-3 < j->y) && (j->y < y+3))){
                return 1;
            }
            /*Caso contrario, ganha pontos extras por nao ter morrido por conta de bomba.*/
            else{
                *ptr_pont+=2;
            }

        }

        /*Posicao do jogador na posicao da bomba.*/
        if((j->x == x) &&(j->y == y)){
            return 1;
        }

        i++; /*Itera e retorna ao loop 'while'*/
    }

    /*Insere o jogador no mapa*/
    x = j->x;
    y = j->y;

    if(t->tabuleiro[x][y] == 'O')
        j->pontos++; /*a "fruta" aumenta a pontuacao do jogador*/
    if(t->tabuleiro[x][y] == 'X')
        return 1;
    t->tabuleiro[x][y] = 'C';


    /*Insere inimigo no mapa*/
    i= 0;
    while(i != num_inim){
        x = inim[i]->x;
        y = inim[i]->y;
        if((j->x == x) &&(j->y == y)){
            return 1;
        }
        t->tabuleiro[x][y] = 'X';
        i++; /*Iteracao aumenta e volta para o inicio do loop 'while'.*/
    }

    /*Insere barreira no mapa*/
    i= 0;
    while(i != n_barreira){
        x = barreira[i]->x;
        y = barreira[i]->y;
        if((j->x == x) &&(j->y == y)){
            return 1;
        }
        t->tabuleiro[x][y] = 'B'; /*O personagem da barreira sera representado por 'B'.*/

        i++;
    }

    /*Insere fruta no mapa*/
    i= 0;
    while(i != *num_fruit){
        /*ordenaPilha_fruit(fruta, num_fruit);*/
        x = fruta[i]->x;
        y = fruta[i]->y;
        t->tabuleiro[x][y] = 'O';
        fruta[i]->tempo--;
        i++;
    }

    return 0;
    }


    /*Controle do jogador.*/
    joga* recebeComando(joga * j){
    joga *n;
    char tecla;

    n=j;

    tecla = getchar();
        if(tecla == '\n')
            tecla = getchar();

        switch(tecla){
        case 'w':  /*move pra cima*/
            if((n->x)== 0)
                break;
            n->x--;
            break;

        case 'a': /*move para a esquerda*/
            if(n->y  ==0)
                break;
            else
                n->y--;
            break;

        case 's': /*move para baixo*/
            if(n->x  == TAMX)
                break;
            else
                n->x++;
            break;

        case 'd': /*move para a direita*/
            if((n->y)== TAMY)
                break;
            else
                n->y++;
            break;
    }
    return n;
}

void EsperaEnter() /*Funcao para o botao 'enter'*/
{
    int tecla;
    do
    {
        tecla = getchar();
    } while(tecla==13);
}

/*Parte 2: Configuracoes*/

int modo1;
int modo2;
int modo3;
int modo5;

/*MENU de configuracoes.*/
void MENUconf(){
    system(CLEAR);
    printf("Configuracoes\n");
    printf("1. Tabuleiro\n");
    printf("2. NPCs\n");
    printf("3. Ativar Modo Rankeado\n");
    printf("4. Voltar\n");
}


/*Mudancas nas configuracoes originais do tabuleiro.*/
int kconf;
int qconf;
void confTabu(){
    int kconf=0;
    int qconf=0;
    int sn=0;

    system(CLEAR);
    printf("Alterar dimensoes de tabuleiro (modo nao-rankeado).\n");
    printf("Altura (MIN 10-MAX 40): ");
    scanf("%d",&kconf);
    if(kconf<10 || kconf>40){
        printf("Valor incorreto!\n");
        printf("Digite um valor valido: ");
        scanf("%d",&kconf);
    }
    printf("Largura (MIN 10- MAX 40): ");
    scanf("%d",&qconf);
    if(qconf<10 || qconf>40){
        printf("Valor incorreto!\n");
        printf("Digite um valor valido: ");
        scanf("%d",&qconf);
    }
    printf("Deseja salvar suas alteracoes?\n");
    printf("0-nao/1-sim.\n");
    scanf("%d",&sn);
    if(sn==1){
        modo1;
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }else if(sn!=1 && sn!=0){
        printf("Valor incorreto!\nDigite um valor valido: ");
        scanf("%d",&sn);
    }else{
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }
}

void mudancasTab(){

}
/*Mudancas nas configuracoes originais dos inimigos.*/
int rconf;
int sconf;
int tconf;
void confNPC(){
    modo2;
    int rconf=0;
    int sconf=0;
    int tconf=0;
    int ns=0;

    system(CLEAR);
    printf("Alterar configuracoes de inimigos (modo nao-rankeado).\n");
    printf("Quantidade de Inimigo(O) (MIN 1- MAX 4): ");
    scanf("%d",&rconf);
    if(rconf<0 || rconf>4){
        printf("Valor incorreto!\n");
        printf("Digite um valor valido: ");
        scanf("%d",&rconf);
    }
    printf("Quantidade de Inimigo(B) (MIN 10- MAX 20): ");
    scanf("%d",&sconf);
    if(sconf<0 || sconf>20){
        printf("Valor incorreto!\n");
        printf("Digite um valor valido: ");
        scanf("%d",&sconf);
    }
    printf("Quantidade de Inimigo(X) (MIN 10- MAX 20): ");
    scanf("%d",&tconf);
    if(tconf<0 || tconf>20){
        printf("Valor incorreto!\n");
        printf("Digite um valor valido: ");
        scanf("%d",&tconf);
    }
    printf("Deseja salvar suas alteracoes?\n");
    printf("0-nao/1-sim\n");
    scanf("%d",&ns);
    if(ns==1){
        modo2;
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }else if(ns!=1 && ns!=0){
        printf("Valor incorreto!\nDigite um valor valido: ");
        scanf("%d",&ns);
    }else{
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }
}

void FileSettings(FILE* arqconf){
  rewind(arqconf);

    if(modo1){
      fscanf(arqconf,"\nTAMX:%d TAMY:%d\n",&TAMX,&TAMY);
      printf("\nTAMX:%d TAMY:%d\n",TAMX,TAMY);
    }else
    if(modo2){
      fscanf(arqconf,"\nO:%d B:%d X:%d\n",&MAXFRUIT,&MAXBARR,&MAXINIM);
      printf("\nO:%d B:%d  X:%d\n",MAXFRUIT,MAXBARR,MAXINIM);
    }else
    if(modo3){
      printf("Configuracoes para modo rankeado:\n");
      printf("Altura e largura do tabuleiro: 30x30\n");
      printf("Quantidade de barreiras: 3\n");
      printf("Quantidade de inimigos: 7\n");
      printf("Tamanho de barreira: 7\n");
      printf("Tamanho de bombas: 10\n");
    }
}


void salvatxtConf(FILE* arqconf){
    char nome[]="ranking.txt";
    arqconf=fopen(nome,"a+");
}

int modoranq;
void settingsRank(){
    system(CLEAR);
    modoranq=0;
    printf("Configuracoes para modo rankeado:\n");
    printf("Altura e largura do tabuleiro: 30x30\n");
    printf("Quantidade de barreiras: 3\n");
    printf("Quantidade de inimigos: 7\n");
    printf("Tamanho de barreira: 7\n");
    printf("Tamanho de bombas: 10\n");
    printf("Deseja jogar no modo rankeado\n");
    printf("0-nao/1-sim\n");
    scanf("%d",&modoranq);
    if(modoranq==1){
        modo3;
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }else
    if(modoranq!=0){
        printf("Valor invalido!\nDigite novamente.\n");
        scanf("%d",&modoranq);
    }else{
        printf("Pressione 'ENTER' para retornar ao INICIO\n");
    }
}

void modoRank(){
    modo3;
}

/*Parte 3: Ranking.*/

ranking rank;

/*Abre arquivo .txt*/
void abre_arquivotxt(FILE* arqrank){
  char nome[]="ranking.txt";
  arqrank=fopen(nome,"a+");
}

/*void ordenaRankDec(FILE* binrank,ranking rank){
    int i=0;
    int temp=0;
*/

/*Abre e escreve no arquivo binario.*/
void registerMatch(FILE *binrank){
    binrank = fopen("ranking.bin", "wb");
    fwrite(rank.nome, sizeof(rank.nome), 1, binrank);
    fclose(binrank);
}

/*Usuario digita nome ou nickname no arquivo*/
void nickRank(ranking rank, FILE *arqrank){
    system(CLEAR);
    printf("Nome/nickname (max 10 caracteres): ");
    scanf("%s",rank.nome);

    if (strlen(rank.nome) > 10){
        printf("Maximo de caracteres atingido!\n");
        printf("Nome/nickname (max 10 caracteres): ");
        scanf("%s",rank.nome);
    }

    fprintf(arqrank, "%s", rank.nome);
    fseek(arqrank, 1, SEEK_SET);
    /*fwrite(&rank, sizeof(ranking), 1, arqrank);*//*Insere os dados no arquivo*/
}

/*Mostra o Ranking para o jogador.*/
void mostrarRank(FILE* arqrank){
  int j=1;
  char nome[11];
  printf("\nNome            Colocacao\n");
  rewind(arqrank);
  do
  {
      fscanf(arqrank,"\n%s          \n",rank.nome);
      printf("\n%s         \n",rank.nome);
      printf("\n");
      if(feof(arqrank)==0){
        j++;
      }
  }while(feof(arqrank)==0 && feof(arqrank)<11);

  printf("\nTecle <ENTER> para continuar!");
}

/*Opcao 4 do menu para mostrar as instrucoes do jogo*/
void regras(){
    system(CLEAR); /*Limpa a tela do usuario.*/
    printf("##########\n");
    printf("INSTRUCOES.\n");
    printf("##########\n");
    printf("\n");
    printf("o jogador controla o 'C' no tabuleiro\n");
    printf("para se movimentar eh necessario digitar uma das teclas de controle por vez\n");
    printf("\nControles:\n");
    printf("w- cima\n");
    printf("s- baixo\n");
    printf("a- esquerda\n");
    printf("d- direita\n");
    printf("\nPersonagens:\n");
    printf("'X'- inimigo violento. persegue o jogador.\n");
    printf("'B'- inimigo passivo. dificulta para o jogador, agindo como uma barreira aleatoria.\n");
    printf("'Q'- inimigo bomba. explode e gera tambem uma area explosiva.\n");
    printf("\nPontos:\n");
    printf("'#'- sobreviver aos ataques explosivos. (+2)\n");
    printf("'O'- ponto bonus para o jogador. (+2)\n");
    printf("\nObjetivo:\n");
    printf("conseguir o maximo de pontos antes do tempo e as jogadas se esgotarem.\n");
    printf("\nGAME OVER:\n");
    printf("tempo maximo esgotado;\n");
    printf("numero de jogadas excedido;\n");
    printf("pontuacao maxima alcancada;\n");
    printf("area explosiva (bomba);\n");
    printf("ataque de inimigo.\n");
    printf("\n");
}

int jogo;
int main(){

    tabu * t;
    joga * j;
    inimigo *inim[MAXINIM];
    fruit *fruta[MAXFRUIT];
    bomba *bombas[MAXBOMBA];
    barreira *barreira[MAXBOMBA];
    int n_inim =0, n_fruit=0, n_bomba=0, n_barreira=0;
    int continua=0;
    int opt;
    int pontuacao=0;
    int *ptr_pont;
    ptr_pont = &pontuacao;
    double diff=0;

    system(CLEAR); /*Limpa a tela do usuario.*/
    printf("\n");
    printf("\n");
    printf("\n");
    printf("*************************\n");
    printf("*************************\n");
    printf("Tabuleiro Randomico\n"); /*Nome do jogo.*/
    printf("*************************\n");
    printf("*************************\n");
    printf("\n");
    printf("\n");
    printf("\n");

    printf("Pressione 'ENTER' para prosseguir.\n");
    EsperaEnter();


    TAMX=0;
    TAMY=0;
    MAXFRUIT=0;
    MAXBARR=0;
    MAXINIM=0;

    n_inim =0; /*Inicializa novamente dados pra evitar lixo de memoria.*/
    n_fruit=0;
    n_bomba=0;
    n_barreira=0;
    continua=0;
    pontuacao=0;
    diff=0;
    int modo;
    int conf;

    /*Inicializa Arquivos*/
    FILE* arqrank = fopen("ranking.txt","a+");
    FILE* binrank = NULL;
    FILE* arqconf = fopen("settings.txt","a+");


    system(CLEAR); /*Limpa a tela do usuario.*/
    /*MENU principal.*/
    printf("##########\n");
    printf("MENU.\n");
    printf("##########\n");
    printf("1- Jogar\n");
    printf("2 -Configuracoes\n");
    printf("3- Ranking\n");
    printf("4- Instrucoes\n");
    printf("5- Sair\n");
    printf("\n");
    printf("Escolha uma opcao: ");
    do{
        fflush(stdin); /*limpa o buffer do teclado antes do jogador pressionar alguma tecla.*/
        scanf("%d",&opt); /*Pede ao jogador para digitar a opcao desejada*/

        switch(opt){
            system(CLEAR);
            case 1: /*Quando o jogador pressiona '1', entra em case 1.*/

                system(CLEAR); /*Limpa a tela do usuario.*/
                jogo=0;
                salvatxtConf(arqconf); /*Arquivo texto de configuracoes.*/
                FileSettings(arqconf);

                printf("Deseja jogar em modo rankeado?\n");
                printf("0-nao 1-sim\n");
                scanf("%d",&modo);
                if(modo!=0 && modo!=1){
                    printf("Opcao invalida!\n");
                    printf("Deseja jogar em modo rankeado?\n");
                    printf("0-nao 1-sim\n");
                    scanf("%d",&modo);
                }
                if((modo==1)||(modoranq==1)){
                    jogo=1;
                    nickRank(rank,arqrank);
                    abre_arquivotxt(arqrank);
                    modoRank();
                }


                /*Definicoes de TABX*/
                if(modo1){
                    TAMX=kconf;
                }else
                    if(modo2){
                        TAMX=20;
                    }else
                        if(modo3){
                            TAMX=30;
                        }else{
                            TAMX=20; /*Tamanho X do grid bidimensional eh 20.*/
                        }

                /*Definicoes de TABY*/
                if(modo1){
                    TAMY=qconf;
                }else
                    if(modo2){
                        TAMY=20;
                    }else
                        if(modo3){
                            TAMY=30;
                        }else{
                            TAMY=20; /*Tamanho Y do grid bidimensional eh 20.*/
                        }

                /*Definicoes de 'O'*/
                if(modo1){
                    MAXFRUIT=4;
                }else
                    if(modo2){
                        MAXFRUIT=rconf;
                    }else
                        if(modo3){
                            MAXFRUIT=8;
                        }else{
                            MAXFRUIT=10; /*Valor maximo de frutas (ponto extra 'O').*/
                        }

                /*Definicoes de 'B'*/
                if(modo1){
                    MAXBARR=20;
                    }else
                    if(modo2){
                        MAXBARR=sconf;
                        }else
                        if(modo3){
                            MAXBARR=30;
                            }else{
                            MAXBARR=20; /*Valor maximo de barreiras (inimigo que deixa rastro 'B')*/
                            }

                /*Definicoes de 'X'*/
                if(modo1){
                    MAXINIM=20;
                    }else
                    if(modo2){
                    MAXINIM=tconf;
                        }else
                        if(modo3){
                        MAXINIM=30;
                            }else{
                            MAXINIM=40; /*Valor maximo de inimigos 'X'.*/
                            }

                system(CLEAR); /*Limpa a tela do usuario.*/
                numJogadas=40;/*Inicializa o jogo com o maximo limite de jogadas para evitar lixo de memoria.*/
                time_t start,end; /*Variaveis da biblioteca "time.h"*/
                setbuf(stdin, 0);
                time (&start); /*Inicio da contagem de tempo*/

                t = iniciaTabuleiro(); /*Chama funcao de inicia tabuleiro no mapa*/
                j = iniciaJogador(); /*Chama funcao que inicia jogador no mapa*/
                inim[n_inim++] = iniciaInimigo();
                bombas[n_bomba++] = iniciaBomba(); /*Cria 20 bombas e aloca a posicao em cada espaco*/
                barreira[n_barreira++] = iniciaBarreira();
                fruta[n_fruit++] = iniciaPonto();
                insereElementos(t,j, inim, n_inim, fruta, &n_fruit, bombas, &n_bomba, ptr_pont, barreira, n_barreira); /*Insere os elementos no mapa*/
                printTabuleiro(t); /*Printa o grid no mapa apos chamar as funcoes de cada personagem.*/

                while((continua==0) && (numJogadas >=0)){
                    t = iniciaTabuleiro();
                    j = recebeComando(j);
                    bombas[n_bomba++] = iniciaBomba(); /*Criou Bombas*/
                    barreira[n_barreira++] = iniciaBarreira();
                    numJogadas = (numJogadas -1);
                    moveInim(inim, n_inim);
                   /*moveBarreira(barreira,n_inim,t);*/
                    continua = insereElementos(t,j, inim, n_inim, fruta, &n_fruit, bombas, &n_bomba,ptr_pont, barreira, n_barreira);
                    system(CLEAR); /*Limpa a tela do usuario.*/
                    printTabuleiro(t); /*Printa Tabuleiro*/
                }
                time (&end); /*Fim da contagem de tempo*/
                diff = difftime (end,start);
                setbuf(stdout, 0);


                system(CLEAR); /*Limpa a tela do usuario.*/

                /*Mensagem de fim de jogo.*/
                printf("\n\n\t\t\tGAME OVER!!\n\n");
                printf("\n\n");
                printf("Tempo decorrido: %1.f s", diff);
                if(diff>60){
                    printf("(OBS: Jogador excedeu o tempo limite!)");
                }
                printf("\n");
                printf("Jogadas restantes: %d", numJogadas);
                if(numJogadas<1){
                    printf("(OBS: Numero de jogadas excedido!)");
                }
                printf("\n");
                printf("Pontuacao Final: %d", *ptr_pont); /*Ponteiro de pontuacao final.*/
                if(*ptr_pont>MAXPTS){
                    printf("(OBS: A pontuacao maxima foi excedida!)");
                }
                printf("\n");
                printf("\n");
                if(modo==1){
                    registerMatch(binrank);
                }

                printf("Pressione 'ENTER' para retornar ao INICIO.\n");
                EsperaEnter();
                getchar();
                main();
                break;

            /*Configuracoes*/
            case 2:
                MENUconf();
                scanf("%d",&conf);

                switch(conf){
                    case 1:
                        confTabu();
                        FileSettings(arqconf);
                        getchar();
                        EsperaEnter();
                        main();
                        break;
                    case 2:
                        confNPC();
                        FileSettings(arqconf);
                        getchar();
                        EsperaEnter();
                        main();
                        break;
                    case 3:
                        settingsRank();
                        FileSettings(arqconf);
                        getchar();
                        EsperaEnter();
                        main();
                        break;
                    case 4:
                        EsperaEnter();
                        main();
                        break;
                    }
                break;

            /*Ranking*/
            case 3:
                system(CLEAR);

                if(jogo==0){
                    printf("\nNome      Colocacao  \n");
                    printf("\n-          -   \n");
                    printf("\nTecle <ENTER> para continuar!");
                }else{
                    mostrarRank(arqrank);
                }
                getchar();
                EsperaEnter();
                main();
                break;

            /*Regras do jogo*/
            case 4:
                regras(); /*Mostra ao jogador as regras do jogo, e em seguida retorna para a main.*/
                printf("Pressione 'ENTER' para retornar ao INICIO.\n");
                getchar();
                EsperaEnter();
                main();
                break;

            /*Exit Game*/
            case 5:
                system(CLEAR);
                getchar();
                printf("FIM DO JOGO.\n"); /*Mensagem de fim de jogo*/
                exit(1);
                break;

            /*Configuracao padrao, caso a opcao seja diferente das implementadas*/
            default:
                printf("Opcao invalida! "); /*Configuracao padrao caso seja digitada alguma tecla nao settada.*/
                printf("Pressione 'ENTER' para retornar ao INICIO.");
                getchar();
                EsperaEnter();
                main();
                break;
        }
    }while(opt>0 && opt<5); /*Switch funciona enquanto 'opt' estiver funcionando.*/

    return 0;
}
