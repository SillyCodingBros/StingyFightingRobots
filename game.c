#include "game.h"

//fichier de fonction utiliser dans tout les executables

//fonction de creation des robots
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire){
    robot new_robot;
    new_robot.name = malloc(strlen(name));
    strcpy(new_robot.name, name);
    new_robot.id = id;
    new_robot.reach = 20;
    new_robot.pos = spawn;
    new_robot.direction = 0;
    new_robot.pv = 100;
    new_robot.speed = 1;
    new_robot.bullet_damage = 33;
    new_robot.speed_bullet = 2;
    new_robot.inventory = inventaire;
    return new_robot;
}

bullet create_bullet(robot *bot, float speed_x, float speed_y){
    bullet new_bullet;
    new_bullet.size = 0;
    new_bullet.speed_x = speed_x;
    new_bullet.speed_y = speed_y;
    new_bullet.pos.x = (int) ((bot->pos.x + 10000*speed_x)+0.5);
    new_bullet.pos.y = (int) ((bot->pos.y + 10000*speed_y)+0.5);
    new_bullet.damage = bot->bullet_damage;
    return new_bullet;
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

//fonction utiliser par str_tok detecter la presence d'un caractere dans un string
char search(char* string, char element){
	for(int i = 0; i < strlen(string); i++){
		if(string[i] == element) return 0;
	}
	return 1;
}

//fonction qui separe la string selon des separateur
char* str_tok(char** str, char* delim){
	char* start_str = NULL;
	int i = 0;
	if (str == NULL) return NULL;
	if (*str == NULL) return NULL;
	while (*(*str+i) != '\0') {
		if (search(delim, *(*str+i)) != 0 && start_str == NULL) {
			start_str = *str+i;
		}
		if (search(delim, *(*str+i)) == 0 && start_str != NULL) {
			*(*str+i) = '\0';
			*str = *str+i+1;
			return start_str;
		}
		i++;
	}
	*str = NULL;
	return start_str;
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
    if (listOfBot != NULL) {
        if (listOfBot->element.id == id) return &(listOfBot->element);
        robot_liste tmp_list = listOfBot;
        while (tmp_list->suite) {
            if (tmp_list->suite->element.id != id) tmp_list = tmp_list->suite;
            else return &(tmp_list->suite->element);
        }
    }
    return NULL;
}

//fonction pour chercher un robot avec l'id
int nb_bot(robot_liste listOfBot){
    int nb_bot = 0;
    while (listOfBot){
        listOfBot = listOfBot->suite;
        nb_bot++;
    }
    return nb_bot;
}
