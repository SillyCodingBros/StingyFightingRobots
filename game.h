#ifndef GAME_H_
#define GAME_H_

// Codé par Leo et Thomas

typedef struct{
  int width;
  int height;
  char* map ;
}map;

typedef struct{
  char* name;                   // nom du script du robot

  char size;                //  taille du robot

  float pos_x;                 // position axe x
  float pos_y;                 // position axe y
  char direction;              // N=1 E=2 S=3 O=4

  char pv;               // points de vie

  int speed;               // vitesse de déplacemment mm/s

  char bullet_damage;     // puissance de l'attaque à distance
  char speed_bullet;      // vitesse de la balle pour le robot basique mm/s

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


#endif  // GAME_H_
