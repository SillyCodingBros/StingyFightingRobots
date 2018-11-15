#include "game.h"

int create_map(char* path_file, map* new_map);
robot create_robot(char* name, char id);

int main(int argc, char const *argv[]) {

  mqd_t server, new_Client;  //definition des file de message utilisé
  msg* demande;  //definition de la structure message a recup dans la file_de_message
  map mapOfGame;  //definition de la map du jeu
  int nbBot;  //nombre d'element dans le tableau de robot
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

//temporraire verification de la map
  printf("verification\n");
  for(int j = 0; j < mapOfGame.height; j++){
    for (int i = 0; i < mapOfGame.width; i++) {
      printf("%c", mapOfGame.map[(j*mapOfGame.width) + i]);
    }
    printf("\n");
  }

//ouverture des file_de_message
  server = mq_open("/server", O_RDONLY | O_CREAT, 0600, NULL);
  new_Client = mq_open("/new_Client", O_WRONLY | O_CREAT, 0600, NULL);

//verification des arguments pour eviter les erreur
  if(argc != 2 && !atoi(argv[1])){
    fprintf(stderr, "usage: %s nb_max_joueur\n", argv[0]);
    return 1;
  }

//recuperation de la taille du buffer pour la file_de_message server
  if (mq_getattr(server, & attr) != 0) {
        perror("mq_getattr");
        return -1;
  }

//set-up du buffer
  taille = attr.mq_msgsize;
  printf("attr.mq_msgsize = %ld\n", attr.mq_msgsize);
  buffer = malloc(taille);

//set-up du nombre de joueur attendu
  nbBot = atoi(argv[1]);
  listOfBot = malloc(nbBot * sizeof(robot));

//recuperation des client et creation des robots
  for (int i = 0; i < atoi(argv[1]); i++) {
    demande = (msg*) buffer;
    while (!demande->action) {
      mq_receive(server, buffer, taille, NULL);
    }
    if(demande->action == 1){
      printf("message recue\n");
      int* strSize = malloc(sizeof(int));
      strSize = (int*) buffer;
      mq_receive(server, buffer, taille, NULL);
      char* name = malloc(*strSize * sizeof(char));
      name = (char*) buffer;
      mq_receive(server, buffer, taille, NULL);
      listOfBot[i] = create_robot(name,i);
    }
    buffer = NULL;
    buffer = realloc(buffer,taille);
  }

//temporraire verification du set-up des robots
  printf("verification robot\n");
  for (int i = 0; i < nbBot; i++) {
    printf("name bot %d is %s\n", i, listOfBot[i].name);
  }

  char pasFini = 1;

  //boucle principale
  while (pasFini) {
    //traitement des messages envoyer par les clients

    //affichage a remplacer par GLUT ou openGL ;)
    for(int j = 0; j < mapOfGame.height; j++){
      for (int i = 0; i < mapOfGame.width; i++) {
        for (int r = 0; r < nbBot; r++) {
          //a changer avec struct coordonnées
          if(listOfBot[r].pos_x == i && listOfBot[r].pos_y == j){
            printf("o");
          }else{
            printf("%c", mapOfGame.map[(j*mapOfGame.width) + i]);
          }
        }
      }
      printf("\n");
    }
    sleep(2);
  }

  return 0;
}


//fonction de creation des robots
robot create_robot(char* name,char id){
  robot new_robot;
  new_robot.name = name;
  new_robot.id = id;
  new_robot.size = 2;
  //coordonnées a set au lancement de la map
  new_robot.direction = 1;
  new_robot.pv = 100;
  new_robot.speed = 500;
  new_robot.bullet_damage = 33;
  new_robot.speed_bullet = 1000;
  new_robot.money = 0;
  return new_robot;
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
