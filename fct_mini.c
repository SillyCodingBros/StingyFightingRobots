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


int avancer(robot *bot, int move, mqd_t server, mqd_t client, char* buffer, int taille) {
    float* modif_axis;
    msg message;
    char concat_msg[sizeof(msg)+sizeof(coord)];
    int recep;
    coord last_pos;

    message.client = bot->id;
    message.action = 2;

    switch (bot->direction) {
        case 0:
            modif_axis = &(bot->pos.y);
            move *= -1;
        case 1:
            modif_axis = &(bot->pos.x);
        case 2:
            modif_axis = &(bot->pos.y);
        case 3:
            modif_axis = &(bot->pos.x);
            move *= -1;
    }
    for (float i = 0; i < (float) fabs(move)/bot->speed; i++) {
        last_pos = bot->pos;
        *modif_axis += bot->speed;
        str_concat(concat_msg,(char*) &message,sizeof(msg),(char*) &(bot->pos),sizeof(coord));
        mq_send(server,concat_msg,sizeof(msg)+sizeof(coord),1);
        recep = reception(client,&buffer,taille,bot,2);
        if (recep != 1) return recep;
        bot->pos = *((coord*) &(buffer[sizeof(msg)]));
        if (bot->pos.x == last_pos.x && bot->pos.y == last_pos.y) {
            return 0;
        }
        sleep(1);
    }
    return 0;
}


void seek(robot *bot, char obj, char *axis, mqd_t server, mqd_t client){
  // demande si un objet de type 'obj' est à porté du robot au serveur
  // attente de d'une reponse serveur
  // reponse_serveur (x,y)
  // reponse_serveur si rien (-1,-1)
  if (strcmp(&obj, "C") == 0) {
    //if (strcmp(axis,"x")==0)
      //reponse_serveur x
    //else if (strcmp(axis,"y")==0)
      //reponse_serveur y
  }
  if (strcmp(&obj, "A") == 0) {
    //if (strcmp(axis,"x")==0)
      //reponse_serveur x
    //else if (strcmp(axis,"y")==0)
      //reponse_serveur y
  }
  if (strcmp(&obj, "B") == 0) {
    //if (strcmp(axis,"x")==0)
      //reponse_serveur x
    //else if (strcmp(axis,"y")==0)
      //reponse_serveur y
  }
}

void rammasser(robot *bot, char obj, mqd_t server, mqd_t client){
  // demande a rammasser un objet de type 'obj' au serveur
  // attente de d'une reponse serveur
  // reponse_serveur = reponse - 1
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
    if (bot->direction < 0) {
        bot->direction = 4 + bot->direction;
    }
    msg message = {bot->id,4};
    char* tmp_msg = malloc(sizeof(msg) + sizeof(char));
    str_concat(tmp_msg, (char*) &message, sizeof(msg), &(bot->direction), sizeof(char));
    mq_send(server,tmp_msg,sizeof(msg)+sizeof(char),1);
    free(tmp_msg);
}

void tirer(robot *bot, float angle, mqd_t server){
    printf("demande de tir\n");
    if (bot->inventory->nb_bullet > 0) {
        bot->inventory->nb_bullet -= 1;
        coord speed = { (float) cos((angle+bot->direction*90-90)*RAD)/10000, (float) sin((angle+bot->direction*90-90)*RAD)/10000};
        printf("speed %f , %f\n", speed.x,speed.y);
        msg message = {bot->id,5};
        char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
        str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &speed, sizeof(coord));
        mq_send(server, tmp_msg, sizeof(msg)+sizeof(coord), 1);
    }
}
