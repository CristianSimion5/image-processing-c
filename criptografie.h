// Simion Cristian
#ifndef CRIPTOGRAFIE_H_INCLUDED
#define CRIPTOGRAFIE_H_INCLUDED

#include "comun.h"

typedef union word {
    uint w;
    byte b[4];
} word;

// punctul 1)
void xorshift32(uint, uint*, uint);

// punctul 2)
pixel *liniarizare(char*, uint*, uint*);

// punctele 4) si 5)
// functii cu permutari
void generare_permutare(uint *P, uint *R, uint dim);
pixel *permuta_pixeli(pixel *L, uint *perm, uint dim);
pixel *inversa_permutarii(pixel *L, uint *perm, uint dim);

// functiile de codificare / decodificare
pixel *codificare(uint SV, pixel *P, uint *R, uint dim);
pixel *decodificare(uint SV, pixel *C, uint *R, uint dim);

// functiile principale de criptare / decriptare, ce apeleaza celelalte functii
void criptare(char *s, char *d, char *k);
void decriptare(char *s, char *d, char *k);

// punctul 6)
uint *frecventa_canal(uint c, pixel *L, uint dim);
void chi_test(char *s);

#endif // CRIPTOGRAFIE_H_INCLUDED
