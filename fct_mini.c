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
    int speed;

    message.client = bot->id;
    message.action = 2;
    speed = bot->speed;

    switch (bot->direction) {
        case 0:
            modif_axis = &(bot->pos.y);
            speed = (int) (-1 * speed * move)/fabs(move);
            break;
        case 1:
            modif_axis = &(bot->pos.x);
            speed = (int) (speed * move)/fabs(move);
            break;
        case 2:
            modif_axis = &(bot->pos.y);
            speed = (int) (speed * move)/fabs(move);
            break;
        case 3:
            modif_axis = &(bot->pos.x);
            speed = (int) (-1 * speed * move)/fabs(move);
            break;
    }
    for (float i = 0; i < (float) fabs(move)/bot->speed; i++) {
        last_pos = bot->pos;
        *modif_axis += speed;
        printf("bot pos : (%f,%f)\n",bot->pos.x,bot->pos.y);
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


int seek(robot *bot, char obj, char *axis, mqd_t server, mqd_t client, char* buffer, int taille){
    msg message;
    char concat_msg[sizeof(msg)+1];
    coord pos_object;
    int recep;

    message.client = bot->id;
    message.action = 6;

    str_concat(concat_msg,(char*) &message,sizeof(msg),&obj,1);
    // demande si un objet de type 'obj' est à porté du robot au serveur
    mq_send(server,concat_msg,sizeof(msg)+1,1);
    // attente de d'une reponse serveur
    recep = reception(client,&buffer,taille,bot,6);
    if (recep != 1) return recep;
    // reponse_serveur (x,y)
    pos_object = *((coord*) &(buffer[sizeof(msg)]));
    printf("l'objet %c est en (%f,%f)\n",obj,pos_object.x,pos_object.y );
    // reponse_serveur si rien (-1,-1)
    if (strcmp(axis, "x") == 0) {
        return pos_object.x;
    }else if (strcmp(axis, "y") == 0) {
        return pos_object.y;
    }
    return -1;
}

int rammasser(robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
    int recep;
    msg message;

    message.client = bot->id;
    message.action = 3;
    // demande a rammasser un objet de type 'obj' au serveur
    mq_send(server,(char*) &message,sizeof(msg),1);
    // attente de d'une reponse serveur
    recep = reception(client,&buffer,taille,bot,3);
    if (recep != 1) return recep;
    // reponse_serveur = reponse - 1
    if (buffer[sizeof(msg)] == 'C' ) {
        bot->inventory->money += *((int*) &(buffer[sizeof(msg)+1]));
    }
    if (buffer[sizeof(msg)] == 'A') {
        bot->inventory->armor += *((int*) &(buffer[sizeof(msg)+1]));
    }
    if (buffer[sizeof(msg)] == 'B') {
        bot->inventory->nb_bullet += *((int*) &(buffer[sizeof(msg)+1]));
    }
    return 1;
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
