/*
 Universidade de Brasilia
 Instituto de Ciencias Exatas
 Departamento de Ciencia da Computacao
 Algoritmos e Programação de Computadores – 2/2017
 Aluno(a): Ingrid C. G. S. Soares
 Matricula: 160125162
 Turma: A
 Versão do compilador: gcc (SUSE Linux) 4.8.5
 Descricao: Jogo em grid 2D (20x20) com jogador principal e personagens secundários, com suas respectivas funcoes
 (aleatorias ou nao), e sendo o objetivo principal o jogador conseguir o maior numero de pontos antes do tempo
 limite se esgotar. Nessa etapa do trabalho havera funcionamento apenas de 'jogar' e 'instrucoes'.
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h> 


#define MAXFRUIT 10 /*Valor maximo de frutas (ponto extra 'O') no jogo eh 10.*/
#define MAXINIM 10 /*Valor maximo de inimigos 'X' no jogo eh 10.*/
#define TAMY 20 /*Tamanho Y do grid bidimensional eh 20.*/
#define TAMX 20 /*Tamanho X do grid bidimensional eh 20.*/
#define MAXBOMBA 10 /*O valor maximo de bombas 'Q' eh 10.*/
#define MAXBARR 5 /*Valor maximo de barreiras (inimigo que deixa rastro 'B') eh 5.*/
#define MAXPTS 30 /*Valor maximo de pontos.*/

int numJogadas = 80; /*Quantidade de jogadas permitidas por jogo.*/


/*Estrututras de dados para guardar os Tipos dos Personagens, Tempo e Tabuleiro (Grid 2D)*/


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
    char tabuleiro[TAMX][TAMY];
}tabu;


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
    
    /*20 por cento de chance de bomba (random: 0-4=5)*/
    if(rand()%4 == 0){
        n->x= rand()%(TAMX-1);
        n->y= rand()%(TAMY -1);
        n->tempo = (rand() % 10);
        /*O tempo tambem e alternado, de acordo com o random.*/
    } 
    return n;
}

/*Coloca o personagem 'B' no plano, e funciona como uma barreira que por onde passa, deixa rastro.*/
barreira* iniciaBarreira(){
    int rx, ry;
    
    barreira* n = (barreira*)malloc(sizeof(barreira)); /*Reserva especifica de memoria. Depende do tamanho do ponteiro barreira.*/
    rx = (rand() %TAMX-3);
    ry = (rand() %TAMY-3);
    
    n->x = rx;
    n->y = ry;
    
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

/*Opcao 4 do menu para mostrar as instrucoes do jogo*/
void regras(){
    system("clear"); /*Limpa a tela do usuario.*/
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
    printf("\nPressione qualquer tecla para retornar ao MENU.\n");
}

/*char qualquerTecla(char q){
    fflush(stdin);
    *q=getchar();
    return *q;
}*/

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
    int botao;
    char q;
    /*Relogio * c;*/
    double diff=0;
    

    system("clear"); /*Limpa a tela do usuario.*/
    printf("\n");
    printf("\n");
    printf("\n");
    printf("*************************\n");
    printf("*************************\n");
    printf("(将棋 (SHOGI)\n"); /*Nome do jogo.*/
    printf("*************************\n");
    printf("*************************\n");
    printf("\n");
    printf("\n");
    printf("\n");
    
    printf("Pressione 'ENTER' para prosseguir.\n");
    EsperaEnter();

    Start: /*Goto Start;*/
    n_inim =0; /*Inicializa novamente dados pra evitar lixo de memoria.*/
    n_fruit=0;
    n_bomba=0;
    n_barreira=0;
    continua=0;
    pontuacao=0;
    diff=0;

    system("clear"); /*Limpa a tela do usuario.*/
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
        /*para linux:__fpurge(stdin); */
        scanf("%d",&opt); /*Pede ao jogador para digitar a opcao desejada*/
        
        Switches: /*Goto do case 2, 3 e default.*/
        switch(opt){
            system("clear");
            case 1: /*Quando o jogador pressiona '1', entra em case 1.*/

                system("clear"); /*Limpa a tela do usuario.*/
                setbuf(stdin, 0);

                time_t start,end;
                time (&start);
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
                    system("clear"); /*Limpa a tela do usuario.*/
                    printTabuleiro(t); /*Printa Tabuleiro*/
                }
                system("clear"); /*Limpa a tela do usuario.*/
                printf("\n\n\t\t\tGAME OVER!!\n\n"); /*Mensagem de fim de jogo.*/
                printf("\n\n");
                time (&end);
                diff = difftime (end,start);
                printf("Tempo decorrido: %1.f s\n", diff);
                printf("Jogadas restantes: %d\n", numJogadas);
                printf("Pontuacao Final: %d\n", *ptr_pont); /*Ponteiro de pontuacao final.*/ 
                setbuf(stdout, 0);
                printf("Pressione qualquer tecla para voltar ao MENU.\n");
                scanf(" %c",&q); /*Jogador pressiona tecla.*/
                if(q){
                    goto Start; /*MENU*/
                }else if(!q) {
                    printf("Opcao invalida.\n");
                    printf("Digite novamente: ");
                    scanf(" %c",&q);
                }
                break;
                
            case 2: /*settings();*/ /*Parte2 do trabalho!*/
                    printf("Sessao indisponivel.\n");
                    printf("Digite outra opcao: ");
                     break;
                    fflush(stdin);
                    scanf("%d",&opt);
                    goto Switches;

            case 3: /*ranking();*/ /*Parte2 do trabalho*/
                printf("Sessao indisponivel.\n");
                printf("Digite outra opcao: ");
                break;
                scanf("%d",&opt);
                goto Switches;
                
            case 4:
                regras(); /*Mostra ao jogador as regras do jogo, e em seguida retorna para a main.*/
                scanf(" %c",&q); /*Jogador pressiona tecla.*/
                if(q){
                    goto Start; /*Caso o jogador desejar jogar novamente*/
                }else if(!q) {
                    printf("Opcao invalida.\n");
                    printf("Digite novamente: ");
                    scanf("%d",&opt);
                }
                break;
                
            case 5: /*Opcao 'Sair' finaliza o programa.*/
                system("clear");
                printf("\n");
                printf("FIM DO JOGO.\n");
                printf("\n");
                return 0;
                break;
                
            default:
                printf("Opcao invalida.\n"); /*Configuracao padrao caso seja digitada alguma tecla nao settada.*/
                printf("Digite uma opcao valida: ");
                break;
                    scanf("%d",&opt);
                    goto Switches;
        }
    } while(opt); /*Switch funciona enquanto 'opt' estiver funcionando.*/
    return 0;    
}