#include "game.h"

/*  fichier des fonctions de debug  */

/*  fonction pour tester les robots  */
void test(robot_liste test){
    robot_liste print = test;
    while (print != NULL) {
        printf(" name %s, pos (%f,%f), pv %d,addr %p --->\n",print->element.name, print->element.pos.x, print->element.pos.y, print->element.pv, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

/*  fonction pour tester les bullet  */
void test2(bullet_liste test){
    bullet_liste print = test;
    while (print != NULL) {
        printf(" pos (%f,%f), speed : (%f,%f) addr %p --->",print->element.pos.x, print->element.pos.y, print->element.speed_x, print->element.speed_y, &print->element);
        print = print->suite;
    }
    printf(" NULL\n");
}

/*  fonction pour tester le dictionnaire  */
void test3(aff* dico){
  while (dico != NULL) {
    printf("%s %d %p -->",dico->name, dico->data, dico);
    dico = dico->next;
  }
  printf("NULL\n");
}

/*  fonction pour tester les commandes  */
void glup(cmd com, int nb_tab){
  int i;
  for (int i = 0; i < nb_tab; i++) {
    printf("\t");
  }
  i = 0;
  printf("com %s : ", com.name);
  for (i = 0; i < com.nb_args; i++) {
    printf("%s ",com.subcom[i].name);
  }
  printf("\n");
  while(i < com.nb_args+com.nb_subcom) {
    glup(com.subcom[i],nb_tab+1);
    i++;
  }
}