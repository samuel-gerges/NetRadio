#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "aux.h"

char *read_at_line(char *file, int line_nbr) {
  /**
   * Cette fonction va récupérer les informations (id, nuémros de ports, addr de diffusion) dans 
   * un fichier -> penser à libérer la mémoire après utilisation.
   */
  
  int cpt = 1;
  FILE *f = fopen(file, "r");
  char *line = malloc(MAX_LINE * sizeof(char));
  
  while(cpt != line_nbr) {
    fgets(line, MAX_LINE, f);
    cpt++;
  }
  fgets(line, MAX_LINE, f);
  fclose(f);

  return line;
}

char *format_id(char *id) {
  char *formatted_id = malloc(LEN_ID*sizeof(char));
  int len_id = strlen(id);
  if(id[len_id-1] == '\n') {
    id[len_id-1] = '\0';  len_id = strlen(id);
  }
  if(len_id > LEN_ID) {
    printf("ERREUR: un id n'est pas adapté au format (i.e. 8 caractères max).\n");
    return NULL;
  }
  else if(len_id < LEN_ID) {
    sprintf(formatted_id, "%s", id);
    for(int i=len_id; i<LEN_ID; i++) {
      formatted_id[i] = '#';
    }
    formatted_id[LEN_ID] = '\0';
    return formatted_id;
  }
  else {
    sprintf(formatted_id, "%s", id);
    return formatted_id;
  }
}

char *format_msg(char *msg) {
  char *formatted_msg = malloc((MAX_MSG+2)*sizeof(char));
  int len_msg = strlen(msg);
  if(msg[len_msg-1] == '\n') {
    msg[len_msg-1] = '\0';  len_msg = strlen(msg);
  }
  if(len_msg > MAX_MSG) {
    printf("ERREUR: un message n'est pas adapté au format (i.e. 140 caractères max).\n");
    return NULL;
  }
  else if(len_msg < MAX_MSG) {
    sprintf(formatted_msg, "%s", msg);
    for(int i=len_msg; i<MAX_MSG; i++) {
      formatted_msg[i] = '#';
    }
    formatted_msg[MAX_MSG] = '\0';
    /*formatted_msg[MAX_MSG] = '\r';
    formatted_msg[MAX_MSG+1] = '\n';
    formatted_msg[MAX_MSG+2] = '\0';*/
    return formatted_msg;
  }
  else {
    sprintf(formatted_msg, "%s", msg);
    //strcat(formatted_msg, "\r\n");
    return formatted_msg;
  }
}

char *format_nb_count(int n) {
  char *formatted_nb = malloc(MAX_CPT*sizeof(char));
  
  char nb[MAX_CPT];
  sprintf(nb, "%d", n);
  int len_nb = strlen(nb);
  
  if(len_nb > MAX_CPT) { //n'est pas censé se produire
    printf("ERREUR: le compteur n'est pas adapté au format (i.e. jusqu'à 9999)\n");
    return NULL;
  }
  else if(len_nb < MAX_CPT) {
    
    if(len_nb == 1) {
      sprintf(formatted_nb, "000%s", nb);
    }
    else if(len_nb == 2) {
      sprintf(formatted_nb, "00%s", nb);
    }
    else if(len_nb == 3) {
      sprintf(formatted_nb, "0%s", nb);
    }
    else { //n'est pas censé se produire
      printf("ERREUR: le compteur n'est pas adapté au format (i.e. différent de "")\n");
    }
    return formatted_nb;
  }
  else {
    sprintf(formatted_nb, "%s", nb);
    return formatted_nb;
  }
}

char *format_nb(char *nb) {
  char *formatted_nb = malloc(3*sizeof(char));
  int len_nb = strlen(nb);
  
  if(nb[len_nb-1] == '\n') {
    nb[len_nb-1] = '\0';  len_nb = strlen(nb);
  }
  
  if(len_nb > 3) {
    printf("ERREUR: ce nombre n'est pas adapté au format (i.e. jusqu'à 999).\n");
    return NULL;
  }
  else if(len_nb < 3) {
    if(len_nb == 1) {
      sprintf(formatted_nb, "00%s", nb);
    }
    else if(len_nb == 2) {
      sprintf(formatted_nb, "0%s", nb);
    }
    else { //n'est pas censé se produire
      printf("ERREUR: le nombre n'est pas adapté au format (i.e. différent de "").\n");
    }
    return formatted_nb;
  }
  else {
    sprintf(formatted_nb, "%s", nb);
    return formatted_nb;
  }
}

char *format_nb2(char *nb) {
  char *formatted_nb = malloc(3*sizeof(char));
  int len_nb = strlen(nb);
  
  if(nb[len_nb-1] == '\n') {
    nb[len_nb-1] = '\0';  len_nb = strlen(nb);
  }
  
  if(len_nb > 2) {
    printf("ERREUR: ce nombre n'est pas adapté au format (i.e. jusqu'à 999).\n");
    return NULL;
  }
  else if(len_nb < 2) {
    if(len_nb == 1) {
      sprintf(formatted_nb, "0%s", nb);
    }
    else if(len_nb == 2) {
      sprintf(formatted_nb, "%s", nb);
    }
    else { //n'est pas censé se produire
      printf("ERREUR: le nombre n'est pas adapté au format (i.e. différent de "").\n");
    }
    return formatted_nb;
  }
  else {
    sprintf(formatted_nb, "%s", nb);
    return formatted_nb;
  }
}

char *format_ip(char *ip) {
  char *formatted_ip = malloc(MAX_IP*sizeof(char));
  strcpy(formatted_ip, "");
  
  char temp[strlen(ip)];
  strcpy(temp, ip);
  
  char *ip_tokens = strtok(temp, ".");
  
  while(ip_tokens != NULL) {
    int len_tok = strlen(ip_tokens);
    if(len_tok == 1) {
      strcat(formatted_ip, "00");
      strcat(formatted_ip, ip_tokens);
      strcat(formatted_ip, ".");
    }
    else if(len_tok == 2) {
      strcat(formatted_ip, "0");
      strcat(formatted_ip, ip_tokens);
      strcat(formatted_ip, ".");
    }
    else if(len_tok == 3) {
      strcat(formatted_ip, ip_tokens);
      strcat(formatted_ip, ".");
    }
    ip_tokens = strtok(NULL, ".");
  }

  formatted_ip[strlen(formatted_ip)-1] = '\0'; //on enlève le dernier caractère qui est un "."*
  return formatted_ip;
}

