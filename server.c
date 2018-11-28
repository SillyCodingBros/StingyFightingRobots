#include "game.h"

int main(int argc, char const *argv[]) {
    //verification des arguments pour eviter les erreur
    if(argc != 2){
      fprintf(stderr, "usage: %s nb_max_joueur\n", argv[0]);
      return 1;
    }

    if(server(atoi(argv[1]))){
        fprintf(stderr, "server : exit failure\n");
        return 1;
    }

    return 0;
}


//fonction pour gerer le server
int server(int nbclient){
    mqd_t server;                  //definition de la file_de_message server
    mqd_t* mq_list;                //liste des file_de_message client
    msg* demande;                  //definition de la structure message a recup dans la file_de_message
    map mapOfGame;                 //definition de la map du jeu
    robot_liste listOfBot;         //tableau des robots presents dans le jeu
    bullet_liste listOfBullet;      //tableau des balles presentes dans le jeu
    struct mq_attr attr;           //recuperatione de la taille du buffer pour la file_de_message
    int taille;                    //taille du buffer pour les file_de_message
    char* buffer;                  //definition du buffer pour la file_de_message
    unsigned int nsec;             //nombre de tour de boucle avant affichage
    int mvp;                       //id du joueur gagnant

    //creation de la map
    if(create_map("map_type_1",&mapOfGame)<0){
      printf("erreur dans la creation de la map\n");
      return 1;
    }

    //ouverture des file_de_message
    server = mq_open("/server", O_RDONLY | O_CREAT, 0600, NULL);

    //recuperation de la taille du buffer pour la file_de_message server
    if (mq_getattr(server, &attr) != 0) {
          perror("mq_getattr");
          return -1;
    }

    //set-up du buffer
    taille = attr.mq_msgsize;
    printf("attr.mq_msgsize = %ld\n", attr.mq_msgsize);
    buffer = malloc(taille);

    //set-up du nombre de joueur attendu
    if (nbclient > mapOfGame.nbSpawn) {
      fprintf(stderr, "trop de joueur\n");
      return -1;
    }
    listOfBot = NULL;
    listOfBullet = NULL;
    demande = NULL;
    mq_list = init(&listOfBot, nbclient, mapOfGame.spawn, mapOfGame.nbSpawn, server, demande, buffer, taille);
    printf("%p\n", listOfBot);

    //attendre que les clients soient pret
    start_game(server, buffer, taille, nbclient);
    //boucle principal du server
    nsec = 0;
    listOfBullet = NULL;
    mvp = -1;
    while (mvp < 0) {
        nsec++;
        struct timespec tp;
        clock_gettime(CLOCK_REALTIME, &tp);
        tp.tv_nsec+= 1;
        if(mq_timedreceive(server, buffer, taille, NULL, &tp) > 0){
            msg message = *((msg*) buffer);
            //analyse et reponse des messages envoyer par les clients
            if (message.action == 2) {
                printf("deplacement\n");
                //regarde si il y a un mur devant le joueur "client"
                if (mapOfGame.map[(int) ((coord*) &buffer[sizeof(msg)])->y * mapOfGame.width + (int) ((coord*) &buffer[sizeof(msg)])->x] == 'w') {
                    //si oui renvois les coord du "client"
                    char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
                    str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &(search_robot(message.client,listOfBot)->pos), sizeof(coord));
                    mq_send(mq_list[(int) message.client], tmp_msg, sizeof(msg)+sizeof(coord), 2);
                    free(tmp_msg);
                }else{
                    //si non renvoie et modifie avec les coord envoyer
                    char* tmp_msg = malloc(sizeof(msg)+sizeof(coord));
                    search_robot(message.client,listOfBot)->pos = *((coord*) &buffer[sizeof(msg)]);
                    str_concat(tmp_msg, (char*) &message, sizeof(msg), (char*) &(search_robot(message.client,listOfBot)->pos), sizeof(coord));
                    mq_send(mq_list[(int) message.client], tmp_msg, sizeof(msg)+sizeof(coord), 2);
                    free(tmp_msg);
                }
            } else if (message.action == 3) {
                search_robot(message.client,listOfBot)->direction = buffer[sizeof(msg)];
            } else if (message.action == 5) {
                coord tmp_speed = *((coord*) &(buffer[sizeof(msg)]));
                bullet new_bullet = create_bullet(search_robot(message.client,listOfBot), tmp_speed.x, tmp_speed.y);
                add_bullet(new_bullet, &listOfBullet);
            } else if (message.action == 6) {
                printf("exit\n");
                suppr_bot(message.client, &listOfBot);
            }
        }
        move_bullet(&listOfBullet, &listOfBot, mapOfGame, mq_list);
        mvp = win(listOfBot);
        if (nsec > 10000) {
            printf("%u\n", nsec);
            affichage(mapOfGame, listOfBot, listOfBullet);
            test(listOfBot);
            test2(listOfBullet);
            nsec = 0;
        }
    }
    msg message = {mvp,0};
    mq_send(mq_list[mvp], (char*) &message, sizeof(msg), 1);
    printf("LE JOUEUR %d A GAGNER\n", mvp);
    mq_unlink("/server");
    return 0;
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
        }else if (mapOfGame.map[((int) tmp_coord.y)*mapOfGame.width+((int) tmp_coord.y)] == 'w' ) {
            suppr_bullet(tmp_list->element,list_bullet);
        }else{
            tmp_list->element.pos = tmp_coord;
        }
        tmp_list = tmp_list->suite;
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
    if (listOfBot->suite == NULL) return listOfBot->element.id;
    return -1;
}


//fonction pour attendre que les clients soient pret
void start_game(mqd_t server, char* buffer, int taille, int nb_bot){
    char* tmp_bot = calloc(0,nb_bot);
    int client_ready = 0;
    while (client_ready == 0) {
        mq_receive(server, buffer, taille, NULL);
        if ( ((msg*) buffer)->action == 1) {
            tmp_bot[ (int) (((msg*) buffer)->client) ] = 1;
        }
        int i;
        for (i = 0; i < nb_bot; i++) {
            if (!tmp_bot[i]) {
                break;
            }
        }
        if (i == nb_bot) {
            client_ready++;
        }
        printf("ready = %d\n", client_ready);
    }
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


//fonction d'initialisation des clients
mqd_t* init(robot_liste* bot_list, int nb_bot, coord* spawn, int nb_spawn, mqd_t server, msg* demande, char* buffer, int taille){
    mqd_t new_Client;
    new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);
    mqd_t* mq_list;
    mq_list = malloc(nb_bot * sizeof(mqd_t));

    //recuperation des client et creation des robots
    for (int i = 0; i < nb_bot; i++) {
        demande = (msg*) buffer;
        mq_receive(server, buffer, taille, NULL);
        if(demande->action == 0){
            printf("message recue\n");
            int* strSize = (int*) &buffer[sizeof(msg)];
            char* name = malloc(*strSize * sizeof(char));
            name = &buffer[sizeof(msg)+sizeof(int)];
            add_bot(create_robot(name,i,spawn[i],NULL),bot_list);
            printf("name %s, id %d, coord (%f,%f)\n",(*bot_list)->element.name, (*bot_list)->element.id, (*bot_list)->element.pos.x, (*bot_list)->element.pos.y );
            sprintf(buffer,"%d",i);
            char* id = malloc(3 * sizeof(char) );
            id[0] = '/';
            id[1] = buffer[0];
            id[2] = '\0';
            mq_list[i] = mq_open(id, O_WRONLY | O_CREAT, 0600, NULL);
            mq_send(new_Client, (char*) id, sizeof(id), 1);
            char* concat_msg = malloc(sizeof(coord) + sizeof(char));
            str_concat(concat_msg, (char*) &(*bot_list)->element.pos, sizeof(coord), &((*bot_list)->element.id), sizeof(char));
            mq_send(mq_list[i], concat_msg, sizeof(coord) + sizeof(char), 1);
        }
        buffer = NULL;
        buffer = malloc(taille);
    }

    test(*bot_list);
    if (mq_close(new_Client)) {
        perror("mq_close");
    }
    if (mq_unlink("/new_Client")) {
        perror("mq_unlink");
    }
    return mq_list;
}
