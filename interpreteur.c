#include "game.h"

/*  fonction de recuperation des lignes dans un fichier */
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

/*  fonction de creation des commande  */
cmd create_cmd(char **ligne, FILE *fd){
  if(ligne == NULL){
    cmd new_cmd = {"script",0,0,NULL};
    new_cmd.subcom = malloc(sizeof(cmd));
    while (feof(fd)==0) {
      new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_subcom+1)*sizeof(cmd));
      char *line[100];
      *line = get_line(fd);
      new_cmd.subcom[new_cmd.nb_subcom] = create_cmd(line,fd);
      ++new_cmd.nb_subcom;
    }
    return new_cmd;
  }
  char *name_cmd = str_tok(ligne," \n");
  cmd new_cmd = {name_cmd,0,0,NULL};
  cmd null = {NULL,0,0,NULL};
  if (name_cmd == NULL) {
    return new_cmd;
  }else if(strcmp(name_cmd,"script")==0){
    char* name_file = str_tok(ligne," \n");
    FILE *fd2 = fopen(name_file, "r");
    cmd tmp = create_cmd(NULL,fd2);
    fclose(fd2);
    return(tmp);
  }else if(strcmp(name_cmd,"move")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"turn")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"coord")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"shoot")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"aim")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"seek")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"while")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
  }
  else if(strcmp(name_cmd,"if")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
  }
  else if(strcmp(name_cmd,"=")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"!=")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"==")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"<=")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,">=")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"<")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,">")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"+")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"-")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"*")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"/")==0)
    new_cmd.nb_args = 2;

  else if(strcmp(name_cmd,"mod")==0)
    new_cmd.nb_args = 2;

  new_cmd.subcom = malloc(new_cmd.nb_args*sizeof(cmd));
  for (int i = 0; i < new_cmd.nb_args; ++i) {
    new_cmd.subcom[i] = create_cmd(ligne,fd);
    if (new_cmd.subcom[i].name == NULL) return null;
  }

  if (new_cmd.nb_subcom != 0) {
    new_cmd.nb_subcom = 0;
    if (fd == NULL) return null;
    if (strcmp(*ligne, "{")==0) {
      *ligne = get_line(fd);
      while (search(*ligne,'}')) {
        new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args+new_cmd.nb_subcom+1)*sizeof(cmd));
        new_cmd.subcom[new_cmd.nb_args+new_cmd.nb_subcom] = create_cmd(ligne,fd);
        ++new_cmd.nb_subcom;
        *ligne = get_line(fd);
      }
    }
  }
  return new_cmd;
}
