#include "game.h"

float get_coord(robot *bot, char *axis){
  //float x = bot->pos->x
  if (strcmp(axis,"x")==0)
    return bot->pos.x;
  else if (strcmp(axis,"y")==0)
    return bot->pos.y;
  return 0;
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

void start(robot* bot, mqd_t server){
    msg message = {bot->id,1};
    mq_send(server, (char*) &message, sizeof(msg), 1);
}

void avancer(robot *bot, int move, mqd_t server, mqd_t client, char* buffer, int taille) {
    short dir = get_direction(bot);
    coord coor = {get_coord(bot,"x"),get_coord(bot,"y")};
    int bot_speed = bot->speed;
    int move_time = (int) (move/bot_speed);
    printf("%d / %d = %d\n", move, bot_speed, move_time);
    msg message = {bot->id,2};

    for (int i = 0; i < move_time; i++) {
        switch (dir) {
            case 0:
                coor.y -= bot_speed;
                break;
            case 1:
                coor.x += bot_speed;
                break;
            case 2:
                coor.y += bot_speed;
                break;
            case 3:
                coor.x -= bot_speed;
                break;
        }
        char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
        str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &coor, sizeof(coord));
        mq_send(server, tmp_msg, sizeof(msg)+sizeof(coord), 1);
        free(tmp_msg);
        mq_receive(client,buffer,taille,NULL);
        while (((msg*) buffer)->action != 2) {
            if (((msg*) buffer)->action == 0) {
                bot->pv = 0;
            }else if (((msg*) buffer)->action == 1) {
                bot->pv = *((int*) &buffer[sizeof(msg)]);
            }
            mq_receive(client,buffer,taille,NULL);
        }
        if ( ((coord*) &buffer[sizeof(msg)])->x == bot->pos.x && ((coord*) &buffer[sizeof(msg)])->y == bot->pos.y) {
            return;
        }else{
            bot->pos = *((coord*) &buffer[sizeof(msg)]);
        }
        sleep(1);
    }
}

void seek(robot *bot, char obj, char axis, mqd_t server, mqd_t client){
  // demande si un objet de type 'obj' est à porté du robot au serveur
  // attente de d'une reponse serveur
  // reponse_serveur
  if (strcmp(&obj, "C") == 0) {
      // return reponse_serveur;
      // si x ou si y
  }
  if (strcmp(&obj, "A") == 0) {
        // return reponse_serveur;
        // si x ou si y
  }
  if (strcmp(&obj, "B") == 0) {
        // return reponse_serveur;
        // si x ou si y
  }
}

void rammasser(robot *bot, char obj, mqd_t server, mqd_t client){
  // demande a rammasser un objet de type 'obj' au serveur
  // attente de d'une reponse serveur
  // reponse_serveur
  // reponse_serveur si rien -1,-1
  if (strcmp(&obj, "C") == 0) {
      // bot->inventory->money += reponse_serveur;
  }
  if (strcmp(&obj, "A") == 0) {
      // bot->inventory->money += reponse_serveur;
  }
  if (strcmp(&obj, "B") == 0) {
      // bot->inventory->money += reponse_serveur;
  }
}

void tourner(robot *bot, short direc, mqd_t server){
    // informe le server du changement de direction
    bot->direction = (bot->direction + direc) % 4;
    msg message = {bot->id,3};
    char* tmp_msg = malloc(sizeof(msg) + sizeof(char));
    str_concat(tmp_msg, (char*) &message, sizeof(msg), &(bot->direction), sizeof(char));
    mq_send(server,tmp_msg,sizeof(msg)+sizeof(char),1);
    free(tmp_msg);
}

void tirer(robot *bot, float angle, mqd_t server){
    printf("demande de tir\n");
    if (bot->inventory->nb_bullet > 0) {
        bot->inventory->nb_bullet -= 1;
        coord speed = { (float) cos(angle*RAD)/10000, (float) sin(angle*RAD)/10000};
        printf("speed %f , %f\n", speed.x,speed.y);
        msg message = {bot->id,5};
        char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
        str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &speed, sizeof(coord));
        mq_send(server, tmp_msg, sizeof(msg)+sizeof(coord), 1);
    }
}
