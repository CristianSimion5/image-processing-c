/*  Simion Cristian
 *  Fisierul contine definitiile complete ale tuturor subprogramelor
 *  din partea comuna a proiectului
 */
#include "comun.h"

int creeaza_header(FILE *f, uint W, uint H) {
    /* fisierul header.txt contine un header implicit, cu campurile de marime,
     * latime si lungime setate la 0
     */
    FILE *g = fopen("header.txt", "rb");
    if (g == NULL) {
        printf("Deschiderea header-ului implicit a esuat!\n");
        return -1;
    }

    /* aceasta portiune copiaza header-ul implicit si completeaza campurile de
     * dimensiune, latime si lungime cu valorile asociate vectorului de pixeli
     */
    byte header[54];
    fread(header, sizeof(header), 1, g);
    fwrite(header, sizeof(header), 1, f);
    // calculam padding-ul si dimensiunea
    uint padding, dimensiune;
    padding = W % 4;
    dimensiune = (W * 3 + padding) * H + 54;
    // scriem dimensiunea
    fseek(f, 2, SEEK_SET);
    fwrite(&dimensiune, sizeof(dimensiune), 1, f);
    // scriem latimea si lungimea
    fseek(f, 18, SEEK_SET);
    fwrite(&W, sizeof(W), 1, f);
    fwrite(&H, sizeof(H), 1, f);
    fclose(g);
    return 0;
}

int exporta_vector(char *nume_fis, pixel *L, uint W, uint H) {
    FILE *f;
    f = fopen(nume_fis, "wb");
    if (f == NULL) {
        printf("Crearea imaginii de iesire a esuat!\n");
        return -1;
    }

    if (creeaza_header(f, W, H) == -1) {
        fclose(f);
        remove(nume_fis);
        return -1;
    }

    /* incepe copierea propriu zisa, dar daca incepem de unde se termina
     * header-ul, trebuie sa completam cu ultima linie salvata, apoi cu penultima,
     * etc. deci de la sfarsitul vectorului la inceput
     */
    uint padding = W % 4;
    fseek(f, 54, SEEK_SET);
    uint current_index = H;
    byte zero = 0;
    while (current_index) {
        fwrite(L + (current_index - 1) * W, W * sizeof(byte), sizeof(pixel), f);
        fwrite(&zero, sizeof(byte), padding, f);
        current_index--;
    }
    fclose(f);
    return 0;
}

int exporta_matrice(char *nume_fis, pixel **M, uint W, uint H, uint offsetW, uint offsetH) {
    FILE *f;
    f = fopen(nume_fis, "wb");
    if (f == NULL) {
        printf("Crearea imaginii de iesire a esuat!\n");
        return -1;
    }

    if (creeaza_header(f, W, H) == -1) {
        fclose(f);
        remove(nume_fis);
        return -1;
    }

    /* incepe copierea propriu zisa, dar daca incepem de unde se termina
     * header-ul, trebuie sa completam cu ultima linie salvata, apoi cu penultima,
     * etc. deci de la sfarsitul matricei la inceput
     */
    uint padding = W % 4;
    fseek(f, 54, SEEK_SET);
    uint current_index = H;
    byte zero = 0;
    while (current_index) {
        fwrite(M[current_index + offsetH - 1] + offsetW, sizeof(pixel), W, f);
        fwrite(&zero, sizeof(byte), padding, f);
        current_index--;
    }
    fclose(f);
    return 0;
}

int exporta_imagine(char *nume_fis, char *s) {
    FILE *f, *g;
    f = fopen(nume_fis, "wb");
    if (f == NULL) {
        printf("Crearea imaginii de iesire a esuat!\n");
        return -1;
    }
    g = fopen(s, "rb");
    if (g == NULL) {
        fclose(f);
        printf("Fisier de intrare lipsa!\n");
        return -1;
    }
    byte a;
    while (fread(&a, sizeof(a), 1, g) == 1)
        fwrite(&a, sizeof(a), 1, f);
    fclose(f);
    fclose(g);
    return 0;
}
