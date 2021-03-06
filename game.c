#include "game.h"

/*  fichier de fonction utiliser dans tout les executables  */

/*  fonction de creation des robots  */
robot create_robot(char* name, char id, coord spawn, inventaire* inventaire){
    robot new_robot;
    new_robot.name = malloc(strlen(name));
    strcpy(new_robot.name, name);
    new_robot.id = id;
    new_robot.reach = 10;
    new_robot.winner = 0;
    new_robot.wait_player = 0;
    new_robot.pick = 2;
    new_robot.pos = spawn;
    new_robot.direction = 0;
    new_robot.pv = 100;
    new_robot.speed = 5;
    new_robot.bullet_damage = 10;
    new_robot.speed_bullet = 200;
    new_robot.inventory = inventaire;
    return new_robot;
}

/*  fonction de creation des balles  */
bullet create_bullet(robot *bot, float speed_x, float speed_y){
    bullet new_bullet;
    new_bullet.size = 0;
    new_bullet.speed_x = speed_x;
    new_bullet.speed_y = speed_y;
    new_bullet.pos.x = (int) ((bot->pos.x + CYCLE*speed_x));
    new_bullet.pos.y = (int) ((bot->pos.y + CYCLE*speed_y));
    new_bullet.damage = bot->bullet_damage;
    return new_bullet;
}

/*  fonction pour concat les msgs avant de les envoyer  */
void str_concat(char* str, char* elem1, int t_elem1, char* elem2, int t_elem2){
    for (int i = 0; i < t_elem1+t_elem2; i++) {
        if(i<t_elem1){
            str[i] = elem1[i];
        }else{
            str[i] = elem2[i-t_elem1];
        }
    }
}

/*  fonction qui cherche la presence d'un caractere dans un string  */
int search(char* string, char element){
	for(int i = 0; i < strlen(string); i++){
		if(string[i] == element) return 0;
	}
	return 1;
}

/*  fonction qui separe la string selon des separateur  */
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

/*  fonction pour ajouter des robot a la liste  */
int add_bot(robot bot, robot_liste* listOfBot){
    elem_robot* new_bot = malloc(sizeof(elem_robot));
    new_bot->element = bot;
    new_bot->suite = *listOfBot;
    *listOfBot = new_bot;
    return 0;
}

/*  fonction pour supprimer un robot de la liste avec l'id  */
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

/*  fonction pour ajouter une balle a la liste  */
int add_bullet(bullet bullet, bullet_liste* listOfBullet){
    elem_bullet* new_bullet = malloc(sizeof(elem_bullet));
    new_bullet->element = bullet;
    new_bullet->suite = *listOfBullet;
    *listOfBullet = new_bullet;
    return 0;
}

/*  fonction pour supprimer une balle  */
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

/*  fonction pour chercher un robot avec l'id  */
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

/* fonction pour calculer la distance entre deux points  */
float distance(coord p1, coord p2){
  return (float) sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
}

/*  fonction du dictionnaire  */
int affect_dico(char* name, int data, aff** dico){
  aff* new_aff;
  aff* i;

  new_aff = malloc(sizeof(aff));
  new_aff->name = malloc(strlen(name));
  strcpy(new_aff->name,name);
  new_aff->data = data;
  new_aff->next = NULL;

  if (dico == NULL) return EXIT_FAILURE;
  if (*dico == NULL) {
    *dico = new_aff;
    return 0;
  }
  if (strcmp((*dico)->name, name) == 0) {
    (*dico)->data = data;
    return 0;
  }
  for (i = *dico; i->next != NULL; i = i->next) {
    if (strcmp(i->next->name, name) == 0) {
      (*dico)->data = data;
      return 0;
    }
  }
  i->next = new_aff;
  return 0;
}

/*  fonction de recherche dans un dico  */
aff* search_in_dico(char* var ,aff* dico){
  while (dico != NULL) {
    if(strcmp(dico->name, var) == 0){
      return dico;
    }
    dico = dico->next;
  }
  return NULL;
}
