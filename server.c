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
    int taille, nsec, mvp, add, nbclient;
    char* buffer, *place, *concat_msg;
    msg demande;
    robot* cur_bot;
    coord new_pos;
    struct mq_attr attr;
    struct timespec tw;

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
    nbclient = 0;
    nsec = 0;
    mvp = -1;
    clock_gettime(CLOCK_REALTIME,&tw);
    while (mvp == -1) {
        nsec++;
        if (mq_timedreceive(server,buffer,taille,NULL,&tw) > 0) {
            demande = *((msg*) buffer);
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
                }
            }else{
                if (demande.action == 1) {
                    printf("suppr\n");
                    mq_close(mq_list[(int) demande.client]);
                    suppr_bot(demande.client,&listOfBot);
                    place[(int) demande.client] = 0;
                    if (nbclient == 2) {
                        nbclient++;
                    }
                    nbclient -=1;
                }else if (demande.action == 2) {
                    printf("move\n");
                    new_pos = *((coord*) &(buffer[sizeof(msg)]));
                    if (mapOfGame.map[((int) new_pos.y)*mapOfGame.width+((int) new_pos.x)] != 'w') {
                        cur_bot->pos = new_pos;
                    }
                    str_concat(concat_msg,(char*) &demande,sizeof(msg),(char*) &(cur_bot->pos),sizeof(coord));
                    mq_send(mq_list[(int) demande.client],concat_msg,sizeof(msg)+sizeof(coord),1);
                }else if (demande.action == 3) {
                    printf("ramasser\n");
                    demande.action = mapOfGame.map[((int) cur_bot->pos.y)*mapOfGame.width+((int) cur_bot->pos.x)];
                    mq_send(mq_list[(int) demande.client],(char*) &demande,sizeof(msg),1);
                }else if (demande.action == 4) {
                    printf("tourner\n");
                    cur_bot->direction = buffer[sizeof(msg)];
                }else if (demande.action == 5) {
                    printf("tirer\n");
                    add_bullet(create_bullet(cur_bot,((coord*) &(buffer[sizeof(msg)]))->x,((coord*) &(buffer[sizeof(msg)]))->y),&listOfBullet);
                }
            }
        }
        move_bullet(&listOfBullet,&listOfBot,mapOfGame,mq_list);
        if (nbclient > 1) {
            mvp = win(listOfBot);
        }
        if (nsec > 10000) {
            affichage(mapOfGame,listOfBot,listOfBullet);
            test(listOfBot);
            test2(listOfBullet);
            nsec = 0;
        }
    }
    demande.client = mvp;
    demande.action = 0;
    printf("{%d,%d}\n",demande.client,demande.action);
    if (mq_send(mq_list[mvp],(char*) &demande,sizeof(msg),1) < 0) perror("mq_send");
    cur_bot = search_robot(mvp,listOfBot);
    printf("%s A GAGNER\n", cur_bot->name);
    mq_unlink("/server");
    mq_unlink("new_Client");
    return 0;
}

//fonction test les robots
void test(robot_liste test){
    robot_liste print = test;
    while (print != NULL) {
        printf(" name %s, pos (%f,%f), addr %p --->",print->element.name, print->element.pos.x, print->element.pos.y, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

//fonction test les bullet
void test2(bullet_liste test){
    bullet_liste print = test;
    while (print != NULL) {
        printf(" pos (%f,%f), addr %p --->",print->element.pos.x, print->element.pos.y, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

//fonction de deplacement des bullets
void move_bullet(bullet_liste* list_bullet, robot_liste* bot_list, map mapOfGame, mqd_t* mq_list){
    bullet_liste tmp_list = *list_bullet;
    while (tmp_list) {
        coord tmp_coord = {tmp_list->element.pos.x + tmp_list->element.speed_x , tmp_list->element.pos.y + tmp_list->element.speed_y};
        robot* tmp_bot = isBot((int) tmp_coord.x, (int)tmp_coord.y, *bot_list);
        if(tmp_bot){
            tmp_bot->pv -= tmp_list->element.damage;
            suppr_bullet(tmp_list->element,list_bullet);
            if (tmp_bot->pv > 0) {
                msg message = {tmp_bot->id,1};
                char* tmp_msg = malloc(sizeof(msg)+sizeof(char));
                str_concat(tmp_msg, (char*) &message, sizeof(msg), &(tmp_list->element.damage), sizeof(char));
                mq_send(mq_list[(int) (tmp_bot->id)], tmp_msg, sizeof(msg)+sizeof(char), 1);
            }else{
                suppr_bot(tmp_bot->id, bot_list);
                msg message = {tmp_bot->id,0};
                mq_send(mq_list[(int) (tmp_bot->id)], (char*) &message, sizeof(msg), 1);
            }
        }else if (mapOfGame.map[((int) tmp_coord.y)*mapOfGame.width+((int) tmp_coord.x)] == 'w' ) {
            suppr_bullet(tmp_list->element,list_bullet);
        }else{
            tmp_list->element.pos = tmp_coord;
        }
        tmp_list = tmp_list->suite;
    }
}

//fonction d'affichage printf()
void affichage(map mapOfGame, robot_liste listOfBot, bullet_liste listOfBullet){
    for (int y = 0; y < mapOfGame.height; y++) {
        for (int x = 0; x < mapOfGame.width; x++) {
            robot* tmp = isBot(x, y, listOfBot);
            if (tmp != NULL) {
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
            }else if (isBullet(x,y,listOfBullet)) {
                printf("*");
            }else{
                printf("%c", mapOfGame.map[y*mapOfGame.width+x]);
            }
        }
        printf("\n");
    }
}

//fonction qui test si un robot est present au coordonnées x, y
robot* isBot(int x, int y, robot_liste listOfBot){
    robot_liste tmp_list = listOfBot;
    while (tmp_list != NULL) {
        if ( (int) (tmp_list->element.pos.x) == x && (int) (tmp_list->element.pos.y) == y ) return &(tmp_list->element);
        tmp_list = tmp_list->suite;
    }
    return NULL;
}

//fonction qui test si il y a une balle au coordonnées x,y;
int isBullet(int x, int y, bullet_liste listOfBullet){
    bullet_liste tmp_list = listOfBullet;
    while (tmp_list != NULL) {
        if ( (int) (tmp_list->element.pos.x) == x && (int) (tmp_list->element.pos.y) == y ) return 1;
        tmp_list = tmp_list->suite;
    }
    return 0;
}

//fonction pour detecter quelle joueur a gagner
int win(robot_liste listOfBot){
    if (listOfBot != NULL) {
        if (listOfBot->suite == NULL) return listOfBot->element.id;
    }
    return -1;
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
    coord spawn[NB_MAX_SPAWN];
    new_map->nbSpawn = 0;
    fseek(f,0,SEEK_SET);
    c = fgetc(f);
    while(c != EOF){
        if(c == 10){
            i++;
            j=0;
        }else{
            new_map->map[(i * new_map->width) + j] = c;
            if(c == 'S'){
                coord tmp = {j,i};
                spawn[new_map->nbSpawn] = tmp;
                new_map->nbSpawn++;
            }
            j++;
        }
        c = fgetc(f);
    }

    //remplir le tableau de spawn
    new_map->spawn = malloc(new_map->nbSpawn * sizeof(coord));
    for (int i = 0; i < new_map->nbSpawn; i++) {
        new_map->spawn[i] = spawn[i];
    }
    if(fclose(f)) return -1;
    return 0;
}
