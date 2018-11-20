#include "game.h"

int main(int argc, char const *argv[]) {
  mqd_t server, client;
  robot bot;

//verification des arguments
  if (argc != 2) {
    fprintf(stderr, "usage: %s joueur_name\n", argv[0]);
    return 1;
  }

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
    int nameSize = 0;
    for (int i = 0; argv[1][i] != 0; i++){
      nameSize++;
    }

  char* name = malloc(nameSize * sizeof(char));
  name = argv[1];
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
    mq_send(server, (char*) &message, sizeof(message), 1);
    mq_send(server, (char*) &nameSize, sizeof(nameSize), 1);
    mq_send(server, (char*) name, nameSize * sizeof(char), 1);
    //attente de confirmation et envoie du nom de la file_de_message du client
    char* FdeM = malloc(3 * sizeof(char));
    FdeM = (char*) buffer;
    while (FdeM[0] != '/') {
      mq_receive(client, buffer, taille, NULL);
    }
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
    coord* tmppos;
    tmppos = (coord*) buffer;
    while (tmppos->x == 0) {
      mq_receive(client, buffer, taille, NULL);
    }
    coord pos;
    pos = *tmppos;
    printf("(%f,%f)\n", pos.x, pos.y);
    *bot = create_robot(name,atoi(&FdeM[1]),pos);

    *ptrclient = client;
    return FdeM;
}
