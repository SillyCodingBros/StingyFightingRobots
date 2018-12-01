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

void printw(cmd w);

cmd create_cmd(char **ligne, FILE *fd){
  //printf("com : %s\n", ligne);
  if(ligne == NULL){
    cmd new_cmd = {"script",0,0,NULL};
    new_cmd.subcom = malloc(sizeof(cmd));
    while (feof(fd)==0) {
      //printf("\n%s : %d\n",new_cmd.name,new_cmd.nb_subcom);
      new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_subcom+1)*sizeof(cmd));
      char *line[100];
      *line = get_line(fd);
      new_cmd.subcom[new_cmd.nb_subcom] = create_cmd(line,fd);
      ++new_cmd.nb_subcom;
    }
    return new_cmd;
  }
  //printf("\nligne avant strtok name : \"%s\" at %p\n", *ligne,ligne);
  char *name_cmd = str_tok(ligne," \n");
  //printf("name of cmd : \"%s\"\n", name_cmd);
  //printf("ligne après strtok name : \"%s\" at %p\n", *ligne, ligne);
  cmd new_cmd = {name_cmd,0,0,NULL};
  cmd if_args;

  if(strcmp(name_cmd,"script")==0){
    FILE *fd2 = fopen(*ligne, "r");
    printf("%s\n", *ligne);
    cmd tmp = create_cmd(NULL,fd2);
    fclose(fd2);
    return(tmp);
  }

  else if(strcmp(name_cmd,"move")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"turn")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"coord")==0)
    new_cmd.nb_args = 1;

  else if(strcmp(name_cmd,"shoot")==0)
    new_cmd.nb_args = 1;

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
  else if(strcmp(name_cmd,"else")==0){
    new_cmd.nb_args = 1;
    new_cmd.nb_subcom = 1;
  }
  else if(strcmp(name_cmd,"affect")==0)
    new_cmd.nb_args = 3;

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
  }

  if (new_cmd.nb_subcom != 0) {
    if (strcmp(new_cmd.name, "if")==0) {
      if_args = new_cmd.subcom[0];
    }
    if (strcmp(new_cmd.name, "else")==0) {
      new_cmd.subcom[0] = if_args;
    }
    new_cmd.nb_subcom = 0;
    if (fd == NULL) {
        cmd null = {NULL,0,0,NULL};
        return null;
    }
    if (strcmp(*ligne, "{")==0) {
      //printf("lecture bloc cmd\n");
      *ligne = get_line(fd);
      while (strcmp(*ligne,"}")) {
        new_cmd.subcom = realloc(new_cmd.subcom,(new_cmd.nb_args+new_cmd.nb_subcom+1)*sizeof(cmd));
        new_cmd.subcom[new_cmd.nb_args+new_cmd.nb_subcom] = create_cmd(ligne,fd);
        ++new_cmd.nb_subcom;
        *ligne = get_line(fd);
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
  //printf("name 1 : %s, name 2 : %s, name 3 : %s\n", w.subcom[0].name,w.subcom[1].name,w.subcom[2].name);
  //printf("name 1 : %s, name 2 : %s\n", w.subcom[0].name,w.subcom[1].name);

  for (int i = 0; i < w.nb_args+w.nb_subcom; ++i) {
    printf("com %d name : %s\n", i, w.subcom[i].name);
    cmd y = w.subcom[i];
    for (int j = 0; j < y.nb_args+y.nb_subcom; ++j) {
      printf("subcom %d name : %s\n", j, y.subcom[j].name);
      cmd x = y.subcom[j];
      for (int k = 0; k < x.nb_args+x.nb_subcom; ++k) {
        printf("sub subcom %d name : %s\n", k, x.subcom[k].name);
      }
    }
    printf("\n");

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
  for (int i = 0; i < script.nb_subcom; i++) {
    if (script.subcom[i].name == "script") {
      glup(script.subcom[i]);
    }
  }


  for (int i = 0; i < script.nb_subcom; i++) {
    printw(script.subcom[i]);
  }


  return 0;
}

