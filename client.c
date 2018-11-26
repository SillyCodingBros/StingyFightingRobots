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
    int taille;
    char* buffer;
    struct mq_attr attr;
    inventaire inventaire = {10,0,0};

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
    char* FdeM = init_client(&bot, &inventaire, server, &client, name, nameSize);

    printf("verif\n");
    printf("name %s, id %d, coord (%f,%f)\n",bot.name, bot.id, bot.pos.x, bot.pos.y );

    //recuperation de la taille du buffer pour la file_de_message server
    if (mq_getattr(client, &attr) != 0) {
        perror("mq_getattr");
        return -1;
    }

    //set-up du buffer
    taille = attr.mq_msgsize;
    printf("attr.mq_msgsize = %ld\n", attr.mq_msgsize);
    buffer = calloc(0, taille);

    //boucle principale du client
    while(bot.pv > 0){
        char* com_scan = malloc(40);
        printf("commande robot %d : ",bot.id);
        fgets(com_scan,40,stdin);
        interprete(com_scan, server, client, &bot, buffer, taille);
        free(com_scan);

        /*attendre commande dans stdin
        recup commande (chaine de charactere)
        separer, analyser (au debut seul start() et script(nom_du_script) autoriser)
        executer
        */
    }
    printf("game over\n");
    //fermeture de la file_de_message
    if (mq_unlink(FdeM)) {
      perror("mq_unlink");
    }

    return 0;
}


//fonction d'analyser et d'exec des commande recup sur stdin
int interprete(char* commande, mqd_t server, mqd_t client, robot* bot, char* buffer, int taille) {
    //actualisation des messages pendant inactivité du client
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    tp.tv_nsec+= 1;
    if(mq_timedreceive(client, buffer, taille, NULL, &tp) > 0){
        if (((msg*) buffer)->action == 0) {
            bot->pv = 0;
            return EXIT_FAILURE;
        }else if (((msg*) buffer)->action == 1) {
            bot->pv = *((int*) &buffer[sizeof(msg)]);
        }
    }
    //traitement de la commande saisi par le client
    char* exec_com;
    char* arg;
    if (commande == NULL) return EXIT_FAILURE;
    exec_com = strtok(commande, " \n");
    if (strcmp(exec_com, "start") == 0) {  /* exec fct start */
        start(bot,server);
    }else if (strcmp(exec_com, "get_pv") == 0) {  /* exec fct get_pv */
        printf("pv = %d\n", get_pv(bot));
    }else if (strcmp(exec_com, "get_money") == 0) {  /* exec fct get_money */
        printf("money = %llu\n", get_money(bot));
    }else if (strcmp(exec_com, "get_coord") == 0) {  /* exec fct get_coord */
        printf("coord = (%f,%f)\n", get_coord(bot).x ,get_coord(bot).y);
    }else if (strcmp(exec_com, "get_bullet") == 0) {  /* exec fct get_coord */
        printf("nb bullet = %d\n", get_nb_bullet(bot) );
    }else if (strcmp(exec_com, "get_armor") == 0) {  /* exec fct get_armor */
        printf("armor = %d\n", get_armor(bot));
    }else if (strcmp(exec_com, "avancer") == 0) {  /* exec fct avancer */
        arg = strtok(NULL, " ");
        avancer(bot,atoi(arg),server,client,buffer,taille);
    }else if (strcmp(exec_com, "tourner") == 0) {  /* exec fct tourner */
        arg = strtok(NULL, " ");
        tourner(bot,atoi(arg),server);
    }else if (strcmp(exec_com, "rammasser") == 0) {  /* exec fct rammasser */
    }else if (strcmp(exec_com, "tirer") == 0) {  /* exec fct tirer */
    }else if (strcmp(exec_com, "quitter") == 0) {  /* exec fct quitter */
        bot->pv = 0;
        msg message = {bot->id,6};
        mq_send(server, (char*) &message, sizeof(msg), 1);
    }else{
        printf("unknown commande\n");
    }
    return 0;
}


//fonction d'initialisation du client
char* init_client(robot* bot, inventaire* inventaire, mqd_t server, mqd_t* ptrclient, char* name, int nameSize){
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
    message.action = 0;
    int msg_size = sizeof(msg) + sizeof(int) + nameSize * sizeof(char);
    char* concat_msg = malloc(msg_size);
    str_concat(concat_msg ,(char*) &message ,sizeof(msg) ,(char*) &nameSize ,sizeof(int));
    str_concat(concat_msg ,concat_msg ,sizeof(msg)+sizeof(int) ,name ,nameSize);
    mq_send(server, concat_msg, msg_size, 1);
    free(concat_msg);

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
    *bot = create_robot(name, buffer[sizeof(coord)], *((coord*) buffer), inventaire);

    *ptrclient = client;
    return FdeM;
}
