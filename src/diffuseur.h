#ifndef DIFF_H
#define DIFF_H

void diffuse(int cpt, int sock_d, struct sockaddr *dest_addr);

void* diffusion(void* param);

void* reception(void* param);

void* enregistrement(void* param);

void launch_diffusion();

void launch_reception();

void launch_enregistrement();

#endif
