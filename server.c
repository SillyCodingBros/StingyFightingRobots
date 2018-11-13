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
  robot* listOfBot;

  if(create_map("map_type_1",&mapOfGame)<0){
    printf("erreur dans la creation de la map\n");
    return -1;
  }

  printf("verification\n");
  for(int j = 0; j < mapOfGame.height; j++){
    printf("\n");
    for (int i = 0; i < mapOfGame.width; i++) {
      printf("%c", mapOfGame.map[(j*mapOfGame.width) + i]);
    }
  }
  printf("\n");

  server = mq_open("/server", O_WRONLY | O_CREAT, 0600, NULL);
  new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);

  if(argc != 2){
    fprintf(stderr, "usage: %s nb_max_joueur\n", argv[0]);
    return 1;
  }

  listOfBot = malloc(atoi(argv[1]) * sizeof(robot));
  while (1) {
    /* code */
  }

  return 0;
}


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
  fseek(f,0,SEEK_SET);
  c = fgetc(f);
  while(c != EOF){
    if(c == 10){
      i++;
      j=0;
    }else{
      new_map->map[(i * new_map->width) + j] = c;
      j++;
    }
    c = fgetc(f);
  }

  if(fclose(f)) return -1;
  return 0;
}
