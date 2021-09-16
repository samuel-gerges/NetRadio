#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* STRUCTURES ET FONCTIONS POUR MESSAGES À DIFFUSER */

struct to_diffuse {
  char *id;
  char *message;
  int num_mess;
};
typedef struct to_diffuse to_diffuse;

struct Elt
{
  to_diffuse td;
  struct Elt *next;
};
typedef struct Elt Elt;

typedef Elt* List;

List add_at(List list, to_diffuse to_diff, int index, int a);

to_diffuse get_diff_at(List list, int index);

List init_list_messages(char *id, char *mess_file);

int list_length(List list);

void print_list(List list);

void free_list(List list);


/* STRUCTURES ET FONCTIONS POUR DIFFUSEURS */

struct diffuseur {
  char *id;
  char *ip1;
  char *port1;
  char *ip2;
  char *port2;
};
typedef struct diffuseur diffuseur;

struct Elt2
{
  diffuseur diffu;
  struct Elt2 *next;
};
typedef struct Elt2 Elt2;
 
typedef Elt2* List2;

int list_length2(List2 list);

List2 add_at2(List2 list, diffuseur diff, int index, int a);

diffuseur get_diff_at2(List2 list, int index);

void print_list2(List2 list);

#endif
