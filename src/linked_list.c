#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "aux.h"

/* LISTES CHAÎNÉES DE MESSAGES */

List add_at(List list, to_diffuse to_diff, int index, int a) {

  List prev_l = list;
  List cur_l = list;
  int cpt = 0;
  
  Elt* new_elt = malloc(sizeof(Elt));
  new_elt->td = to_diff;
  new_elt->next = NULL;
  
  if(list_length(list)>=index) {
    
    if(index == 0) {
      new_elt->next = list;
      return new_elt;
    }
    
    while(cpt < index) {
      prev_l = cur_l;
      cur_l = cur_l->next;
      cpt++;
    }
    
    prev_l->next = new_elt;
    new_elt->next = cur_l;
    
    return list;
  }
  else {
    printf("liste trop petite\n");
  }
  return NULL;
}

to_diffuse get_diff_at(List list, int index) {
  List temp = list;
  if(temp == NULL) {
    printf("La liste est vide\n");
    exit(-1);
  }

  if(list_length(list)<index) {
    printf("liste trop petite get\n");
    exit(-1);
  }

  if(index == 0) {
    return list->td;
  }
  
  int cpt = 0;
  while(cpt < index) {
    cpt++;
    temp = temp->next;
  }
  return temp->td;
}

List init_list_messages(char *id, char *mess_file) {

  List list_msgs = NULL;
  char msg[MAX_MSG];
  int i = 0;
  
  FILE *file = fopen(mess_file, "r");

  while( (fgets(msg, MAX_MSG, file)) != NULL) {
      
    to_diffuse to_diff = {0};
    to_diff.id = calloc(LEN_ID, sizeof(char));
    char* id_formatted = format_id(id);
    memcpy(to_diff.id, id_formatted, strlen(id_formatted));
    free(id_formatted);
    to_diff.message = calloc(MAX_MSG, sizeof(char));
    char* msg_formatted = format_msg(msg);
    memcpy(to_diff.message, msg_formatted, strlen(msg_formatted));
    free(msg_formatted);
    to_diff.num_mess = 0;
      
    list_msgs = add_at(list_msgs, to_diff, i, 1);

    i++;
  }

  fclose(file);
  return list_msgs;
}

int list_length(List list) {
  List temp = list;
  int length = 0;
  while(temp != NULL) {
    length++;
    temp = temp->next;
		
  }
  return length;
}

void print_list(List list) {
  int i = 1;
  List temp = list;
  while(temp != NULL) {
    if((temp->td.id != NULL) && (temp->td.message != NULL)) printf("%d : id = %s | msg = %s\n", i, temp->td.id, temp->td.message);

    temp = temp->next;
    i++;
  }
}

void free_list(List list) {
  List temp_next = NULL;
  while(list != NULL) {
    temp_next = list->next;

    free(list->td.id);
    free(list->td.message);
    free(list);

    list = temp_next;
  }
}

/* LISTES CHAÎNÉES DE DIFFUSEURS */

int list_length2(List2 list) {
  List2 temp = list;
  int length = 0;
  while(temp != NULL) {
    length++;
    temp = temp->next;
		
  }
  return length;
}

List2 add_at2(List2 list, diffuseur diff, int index, int a) {

  List2 prev_l = list;
  List2 cur_l = list;
  int cpt = 0;
  
  Elt2* new_elt = malloc(sizeof(Elt2));
  new_elt->diffu = diff;
  new_elt->next = NULL;
  
  if(list_length2(list)>=index) {
    
    if(index == 0) {
      new_elt->next = list;
      return new_elt;
    }
    
    while(cpt < index) {
      prev_l = cur_l;
      cur_l = cur_l->next;
      cpt++;
    }
    
    prev_l->next = new_elt;
    new_elt->next = cur_l;
    
    return list;
  }
  else {
    printf("liste trop petite\n");
  }
  return NULL;
}

diffuseur get_diff_at2(List2 list, int index) {
  List2 temp = list;
  if(temp == NULL) {
    printf("La liste est vide\n");
    exit(-1);
  }

  if(list_length2(list)<index) {
    printf("liste trop petite get\n");
    exit(-1);
  }

  if(index == 0) {
    return list->diffu;
  }
  
  int cpt = 0;
  while(cpt < index) {
    cpt++;
    temp = temp->next;
  }
  return temp->diffu;
}

void print_list2(List2 list) {
  int i = 1;
  List2 temp = list;
  while(temp != NULL) {
  printf("%d : id = %s | ip1 = %s | port1 = %s | ip2 = %s | port2 = %s\n", i, temp->diffu.id, temp->diffu.ip1, temp->diffu.port1, temp->diffu.ip2, temp->diffu.port2);

    temp = temp->next;
    i++;
  }
}

