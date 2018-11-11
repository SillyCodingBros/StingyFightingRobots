#ifndef STRUCT_H_
#define STRUCT_H_

struct message{
  char client;
  char action;
};
typedef struct message msg;

struct un{
  int x;
  int y;
};
typedef struct un pos;

struct deux{
  char vie;
};
typedef struct deux truc;

#endif
