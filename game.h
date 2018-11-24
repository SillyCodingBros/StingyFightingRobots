#ifndef GAME_H_
#define GAME_H_

// Codé par Leo et Thomas

#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

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
int isBot(int x, int y, robot* bot_list, int nb_bot);
int server(int nbclient);

//fonction de client.c
char* init_client(robot* bot, mqd_t server, mqd_t* ptrclient, char* name, int nameSize);
int client(char* name);

//fonction de game.h
robot create_robot(char* name, char id, coord spawn);
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
    char direction;          // N=1 E=2 S=3 O=4
    char pv;                 // points de vie
    int speed;               // vitesse de déplacemment mm/s a diviser par 1000
    char bullet_damage;      // puissance de l'attaque à distance
    int speed_bullet;        // vitesse de la balle pour le robot basique mm/s a diviser par 1000
    int money;               // L'ARGENT !!!!!!!
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

//fonction de creation des robots
robot create_robot(char* name, char id, coord spawn){
    robot new_robot;
    new_robot.name = name;
    new_robot.id = id;
    new_robot.size = 2;
    new_robot.pos = spawn;
    new_robot.direction = 1;
    new_robot.pv = 100;
    new_robot.speed = 500;
    new_robot.bullet_damage = 33;
    new_robot.speed_bullet = 1000;
    new_robot.money = 0;
    return new_robot;
}

//fonction pour concat les msgs avant de les envoyer
void str_concat(char* str, char* elem1, int t_elem1, char* elem2, int t_elem2){
    for (int i = 0; i < t_elem1+t_elem2; i++) {
        if(i<t_elem1){
            str[i] = elem1[i];
        }else{
            str[i] = elem2[i-t_elem1];
        }
    }
}

#endif  // GAME_H_
