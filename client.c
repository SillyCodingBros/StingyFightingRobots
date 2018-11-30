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
    printf("reception\n");
    msg message;
    char done;
    struct timespec tw;

    done = 1;
    clock_gettime(CLOCK_REALTIME,&tw);
    while (done) {
        if (mq_timedreceive(fdem,*buffer,taille,NULL,&tw) > 0){
            message = *((msg*) *buffer);
            printf("{%d,%d}\n",message.client,message.action);
            if (message.client == -1) {
                bot->pv -= message.action;
                if (bot->pv <= 0) {
                    return -1;
                }
            }else {
                if (message.action == 0) {
                    printf("gagné\n");
                    return 2;
                } else if (message.action == 4) {
                    printf("reprise de la partie dans %d sec\n", *(buffer[sizeof(msg)]));
                } else if (message.action == obj && obj != 0) {
                    return 1;
                }
            }
        }else if (obj == 0) {
            done = 0;
        }
    }
    return 0;
}


int client(char* name){
    mqd_t server, client;
    msg message;
    int taille;
    char *buffer, *FdeM, *concat_msg;
    char *com_scan, *arg, *exec_com;
    struct mq_attr attr;
    struct timespec tw;
    robot bot;
    inventaire inventory;

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
    printf("send\n");
    clock_gettime(CLOCK_REALTIME,&tw);
    mq_receive(client,buffer,taille,NULL);
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
    inventory.nb_bullet = 10;
    inventory.money = 0;
    inventory.armor = 0;
    bot = create_robot(name, message.client, *((coord*) &(buffer[sizeof(msg)])), &inventory);
    com_scan = malloc(40);
    arg = malloc(40);
    while (bot.pv > 0) {
        printf("commande robot %d : ",bot.id);
        com_scan = realloc(0,40);
        fgets(com_scan,40,stdin);
        exec_com = str_tok(&com_scan, " \n");
        if (reception(server,&buffer,taille,&bot,0) == 2){
            printf("GAGNÉ\n");
            break;
        }
        if (exec_com != NULL) {
            if (strcmp(exec_com, "script") == 0) {
                arg = str_tok(&com_scan, " \n");
                if (arg != NULL){
                    script(&bot,arg,server,client,buffer,taille);
                }
            }else if (strcmp(exec_com, "get_dir") == 0) {
                printf("direction = %d\n", get_direction(&bot));
            }else if (strcmp(exec_com, "get_pv") == 0) {
                printf("pv = %d\n", get_pv(&bot));
            }else if (strcmp(exec_com, "get_money") == 0) {
                printf("money = %llu\n", get_money(&bot));
            }else if (strcmp(exec_com, "get_coord") == 0) {
                printf("coord = (%f,%f)\n", get_coord(&bot,"x") ,get_coord(&bot,"y"));
            }else if (strcmp(exec_com, "get_bullet") == 0) {
                printf("nb bullet = %d\n", get_nb_bullet(&bot) );
            }else if (strcmp(exec_com, "get_armor") == 0) {
                printf("armor = %d\n", get_armor(&bot));
            }else if (strcmp(exec_com, "avancer") == 0) {
                arg = str_tok(&com_scan, " \n");
                if (arg != NULL){
                    avancer(&bot,atoi(arg),server,client,buffer,taille);
                }
            }else if (strcmp(exec_com, "observer") == 0) {
                arg = str_tok(&com_scan, " \n");
                if (arg != NULL){
                    printf("l'objet %c est en (%d,%d)\n",arg[0],seek(&bot,arg[0],"x",server,client,buffer,taille),seek(&bot,arg[0],"y",server,client,buffer,taille) );
                }
            }else if (strcmp(exec_com, "tourner") == 0) {
                arg = str_tok(&com_scan, " \n");
                if (arg != NULL){
                    tourner(&bot,atoi(arg),server);
                }
            }else if (strcmp(exec_com, "rammasser") == 0) {
                rammasser(&bot,server,client,buffer,taille);
            }else if (strcmp(exec_com, "tirer") == 0) {
                arg = str_tok(&com_scan, " \n");
                if (arg != NULL){
                    tirer(&bot,atoi(arg),server);
                }
            }else if (strcmp(exec_com, "quitter") == 0) {
                msg message = {bot.id,1};
                mq_send(server, (char*) &message, sizeof(msg), 1);
                break;
            }else{
                printf("unknown commande\n");
            }
        }
    }
    mq_close(client);
    mq_unlink(FdeM);
    return 0;
}
