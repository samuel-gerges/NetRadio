#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include "linked_list.h"
#include "aux.h"
#include <sys/types.h>
#include <sys/wait.h>

/* VARIABLES GLOBALES */

pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

List2 DIFF_LIST = NULL; //liste de messages diffusés

int DIFF_CPT = 0; //compteur des diffuseurs stockés

/* FONCTIONS */

struct fun_args {
  int *sock;
  struct sockaddr_in caller;
};
typedef struct fun_args fun_args;

void verification() {

  struct addrinfo *first;
  struct addrinfo h;
  memset(&h, 0, sizeof h);
  h.ai_family = AF_INET;

  int r = getaddrinfo("localhost", NULL, &h, &first);
  
  if(r == 0) {
    struct sockaddr_in *address_in;
    
    if(first != NULL) {
      
      address_in = (struct sockaddr_in *)first -> ai_addr;
      address_in -> sin_family = AF_INET;

      int sock_v = socket(AF_INET, SOCK_STREAM, 0);

	int len_diff_list = list_length2(DIFF_LIST);
	for(int i=0; i<len_diff_list; i++) {
	  diffuseur diff = get_diff_at2(DIFF_LIST, i);
	  address_in -> sin_port = htons(atoi(diff.port2)); //ici il faudra boucler sur les ports de recéptions de tous les diffuseurs présents dans l'annuaire
      
	  connect(sock_v, (struct sockaddr *)address_in, sizeof(struct sockaddr_in));

	  char *veri = "RUOK\r\n";
	  char conf[LEN_CONF+2];
  
	  send(sock_v, veri, (LEN_CONF+2)*sizeof(char), 0);
  
	  int rec_conf = recv(sock_v, conf, (LEN_CONF+2)*sizeof(char), 0);
	  conf[rec_conf] = '\0';

	  printf("%s", conf);
	}
  
      close(sock_v);
    }
  }
}

void* reception(void* param) {
  fun_args *p_ptr = (fun_args *)param; //on désinfère le paramètre
  int sock_r = *((int *)p_ptr->sock); //on récupère la socket dans la structure
  
  char mess_rec[4+LEN_ID+MAX_IP+LEN_PORT+MAX_IP+LEN_PORT+5];; //buffer pour recevoir (c'est la taille max d'un msg qu'on peut recevoir)
  int recvd;
  
  while((recvd = recv(sock_r, mess_rec, (4+LEN_ID+MAX_IP+LEN_PORT+MAX_IP+LEN_PORT+5)*sizeof(char), 0)) > 0) {    
    mess_rec[recvd] = '\0';
    
    if(!strncmp(mess_rec, "REGI ", 5)) {

      if(DIFF_CPT < 20) {
	int cpt = 1;
	
	diffuseur diff = {0};

	char *tok = strtok(mess_rec, " ");
      
	while(tok != NULL) {
	  if(cpt == 2) {
	    diff.id = calloc(LEN_ID, sizeof(char));
	    memcpy(diff.id, tok, LEN_ID);
	  }
	  else if(cpt == 3) {
	    diff.ip1 = calloc(MAX_IP, sizeof(char));
	    memcpy(diff.ip1, tok, MAX_IP);
	  }
	  else if(cpt == 4) {
	    diff.port1 = calloc(LEN_PORT, sizeof(char));
	    memcpy(diff.port1, tok, LEN_PORT);
	  }
	  else if(cpt == 5) {
	    diff.ip2 = calloc(MAX_IP, sizeof(char));
	    memcpy(diff.ip2, tok, MAX_IP);
	  }
	  else if(cpt == 6) {
	    diff.port2 = calloc(LEN_PORT, sizeof(char));
	    tok[LEN_PORT] = '\0';
	    memcpy(diff.port2, tok, LEN_PORT);
	  }
	  cpt++;
	  tok = strtok(NULL, " ");
	}

	pthread_mutex_lock(&verrou);
      
	if(list_length2(DIFF_LIST) == 0) {
	
	  DIFF_LIST = add_at2(DIFF_LIST, diff, 0, 0);
	}
	else {
	  DIFF_LIST = add_at2(DIFF_LIST, diff, DIFF_CPT, 0);
	}

	DIFF_CPT++;

	pthread_mutex_unlock(&verrou);
	
	verification();

	char *confirmation1 = "REOK\r\n";
	send(sock_r, confirmation1, (LEN_CONF+2)*sizeof(char), 0);

	printf("\nDiffuseur enregistré !\n");
      }
      else {
	char *refus = "RENO\r\n";
	send(sock_r, refus, (LEN_CONF+2)*sizeof(char), 0);
      }
    }
    else if(!strcmp(mess_rec, "LIST\r\n")) {
      if(!(list_length2(DIFF_LIST))) {
	char *erreur2 = "VIDE";
	send(sock_r, erreur2, strlen(erreur2)*sizeof(char), 0);
      }
      
      else {
	
	char msg_d[MAX_MSG_REGI];
	char first_mess[9];
	List2 temp = DIFF_LIST;
	int temp_len = list_length2(temp);
	
	char temp_len_str[2];
	sprintf(temp_len_str, "%d", temp_len);
	char *len_formatted = format_nb2(temp_len_str);

	sprintf(first_mess, "LINB %s\r\n", len_formatted);
	send(sock_r, first_mess, 9*sizeof(char), 0);
	free(len_formatted);
	
	for(int i = 0; i<temp_len; i++) {
	  diffuseur diff = get_diff_at2(temp, i);
	  char *id_formatted = format_id(diff.id);
	  char *ip1_formatted = format_ip(diff.ip1);
	  char *port1_formatted = format_nb_count(atoi(diff.port1));
	  char *ip2_formatted = format_ip(diff.ip2);
	  char *port2_formatted = format_nb_count(atoi(diff.port2));
	  
	  sprintf(msg_d, "ITEM %s %s %s %s %s\r\n", id_formatted, ip1_formatted, port1_formatted, ip2_formatted, port2_formatted);
	  send(sock_r, msg_d, (MAX_MSG_REGI)*sizeof(char), 0);
	  
	  free(id_formatted);
	  free(ip1_formatted);
	  free(port1_formatted);
	  free(ip2_formatted);
	  free(port2_formatted);
	}
      }
    }
    else {
      char *error_msg = "ERREUR: Votre demande n'est pas reconnue par notre service, veuillez la reformuler.\n";
      send(sock_r, error_msg, strlen(error_msg)*sizeof(char), 0);
    }
  }
  printf("Un utilisateur s'est déconnecté du gestionnaire\n");
  //free(param);
  close(sock_r);
  return NULL;
}

void launch_reception() {
  int sock_rec = socket(PF_INET, SOCK_STREAM, 0); //socket pour recevoir
  
  struct sockaddr_in addr_socket_rec;
  memset(&addr_socket_rec, 0, sizeof(addr_socket_rec));
  addr_socket_rec.sin_family = AF_INET;
  
  char *recep_port = read_at_line("config_files/gest1_config", 1);
  if(recep_port[strlen(recep_port)-1] == '\n') recep_port[strlen(recep_port)-1] = '\0';
  
  printf("--------------------------------------------------------\n\n");
  printf("Lancement du gestionnaire sur le port de recéption %s\n\n", recep_port);
  printf("--------------------------------------------------------\n\n");
  
  addr_socket_rec.sin_port=htons(atoi(recep_port));
  addr_socket_rec.sin_addr.s_addr=htonl(INADDR_ANY);
  free(recep_port);
  int ret_value3 = bind(sock_rec, (struct sockaddr *)&addr_socket_rec, sizeof(struct sockaddr_in)); //troisième bind pour la réception
  if(ret_value3 != 0) {
    fprintf(stderr, "ERROR: Problème de bind(réception gestionnaire)\n");
    exit(-1);
  }

  ret_value3 = listen(sock_rec, 0);
  if(ret_value3 != 0) {
    fprintf(stderr, "ERROR: Problème de listen(réception gestionnaire)\n");
    exit(-1);
  }

  struct sockaddr_in transmitter; //on stocke les infos de l'émetteur pour lui répondre
  socklen_t tr_len = sizeof(transmitter);
  
  while(1) {
    
    int *sock_rec_ptr = (int *)malloc(sizeof(int));
    *sock_rec_ptr = accept(sock_rec, (struct sockaddr *)&transmitter, &tr_len);
    
    if(*sock_rec_ptr >= 0) {

      fun_args p = {sock_rec_ptr, transmitter};
      fun_args *p_ptr = &p;
      
      printf("Nouvelle connexion au gestionnaire\n\n");
      pthread_t th;
      int ret_value = pthread_create(&th, NULL, reception, p_ptr);
      if(ret_value != 0) {
	printf("ERROR: Le thread ne s'est pas créé(réception gestionnaire)\n");
	exit(0);
      }
    }
  }
}

/*void launch_verification() {
      
  if(r2 != -1) {

  int *sock_ver_ptr = (int *)malloc(sizeof(int));
  *sock_ver_ptr = sock_ver;
  
  pthread_t th; // on crée un thread pour que l'enregistrement ne soit pas bloquant
  int ret_value = pthread_create(&th, NULL, verification, sock_ver_ptr);
  if(ret_value != 0) {
  printf("ERROR: Le thread ne s'est pas créé(enregistrement gestionnaire)\n");
  exit(0);
  }
  }
  }
  }
  }*/

int main() {

  /*int f;

  f = fork();
  switch(f) {
    
  case -1:
    perror("fork");
    exit(EXIT_FAILURE);
  case 0:
    wait(NULL);
    while(1) {
      verification();
      sleep(9);
    }
    exit(EXIT_SUCCESS);
  default:
    launch_reception();
    wait(NULL);
    }*/

  //verification();
  launch_reception();
  
  return 0;
}
