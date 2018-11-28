//*#ifndef _POSIX_C_SOURCE
//*#define _POSIX_C_SOURCE 199309L
//*#endif // _POSIX_C_SOURCE

#ifndef GAME_H_
#define GAME_H_

// Codé par Leo et Thomas

#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define NB_MAX_SPAWN 10
#define RAD (3.14159265/180)

typedef struct message msg;
typedef struct coordonnees coord;
typedef struct map map;
typedef struct robot robot;
typedef struct elem_robot elem_robot;
typedef struct elem_robot* robot_liste;
typedef struct inventaire inventaire;
typedef struct bullet bullet;
typedef struct elem_bullet elem_bullet;
typedef struct elem_bullet* bullet_liste;
typedef struct chest chest;
typedef struct commande cmd;

//fonctions de server.c
int create_map(char* path_file, map* new_map);
mqd_t* init(robot_liste* bot_list, int nb_bot, coord* spawn, int nb_spawn, mqd_t server, msg* demande, char* buffer, int taille);
robot* isBot(int x, int y, robot_liste listOfBot);
int isBullet(int x, int y, bullet_liste listOfBullet);
int server(int nbclient);
int win(robot_liste bot_list);
void start_game(mqd_t server, char* buffer, int taille, int nb_bot);
void affichage(map mapOfGame, robot_liste listOfBot, bullet_liste listOfBullet);
void move_bullet(bullet_liste* list_bullet, robot_liste* bot_list, map mapOfGame, mqd_t* mq_list);

void test(robot_liste listOfBot);
void test2(bullet_liste test);

//fonctions de client.c
char* init_client(robot* bot, inventaire* inventaire, mqd_t server, mqd_t* ptrclient, char* name, int nameSize);
int client(char* name);
int interprete(char* commande, mqd_t server, mqd_t client, robot* bot, char* buffer, int taille);

//fonctions fct_mini.c
float get_coord(robot *bot, char *axis);
short get_direction(robot *bot);
short get_pv(robot *bot);
unsigned long long get_money(robot *bot);
short get_nb_bullet(robot *bot);
short get_armor(robot *bot);
void avancer(robot *bot, int move, mqd_t serveur, mqd_t client, char* buffer, int taille);
void start(robot* bot, mqd_t server);
void tourner(robot *bot, short direc, mqd_t server);
void tirer(robot *bot, float angle, mqd_t serveur);
void script(robot *bot, char *name, mqd_t server, mqd_t client, char* buffer, int taille);

//fonctions de game.c
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire);
bullet create_bullet(robot *bot, float speed_x, float speed_y);
void str_concat(char* str, char* elem1, int t_elem1, char* elem2, int t_elem2);
char* str_tok(char** test, char* delim);
char search(char* string, char element);
int add_bot(robot bot, robot_liste* listOfBot);
int suppr_bot(char id, robot_liste* listOfBot);
int add_bullet(bullet bullet, bullet_liste* listOfBullet);
int suppr_bullet(bullet bullet, bullet_liste* listOfBullet);
robot* search_robot(char id, robot_liste listOfBot);

// fonctions de interpreteur.c
cmd create_cmd(char *ligne, FILE *fd);
void glup(cmd com);

//structure pour stocker les coordonnées des elements sur la map
struct coordonnees{
    float x;
    float y;
};


struct map{
    int nbSpawn;        //nombre de spawn de la map
    coord* spawn;       //liste des spawn de la map
    int width;          //largeur de la map
    int height;         //hauteur de la map
    char* map ;         //tableau pour stocker la map
};


struct robot{
    char* name;              // nom du script du robot
    char id;                 // num de la file_de_message
    char reach;               // taille du robot
    coord pos;               // position
    char direction;          // N=0 E=1 S=2 O=3
    char pv;                 // points de vie
    int speed;               // vitesse de déplacemment mm/s a diviser par 1000
    char bullet_damage;      // puissance de l'attaque à distance
    int speed_bullet;        // vitesse de la balle pour le robot basique mm/s a diviser par 1000
    inventaire* inventory;    // inventaire du robot
};

struct elem_robot{
    robot element;
    robot_liste suite;
};

struct inventaire{
    short nb_bullet;
    unsigned long long int money;
    short armor;
};


struct bullet{
    char size;            //  taille de la balle
    coord pos;            // position
    float speed_x;        // vitesse de la balle en x
    float speed_y;        // vitesse de la balle en y
    char damage;          // dégats de la balle
};


struct elem_bullet{
    bullet element;
    bullet_liste suite;
};


struct chest{
    char size;    //  taille du coffre
    coord pos;    //  position
    char value;   // L'ARGENT !!!!!! dans le coffre
};

//structure de header de message a envoyer au server
struct message{
    char client;  //id du client qui envoie le message
    char action;  //action demander par le client
};

//structure des commandes pour l'interpréteur
struct commande {
    char* name;     //le nom de la commande
    int nb_args;    //nombre d'arguments
    int nb_subcom;  //nombre de sous commandes
    cmd* subcom;    //tableau des sous commandes
};


#endif  // GAME_H_
