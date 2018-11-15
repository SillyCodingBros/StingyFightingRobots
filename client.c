#include "game.h"

int main(int argc, char const *argv[]) {
  mqd_t server;
  msg message;
  robot bot;

//verification des arguments
  if (argc != 2) {
    fprintf(stderr, "usage: %s joueur_name\n", argv[0]);
    return 1;
  }

//recherche de la taille du nom passer en argument
  int nameSize = 0;
  for (int i = 0; argv[1][i] != 0; i++){
    nameSize++;
  }

//ouverture de la file_de_message server en ecriture
  server = mq_open("/server", O_WRONLY, 0600, NULL);
  if (server == -1) {
    fprintf(stderr, "server is offline\n");
    return 1;
  }

//ouverture de la file_de_message de recuperation des client set-up par le server
  if(mq_open("/new_Client", O_RDONLY, 0600, NULL) == -1){
    fprintf(stderr, "server is full\n");
    return 1;
  }

//demande de creation de robot pour rejoindre la partie
  message.action = 1;
  mq_send(server, (char*) &message, sizeof(message), 1);
  mq_send(server, (char*) &nameSize, sizeof(int), 1);
  mq_send(server, (char*) argv[1], nameSize * sizeof(char), 1);

  while (1) {
    /* code */
  }
  return 0;
}
