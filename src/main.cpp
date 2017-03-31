/**
 * \file main.cpp
 * \brief Main file for the project "Space Invaders"
 * \author Luca Di Mauro
 */

#include "./SpaceInvaders.h"
#include "Components.h"
#include <cstdlib>
#include <iostream>
#include "bbb_joystick.cpp"
#include <thread>


/** \brief Function that print on stdscr start messge */
void startMessage ();

void game_loop(game_t * & game, char & r_direction, bool & photo, bool & button, bool & end) {

   while (!end)
   {
      // ..(1.a)..
      //direction= getch ();

	  if (r_direction == 'l') {
	    game->moveLeft (1);
	  }	else if (r_direction == 'r') {
	    game->moveRight (1);
	  }  
	
	  if (photo) 
	    game->shoot();
	
	  if (button)
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
      if ((rand() % 19813)==0)
	 // I create new enemy
	 game->addEnemy (((rand ())%(COLS)), 1, ((rand())%4));
      

      // ..(5).. print on stdscr and refresh
      game->print ();
      r_direction = 's';
      //direction=0;
      refresh ();
   }

}

/** \brief Main function of game */
int main ()
{
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

   std::thread t1 (bbb_joystick_read, ref(r_direction), ref(photo), ref(button));
   std::thread t2 (game_loop, ref(game), ref(r_direction), ref(photo), ref(button), ref(end));

   t1.join();
   t2.join();

   endwin ();
   return 0;
}
