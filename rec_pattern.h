// Simion Cristian
#ifndef REC_PATTERN_H_INCLUDED
#define REC_PATTERN_H_INCLUDED

#include <math.h>
#include <string.h>

#include "comun.h"

typedef struct fereastra {
    uint l1, c1;
    uint l2, c2;
    double corr;
    byte cifra;
    byte valid;
} fereastra;

// punctul 7)
int filtru_grayscale(char*, char*, char*);
byte **matricizare_grayscale(char*, uint*, uint*, uint, uint);
pixel **matricizare_color(char*, uint*, uint*, uint, uint);
double calculeaza_corelatia(byte**, byte**, uint, uint, uint, uint, uint, double, double);
void template_matching(char*, char*, char*, double, fereastra*, uint*, int, uint);

void preia_date(char*, uint*, uint*);

//punctul 8)
void contur(pixel**, fereastra, pixel);
void coloreaza(char*, char*, fereastra*, uint);

//punctele 9) si 10)
void elimina_non_maxime(fereastra*, uint);

#endif // REC_PATTERN_H_INCLUDED
