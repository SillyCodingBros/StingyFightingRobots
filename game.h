#ifndef GAME_H_
#define GAME_H_

// Codé par Leo et Thomas

#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define NB_MAX_SPAWN 10

typedef struct message msg;
typedef struct coordonnees coord;
typedef struct map map;
typedef struct robot robot;
typedef struct inventaire inventaire;
typedef struct bullet bullet;
typedef struct chest chest;

//fonction de server.c
int create_map(char* path_file, map* new_map);
mqd_t* init(robot* bot_list, int nb_bot, coord* spawn, int nb_spawn, mqd_t server, msg* demande, char* buffer, int taille);
robot* isBot(int x, int y, robot* bot_list, int nb_bot);
int server(int nbclient);
void start_game(mqd_t server, char* buffer, int taille, int nb_bot);
int win(robot* bot_list, int nb_bot);
void affichage(map mapOfGame, robot* listOfBot, int nbclient);

//fonction de client.c
char* init_client(robot* bot, inventaire* inventaire, mqd_t server, mqd_t* ptrclient, char* name, int nameSize);
int client(char* name);
int interprete(char* commande, mqd_t server, mqd_t client, robot* bot, char* buffer, int taille);

//fonction fct_mini.c
coord get_coord(robot *bot);
short get_direction(robot *bot);
short get_pv(robot *bot);
unsigned long long get_money(robot *bot);
short get_nb_bullet(robot *bot);
short get_armor(robot *bot);
void avancer(robot *bot, int move, mqd_t serveur, mqd_t client, char* buffer, int taille);
void start(robot* bot, mqd_t server);
void tourner(robot *bot, short direc, mqd_t server);

//fonction de game.c
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire);
void str_concat(char* str, char* elem1, int t_elem1, char* elem2, int t_elem2);

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
    char size;               // taille du robot
    coord pos;               // position
    char direction;          // N=0 E=1 S=2 O=3
    char pv;                 // points de vie
    int speed;               // vitesse de déplacemment mm/s a diviser par 1000
    char bullet_damage;      // puissance de l'attaque à distance
    int speed_bullet;        // vitesse de la balle pour le robot basique mm/s a diviser par 1000
    inventaire* inventory;    // inventaire du robot
};


struct inventaire{
    short nb_bullet;
    unsigned long long int money;
    short armor;
};


struct bullet{
    char size;            //  taille de la balle
    coord pos;            // position
    int speed;            // vitesse de la balle
    float speed_x;        // vitesse de la balle en x
    float speed_y;        // vitesse de la balle en y
    char damage;          // dégats de la balle
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

#endif  // GAME_H_
