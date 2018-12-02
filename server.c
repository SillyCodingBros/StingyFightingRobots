#include "game.h"

int main(int argc, char *argv[]) {
    //verification des arguments pour eviter les erreur
    if(argc != 2){
      fprintf(stderr, "usage: %s nom_map\n", argv[0]);
      return 1;
    }

    if(server(argv[1])){
        fprintf(stderr, "server : exit failure\n");
        return 1;
    }

    return 0;
}

int server(char* map_name){
    map mapOfGame;
    robot_liste listOfBot;
    bullet_liste listOfBullet;
    mqd_t server, new_Client, *mq_list;
    int taille, nsec, mvp, add, nbclient, done, time_set;
    char* buffer, *place, *concat_msg;
    msg demande;
    robot* cur_bot;
    coord new_pos;
    struct mq_attr attr;
    struct timespec tw, t1, t2;

    if (create_map(map_name,&mapOfGame)) return EXIT_FAILURE;
    server = mq_open("/server", O_RDONLY, 0600, NULL);
    if (server != -1){
        close(server);
        mq_unlink("/server");
    }
    new_Client = mq_open("/new_Client", O_RDONLY, 0600, NULL);
    if (new_Client != -1) {
        close(new_Client);
        mq_unlink("new_Client");
    }
    server = mq_open("/server", O_RDONLY | O_CREAT, 0600, NULL);
    new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);
    if (mq_getattr(server,&attr)) {
        perror("mq_getattr");
        return EXIT_FAILURE;
    }
    taille = attr.mq_msgsize;
    buffer = malloc(taille);
    listOfBot = NULL;
    listOfBullet = NULL;
    mq_list = malloc(mapOfGame.nbSpawn * sizeof(mqd_t));
    place = calloc(0,mapOfGame.nbSpawn);
    concat_msg = malloc(taille);
    nbclient = nsec = time_set = 0;
    mvp = -1;
    done = 1;
    clock_gettime(CLOCK_REALTIME,&tw);
    while (done) {
      nsec++;
      if (nbclient == 0) {
        clock_gettime(CLOCK_REALTIME,&t1);
        if (!time_set) {
          t2 = t1;
          t2.tv_sec += 15;
          time_set++;
        }else {
          if (t1.tv_sec > t2.tv_sec) {
            done = 0;
          }
        }
      }else time_set = 0;
      if (mq_timedreceive(server,buffer,taille,NULL,&tw) > 0) {
        demande = *((msg*) buffer);
        //printf("{%d,%d}\n",demande.client,demande.action);
        cur_bot = search_robot(demande.client,listOfBot);
        if (cur_bot == NULL && demande.action == -1) {
          add = search_place(place,mapOfGame.nbSpawn);
          if (add == -1) {
            mq_send(new_Client,(char*) &demande, sizeof(msg), 1);
          }else{
            nbclient++;
            char* id = calloc(0,((int) (mapOfGame.nbSpawn/10))+1);
            id[0] = '/';
            sprintf(id+1,"%d",add);
            mq_list[add] = mq_open(id,O_WRONLY,0600,NULL);
            if (mq_list[add] != -1) {
              close(mq_list[add]);
              mq_unlink(id);
            }
            mq_list[add] = mq_open(id,O_WRONLY | O_CREAT,0600,NULL);
            cur_bot = malloc(sizeof(robot));
            *cur_bot = create_robot(buffer+sizeof(msg),add,mapOfGame.spawn[add],NULL);
            add_bot(*cur_bot,&listOfBot);
            demande.client = add;
            demande.action = 1;
            str_concat(concat_msg,(char*) &demande,sizeof(msg),(char*) &(search_robot(add,listOfBot)->pos),sizeof(coord));
            if (mq_send(new_Client,concat_msg,sizeof(msg)+sizeof(coord),1) < 0) {
              perror("mq_send");
              return 1;
            }
            place[add] = 1;
            free(id);
            if (nbclient == 2) {
              start(mq_list);
            }
          }
        }else{
          if (demande.action == 1) {
            printf("suppr\n");
            mq_close(mq_list[(int) demande.client]);
            suppr_bot(demande.client,&listOfBot);
            place[(int) demande.client] = 0;
            nbclient -=1;
            if (nbclient == 1) {
              demande.client = listOfBot->element.id;
              demande.action = -1;
              mq_send(mq_list[(int) demande.client],(char*) &demande,sizeof(msg),1);
            }
          }else if (nbclient == 1) {
            demande.client = listOfBot->element.id;
            demande.action = -1;
            mq_send(mq_list[(int) demande.client],(char*) &demande,sizeof(msg),1);
          }else if (demande.action == 2) {
            new_pos = *((coord*) &(buffer[sizeof(msg)]));
            if (mapOfGame.map[((int) (new_pos.y+0.5))*mapOfGame.width+((int) (new_pos.x+0.5))] != 'W') {
              cur_bot->pos = new_pos;
            }
            str_concat(concat_msg,(char*) &demande,sizeof(msg),(char*) &(cur_bot->pos),sizeof(coord));
            mq_send(mq_list[(int) demande.client],concat_msg,sizeof(msg)+sizeof(coord),1);
          }else if (demande.action == 3) {
            new_pos = bot_interact(mapOfGame,listOfBot,cur_bot,NULL);
            char* tmp_msg = malloc(sizeof(msg)+1);
            add = (int)(rand() / (double)RAND_MAX * (10 - 1));
            str_concat(tmp_msg,(char*) &demande,sizeof(msg),&mapOfGame.map[((int) new_pos.y)*mapOfGame.width+((int) new_pos.x)],1);
            str_concat(concat_msg,tmp_msg,sizeof(msg)+1,(char*) &add,sizeof(int));
            mq_send(mq_list[(int) demande.client],concat_msg,sizeof(msg)+1+sizeof(int),1);
            mapOfGame.map[((int) new_pos.y)*mapOfGame.width+((int) new_pos.x)] = ' ';
          }else if (demande.action == 4) {
            cur_bot->direction = buffer[sizeof(msg)];
          }else if (demande.action == 5) {
            add_bullet(create_bullet(cur_bot,((coord*) &(buffer[sizeof(msg)]))->x,((coord*) &(buffer[sizeof(msg)]))->y),&listOfBullet);
          }else if (demande.action == 6) {
            new_pos = bot_interact(mapOfGame,listOfBot,cur_bot,buffer);
            str_concat(concat_msg,(char*) &demande,sizeof(msg),(char*) &new_pos,sizeof(coord));
            mq_send(mq_list[(int) demande.client],concat_msg,sizeof(msg)+sizeof(coord),1);
          }
        }
      }
      move_bullet(&listOfBullet,&listOfBot,mapOfGame,mq_list);
      if (nbclient > 1) {
        mvp = win(listOfBot);
      }
      if (mvp != -1) {
        done = 0;
        demande.client = mvp;
        demande.action = 0;
        printf("{%d,%d}\n",demande.client,demande.action);
        if (mq_send(mq_list[mvp],(char*) &demande,sizeof(msg),1) < 0) perror("mq_send");
        cur_bot = search_robot(mvp,listOfBot);
        printf("%s A GAGNER\n", cur_bot->name);
      }
      if (nsec > CYCLE && nbclient > 1) {
        printf("\f");
        affichage(mapOfGame,listOfBot,listOfBullet);
        //test(listOfBot);
        //test2(listOfBullet);
        nsec = 0;
      }
    }
    mq_unlink("/server");
    mq_unlink("new_Client");
    return 0;
}

//fonction d'interaction entre la map et les joueurs (sert a detecter un objet pour pick et seek)
coord bot_interact(map mapOfGame, robot_liste listOfBot, robot* bot, char* buffer){
  coord pos_object;
  robot* tmp_bot;
  int range;

  pos_object = bot->pos;
  if (buffer == NULL)
    range = bot->pick;
  else
    range = bot->reach;
  for (int r = 0; r < range; r++) {
    for (int i = bot->pos.y-r; i < bot->pos.y+r; i++) {
      for (int j = bot->pos.x-r; j < bot->pos.x+r; j++) {
        if (buffer == NULL) {
          if (search("ABC",mapOfGame.map[i*mapOfGame.width+j]) == 0) {
            pos_object.x = j;
            pos_object.y = i;
            return pos_object;
          }
        }else if (buffer[sizeof(msg)] == 'R') {
          tmp_bot = isBot(j,i,listOfBot);
          if (tmp_bot != NULL && (tmp_bot->pos.x != bot->pos.x || tmp_bot->pos.y != bot->pos.y)) {
            return tmp_bot->pos;
          }
        }else if (mapOfGame.map[i*mapOfGame.width+j] == buffer[sizeof(msg)]) {
          pos_object.x = j;
          pos_object.y = i;
          return pos_object;
        }
      }
    }
  }
  return pos_object;
}

//fonction test les robots
void test(robot_liste test){
    robot_liste print = test;
    while (print != NULL) {
        printf(" name %s, pos (%f,%f), pv %d,addr %p --->\n",print->element.name, print->element.pos.x, print->element.pos.y, print->element.pv, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

//fonction test les bullet
void test2(bullet_liste test){
    bullet_liste print = test;
    while (print != NULL) {
        printf(" pos (%f,%f), speed : (%f,%f) addr %p --->",print->element.pos.x, print->element.pos.y, print->element.speed_x, print->element.speed_y, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

//fonction start
void start(mqd_t* mq_list) {
  int sec;
  msg message;
  char concat_msg[sizeof(msg)+sizeof(int)];

  sec = 5;
  message.action = 4;
  while (sec >= 0) {
    message.client = 0;
    str_concat(concat_msg,(char*) &message,sizeof(msg),(char*) &sec,sizeof(int));
    mq_send(mq_list[(int) message.client],concat_msg,sizeof(msg)+sizeof(int),1);
    message.client = 1;
    str_concat(concat_msg,(char*) &message,sizeof(msg),(char*) &sec,sizeof(int));
    mq_send(mq_list[(int) message.client],concat_msg,sizeof(msg)+sizeof(int),1);
    sleep(1);
    sec--;
  }
}

//fonction de deplacement des bullets
void move_bullet(bullet_liste* list_bullet, robot_liste* bot_list, map mapOfGame, mqd_t* mq_list){
    bullet_liste tmp_list;
    coord tmp_coord;
    robot* tmp_bot;
    msg message;

    tmp_list = *list_bullet;
    while (tmp_list) {
        tmp_coord.x = tmp_list->element.pos.x + tmp_list->element.speed_x;
        tmp_coord.y = tmp_list->element.pos.y + tmp_list->element.speed_y;
        tmp_bot = isBot((int) tmp_coord.x, (int) tmp_coord.y, *bot_list);
        if(tmp_bot){
            tmp_bot->pv -= tmp_list->element.damage;
            tmp_bot->reach = (int) (tmp_bot->reach * tmp_bot->pv/100);
            tmp_bot->speed = (int) (tmp_bot->speed * tmp_bot->pv/100);
            suppr_bullet(tmp_list->element,list_bullet);
            message.client = -1;
            message.action = tmp_list->element.damage;
            mq_send(mq_list[(int) tmp_bot->id],(char*) &message,sizeof(msg),1);
        }else if (search("wW",mapOfGame.map[((int) tmp_coord.y)*mapOfGame.width+((int) tmp_coord.x)]) == 0) {
            suppr_bullet(tmp_list->element,list_bullet);
        }else{
            tmp_list->element.pos = tmp_coord;
        }
        tmp_list = tmp_list->suite;
    }
}

//fonction d'affichage printf()
void affichage(map mapOfGame, robot_liste listOfBot, bullet_liste listOfBullet){
  coord test_range;
  for (int y = 0; y < mapOfGame.height; y++) {
    for (int x = 0; x < mapOfGame.width; x++) {
      robot* tmp = isBot(x, y, listOfBot);
      if (tmp != NULL) {
          if (tmp->pv < 0) {
            printf("O");
          }else{
            switch (tmp->direction) {
              case 0:
                printf("^");
                break;
              case 1:
                printf(">");
                break;
              case 2:
                printf("v");
                break;
              case 3:
                printf("<");
                break;
            }
          }
      }else if (isBullet(x,y,listOfBullet)) {
        printf("*");
      }else{
        test_range.x = (float) x;
        test_range.y = (float) y;
        if (in_range(test_range,listOfBot)==0) {
          printf("?");
        }else{
          printf("%c", mapOfGame.map[y*mapOfGame.width+x]);
        }
      }
    }
    printf("\n");
  }
}

//fonction qui test si un robot est present au coordonnées x, y
robot* isBot(int x, int y, robot_liste listOfBot){
    robot_liste tmp_list = listOfBot;
    while (tmp_list != NULL) {
        if ( (int) (tmp_list->element.pos.x+0.5) == x && (int) (tmp_list->element.pos.y+0.5) == y ) return &(tmp_list->element);
        tmp_list = tmp_list->suite;
    }
    return NULL;
}

//fonction qui test si il y a une balle au coordonnées x,y;
int isBullet(int x, int y, bullet_liste listOfBullet){
    bullet_liste tmp_list = listOfBullet;
    while (tmp_list != NULL) {
        if ( (int) (tmp_list->element.pos.x+0.5) == x && (int) (tmp_list->element.pos.y+0.5) == y ) return 1;
        tmp_list = tmp_list->suite;
    }
    return 0;
}

//fonction pour savoir si le point est dans le range d'un bot
int in_range(coord pos,robot_liste listOfBot){
  int distance;
  while(listOfBot){
    distance = sqrt(pow(listOfBot->element.pos.x-pos.x,2)+pow(listOfBot->element.pos.y-pos.y,2));
    if (distance < listOfBot->element.reach) return 1;
    listOfBot = listOfBot->suite;
  }
  return 0;
}

//fonction pour detecter quelle joueur a gagner
int win(robot_liste listOfBot){
    int winner = -1;
    while (listOfBot != NULL) {
        if (winner == -1 && listOfBot->element.pv > 0) {
            winner = listOfBot->element.id;
        }else if (listOfBot->element.pv > 0) {
            winner = -1;
            break;
        }
        listOfBot = listOfBot->suite;
    }
    return winner;
}

int search_place(char* place,int nb_place){
    for (int i = 0; i < nb_place; i++) {
        if (place[i] == 0) return i;
    }
    return -1;
}


//fonction de creation de la map
int create_map(char* path_file, map* new_map){
    char c;
    FILE* f = NULL;
    f = fopen(path_file, "r");
    if(f == NULL) return -1;

    //trouver la taille de la map
    fseek(f,0,SEEK_SET);
    int tmp = 0;
    do {
        c = fgetc(f);
        tmp++;
    } while(c != 10);
    new_map->width = tmp;
    fseek(f,0,SEEK_END);
    new_map->height = ftell(f) / new_map->width;

    //allouer la place pour tableau
    new_map->map = malloc(new_map->width * new_map->height * sizeof(char));

    //remplir le tableau avec le file
    int i = 0, j = 0;
    new_map->spawn = malloc(sizeof(coord));
    new_map->nbSpawn = 0;
    fseek(f,0,SEEK_SET);
    c = fgetc(f);
    while(c != EOF){
        if(c == 10){
            i++;
            j=0;
        }else{
            if(c == 'S'){
                coord tmp = {j,i};
                new_map->spawn = realloc(new_map->spawn,(new_map->nbSpawn+1)*sizeof(coord));
                new_map->spawn[new_map->nbSpawn] = tmp;
                new_map->nbSpawn++;
                c = ' ';
            }
            new_map->map[(i * new_map->width) + j] = c;
            j++;
        }
        c = fgetc(f);
    }
    if(fclose(f)) return -1;
    return 0;
}
