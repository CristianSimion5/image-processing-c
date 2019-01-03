/*  Simion Cristian
 *  Programul principal al proiectului
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comun.h"
#include "criptografie.h"
#include "rec_pattern.h"

void cript1() {
    FILE *f = fopen("cript1.txt", "r");
    if (f == NULL) {
        printf("Fisierul ce contine caile imaginilor destinate criptarii si pe cea a cheii secrete nu a fost gasit!\n");
        exit(EXIT_FAILURE);
    }
    char c_cale1[101] = {0}, c_cale2[101] = {0}, c_cheie[101] = {0};
    fscanf(f, "%100s\n%100s\n%100s\n", c_cale1, c_cale2, c_cheie);
    fclose(f);
    criptare(c_cale1, c_cale2, c_cheie);

    printf("Valorile testului chi-patrat pentru imaginea initiala sunt:\n");
    chi_test(c_cale1);
    printf("Valorile testului chi-patrat pentru imaginea criptata sunt:\n");
    chi_test(c_cale2);
}

void cript2() {
    FILE *f = fopen("cript2.txt", "r");
    char d_cale1[101] = {0}, d_cale2[101] = {0}, d_cheie[101] = {0};
    if (f == NULL) {
        printf("Fisierul ce contine caile imaginilor destinate decriptarii si pe cea a cheii secrete nu a fost gasit!\n");
        exit(EXIT_FAILURE);
    }
    fscanf(f, "%100s\n%100s\n%100s\n", d_cale1, d_cale2, d_cheie);
    fclose(f);
    decriptare(d_cale1, d_cale2, d_cheie);
}

void rec_p() {
    FILE *f = fopen("rec_p.txt", "r");
    if (f == NULL) {
        printf("Fisierul ce contine caile imaginii si sabloanelor nu a fost gasit!\n");
        exit(EXIT_FAILURE);
    }

    double prag = 0.5;
    uint nr_f = 0, W, H, i, sterge_gs;
    fereastra *D, *aux;
    char nume_imagine[101] = {0}, nume_cale[101] = {0}, nume_sablon[101] = {0}, nume_dest[101] = {0};

    fscanf(f, "%100s\n%100s\n%100s\n%u", nume_imagine, nume_cale, nume_dest, &sterge_gs);
    fclose(f);
    preia_date(nume_imagine, &W, &H);
    D = (fereastra*) malloc(10 * W * H * sizeof(fereastra));
    if (D == NULL) {
        printf("Alocarea memoriei pentru vectorul de ferestre a esuat!\n");
        exit(EXIT_FAILURE);
    }
    for (i = 0; i <= 9; i++) {
        strcpy(nume_sablon, nume_cale);
        strcat(nume_sablon, "cifra");
        nume_sablon[strlen(nume_sablon) + 1] = '\0';
        nume_sablon[strlen(nume_sablon)] = i + '0';
        strcat(nume_sablon, ".bmp");
        template_matching(nume_imagine, nume_sablon, "exportari/grayscale/", prag, D, &nr_f, i, sterge_gs);
    }

    aux = (fereastra*) realloc(D, nr_f * sizeof(fereastra));
    if (aux == NULL) {
        free(D);
        printf("Realocarea memoriei pentru vectorul de ferestre a esuat!\n");
        exit(EXIT_FAILURE);
    }
    D = aux;
    elimina_non_maxime(D, nr_f);
    coloreaza(nume_imagine, nume_dest, D, nr_f);
    free(D);
}

int main() {
    cript1();
    cript2();
    rec_p();
    return 0;
}
