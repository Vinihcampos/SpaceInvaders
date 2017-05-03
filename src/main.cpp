/**
 * \file main.cpp
 * \brief Main file for the project "Space Invaders"
 * \author Luca Di Mauro
 */

#include "./SpaceInvaders.h"
#include "Components.h"
#include <cstdlib>
#include <sstream>
#include <iostream>
#include "bbb_joystick.cpp"
#include <thread>
#include <pthread.h>
#include <netinet/in.h> //htons
#include <arpa/inet.h>  //inet_addr
#include <cstdio>       //printf
#include <sys/socket.h> //socket
#include <unistd.h>     //close
#include <cstring>      //memset

#define MAXMSG 1024
#define MAXNAME 100
#define PORTNUM 4325

/** \brief Function that print on stdscr start messge */
void startMessage ();

//void socketHandler(int socketDescriptor,Mensagem mensagem)
void socketHandler(int socketDescriptor, std::string mensagem, bool & controlChanged)
{
    int byteslidos;

    //Verificando erros
    if ( socketDescriptor == -1)
    {
        printf("Falha ao executar accept()");
        exit(EXIT_FAILURE);
    }

    //receber uma msg do cliente
    //printf("Servidor vai ficar esperando uma mensagem\n");
    byteslidos = recv(socketDescriptor,&mensagem,sizeof(mensagem),0);

    //printf("byteslidos: %d\n", byteslidos);

    if (byteslidos == -1)
    {
        printf("Falha ao executar recv()");
        //exit(EXIT_FAILURE);
    }
    else if (byteslidos == 0)
    {
        printf("Cliente finalizou a conexão\n");
        exit(EXIT_SUCCESS);
    }

	controlChanged = true;
    //printf("Servidor recebeu a seguinte msg do cliente [%s:%d]: %s \n",mensagem.nome,mensagem.idade,mensagem.msg);

    //close(socketDescriptor);
}

void game_loop(game_t * & game, char & r_direction, bool & photo, bool & button, bool & end) {
	  if (r_direction == 'l') {
	    game->moveLeft (1);
	  }	else if (r_direction == 'r') {
	    game->moveRight (1);
	  }  
	
	  if (photo && r_direction == 's') 
	    game->shoot();
	
	  if (button && !photo && r_direction == 's')
	    end=TRUE;

      // ..(2.a)..
      game->moveBullet ();
      // ..(2.b)..
      game->moveEnemies ();

      // ..(3.a)..
      // Check if there are collision between bullets and enemies and, possibly, manages them
      game->checkCollision ();

      // ..(3.b)..
      // Check if there are collision between enemies and cannon
      if (game->checkCannonCollision ())
      {
		 erase ();
		 mvprintw (20, 20, "************** SEI MORTO!!!!!! **************");
		 refresh ();
		 napms (3000);
		 end=TRUE;
      }
      
      // ..(4)..
      if ((rand() % 19813) < 400)
	 // I create new enemy
	 game->addEnemy (((rand ())%(COLS)), 1, ((rand())%4));

      // ..(5).. print on stdscr and refresh
      game->print ();
      refresh ();
}

/** \brief Main function of game */
int main ()
{

   //variáveis do servidor
   struct sockaddr_in endereco;
   int socketId;

   //variáveis relacionadas com as conexões clientes
   struct sockaddr_in enderecoCliente;
   socklen_t tamanhoEnderecoCliente = sizeof(struct sockaddr);
   int conexaoClienteId;

   //mensagem enviada pelo cliente
   //Mensagem mensagem;
   std::string mensagem;

   /*
    * Configurações do endereço
   */
   memset(&endereco, 0, sizeof(endereco));
   endereco.sin_family = AF_INET;
   endereco.sin_port = htons(PORTNUM);
   //endereco.sin_addr.s_addr = INADDR_ANY;
   endereco.sin_addr.s_addr = inet_addr("10.7.120.17");

   /*
    * Criando o Socket
    *
    * PARAM1: AF_INET ou AF_INET6 (IPV4 ou IPV6)
    * PARAM2: SOCK_STREAM ou SOCK_DGRAM
    * PARAM3: protocolo (IP, UDP, TCP, etc). Valor 0 escolhe automaticamente
   */
   socketId = socket(AF_INET, SOCK_STREAM, 0);

   //Verificar erros
   if (socketId == -1)
   {
       printf("Falha ao executar socket()\n");
       exit(EXIT_FAILURE);
   }

   printf("Aguardando conexão do cliente...\n");
   //Conectando o socket a uma porta. Executado apenas no lado servidor
   while ( bind (socketId, (struct sockaddr *)&endereco, sizeof(struct sockaddr)) == -1 )
   {
       //printf("Falha ao executar bind()\n");
       //exit(EXIT_FAILURE);
   }

   //Habilitando o servidor a receber conexoes do cliente
   if ( listen( socketId, 10 ) == -1)
   {
       printf("Falha ao executar listen()\n");
       exit(EXIT_FAILURE);
   }

   printf("Servidor: esperando conexões clientes\n");

   //Servidor fica bloqueado esperando uma conexão do cliente
   conexaoClienteId = accept( socketId,(struct sockaddr *) &enderecoCliente,&tamanhoEnderecoCliente );   

   printf("Servidor: recebeu conexão de %s, que comecem os jogos!\n", inet_ntoa(enderecoCliente.sin_addr));

   int direction;
   bool end= FALSE;

   initscr ();
   keypad (stdscr, TRUE);
   curs_set (0);
   noecho ();
   nodelay(stdscr,TRUE);
   srand (time(NULL));

   game_t *game= new game_t (COLS, LINES, COLS/2, LINES-1, 1);

   // Print start message
   move (0,0);
   game->startMessage ();
   while ((direction=getch ()) != (int)'s')
   {
      if (direction== (int) 'q')
	 {
	    end=TRUE;
	    break ;
	 }
   }

   move(0, 0);
   erase ();

   direction=0;

   // GAME LOOP:
   // (1.a)..GET A COMMAND______(1.b)..PARSE COMMAND______(2.a)..MOVE BULLIST______(2.b)..MOVE ENEMIES______(3.a-b)..CHECK COLLISION______(4)..GENERATE ENEMIES______(5)..PRINT
   
   char r_direction = 's';
   bool photo, button;

   bool controlChanged = true;
   while (!end) {
      if (controlChanged) {
	      // Update controls
		  std::stringstream s {mensagem};
		  // [dir]:char [photo]:bool [button]:bool 
		  std::string temp;
		  s >> temp;
		  r_direction = temp[0]; 
		  s >> temp;
		  photo = temp == "1";
		  s >> temp;
		  button = temp == "1";
		  //s >> temp;
		  //end = temp == "1";
		  controlChanged = false;
		  thread t(socketHandler,conexaoClienteId, ref(mensagem), ref(controlChanged));
		  t.detach();
	  }
	  //std::thread tpot(bbb_potentiometer, ref(r_direction));
      //tpot.join();
      //std::thread tldr(bbb_ldr, ref(photo), ref(r_direction));
      //tldr.join();
      //std::thread tbutton(bbb_button, ref(button), ref(r_direction), ref(photo));
      //tbutton.join();
      std::thread tgame(game_loop, ref(game), ref(r_direction), ref(photo), ref(button), ref(end));
      tgame.join();
   }

   endwin ();
   return 0;
}
