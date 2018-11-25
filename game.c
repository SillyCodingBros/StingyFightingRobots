#include "game.h"

//fichier de fonction utiliser dans tout les executables

//fonction de creation des robots
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire){
    robot new_robot;
    new_robot.name = name;
    new_robot.id = id;
    new_robot.size = 2;
    new_robot.pos = spawn;
    new_robot.direction = 0;
    new_robot.pv = 100;
    new_robot.speed = 1;
    new_robot.bullet_damage = 33;
    new_robot.speed_bullet = 2;
    new_robot.inventory = inventaire;
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
