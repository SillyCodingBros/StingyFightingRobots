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
  char *name_cmd = str_tok(&ligne," ");
  cmd new_cmd = {name_cmd,0,0,NULL};

  if(strcmp(name_cmd,"move")==0){
    new_cmd.nb_args = 1;
  }
  if(strcmp(name_cmd,"while")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
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
  }
  if(strcmp(name_cmd,"affect")==0){
    new_cmd.nb_args = 3;
  }

  new_cmd.subcom = malloc(new_cmd.nb_args*sizeof(cmd));
  for (int i = 0; i < new_cmd.nb_args; ++i) {
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
  for (int i = 0; i < com.nb_subcom; ++i) {
    printf("\t");
    glup(com.subcom[i+com.nb_args]);
  }
}

/*
int main() {
  FILE *fd = fopen("bot_script_1", "r");

  if (fd == NULL){
    printf("There's no file in here !\n");
    return 1;
  }

  cmd script = create_cmd(NULL,fd);

  glup(script);

  return 0;
}
*/



