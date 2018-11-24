#include "game.h"

int main(int argc, char const *argv[]) {
    //verification des arguments
    if (argc != 2) {
        fprintf(stderr, "usage: %s joueur_name\n", argv[0]);
        return 1;
    }

    if(client(argv[1])){
        fprintf(stderr, "client : exit failure\n");
        return 1;
    }

    return 0;
}


//fonction qui gere le client
int client(char* name){

    mqd_t server, client;
    robot bot;
    int nameSize;

    //ouverture de la file_de_message server en ecriture
    server = mq_open("/server", O_WRONLY, 0600, NULL);
    if (server == -1) {
      fprintf(stderr, "server is offline\n");
      return 1;
    }

    //ouverture de la file_de_message de recuperation des client set-up par le server
    client = mq_open("/new_Client", O_RDONLY, 0600, NULL);
    if (client == -1) {
      fprintf(stderr, "server is full\n");
      return 1;
    }

    //recherche de la taille du nom passer en argument
    nameSize = 0;
    for (int i = 0; name[i] != 0; i++){
        nameSize++;
    }

    //initialisation du client
    char* FdeM = init_client(&bot, server, &client, name, nameSize);

    printf("verif\n");
    printf("name %s, id %d, coord (%f,%f)\n",bot.name, bot.id, bot.pos.x, bot.pos.y );

    //fermeture de la file_de_message
    if (mq_unlink(FdeM)) {
      perror("mq_unlink");
    }

    return 0;

}


char* init_client(robot* bot, mqd_t server, mqd_t* ptrclient, char* name, int nameSize){
    mqd_t client = *ptrclient;
    msg message;
    struct mq_attr attr;
    int taille;
    char* buffer;

    //recuperation de la taille du buffer pour la file_de_message server
    if (mq_getattr(client, &attr) != 0) {
        perror("mq_getattr");
        return NULL;
    }

    //set-up du buffer
    taille = attr.mq_msgsize;
    printf("attr.mq_msgsize = %ld\n", attr.mq_msgsize);
    buffer = calloc(0, taille);

    //demande de creation de robot pour rejoindre la partie
    message.action = 1;
    int msg_size = sizeof(msg) + sizeof(int) + nameSize * sizeof(char);
    char* concat_msg = malloc(msg_size);
    str_concat(concat_msg ,(char*) &message ,sizeof(msg) ,(char*) &nameSize ,sizeof(int));
    str_concat(concat_msg ,concat_msg ,sizeof(msg)+sizeof(int) ,name ,nameSize);
    mq_send(server, concat_msg, msg_size, 1);

    //changement de la file_de_message client sur file_de_message dedié au client
    char* FdeM = malloc(3 * sizeof(char));
    FdeM = (char*) buffer;
    mq_receive(client, buffer, taille, NULL);
    if (mq_close(client)) {
      perror("mq_close");
    }
    client = mq_open(FdeM, O_RDONLY, 0600, NULL);
    if(client < 0){
      perror("mq_open");
      return NULL;
    }

    //reset-up du buffer pour recup des message d'une autre mqueue
    buffer = calloc(0, taille);

    //recuperation des coord
    mq_receive(client, buffer, taille, NULL);
    *bot = create_robot(name,buffer[sizeof(coord)], *((coord*) buffer));

    *ptrclient = client;
    return FdeM;
}
