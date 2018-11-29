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

int not_equals(int a, int b){
  if (a==b)
    return 0;
  else
    return 1;
}

int equals(int a, int b){
  return !not_equals(a,b);
}

void mini_2_c(cmd sub_com, robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
  if(strcmp(sub_com.name, "move") == 0)
    avancer(bot,atoi(sub_com.subcom->name),server,client,buffer,taille);
  else if(strcmp(sub_com.name, "pick") == 0){
    //rammasser sub_com.subcom->name
  }
  else if(strcmp(sub_com.name, "turn") == 0){
    tourner(bot,atoi(sub_com.subcom->name),server);
  }
  else if(strcmp(sub_com.name, "shoot") == 0)
    tirer(bot,atoi(sub_com.subcom->name),server);
  else {
    //regarder (seek)
  }

}

/* évalue tous ce qui returne une valeur */
short eval(cmd sub_com, robot *bot){
  if(strcmp(sub_com.name, "pv") == 0)
    return get_pv(bot);

  else if(strcmp(sub_com.name, "steer") == 0)
    return get_direction(bot);

  else if(strcmp(sub_com.name, "money") == 0)
    return get_money(bot);

  else if(strcmp(sub_com.name, "nb_bullet") == 0)
    return get_nb_bullet(bot);

  else if(strcmp(sub_com.name, "armor") == 0)
    return get_armor(bot);

  else if(strcmp(sub_com.name, "coord") == 0)
    return get_coord(bot,sub_com.subcom->name);

  else if(strcmp(sub_com.name, "!=") == 0)
    return eval(sub_com.subcom[0],bot) != eval(sub_com.subcom[1],bot);

  else if(strcmp(sub_com.name, "==") == 0)
    return eval(sub_com.subcom[0],bot) == eval(sub_com.subcom[1],bot);

  else if(strcmp(sub_com.name, ">") == 0)
    return eval(sub_com.subcom[0],bot) > eval(sub_com.subcom[1],bot);

  else if(strcmp(sub_com.name, "<") == 0)
    return eval(sub_com.subcom[0],bot) < eval(sub_com.subcom[1],bot);

  else if(strcmp(sub_com.name, ">=") == 0)
    return eval(sub_com.subcom[0],bot) >= eval(sub_com.subcom[1],bot);

  else if(strcmp(sub_com.name, "<=") == 0){
    return eval(sub_com.subcom[0],bot) <= eval(sub_com.subcom[1],bot);
  }
  return atoi(sub_com.subcom->name);
}

void interp (cmd com, robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
  for (int i = 0; i < com.nb_subcom; ++i) {
    cmd sub_com = com.subcom[i+com.nb_args];
    if (sub_com.nb_subcom == 0){
      mini_2_c(sub_com,bot,server,client,buffer,taille);
    }
    else {
      if(strcmp(sub_com.name, "while") == 0){
        while (eval(sub_com.subcom[0],bot)) {
          for (int i = 1; i <= sub_com.nb_subcom; ++i) {
            interp(sub_com.subcom[i],bot,server,client,buffer,taille);
          }
        }
      }
    }

    //printf("name %s, nb_args %d, nb_subcom %d\n", com.name, com.nb_args, com.nb_subcom);
  }
}

void script(robot *bot, char *name, mqd_t server, mqd_t client, char* buffer, int taille){
  FILE *fd = fopen(name, "r");
  cmd com = create_cmd(NULL, fd);
  /*
  printf("name %s, nb_args %d, nb_subcom %d\n", com.name, com.nb_args, com.nb_subcom);
  for (int i = 0; i < com.nb_subcom; ++i) {
    printf("\t");
    glup(com.subcom[i+com.nb_args]);
  }
  */
  interp(com,bot,server,client,buffer,taille);
}















