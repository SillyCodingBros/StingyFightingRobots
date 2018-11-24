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


//fonction qui test si un robot est present au coordonnées x, y
int isBot(int x, int y, robot* bot_list, int nb_bot){
    for (int r = 0; r < nb_bot; r++) {
        if((int) bot_list[r].pos.x == x && (int) bot_list[r].pos.y == y){
            return 1;
        break;
        }
    }
    return 0;
}


//fonction pour gerer le server
int server(int nbclient){
    mqd_t server;  //definition de la file_de_message server
    mqd_t* mq_list; //liste des file_de_message client
    msg* demande;  //definition de la structure message a recup dans la file_de_message
    map mapOfGame;  //definition de la map du jeu
    robot* listOfBot;  //tableau des robots presents dans le jeu
    int nbBullet;  //nombre d'element dans le tableau des balles
    bullet* listOfBullet; //tableau des balles presentes dans le jeu
    struct mq_attr attr;  //recuperatione de la taille du buffer pour la file_de_message
    int taille;
    char* buffer;  //definition du buffer pour la file_de_message

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
    listOfBot = malloc(nbclient * sizeof(robot));
    demande = NULL;
    mq_list = init(listOfBot, nbclient, mapOfGame.spawn, mapOfGame.nbSpawn, server, demande, buffer, taille);
    mq_unlink("/server");
    return 0;
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
mqd_t* init(robot* bot_list, int nb_bot, coord* spawn, int nb_spawn, mqd_t server, msg* demande, char* buffer, int taille){
    mqd_t new_Client;
    new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);
    mqd_t* mq_list;
    mq_list = malloc(nb_bot * sizeof(mqd_t));

    //recuperation des client et creation des robots
    for (int i = 0; i < nb_bot; i++) {
        demande = (msg*) buffer;
        mq_receive(server, buffer, taille, NULL);
        if(demande->action == 1){
            printf("message recue\n");
            int* strSize = (int*) &buffer[sizeof(msg)];
            char* name = malloc(*strSize * sizeof(char));
            name = &buffer[sizeof(msg)+sizeof(int)];
            bot_list[i] = create_robot(name,i,spawn[i]);
            printf("name %s, id %d, coord (%f,%f)\n",bot_list[i].name, bot_list[i].id, bot_list[i].pos.x, bot_list[i].pos.y );
            sprintf(buffer,"%d",i);
            char* id = malloc(3 * sizeof(char) );
            id[0] = '/';
            id[1] = buffer[0];
            id[2] = '\0';
            mq_list[i] = mq_open(id, O_WRONLY | O_CREAT, 0600, NULL);
            mq_send(new_Client, (char*) id, sizeof(id), 1);
            char* concat_msg = malloc(sizeof(coord) + sizeof(char));
            str_concat(concat_msg, (char*) &bot_list[i].pos, sizeof(coord), &(bot_list[i].id), sizeof(char));
            mq_send(mq_list[i], concat_msg, sizeof(coord) + sizeof(char), 1);
        }
        buffer = NULL;
        buffer = malloc(taille);
    }
    if (mq_unlink("/new_Client")) {
        perror("mq_unlink");
    }
    return mq_list;
}
