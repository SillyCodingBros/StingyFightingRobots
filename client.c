#include "structure.h"
#include "game.h"
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  mqd_t server;
  msg message;
  robot bot;

  if (argc != 2) {
    fprintf(stderr, "usage: %s joueur_name\n", argv[0]);
    return 1;
  }

  int nameSize = 0;
  for (int i = 0; argv[1][i] != 0; i++){
    nameSize++;
  }

  server = mq_open("/server", O_WRONLY, 0600, NULL);
  if (server == -1) {
    fprintf(stderr, "server is offline\n");
    return 1;
  }

  if(mq_open("/new_Client", O_RDONLY, 0600, NULL) == -1){
    fprintf(stderr, "server is full\n");
    return 1;
  }

  message.action = 1;
  mq_send(server, (char*) &message, sizeof(message), 1);
  mq_send(server, (char*) &nameSize, sizeof(int), 1);
  mq_send(server, (char*) argv[1], nameSize * sizeof(char), 1);

  while (1) {
    /* code */
  }
  return 0;
}
