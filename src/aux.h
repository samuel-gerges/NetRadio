#ifndef AUX_FUNC_H
#define AUX_FUNC_H

/* CONSTANTES */

#define MAX_MSG_DIFF 161 // DIFF num-mess id message
#define MAX_MSG_REGI 57 // REGI id ip1 port1 ip2 port2
#define MAX_MSG 140 // taille max d'un message
#define LEN_ID 8 // taille d'un id
#define MAX_LINE 80// taille max d'une ligne
#define MAX_CPT 4 // taille max du compteur de message
#define LEN_PORT 4 // taille d'un port
#define LEN_CONF 4 // taille d'un message de confirmation
#define MAX_IP 15 // taille max d'une adresse IP

char *read_at_line(char *file, int line_nbr);

char *format_id(char *id);

char *format_msg(char *msg);

char *format_nb_count(int n);

char *format_nb(char *nb);

char *format_nb2(char *nb);

char *format_ip(char *ip);


#endif
