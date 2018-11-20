#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>

coords get_coord(robot *bot){
  //float x = bot->pos->x
  return bot->pos;
}

short get_direction(robot *bot){
  return bot->direction;
}

short get_pv(robot *bot){
  return bot->pv;
}

unsigned long long get_money(robot *bot){
  return bot->inventory->money;
}

short get_nb_bullet(robot *bot){
  return bot->inventory->nb_bullet;
}

short get_armor(robot *bot){
  return bot->inventory->armor;
}

void avancer(robot *bot, int move, mqd_t serveur, mqd_t client) {
  short dir = get_direction(bot);
  coords coor = get_coord(bot);
  int bot_speed = bot->speed;
  int move_time = move/bot_speed;

  for (int i = 0; i < move_time; i++) {
      switch (dir) {
        case 1:
          coor.y += bot_speed;
          break;
        case 2:
          coor.x += bot_speed;
          break;
        case 3:
          coor.y -= bot_speed;
          break;
        case 4:
          coor.x -= bot_speed;
          break;
      }

      /* Envoi headers/requêtes
        mq_send(...)

        while(mq_recieve(...))


        */

  }
  /* while (bot->pos->x != coor.x && bot->pos->y != coor.y) {
    code
  }*/
}

void rammasser(robot *bot, char obj, mqd_t serveur, mqd_t client){
  // demande a rammasser un objet de type 'obj' au serveur
  // attente de d'une reponse serveur
  // reponse_serveur = reponse - 1
  switch (obj) {
    case "C":
      // bot->inventory->money += reponse_serveur;
      break;
    case "A":
      // bot->inventory->armor += reponse_serveur;
      break;
    case "B":
      // bot->inventory->nb_bullet += reponse_serveur;
      break;
  }
}

void touner(robot *bot, short direc, mqd_t serveur, mqd_t client){
  // demande de changement de direction vers 'direc' au serveur
}

void tirer(robot *bot, float angle, mqd_t serveur, mqd_t client){
  if (bot->inventory->nb_bullet > 0) {
    //demande de tir au reponse_serveur
    bot->inventory->nb_bullet -= 1;
  }
}



int main(int argc, char const *argv[]) {
  robot bot = {NULL,2,{0.0,0.0},1,100,42,350,3};

  printf("coord %f %f, direction %d, pv %d, money %d\n",get_coord(&bot).x,get_coord(&bot).y,get_direction(&bot),get_pv(&bot),get_money(&bot) );

  return 0;
}