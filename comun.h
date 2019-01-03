// Simion Cristian
#ifndef COMUN_H_INCLUDED
#define COMUN_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef struct pixel {
    byte p[3];
} pixel;

int creeaza_header(FILE*, uint, uint);

// punctul 3)
int exporta_vector(char*, pixel*, uint, uint);

int exporta_matrice(char*, pixel**, uint, uint, uint, uint);
int exporta_imagine(char*, char*);

#endif // COMUN_H_INCLUDED
