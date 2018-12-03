#ifndef GAME_H_
#define GAME_H_

/* Codé par Leo et Thomas */

#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*#define _POSIX_C_SOURCE 199309L */
#define CYCLE 10000
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
typedef struct affect aff;

/*  fonctions de server.c  */
int create_map(char* path_file, map* new_map);
robot* isBot(int x, int y, robot_liste listOfBot);
int isBullet(int x, int y, bullet_liste listOfBullet);
int server(char* map_name);
int win(robot_liste bot_list);
coord bot_interact(map mapOfGame, robot_liste listOfBot, robot* bot, char* buffer);
void affichage(map mapOfGame, robot_liste listOfBot, bullet_liste listOfBullet);
void move_bullet(bullet_liste* list_bullet, robot_liste* bot_list, map mapOfGame, mqd_t* mq_list);
int search_place(char* place,int nb_place);
int in_range(coord pos,robot_liste listOfBot);
void start(mqd_t* mq_list);

/*  debug.c  */
void test(robot_liste listOfBot);
void test2(bullet_liste test);
void test3(aff* dico);
void glup(cmd com, int nb_tab);

/*  fonctions de client.c  */
int client(char* name);
int reception(mqd_t fdem, char** buffer, int taille, robot* bot, char obj);

/*  fonctions fct_mini.c  */
float get_coord(robot *bot, char *axis);
short get_direction(robot *bot);
short get_pv(robot *bot);
unsigned long long get_money(robot *bot);
short get_nb_bullet(robot *bot);
short get_armor(robot *bot);
int seek(robot *bot, char *obj, char *axis, mqd_t server, mqd_t client, char* buffer, int taille);
int ramasser(robot *bot, mqd_t server, mqd_t client, char* buffer, int taille);
int avancer(robot *bot, int move, mqd_t server, mqd_t client, char* buffer, int taille);
int tourner(robot *bot, short direc, mqd_t server);
int tirer(robot *bot, float angle, mqd_t serveur);
int interp(cmd sub_com, robot *bot, mqd_t server, mqd_t client, char* buffer, int taille, aff **dico);

/*  fonctions de game.c  */
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire);
bullet create_bullet(robot *bot, float speed_x, float speed_y);
void str_concat(char* str, char* elem1, int t_elem1, char* elem2, int t_elem2);
int search(char* string, char element);
char* str_tok(char** test, char* delim);
int add_bot(robot bot, robot_liste* listOfBot);
int suppr_bot(char id, robot_liste* listOfBot);
int add_bullet(bullet bullet, bullet_liste* listOfBullet);
int suppr_bullet(bullet bullet, bullet_liste* listOfBullet);
robot* search_robot(char id, robot_liste listOfBot);
float distance(coord p1, coord p2);
int affect_dico(char* name, int data, aff** dico);
aff* search_in_dico(char* var ,aff* dico);

/* fonctions de interpreteur.c  */
char* get_line(FILE *fd);
cmd create_cmd(char **ligne, FILE *fd);

/*  structure pour stocker les coordonnées des elements sur la map  */
struct coordonnees{
    float x;
    float y;
};

/*  structure pour stocker la map  */
struct map{
    int nbSpawn;        /* nombre de spawn de la map */
    coord* spawn;       /* liste des spawn de la map */
    int width;          /* largeur de la map */
    int height;         /* hauteur de la map */
    char* map ;         /* tableau pour stocker la map */
};

/*  structure pour stocker un robot  */
struct robot{
    char* name;              /* nom du script du robot */
    char id;                 /* num de la file_de_message */
    char winner;             /* indicateur pour pouvoir gagner */
    char wait_player;        /* indicateur pour etre en attente */
    char reach;              /* champ de vision */
    char pick;               /* porté de ramassage */
    coord pos;               /* position */
    char direction;          /* N=0 E=1 S=2 O=3 */
    char pv;                 /* points de vie */
    int speed;               /* vitesse de déplacemment mm/s a diviser par 1000 */
    char bullet_damage;      /* puissance de l'attaque à distance */
    int speed_bullet;        /* vitesse de la balle pour le robot basique mm/s a diviser par 1000 */
    inventaire* inventory;   /* inventaire du robot */
};

/*  structure maillon pour la liste chainé de robot  */
struct elem_robot{
    robot element;         /* element robot du maillon */
    robot_liste suite;     /* pointeur sur la suite de la liste */
};

/*  structure pour stocker les items ramasser par les robots  */
struct inventaire{
    short nb_bullet;                       /* nombre de balle du robot */
    unsigned long long int money;          /* scors du robot */
    short armor;                           /* nombre d'armure du robot */
};

/*  structure pour stocker les données d'une balle  */
struct bullet{
    char size;            /* taille de la balle */
    coord pos;            /* position */
    float speed_x;        /* vitesse de la balle en x */
    float speed_y;        /* vitesse de la balle en y */
    char damage;          /* dégats de la balle */
};

/*  structure maillon pour la liste chainé de balle  */
struct elem_bullet{
    bullet element;             /* element balle du maillon */
    bullet_liste suite;         /* pointeur sur la suite de la liste */
};

/*  structure pour stocker les données d'un coffre  */
struct chest{
    char size;    /* taille du coffre */
    coord pos;    /* position */
    char value;   /* L'ARGENT !!!!!! dans le coffre */
};

/* structure de header de message a envoyer au server */
struct message{
    char client;     /* id du client qui envoie le message */
    char action;     /* action demander par le client */
};

/*  structure des commandes pour l'interpréteur  */
struct commande {
    char* name;       /* le nom de la commande */
    int nb_args;      /* nombre d'arguments */
    int nb_subcom;    /* nombre de sous commandes */
    cmd* subcom;      /* tableau des sous commandes */
};

/*  structure des affectations variable du joueur  */
struct affect {
    char* name;     /* le nom de la variable */
    int data;       /* valeur de la variable */
    aff* next;      /* struct de la prochaine affectation */
};


#endif  /* GAME_H_ */
