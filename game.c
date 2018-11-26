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

//fonction pour ajouter des robot a la liste
int add_bot(robot bot, robot_liste* listOfBot){
    elem_robot* new_bot = malloc(sizeof(elem_robot));
    new_bot->element = bot;
    new_bot->suite = *listOfBot;
    *listOfBot = new_bot;
    return 0;
}

//fonction pour supprimer un robot de la liste avec l'id
int suppr_bot(char id, robot_liste* listOfBot){
    if (*listOfBot == NULL) return EXIT_FAILURE;
    if ((*listOfBot)->element.id == id) {
        elem_robot* tmp = *listOfBot;
        *listOfBot = (*listOfBot)->suite;
        free(tmp);
        return 0;
    }
    for (elem_robot* actuel = *listOfBot; actuel->suite != NULL; actuel = actuel->suite) {
        if (actuel->suite->element.id == id) {
            elem_robot* tmp = actuel->suite;
            actuel->suite = actuel->suite->suite;
            free(tmp);
            return 0;
        }
    }
    return EXIT_FAILURE;
}

//fonction pour ajouter une balle a la liste
int add_bullet(bullet bullet, bullet_liste* listOfBullet){
    elem_bullet* new_bullet = malloc(sizeof(elem_bullet));
    new_bullet->element = bullet;
    new_bullet->suite = *listOfBullet;
    *listOfBullet = new_bullet;
    return 0;
}

//fonction pour supprimer une bullet
int suppr_bullet(bullet bullet, bullet_liste* listOfBullet){
    if (*listOfBullet == NULL) return EXIT_FAILURE;
    if ((*listOfBullet)->element.pos.x == bullet.pos.x && (*listOfBullet)->element.pos.y == bullet.pos.y) {
        elem_bullet* tmp = *listOfBullet;
        *listOfBullet = (*listOfBullet)->suite;
        free(tmp);
        return 0;
    }
    for (elem_bullet* actuel = *listOfBullet; actuel->suite != NULL; actuel = actuel->suite) {
        if (actuel->suite->element.pos.x == bullet.pos.x && actuel->suite->element.pos.y == bullet.pos.y) {
            elem_bullet* tmp = actuel->suite;
            actuel->suite = actuel->suite->suite;
            free(tmp);
            return 0;
        }
    }
    return EXIT_FAILURE;
}

//fonction pour chercher un robot avec l'id
robot* search_robot(char id, robot_liste listOfBot){
    if (listOfBot->element.id == id) return &(listOfBot->element);
    robot_liste tmp_list = listOfBot;
    while (tmp_list->suite) {
        if (tmp_list->suite->element.id != id) tmp_list = tmp_list->suite;
        else return &(tmp_list->suite->element);
    }
    return NULL;
}
