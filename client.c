#include "game.h"

int main(int argc, char *argv[]) {
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


//fonction qui traite les infos de la file de message
int reception(mqd_t fdem, char** buffer, int taille, robot* bot, char obj){
  //printf("reception\n");
  msg message;
  char done;
  int timer;
  char tmp_buf[taille];
  struct timespec tw;

  done = 1;
  clock_gettime(CLOCK_REALTIME,&tw);
  while (done) {
    if (mq_timedreceive(fdem,tmp_buf,taille,NULL,&tw) > 0){
      message = *((msg*) tmp_buf);
      //printf("{%d,%d}\n",message.client,message.action);
      if (message.client == -1) {
        bot->pv -= message.action;
        bot->reach = (int) (bot->reach * bot->pv/100);
        bot->speed = (int) (bot->speed * bot->pv/100);
        if (bot->pv <= 0) {
          bot->winner = -1;
          return -1;
        }
      }else {
        if (message.action == -1) {
          printf("en attente des joueur\n");
          bot->wait_player = 1;
          return -1;
        } else if (message.action == 0) {
          bot->winner = 1;
          return -1;
        } else if (message.action == 4) {
          timer = *((int*) &(tmp_buf[sizeof(msg)]));
          printf("reprise de la partie dans %d sec\n", timer);
          if (timer == 0) {
            return 0;
          }
        } else if (message.action == obj && obj != 0) {
          *buffer = tmp_buf;
          return 1;
        }
      }
    }else if (obj == 0) {
      done = 0;
    }
  }
  return -1;
}


int client(char* name){
    mqd_t server, client;
    msg message;
    int taille, done;
    char *buffer, *FdeM, *concat_msg;
    char *com_scan, *exec_com;
    struct mq_attr attr;
    robot bot;
    inventaire inventory;
    aff *dico;

    server = mq_open("/server",O_WRONLY,0600,NULL);
    client = mq_open("/new_Client",O_RDONLY,0600,NULL);
    if (server == -1 || client == -1) {
        fprintf(stderr, "server offline\n");
        return EXIT_FAILURE;
    }

    if (mq_getattr(server,&attr)) {
        perror("mq_getattr");
        return EXIT_FAILURE;
    }

    taille = attr.mq_msgsize;
    buffer = malloc(taille);
    concat_msg = malloc(taille);

    message.client = -1;
    message.action = -1;
    str_concat(concat_msg,(char*) &message,sizeof(msg),name,strlen(name)+1);
    mq_send(server,concat_msg,taille,1);
    mq_receive(client,buffer,taille,0);
    message = *((msg*) buffer);
    if (message.client == -1) {
        fprintf(stderr, "game is full\n");
        return EXIT_FAILURE;
    }
    FdeM = calloc(0,3);
    FdeM[0] = '/';
    sprintf(FdeM+1,"%d",message.client);
    mq_close(client);

    client = mq_open(FdeM,O_RDONLY,0600,NULL);
    if (client == -1) {
        perror("mq_open");
        return EXIT_FAILURE;
    }
    inventory.nb_bullet = 100;
    inventory.money = 0;
    inventory.armor = 0;
    dico = NULL;
    done = 1;
    bot = create_robot(name, message.client, *((coord*) &(buffer[sizeof(msg)])), &inventory);
    com_scan = malloc(40);
    while (reception(client,&buffer,taille,&bot,0) < 0);
    while (done) {
        printf("commande robot %d : ",bot.id);
        com_scan = realloc(0,40);
        fgets(com_scan,40,stdin);
        exec_com = malloc(strlen(com_scan));
        strcpy(exec_com,com_scan);
        reception(client,&buffer,taille,&bot,0);
        //test3(dico);
        if (bot.winner == 1){
          printf("GAGNÉ\n");
          done = 0;
          break;
        }else if (bot.winner == -1) {
          printf("PERDU\n");
          done = 0;
          break;
        }
        printf("return %d\n", interp(create_cmd(&exec_com,NULL),&bot,server,client,buffer,taille,&dico));
    }
    mq_close(client);
    mq_unlink(FdeM);
    return 0;
}
