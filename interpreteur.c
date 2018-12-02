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

  if(strcmp(name_cmd,"script")==0){
    char* name_file = str_tok(ligne," \n");
    FILE *fd2 = fopen(name_file, "r");
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
  }

  if (new_cmd.nb_subcom != 0) {
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

void printw(cmd w){
  //printf("name 1 : %s, name 2 : %s, name 3 : %s\n", w.subcom[0].name,w.subcom[1].name,w.subcom[2].name);
  //printf("name 1 : %s, name 2 : %s\n", w.subcom[0].name,w.subcom[1].name);

  for (int i = 0; i < w.nb_args+w.nb_subcom; ++i) {
    printf("com %d name : %s\n", i, w.subcom[i].name);
    cmd y = w.subcom[i];
    for (int j = 0; j < y.nb_args+y.nb_subcom; ++j) {
      printf("\t");
      printf("subcom %d name : %s\n", j, y.subcom[j].name);
      cmd x = y.subcom[j];
      for (int k = 0; k < x.nb_args+x.nb_subcom; ++k) {
        printf("\t\t");
        printf("sub subcom %d name : %s\n", k, x.subcom[k].name);
      }
    }
    printf("\n");

  }
}

/*
int main() {
  FILE *fd = fopen("bot_1", "r");

  if (fd == NULL){
    printf("There's no file in here !\n");
    return 1;
  }
  char* user_com = "script bot_1";
  char* com = malloc(13);
  strcpy(com,user_com);
  printf("%s\n", com);
  cmd script = create_cmd(&com,NULL);

  glup(script,0);

  return 0;
}*/
