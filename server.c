#include "structure.h"
#include "game.h"
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

int create_map(char* path_file, map* new_map);

int main(int argc, char const *argv[]) {

  mqd_t server, new_Client;
  msg* demande;
  map mapOfGame;

  if(create_map("map_type_1",&mapOfGame)<0){
    printf("erreur dans l'ouverturedu fichier\n");
    return -1;
  }

  for(int j = 0; j < 42; j++){
    printf("\n");
    for (int i = 0; i < 75; i++) {
      printf("%c", mapOfGame.map[j][i]);
    }
  }
  printf("\n");

  server = mq_open("/server", O_WRONLY | O_CREAT, 0600, NULL);
  new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);

  if(argc != 2){
    fprintf(stderr, "usage: %s nb_max_joueur\n", argv[0]);
    return 1;
  }

  while (1) {
    /* code */
  }
  return 0;
}


int create_map(char* path_file, map* new_map){

  int i = 0, j = 0;
  char c;
  FILE* f = NULL;

  f = fopen(path_file, "r");
  int tmp = fseek(f,0,SEEK_SET);

  if(tmp) printf("%d\n", tmp);

  if(f==NULL) return -1;

  c = fgetc(f);
  while(c != EOF){
    if(c == 10){
      i++;
      j=0;
    }else{
      new_map->map[i][j] = c;
      j++;
    }
    c = fgetc(f);
  }

  return 0;
}
