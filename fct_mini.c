#include "game.h"

float get_coord(robot *bot, char *axis){
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
    float* modif_axis, speed;
    msg message;
    char concat_msg[sizeof(msg)+sizeof(coord)];
    int recep, nsec, cycle, distance;
    coord last_pos, init_pos;

    message.client = bot->id;
    message.action = 2;
    cycle = CYCLE*100;
    speed = ((float) bot->speed) / cycle;
    nsec = 0;

    switch (bot->direction) {
      case 0:
        modif_axis = &(bot->pos.y);
        speed = (-1 * speed * move)/fabs(move);
        distance = fabs((bot->pos.y-move)-bot->pos.y);
        break;
      case 1:
        modif_axis = &(bot->pos.x);
        speed = (speed * move)/fabs(move);
        distance = fabs((bot->pos.y+move)-bot->pos.y);
        break;
      case 2:
        modif_axis = &(bot->pos.y);
        speed = (speed * move)/fabs(move);
        distance = fabs((bot->pos.y+move)-bot->pos.y);
        break;
      case 3:
        modif_axis = &(bot->pos.x);
        speed = (-1 * speed * move)/fabs(move);
        distance = fabs((bot->pos.x-move)-bot->pos.x);
        move *= -1;
        break;
    }

    init_pos = bot->pos;
    while(sqrt(pow((bot->pos.x-init_pos.x),2)+pow((bot->pos.y-init_pos.y),2)) < distance) {
        last_pos = bot->pos;
        *modif_axis += speed;
        //printf("(%f,%f)\n", bot->pos.x,bot->pos.y);
        if (nsec == CYCLE) {
            str_concat(concat_msg,(char*) &message,sizeof(msg),(char*) &(bot->pos),sizeof(coord));
            mq_send(server,concat_msg,sizeof(msg)+sizeof(coord),1);
            recep = reception(client,&buffer,taille,bot,2);
            if (recep != 1) return recep;
            bot->pos = *((coord*) &(buffer[sizeof(msg)]));
            if (bot->pos.x == last_pos.x && bot->pos.y == last_pos.y) {
              return -1;
            }
            nsec = 0;
        }
        nsec++;
    }

    str_concat(concat_msg,(char*) &message,sizeof(msg),(char*) &(bot->pos),sizeof(coord));
    mq_send(server,concat_msg,sizeof(msg)+sizeof(coord),1);
    recep = reception(client,&buffer,taille,bot,2);
    if (recep != 1) return recep;
    bot->pos = *((coord*) &(buffer[sizeof(msg)]));
    return 0;
}


int seek(robot *bot, char *obj, char *axis, mqd_t server, mqd_t client, char* buffer, int taille){
    msg message;
    char concat_msg[sizeof(msg)+1];
    coord pos_object;
    int recep;

    message.client = bot->id;
    message.action = 6;

    str_concat(concat_msg,(char*) &message,sizeof(msg),obj,1);
    // demande si un objet de type 'obj' est à porté du robot au serveur
    mq_send(server,concat_msg,sizeof(msg)+1,1);
    // attente de d'une reponse serveur
    recep = reception(client,&buffer,taille,bot,6);
    if (recep != 1) return recep;
    // reponse_serveur (x,y)
    pos_object = *((coord*) &(buffer[sizeof(msg)]));
    // reponse_serveur si rien (-1,-1)
    if (strcmp(axis, "x") == 0) {
        //printf("axis : %s\n",axis);
        return pos_object.x;
    }else if (strcmp(axis, "y") == 0) {
        //printf("axis : %s\n",axis);
        return pos_object.y;
    }
    return 0;
}

int ramasser(robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
  int recep;
  msg message;

  message.client = bot->id;
  message.action = 3;
  // demande a ramasser un objet de type 'obj' au serveur
  mq_send(server,(char*) &message,sizeof(msg),1);
  // attente de d'une reponse serveur
  recep = reception(client,&buffer,taille,bot,3);
  if (recep != 1)
    return recep;
  // reponse_serveur = reponse - 1
  if (buffer[sizeof(msg)] == 'C' ) {
    bot->inventory->money += *((int*) &(buffer[sizeof(msg)+1]));
    return 0;
  }
  if (buffer[sizeof(msg)] == 'A') {
    bot->inventory->armor += *((int*) &(buffer[sizeof(msg)+1]));
    return 0;
  }
  if (buffer[sizeof(msg)] == 'B') {
    bot->inventory->nb_bullet += *((int*) &(buffer[sizeof(msg)+1]));
    return 0;
  }
  return -1;
}

int tourner(robot *bot, short direc, mqd_t server){
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
  return 0;
}

int tirer(robot *bot, float angle, mqd_t server){
    int cycle = CYCLE*100;
    printf("demande de tir\n");
    if (bot->inventory->nb_bullet > 0) {
        bot->inventory->nb_bullet -= 1;
        coord speed = { (float) cos((angle+bot->direction*90-90)*RAD)/cycle*bot->speed_bullet, (float) sin((angle+bot->direction*90-90)*RAD)/cycle*bot->speed_bullet};
        printf("speed %f , %f\n", speed.x,speed.y);
        msg message = {bot->id,5};
        char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
        str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &speed, sizeof(coord));
        mq_send(server, tmp_msg, sizeof(msg)+sizeof(coord), 1);
        return 0;
    }
    return -1;
}

/* évalue tous ce qui returne une valeur */
int eval(cmd sub_com, robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
    if (strcmp(sub_com.name, "quit") == 0) {
      msg message = {bot->id,1};
      mq_send(server, (char*) &message, sizeof(msg), 1);
      bot->pv = 0;
    }
    else if(strcmp(sub_com.name, "move") == 0)
      return avancer(bot,eval(*sub_com.subcom, bot,server,client,buffer,taille),server,client,buffer,taille);

    else if(strcmp(sub_com.name, "pick") == 0)
      return ramasser(bot, server, client, buffer, taille);

    else if(strcmp(sub_com.name, "turn") == 0)
      return tourner(bot,eval(*sub_com.subcom, bot,server,client,buffer,taille),server);

    else if(strcmp(sub_com.name, "shoot") == 0)
      return tirer(bot,eval(*sub_com.subcom, bot,server,client,buffer,taille),server);

    else if(strcmp(sub_com.name, "seek") == 0)
      return seek(bot, sub_com.subcom[0].name, sub_com.subcom[1].name, server, client, buffer, taille);

    else if(strcmp(sub_com.name, "pv") == 0)
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
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) != eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "==") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) == eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, ">") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) > eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "<") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) < eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, ">=") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) >= eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "<=") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) <= eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "+") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) + eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "-") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) - eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "*") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) * eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "/") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) / eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    else if(strcmp(sub_com.name, "mod") == 0)
      return eval(sub_com.subcom[0],bot,server,client,buffer,taille) % eval(sub_com.subcom[1],bot,server,client,buffer,taille);

    return atoi(sub_com.name);
    //return -1;
}

int interp(cmd sub_com, robot *bot, mqd_t server, mqd_t client, char* buffer, int taille){
  if (sub_com.nb_subcom > 0 && sub_com.nb_args == 0) {
    if (strcmp(sub_com.name, "script") == 0) {
      printf("je suis un script\n");
      for (int i = 0; i < sub_com.nb_subcom+sub_com.nb_args; ++i) {
        interp(sub_com.subcom[i],bot,server,client,buffer,taille);
      }
    }
  }
  if (sub_com.nb_subcom == 0){
    return eval(sub_com,bot,server,client,buffer,taille);
  }
  else {
    if(strcmp(sub_com.name, "while") == 0){
      printf("c'est un while\n");
      while (eval(sub_com.subcom[0],bot,server,client,buffer,taille)) {
        for (int i = 1; i <= sub_com.nb_subcom; ++i) {
          interp(sub_com.subcom[i],bot,server,client,buffer,taille);
        }
      }
    }
    if(strcmp(sub_com.name, "if") == 0){
      printf("c'est un if\n");
      if (eval(sub_com.subcom[0],bot,server,client,buffer,taille)) {
        for (int i = 1; i <= sub_com.nb_subcom; ++i) {
          interp(sub_com.subcom[i],bot,server,client,buffer,taille);
        }
      }
    }
  }
  return -1;
}

void script(robot *bot, char *name, mqd_t server, mqd_t client, char* buffer, int taille){
  FILE *fd = fopen(name, "r");
  cmd com = create_cmd(NULL, fd);
  interp(com,bot,server,client,buffer,taille);
  fclose(fd);
}
