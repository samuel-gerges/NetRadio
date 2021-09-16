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

/* VARIABLES GLOBALES */

pthread_mutex_t verrou = PTHREAD_MUTEX_INITIALIZER;

int MSG_CPT = 0; //compteur des messages diffusés

List MSGS_LIST = NULL; //liste de messages

List MSGS_LIST_DIFF = NULL; //liste de messages diffusés

char *CONFIG_FILE = NULL;

/* FONCTIONS */

struct fun_args {
  int *sock;
  struct sockaddr_in caller;
};
typedef struct fun_args fun_args;

void diffuse(int cpt, int sock_d, struct sockaddr *dest_addr) {

  /**
   * Cette fonction va parcourir la liste de messages et les diffuser un par un avec une seconde 
   * d'écart entre chaque envoi. De plus, les messages diffusés sont stockés dans une autre liste 
   * qui enregistre les messages diffusés avec leur numéro unique (allant jusqu'à 9999).
   */
  
  char msg_d[MAX_MSG_DIFF];
  List temp = MSGS_LIST;
  
  while(temp != NULL) {
    MSG_CPT++;
    temp->td.num_mess = MSG_CPT;
    
    MSGS_LIST_DIFF = add_at(MSGS_LIST_DIFF, temp->td, 0, 1);

    char *formatted_cpt = format_nb_count(MSG_CPT);
    
    sprintf(msg_d, "DIFF %s %s %s\r\n", formatted_cpt, temp->td.id, temp->td.message);
    printf("%s\n", msg_d);
    sendto(sock_d, msg_d, MAX_MSG_DIFF, 0, dest_addr, (socklen_t)sizeof(struct sockaddr_in));
    free(formatted_cpt);
    
    temp = temp->next;
    sleep(1);
  }
  free_list(temp);
}

void* diffusion(void* param) {

  /**
   * Dans cette fonction, on récupère les infos du diffuseur dans son fichier de configuration. 
   * Avec ces infos on crée une sockaddr et on initialise la liste de messages. Ensuite on boucle 
   * indéfiniment en appelant 'diffuse' avec la sockaddr déclarée.
   */
  
  int sock_d = *((int *)param); //on récupère la socket

  struct addrinfo *first_info;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  //on récupère les infos dans le fichier de config
  char *diff_addr = read_at_line(CONFIG_FILE, 2);
  if(diff_addr[strlen(diff_addr)-1] == '\n') diff_addr[strlen(diff_addr)-1] = '\0'; //ici j'enlève le dernier caractère de la chaîne car mon éditeur de texte rajoute des '\n' à la fin des lignes
  char *diff_port = read_at_line(CONFIG_FILE, 4);
  if(diff_port[strlen(diff_port)-1] == '\n') diff_port[strlen(diff_port)-1] = '\0';
  char *recep_port = read_at_line(CONFIG_FILE, 5);
  if(recep_port[strlen(recep_port)-1] == '\n') recep_port[strlen(recep_port)-1] = '\0';
  
  int ret_value = getaddrinfo(diff_addr, diff_port, &hints, &first_info);

  if(ret_value != 0) {
    fprintf(stderr, "ERREUR: Problème de getaddrinfo\n");
    exit(-1);
  }
  
  if(first_info == NULL) {
    fprintf(stderr, "ERREUR: Problème de struct addrinfo\n");
    exit(-1);
  }

  struct sockaddr *dest_addr = first_info->ai_addr;

  char *diff_id = read_at_line(CONFIG_FILE, 1);
  if(diff_id[strlen(diff_id)-1] == '\n') diff_id[strlen(diff_id)-1] = '\0';

  printf("--------------------------------------------------\n\n");
  printf("Lancement du diffuseur %s\n\n", diff_id);
  printf("Adresse de diffusion : '%s'\n", diff_addr);
  printf("Port de diffusion : %s\n", diff_port);
  printf("Port de réception : %s\n", recep_port);
  printf("--------------------------------------------------\n\n");

  free(diff_addr);
  free(diff_port);
  free(recep_port);

  MSGS_LIST = init_list_messages(diff_id, "config_files/diff_messages");
  
  while(1) {
    diffuse(MSG_CPT, sock_d, dest_addr);
  }
  
  free(diff_id);
  free(param);
  close(sock_d);
  return NULL;
}

void* reception(void* param) {

  /**
   * Cette fonction va attendre un message sur la socket de réception (passée en argument) et va 
   * agir selon ce qu'elle reçoit (MESS ..., LAST n, RUOK), selon les spécifications du sujet.
   */
  
  fun_args *p_ptr = (fun_args *)param; //on désinfère le paramètre
  int sock_r = *((int *)p_ptr->sock); //on récupère la socket dans la structure
  
  char mess_rec[4+1+LEN_ID+1+MAX_MSG+2]; //buffer pour recevoir (c'est la taille max d'un msg qu'on peut recevoir d'un client "MESS ID MSG\r\n")
  int recvd;
  
  while((recvd = recv(sock_r, mess_rec, (4+1+LEN_ID+1+MAX_MSG+2)*sizeof(char), 0)) > 0) {
    mess_rec[recvd] = '\0';
    
    if(!strncmp(mess_rec, "MESS ", 5)) {

      to_diffuse to_diff = {0};

      //parsing de la chaîne reçue
      char *string1 = mess_rec;
      string1[4] = '\0';
      char *string2 = mess_rec + 4 + 1;
      string2[LEN_ID] = '\0';
      char *string3 = string2 + LEN_ID + 1;
      string3[MAX_MSG] = '\0';
      
      to_diff.id = calloc(LEN_ID, sizeof(char));
      memcpy(to_diff.id, string2, LEN_ID);

      to_diff.message = calloc(MAX_MSG, sizeof(char));
      memcpy(to_diff.message, string3, MAX_MSG);

      pthread_mutex_lock(&verrou);
      MSGS_LIST = add_at(MSGS_LIST, to_diff, ((MSG_CPT)%list_length(MSGS_LIST))+1, 0);
      pthread_mutex_unlock(&verrou);
      
      char *confirmation1 = "ACKM\r\n";
      send(sock_r, confirmation1, (LEN_CONF+2)*sizeof(char), 0);
    }
    else if(!strncmp(mess_rec, "LAST ", 5)) {

      //parsing de la chaîne reçue
      char *string1 = strtok(mess_rec, " ");;
      string1 = strtok(NULL, " ");
      int nb_mess = atoi(string1);
      if(nb_mess == 0) {
	char *erreur2 = "ERREUR: Veuillez entrer un nombre différent de 0.\n";
	send(sock_r, erreur2, strlen(erreur2)*sizeof(char), 0);
      }
      else {
	
	char msg_d[MAX_MSG_DIFF];
	List temp = MSGS_LIST_DIFF;
	int temp_len = list_length(temp);
	
	if(nb_mess > temp_len) nb_mess = temp_len;
	
	for(int i = 0; i<nb_mess; i++) {
	  to_diffuse td = get_diff_at(temp, i);
	  char *formatted_cpt = format_nb_count(td.num_mess);
	  
	  sprintf(msg_d, "OLDM %s %s %s\r\n", formatted_cpt, td.id, td.message);
	  send(sock_r, msg_d, MAX_MSG_DIFF*sizeof(char), 0);
	  free(formatted_cpt);
	}
	
	char *confirmation2 = "ENDM\r\n";
	send(sock_r, confirmation2, (LEN_CONF+2)*sizeof(char), 0);
      }
    }
    else if(!strcmp(mess_rec, "RUOK\r\n")) {
      char *confirmation3 = "IMOK\r\n";
      send(sock_r, confirmation3, (LEN_CONF+2)*sizeof(char), 0);
    }
    else {
      char *error_msg = "ERREUR: Votre demande n'est pas reconnue par notre service, veuillez la reformuler.\n";
      send(sock_r, error_msg, strlen(error_msg)*sizeof(char), 0);
    }
  }
  
  printf("Un utilisateur s'est déconnecté.\n\n");
  close(sock_r);
  return NULL;
}

void* enregistrement(void* param) {
  
  /**
   * Connexion en TCP à un gestionnaire pour s'enregistrer. On va chercher toutes les infos dont 
   * on a besoin dans le fichier de config, puis on envoie le message REGI ... au gestionnaire.
   */
  
  int sock_e = *((int *)param); //on récupère la socket

  char msg[4+LEN_ID+MAX_IP+LEN_PORT+MAX_IP+LEN_PORT+5+2];
  char conf[LEN_CONF+2];
  
  char *diff_addr = read_at_line(CONFIG_FILE, 2);
  if(diff_addr[strlen(diff_addr)-1] == '\n') diff_addr[strlen(diff_addr)-1] = '\0'; //ici j'enlève le dernier caractère de la chaîne car mon éditeur de texte rajoute des '\n' à la fin des lignes
  char *diff_addr2 = read_at_line(CONFIG_FILE, 3);
  if(diff_addr2[strlen(diff_addr2)-1] == '\n') diff_addr2[strlen(diff_addr2)-1] = '\0';
  char *diff_port = read_at_line(CONFIG_FILE, 4);
  if(diff_port[strlen(diff_port)-1] == '\n') diff_port[strlen(diff_port)-1] = '\0';
  char *recep_port = read_at_line(CONFIG_FILE, 5);
  if(recep_port[strlen(recep_port)-1] == '\n') diff_port[strlen(recep_port)-1] = '\0';

  char *diff_id = read_at_line(CONFIG_FILE, 1);
  if(diff_id[strlen(diff_id)-1] == '\n') diff_id[strlen(diff_id)-1] = '\0';
  
  sprintf(msg, "REGI %s %s %s %s %s\r\n", diff_id, diff_addr, diff_port, diff_addr2, recep_port);
  send(sock_e, msg, (4+LEN_ID+MAX_IP+LEN_PORT+MAX_IP+LEN_PORT+5+2)*sizeof(char), 0);

  int rec_conf = recv(sock_e, conf, (LEN_CONF+2)*sizeof(char), 0);
  conf[rec_conf] = '\0';
  printf("%s", conf);
  
  free(diff_addr);
  free(diff_addr2);
  free(diff_port);
  free(recep_port);
  free(diff_id);

  free(param);
  close(sock_e);
  return NULL;
}

/**
 * Les fonctions 'launch_...()' qui suivent créent des threads pour lancer les différentes 
 * fonctions implémentées ci-dessus, avec à chaque fois le pointeur vers la socket en argument.
 */

void launch_diffusion() {
  int sock_diff = socket(PF_INET, SOCK_DGRAM, 0); //socket pour diffuser

  int *sock_diff_ptr = (int *)malloc(sizeof(int));
  *sock_diff_ptr = sock_diff;
  
  pthread_t th; // on crée un thread pour la diffusion afin de pouvoir recevoir des messages d'autres entités en même temps
  int ret_value = pthread_create(&th, NULL, diffusion, sock_diff_ptr);
  if(ret_value != 0) {
    printf("ERROR: Le thread ne s'est pas créé(diffusion)\n");
    exit(0);
  }
}

void launch_reception() {
  int sock_rec = socket(PF_INET, SOCK_STREAM, 0); //socket pour recevoir
  
  struct sockaddr_in addr_socket_rec;
  memset(&addr_socket_rec, 0, sizeof(addr_socket_rec));
  addr_socket_rec.sin_family = AF_INET;
  
  char *recep_port = read_at_line(CONFIG_FILE, 5);
  if(recep_port[strlen(recep_port)-1] == '\n') recep_port[strlen(recep_port)-1] = '\0';
  
  addr_socket_rec.sin_port=htons(atoi(recep_port));
  addr_socket_rec.sin_addr.s_addr=htonl(INADDR_ANY);
  free(recep_port);
  
  int ret_value3 = bind(sock_rec, (struct sockaddr *)&addr_socket_rec, sizeof(struct sockaddr_in)); //troisième bind pour la réception
  if(ret_value3 != 0) {
    fprintf(stderr, "ERROR: Problème de bind(réception diffuseur)\n");
    exit(-1);
  }

  ret_value3 = listen(sock_rec, 0);
  if(ret_value3 != 0) {
    fprintf(stderr, "ERROR: Problème de listen(réception diffuseur)\n");
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
      
      printf("Un utilisateur s'est connecté.\n\n");
      pthread_t th;
      int ret_value = pthread_create(&th, NULL, reception, p_ptr);
      if(ret_value != 0) {
	printf("ERROR: Le thread ne s'est pas créé(reception diffuseur)\n");
	exit(0);
      }
    }
  }
}

void launch_enregistrement() {
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
      address_in -> sin_port = htons(2626); //port du gestionnaire à récupérer dans gestio_config
      
      int sock_enr = socket(AF_INET, SOCK_STREAM, 0);
      int r2 = connect(sock_enr, (struct sockaddr *)address_in, sizeof(struct sockaddr_in));
      
      if(r2 != -1) {

	int *sock_enr_ptr = (int *)malloc(sizeof(int));
	*sock_enr_ptr = sock_enr;
  
	pthread_t th;
	int ret_value = pthread_create(&th, NULL, enregistrement, sock_enr_ptr);
	if(ret_value != 0) {
	  printf("ERROR: Le thread ne s'est pas créé(enregistrement diffuseur)\n");
	  exit(0);
	}
      }
    }
  }
}

int main(int argc, char *argv[]) {

  /**
   * Dans le main, on se contente de lancer les 3 fonctions qui vont lancer des threads et 
   * s'exéctuer en parallèle.
   */

  if(argc != 2) {
    printf("ERREUR: Pour exécuter le programme faire : ./diff fichier_config\n");
    return 0;
  }

  CONFIG_FILE = malloc(strlen(argv[1])*sizeof(char));
  strcpy(CONFIG_FILE, argv[1]);

  launch_diffusion();
  launch_enregistrement();
  launch_reception();
  
  free(CONFIG_FILE);
  
  return 0;
}
