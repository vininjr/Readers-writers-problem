#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define LEITORES    5
#define ESCRITORES    3

pthread_t escritor_thread[ESCRITORES];
pthread_t leitor_thread[LEITORES];

pthread_mutex_t BD;         //exclusao mutua para regioes criticas... controla o acesso ao banco de dados
pthread_mutex_t rc_control;      //exclusao mutua para regioes cri­ticas... controla o acesso a variável rc
int   rc;                   //quantidade de leitores lendo ou querendo ler a base de dados

char banco[500] = "";
char concat[5];

void read_BD(int i);
void use_BD(void);
void think_up_data(void);
void write_BD(int i);
void reader(void *arg);
void writer(void *arg);


void main()
{
    int i,j;

    pthread_mutex_init(&BD, NULL);    //ESTA CRIANDO O MUTEX PARA BD: MUTEX = EXCLUSÃO MUTUA.
    pthread_mutex_init(&rc_control, NULL); 

    for(i=0; i<ESCRITORES; i++)
    {
        pthread_create( &escritor_thread[i], NULL,(void *) writer,(void *) &i);
    }

    for(j=0; j<LEITORES; j++)
    {
        pthread_create( &leitor_thread[j], NULL,(void *) reader,(void *) &j);
    }

    for(i=0; i<ESCRITORES; i++)
    {
        pthread_join(escritor_thread[i], NULL);
    }

    for(i=0; i<LEITORES; i++)
    {
        pthread_join(leitor_thread[i], NULL);
    }


    exit(0);
}



void reader(void *j)
{
    int i = *(int*) j;
    while (1)
    {

        pthread_mutex_lock(&rc_control);           //down(&mutex); garante acesso exclusivo a variavel rc
        rc=rc+1;                              //um novo leitor

        if(rc==1)  pthread_mutex_lock(&BD);   //caso este seja o primeiro leitor...
        pthread_mutex_unlock(&rc_control);         //up(&mutex); libera o acesso a variavel rc

        read_BD(i);                     //le a base de dados
        pthread_mutex_lock(&rc_control);           //down(&mutex); garante acesso exclusivo a variavel rc
        rc=rc-1;                              //um leitor a menos...

        if(rc==0) pthread_mutex_unlock(&BD);  //caso este seja o ultimo leitor...
        pthread_mutex_unlock(&rc_control);         //up(&mutex); libera o acesso a variavel rc

        use_BD();                      //utiliza as informacoes da base de dados para algum trabalho...
    }
}

void writer(void *j)
{

    int i = *(int*) j;
    while(1)     

    {
        think_up_data();            //pensa em informações para adicionar a base de dados
        pthread_mutex_lock(&BD);    //down(&BD); garante acesso exclusivo a base de dados
        write_BD(i);          //escreve novas informacoes na base de dados

        pthread_mutex_unlock(&BD);  //up(&BD); libera o acesso a base de dados
    }
}

void read_BD(int i)
{
    int readingtime;
    readingtime = rand() % 3;

    printf("Leitor %d lendo banco de dados.\n",i);
    sleep(readingtime);
}
void use_BD()
{
    int usetime;
    usetime = rand() % 10;

    printf("Leitor utilizando conhecimento adquirido no banco de dados\n");
    sleep(usetime);
}
void think_up_data()
{

    int thinktime;
    thinktime = rand() % 10;

    concat[0] = ' ';
    concat[1] = 'H';
    concat[2] = 'U';
    concat[3] = 'E';
    concat[4] = ' ';

    printf("Escritor pensando no que irá escrever\n");
    sleep(thinktime);
}
void write_BD(int i)
{

    int writetime;
    writetime = rand() % 6;

    strcat(banco,concat);

    printf("Escritor %d escrevendo no banco de dados: %s\n",i,banco);
    sleep(writetime);
}
