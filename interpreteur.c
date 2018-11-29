#include "game.h"

char* get_line(FILE *fd){
  int ch,i = 0;
  char *ligne = malloc(100);

  while(ch != EOF){
    ch = fgetc(fd);
    if(ch == '\n' || ch == EOF){
      ligne[i] = '\0';
      return ligne;
    }
    ligne[i] = ch;
    ++i;
  }
  return NULL;
}


cmd create_cmd(char *ligne, FILE *fd){
  if(ligne == NULL){
    cmd new_cmd = {"script",0,0,NULL};
    new_cmd.subcom = malloc(sizeof(cmd));
    while (feof(fd)==0) {
      new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_subcom+1)*sizeof(cmd));
      new_cmd.subcom[new_cmd.nb_subcom] = create_cmd(get_line(fd),fd);
      ++new_cmd.nb_subcom;
    }
    return new_cmd;
  }
  printf("\nligne avant strtok name : \"%s\"\n", ligne);
  char *name_cmd = str_tok(&ligne," ");
  printf("name of cmd : \"%s\"\n", name_cmd);
  printf("ligne après strtok name : \"%s\"\n", ligne);
  cmd new_cmd = {name_cmd,0,0,NULL};

  if(strcmp(name_cmd,"while")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
    //new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args)*sizeof(cmd));
    //new_cmd.subcom[0] = create_cmd(ligne,fd);
    //ligne = get_line(fd);
  }
  if(strcmp(name_cmd,"move")==0){
    new_cmd.nb_args = 1;
  }
  if(strcmp(name_cmd,"turn")==0){
    new_cmd.nb_args = 1;
  }
  if(strcmp(name_cmd,"coord")==0){
    new_cmd.nb_args = 1;
  }
  if(strcmp(name_cmd,"shoot")==0){
    new_cmd.nb_args = 1;
  }
  if(strcmp(name_cmd,"if")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
    //new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args)*sizeof(cmd));
    //new_cmd.subcom[0] = create_cmd(ligne,fd);
    //ligne = get_line(fd);
  }
  if(strcmp(name_cmd,"affect")==0){
    new_cmd.nb_args = 3;
  }
  if(strcmp(name_cmd,"!=")==0){
    new_cmd.nb_args = 2;
  }
  if(strcmp(name_cmd,"==")==0){
    new_cmd.nb_args = 2;
  }

  /*
  if (new_cmd.nb_subcom != 0) {
    //ligne = get_line(fd);
    new_cmd.nb_subcom = 0;
    printf("la ligne entre les crochets : %s\n", ligne);
    while (strcmp(ligne,"}")) {
      new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args+new_cmd.nb_subcom+1)*sizeof(cmd));
      printf("la ligne qui est ajouter en subcom : %s\n", ligne);
      new_cmd.subcom[new_cmd.nb_args+new_cmd.nb_subcom] = create_cmd(ligne,fd);
      ++new_cmd.nb_subcom;
      printf("l'index new_cmd.nb_subcom ++ : %d\n", new_cmd.nb_subcom);
      ligne = get_line(fd);
      printf("la ligne suivante : %s\n", ligne);
    }
    //ligne = get_line(fd);
  }
  */

  new_cmd.subcom = malloc(new_cmd.nb_args*sizeof(cmd));
  for (int i = 0; i < new_cmd.nb_args; ++i) {
    printf("name cmd \"%s\" num_args %d ligne qui va être cmd : \"%s\"\n", new_cmd.name, new_cmd.nb_args, ligne);
    new_cmd.subcom[i] = create_cmd(ligne,fd);
  }

  if (new_cmd.nb_subcom != 0) {
    new_cmd.nb_subcom = 0;
    if (strcmp(str_tok(&ligne," "), "{")) {
      ligne = get_line(fd);
      while (strcmp(ligne,"}")) {
        new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args+new_cmd.nb_subcom+1)*sizeof(cmd));
        new_cmd.subcom[new_cmd.nb_args+new_cmd.nb_subcom] = create_cmd(ligne,fd);
        ++new_cmd.nb_subcom;
        ligne = get_line(fd);
      }
    }
  }

  return new_cmd;

}

void glup(cmd com){
  printf("name %s, nb_args %d, nb_subcom %d, name subcom %s\n", com.name, com.nb_args, com.nb_subcom, com.subcom->name);
  for (int i = 0; i < com.nb_subcom && com.nb_args == 0; ++i) {
    printf("\t");
    /*for (int j = 0; j <= com.nb_subcom+com.nb_args && com.nb_args == 1; j++) {
      printf("\t");
      glup(com.subcom->subcom[j]);
    }*/
    glup(com.subcom[i+com.nb_args]);
  }
}

void printw(cmd w){
  printf("name 1 : %s, name 2 : %s, name 3 : %s\n", w.subcom[0].name,w.subcom[1].name,w.subcom[2].name);

  for (int i = 0; i < 3; ++i) {
    cmd y = w.subcom[i];
    for (int j = 0; j < y.nb_args; ++j) {
      printf("subcom %d name : %s\n", i, y.subcom[j].name);
    }

  }
}


int main() {
  FILE *fd = fopen("bot_1", "r");

  if (fd == NULL){
    printf("There's no file in here !\n");
    return 1;
  }

  cmd script = create_cmd(NULL,fd);

  glup(script);

  printw(script.subcom[4]);

  return 0;
}



