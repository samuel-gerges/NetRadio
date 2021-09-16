#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include "aux.h"

/* VARIABLES GLOBALES */

char *CONFIG_FILE = NULL;

int MSG_TO_SEND = 1;


struct fun_args_abonnement {
  int *sock;
  char *addr;
};
typedef struct fun_args_abonnement fun_args_abo;














void *abonnement2(void *param) {
  /**
   * Cette fonction va faire s'abonner le client au diffuseur et ainsi permettre la réception 
   * des messages diffusés.
   */

  fun_args_abo *ptr = (fun_args_abo *)param;
  int sock_e = *((int *)ptr->sock); //on récupère la socket

  struct ip_mreq mreq;

  mreq.imr_multiaddr.s_addr = inet_addr(ptr->addr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  setsockopt(sock_e, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
  printf("problème\n");
  char buff[MAX_MSG_DIFF];
  while(1) {
    int rec = recv(sock_e, buff, MAX_MSG_DIFF, 0);
    buff[rec] = '\0';
    printf("%s\n", buff);
  }

  free(param);
  close(sock_e);
  return NULL;
}

void launch_abonnement2(char *addr, char *port) {

  /**
   * Cette fonction va lancer un thread exécutant la fonction 'abonnement' afin que le client 
   * puisse également se connecter en TCP et intéragir avec le diffuseur en même temps qu'il 
   * reçoit ses messages de diffusion.
   */
  
  int sock_e = socket(PF_INET, SOCK_DGRAM, 0);
  
  int set = 1;
  setsockopt(sock_e, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set));
  
  struct sockaddr_in address_sock_e;
  address_sock_e.sin_family = AF_INET;
  address_sock_e.sin_port = htons(atoi(port));
  address_sock_e.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int ret_value1 = bind(sock_e, (struct sockaddr *)&address_sock_e, sizeof(struct sockaddr_in));
  if(ret_value1 != 0) {
    printf("ERROR: Le bind ne s'est pas créé(abonnement client)\n");
    exit(0);
  }
  
  int *sock_e_ptr = (int *)malloc(sizeof(int));
  *sock_e_ptr = sock_e;
  
if(*sock_e_ptr >= 0) {
  
  fun_args_abo args = {sock_e_ptr, addr};

	  fun_args_abo *ptr = &args;

  pthread_t th2;
  int ret_value2 = pthread_create(&th2, NULL, abonnement2, ptr);
  if(ret_value2 != 0) {
    printf("ERROR: Le thread ne s'est pas créé(abonnement client)");
    exit(0);
  }
  }
}




















void abonnement(char *addr, char *port) {

  /**
   * Cette fonction va faire s'abonner le client au diffuseur et ainsi permettre la réception 
   * des messages diffusés.
   */
  
  int sock_e = socket(PF_INET, SOCK_DGRAM, 0);
  
  int set = 1;
  setsockopt(sock_e, SOL_SOCKET, SO_REUSEPORT, &set, sizeof(set));
  
  struct sockaddr_in address_sock_e;
  address_sock_e.sin_family = AF_INET;
  address_sock_e.sin_port = htons(atoi(port));
  address_sock_e.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int ret_value1 = bind(sock_e, (struct sockaddr *)&address_sock_e, sizeof(struct sockaddr_in));
  if(ret_value1 != 0) {
    printf("ERROR: Le bind ne s'est pas créé(abonnement client)\n");
    exit(0);
  }

  struct ip_mreq mreq;

  mreq.imr_multiaddr.s_addr = inet_addr(addr);
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  setsockopt(sock_e, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
  
  char buff[MAX_MSG_DIFF];
  while(1) {
    int rec = recv(sock_e, buff, MAX_MSG_DIFF, 0);
    buff[rec] = '\0';
    printf("%s\n", buff);
  }

  close(sock_e);
}

void envoi(char *port) {

  /**
   * Dans cette fonction on va se connecter en TCP au diffuseur pour pouvoir intéragir avec lui 
   * (MESS ..., LAST n).
   */

  struct addrinfo *first;
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;

  char *machine = read_at_line(CONFIG_FILE, 2);
  if(machine[strlen(machine)-1] == '\n') machine[strlen(machine)-1] = '\0';
  
  int ret_value1 = getaddrinfo(machine, NULL, &hints, &first);
  free(machine);
  
  if(ret_value1 != 0) {
    printf("ERROR: Problème de getaddrinfo(envoi client)\n");
    exit(0);
  }
  struct sockaddr_in *address_in;
    
  if(first != NULL) {
      
    address_in = (struct sockaddr_in *)first -> ai_addr;
    address_in -> sin_family = AF_INET;
    address_in -> sin_port = htons(atoi(port));
      
    int sock_e = socket(AF_INET, SOCK_STREAM, 0);

    int ret_value2 = connect(sock_e, (struct sockaddr *)address_in, sizeof(struct sockaddr_in));

    if(ret_value2 != 0) {
      printf("ERROR: Le connect ne s'est pas effectué correctement(envoi client)\n");
      exit(0);
    }

    printf("Choisissez l'action a effectuer :\n'M' : envoyer un message à faire diffuser.\n'L n', avec n un nombre : demander les derniers messages diffusés.\n");
    
    while(1) {
      char action[100];
      int lu = read(0, action, 100);
      action[lu] = '\0';

      int len_action = strlen(action);
      if(len_action > 9) {
	printf("ERREUR: Veuillez respecter la syntaxe indiquée.\n");
	continue;
      }

      if(action[0] == 'M' && len_action==2) {
	char conf[LEN_CONF+2];
	char *message = read_at_line("config_files/client_messages", MSG_TO_SEND);
	MSG_TO_SEND++;
	char *buff_formatted = format_msg(message);
	char *id_client = read_at_line(CONFIG_FILE, 1);
	char *id_formatted = format_id(id_client);
	
	free(message);
	free(id_client);
	    
	char msg_d[MAX_MSG_DIFF-5];
	sprintf(msg_d, "MESS %s %s\r\n", id_formatted, buff_formatted);
	send(sock_e, msg_d, (MAX_MSG_DIFF-5)*sizeof(char), 0);
	
	free(buff_formatted);
	free(id_formatted);

	int rec_conf1 = recv(sock_e, conf, (LEN_CONF+2)*sizeof(char), 0);
	conf[rec_conf1] = '\0';
	printf("%s", conf);
      }
      else if(action[0] == 'L' && action[1] == ' ' && len_action >= 3) {
	char nb_mess[MAX_CPT];
	sprintf(nb_mess, "%s", "");
	int cpt = 1;
	int err = 1;
	    
	char *tok = strtok(action, " ");
      
	while(tok != NULL) {
	  if(cpt == 2) {
	    sprintf(nb_mess, "%s", tok);
	  }
	  else if(cpt >= 3) {
	    err = 0;
	    printf("ERREUR: Veuillez respecter la syntaxe indiquée.\n");
	    break;
	  }
	  cpt++;
	  tok = strtok(NULL, " ");
	}
	if(err == 1) {
	  char msg_d[5+MAX_CPT+2];
	  char *nb_mess_formatted = format_nb(nb_mess);
	  sprintf(msg_d, "LAST %s\r\n", nb_mess);
	  free(nb_mess_formatted);
	  msg_d[5+strlen(nb_mess)] = '\0';
	  send(sock_e, msg_d, strlen(msg_d)*sizeof(char), 0);

	  int nb_mess_i = atoi(nb_mess);
	  char last_msgs[MAX_MSG_DIFF];
	  char conf[LEN_CONF+2];
	  int size_rec = 0;
	    
	  for(int i = 0; i<nb_mess_i; i++) {
	    size_rec = recv(sock_e, last_msgs, (MAX_MSG_DIFF)*sizeof(char), 0);
	    last_msgs[size_rec] = '\0';
	    printf("%s\n", last_msgs);
	  }
	  int rec_conf2 = recv(sock_e, conf, (LEN_CONF+2)*sizeof(char), 0);
	  conf[rec_conf2] = '\0';
	  printf("%s", conf);
	}
      }
      else {
	printf("ERREUR: Veuillez respecter la syntaxe indiquée.\n");
      }
    }

    close(sock_e);
  }
}

void demande_liste() {

  /**
   *
   */

  struct addrinfo *first;
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  
  int ret_value1 = getaddrinfo("localhost", NULL, &hints, &first);
  
  if(ret_value1 != 0) {
    printf("ERROR: Problème de getaddrinfo(demande client)\n");
    exit(0);
  }
  struct sockaddr_in *address_in;
    
  if(first != NULL) {
      
    address_in = (struct sockaddr_in *)first -> ai_addr;
    address_in -> sin_family = AF_INET;
    address_in -> sin_port = htons(2626);
      
    int sock_e = socket(AF_INET, SOCK_STREAM, 0);

    int ret_value2 = connect(sock_e, (struct sockaddr *)address_in, sizeof(struct sockaddr_in));

    if(ret_value2 != 0) {
      printf("ERROR: Le connect ne s'est pas effectué correctement(demande client)\n");
      exit(0);
    }
	    
    char first_mess[9];
    char num_diff[2];
    
    send(sock_e, "LIST\r\n", 6*sizeof(char), 0);

    int size_rec = recv(sock_e, first_mess, 9*sizeof(char), 0);
    first_mess[size_rec] = '\0';
    printf("%s", first_mess);

    int cpt = 1;
	    
    char *tok = strtok(first_mess, " ");
    while(tok != NULL) {
      if(cpt == 2) {
	sprintf(num_diff, "%s", tok);
      }
      cpt++;
      tok = strtok(NULL, " ");
    }

    int num_diff_i = atoi(num_diff);

    char diff_list[MAX_MSG_REGI];
    
    for(int i = 0; i<num_diff_i; i++) {
      size_rec = recv(sock_e, diff_list, MAX_MSG_REGI*sizeof(char), 0);
      diff_list[size_rec] = '\0';
      printf("%s\n", diff_list);
    }

    close(sock_e);
  }
}

int main(int argc, char *argv[]) {
  
  if(argc != 2) {
    printf("ERREUR: Pour exécuter le programme faire : ./client fichier_config\n");
    return 0;
  }
  
  CONFIG_FILE = malloc(strlen(argv[1])*sizeof(char));
  strcpy(CONFIG_FILE, argv[1]);

  printf("Voici la liste des diffuseurs enregistrés chez nous :\n");
  demande_liste();
  printf("\n\n");

  printf("Veuillez fournir les informations du diffuseur auquel vous souhaitez vous connecter.\n");
  char addr_diff[MAX_IP+1];
  char port_udp[MAX_CPT+1];
  char port_tcp[MAX_CPT+1];
  char action[100];

  printf("Adresse de diffusion :\n");
  int lu = read(0, addr_diff, MAX_IP+1);
  addr_diff[lu-1] = '\0';
  
  printf("Port de diffusion :\n");
  lu = read(0, port_udp, MAX_CPT+1);
  port_udp[lu-1] = '\0';

  printf("Port de réception :\n");
  lu = read(0, port_tcp, MAX_CPT+1);
  port_tcp[lu-1] = '\0';

  printf("Que souhaitez-vous faire ?\n1 : écoute des messages diffusés seulement\n2 : connexion en TCP pour intéragir avec le diffuseur seulement\n3 : les deux choix précédents\n");
  while(1) {
    lu = read(0, action, 100);
    if(lu != 2) {
      printf("ERREUR: Veuillez respecter la syntaxe indiquée.");
    }
    else {
      action[lu-1] = '\0';

      if(!strcmp(action, "1")) {
	abonnement(addr_diff, port_udp);
      }
      else if(!strcmp(action, "2")) {
	envoi(port_tcp);
      }
      else if(!strcmp(action, "3")) {
      	launch_abonnement2(addr_diff, port_udp);
      	envoi(port_tcp);
      }
      else {
	printf("ERREUR: Veuillez respecter la syntaxe indiquée.");
      }
    }
  }

  free(CONFIG_FILE);
  
  return 0;
}
