#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct commande cmd;

struct commande {
    char* name;     //le nom de la commande
    int nb_args;    //nombre d'arguments
    int nb_subcom;  //nombre de sous commande
    cmd* subcom;    //tableau des sous commande (pour la fonction "basic", exemple: avancer, le tableau vaut NULL)
};

typedef struct commande_infos cmdi;

struct commande_infos {
    char* name;     //le nom de la commande
    int nb_args;    //nombre d'arguments
};

cmdi get_coord = {"get_coord",1};
cmdi dir = {"dir",0};
cmdi pv = {"pv",0};
cmdi money = {"money",0};
cmdi nb_bullet = {"nb_bullet",0};
cmdi armor = {"armor",0};
cmdi avancer = {"avancer",1};
cmdi start = {"start",0};
cmdi tourner = {"tourner",1};

int main() {
  FILE *file = fopen("bot_script_1", "r");
  int size;
  int j,ch,lines = 0;
  //char count = -1;
  //cmd script;
  //size_t n = 0;
  size = lseek(open("bot_script_1",O_RDONLY), 0, SEEK_END);
  printf("%d\n", size);

  char* lecture = malloc(size);
  //char* ligne = malloc(size);
  char c;

  while(!feof(file)){
    ch = fgetc(file);
    if(ch == '\n'){
      lines++;
    }
  }
  printf("%d\n", lines);
  while(c != EOF){
    for (int x = 0; x < lines; x++) {
      int i = 0;
      char* ligne = malloc(size);
      while (c != '\n') {
        c = fgetc(file);
        ligne[i] = c;
        i++;
      }
      printf("%s", ligne);
    }

    //lecture[j] =
    //j++;

  }

  /*

  //name = malloc(size);

  if (file == NULL) return 1;

  while((c = fgetc(file)) != EOF){
    name[n++] = (char) c;*/
    /*c = getc(fp);
    printf("%c - %d\n", c,c);
    if (c != EOF) {
      putchar(c);
    }
  }*/
  //fclose(fp);
  //printf("\n %d \n", c);
  return 0;
}


/*
char *readFile(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;

    if (file == NULL)
        return NULL; //could not open file

    code = malloc(1000);

    while ((c = fgetc(file)) != EOF)
    {
        code[n++] = (char) c;
    }

    // don't forget to terminate with the null character
    code[n] = '\0';

    return code;
}

#include <stdio.h>
int main() {
    FILE *fp;
    int c;
    fp = fopen(__FILE__,"r");
    do {
         c = getc(fp);
         putchar(c);
    }
    while(c != EOF);
    fclose(fp);
    return 0;
}

*/