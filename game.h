#ifndef GAME_H_
#define GAME_H_

// Codé par Leo et Thomas

#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct{
  int width;
  int height;
  char* map ;
}map;

typedef struct{
  char* name;                   // nom du script du robot
  char id;

  char size;                    //  taille du robot

  float pos_x;                 // position axe x
  float pos_y;                 // position axe y
  char direction;              // N=1 E=2 S=3 O=4

  char pv;               // points de vie

  int speed;               // vitesse de déplacemment mm/s a diviser par 1000 pour application

  char bullet_damage;     // puissance de l'attaque à distance
  int speed_bullet;      // vitesse de la balle pour le robot basique mm/s a diviser par 1000 pour application

  int money;              // L'ARGENT !!!!!!!

}robot;

typedef struct{

  char size;               //  taille de la balle

  float pos_x;            // position axe x
  float pos_y;            // position axe y

  int speed;             // vitesse de la balle
  float speed_x;        // vitesse de la balle en x
  float speed_y;        // vitesse de la balle en y

  char damage;          // dégats de la balle

}bullet;

typedef struct{
  char size;    //  taille du coffre

  char pos_x;   //  position axe x
  char pos_y;   //  position axe y

  char value;   // L'ARGENT !!!!!! dans le coffre

}chest;

//structure de header de message a envoyer au server
struct message{
  char client;  //id du client qui envoie le message
  char action;  //action demander par le client
};
typedef struct message msg;

#endif  // GAME_H_
