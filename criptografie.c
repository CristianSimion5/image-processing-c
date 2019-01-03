/*  Simion Cristian
 *  Fisierul contine definitiile complete ale tuturor subprogramelor
 *  din partea de criptografie a proiectului
 */
#include "criptografie.h"

// genereaza numere pseudo-aleatoare
void xorshift32(uint seed, uint *R, uint dim) {
    R[0] = seed;
    uint i;
    for (i = 1; i < dim; i++) {
        R[i] = R[i - 1];
        R[i] ^= R[i] << 13;
        R[i] ^= R[i] >> 17;
        R[i] ^= R[i] << 5;
    }
}

// salveaza o imagine intr-un vector
pixel *liniarizare(char *nume_fis, uint *w, uint *h) {
    FILE *f = fopen(nume_fis, "rb");
    if (f == NULL) {
        printf("Imagine de intrare lipsa!\n");
        return NULL;
    }

    uint W, H, currentH, padding;
    pixel *L;

    // cautam dimensiunile
    fseek(f, 18, SEEK_SET);
    ftell(f);
    fread(&W, sizeof(W), 1, f);
    fread(&H, sizeof(H), 1, f);

    L = (pixel*) malloc(W * H * sizeof(pixel));
    if (L == NULL) {
        fclose(f);
        printf("Alocarea memoriei pentru liniarizarea imaginii a esuat!\n");
        return NULL;
    }
    padding = W % 4;
    currentH = 1;
    // parcurgem fisierul invers, citind cate o linie intreaga
    while (currentH <= H) {
        int parcurs = (W * 3 + padding) * currentH;
        fseek(f, -parcurs, SEEK_END);
        fread(L + W * (currentH - 1), sizeof(pixel), W, f);
        currentH++;
    }
    *w = W;
    *h = H;
    fclose(f);
    return L;
}

void generare_permutare(uint *P, uint *R, uint dim) {
    uint r, i, aux;
    for (i = 0; i < dim; i++)
        P[i] = i;
    for (i = dim - 1; i >= 1; i--) {
        r = R[dim - i] % (i + 1);
        aux = P[i];
        P[i] = P[r];
        P[r] = aux;
    }
}

pixel *permuta_pixeli(pixel *L, uint *perm, uint dim) {
    pixel *P = (pixel*) malloc(dim * sizeof(pixel));
    if (P == NULL) {
        printf("Alocarea memoriei pentru imaginea permutata a esuat.\n");
        return NULL;
    }
    uint i;
    for (i = 0; i < dim; i++)
        P[perm[i]] = L[i];
    return P;
}

pixel *inversa_permutarii(pixel *L, uint *perm, uint dim) {
    pixel *P = (pixel*) malloc(dim * sizeof(pixel));
    if (P == NULL) {
        printf("Alocarea memoriei pentru imaginea permutata invers a esuat.\n");
        return NULL;
    }
    uint i;
    for (i = 0; i < dim; i++)
        P[i] = L[perm[i]];
    return P;
}

// realizeaza operatia XOR intre bitii unui pixel si bitii unui numar
pixel xor_pw(pixel x, word y) {
    pixel s;
    s.p[0] = x.p[0] ^ y.b[0];
    s.p[1] = x.p[1] ^ y.b[1];
    s.p[2] = x.p[2] ^ y.b[2];
    return s;
}

// realizeaza operatia XOR intre bitii a doi pixeli
pixel xor_pp(pixel x, pixel y) {
    pixel s;
    s.p[0] = x.p[0] ^ y.p[0];
    s.p[1] = x.p[1] ^ y.p[1];
    s.p[2] = x.p[2] ^ y.p[2];
    return s;
}

// se substituie pixelii conform relatiei
pixel *codificare(uint SV, pixel *P, uint *R, uint dim) {
    pixel *C = (pixel*) malloc(dim * sizeof(pixel));
    if (C == NULL) {
        printf("Alocarea memoriei pentru imaginea codificata a esuat.\n");
        return NULL;
    }
    word nr;
    uint i;
    nr.w = SV;
    C[0] = xor_pw(P[0], nr);
    nr.w = R[dim];
    C[0] = xor_pw(C[0], nr);
    for(i = 1; i < dim; i++) {
        nr.w = R[dim + i];
        C[i] = xor_pw(xor_pp(C[i - 1], P[i]), nr);
    }
    return C;
}

// se decodifica pixelii conform relatiei inverse a substitutiei
pixel *decodificare(uint SV, pixel *C, uint *R, uint dim) {
    pixel *C0 = (pixel*) malloc(dim * sizeof(pixel));
    if (C0 == NULL) {
        printf("Alocarea memoriei pentru imaginea decodificata a esuat.\n");
        return NULL;
    }
    word nr;
    uint i;
    nr.w = SV;
    C0[0] = xor_pw(C[0], nr);
    nr.w = R[dim];
    C0[0] = xor_pw(C0[0], nr);
    for(i = 1; i < dim; i++) {
        nr.w = R[dim + i];
        C0[i] = xor_pw(xor_pp(C[i - 1], C[i]), nr);
    }
    return C0;
}

// creeaza vectorii utilizati atat in functia de criptare, cat si in cea de decriptare
void initializare(char *s, char *d, char *k, pixel **L, uint **R, uint **perm, uint *W, uint *H, uint *R0, uint *SV) {
    *L = liniarizare(s, W, H);
    if (*L == NULL)
        exit(EXIT_FAILURE);

    // citim cheia secreta comuna
    FILE *f = fopen(k, "r");
    if (f == NULL) {
        free(*L);
        printf("Fisierul ce contine cheia secreta comuna nu a fost gasit!\n");
        exit(EXIT_FAILURE);
    }
    fscanf(f, "%u %u", R0, SV);
    fclose(f);

    // generam numerele aleatoare prin XORSHIFT32
    *R = (uint*) malloc(2 * (*W) * (*H) * sizeof(uint));
    if (*R == NULL) {
        free(*L);
        printf("Generarea numerelor pseudo-aleatoare a esuat.\n");
        exit(EXIT_FAILURE);
    }
    xorshift32(*R0, *R, 2 * (*W) * (*H));

    // generam permutarea prin algoritmul lui Durstenfeld cu numerele aleatoare calculate anterior
    *perm = (uint*) malloc((*W) * (*H) * sizeof(uint));
    if (*perm == NULL) {
        free(*L);
        free(*R);
        printf("Generarea permutarii a esuat.\n");
        exit(EXIT_FAILURE);
    }
    generare_permutare(*perm, *R, (*W) * (*H));
}

void criptare(char *s, char *d, char *k) {
    pixel *L = NULL, *P = NULL, *C = NULL;
    uint W, H, R0, SV;
    uint *R = NULL, *perm = NULL;

    initializare(s, d, k, &L, &R, &perm, &W, &H, &R0, &SV);
    // permutam pixelii
    P = permuta_pixeli(L, perm, W * H);
    free(L);
    free(perm);
    if (P == NULL) {
        free(R);
        exit(EXIT_FAILURE);
    }

    C = codificare(SV, P, R, W * H);
    free(R);
    free(P);
    if (C == NULL)
        exit(EXIT_FAILURE);

    int rez = exporta_vector(d, C, W, H);
    free(C);
    if (rez == -1) {
        printf("Salvarea imaginii criptate a esuat!\n");
        exit(EXIT_FAILURE);
    }
}

void decriptare(char *s, char *d, char *k) {
    pixel *C1 = NULL, *C2 = NULL, *D = NULL;
    uint W, H, R0, SV;
    uint *R = NULL, *perm = NULL;

    initializare(s, d, k, &C1, &R, &perm, &W, &H, &R0, &SV);
    C2 = decodificare(SV, C1, R, W * H);
    free(R);
    free(C1);
    if (C2 == NULL) {
        free(perm);
        exit(EXIT_FAILURE);
    }

    // permutam pixelii in functie de inversa permutarii generate
    D = inversa_permutarii(C2, perm, W * H);
    free(C2);
    free(perm);
    if (D == NULL)
        exit(EXIT_FAILURE);

    int rez = exporta_vector(d, D, W, H);
    free(D);
    if (rez == -1) {
        printf("Salvarea imaginii decriptate a esuat!\n");
        exit(EXIT_FAILURE);
    }
}

uint *frecventa_canal(uint c, pixel *L, uint dim) {
    uint *f = (uint*) calloc(256, sizeof(uint));
    if (f == NULL) {
        printf("Alocarea memoriei pentru vectorul de frecventa pe un canal de culoare a esuat.\n");
        return NULL;
    }
    uint i;
    for (i = 0; i < dim; i++)
        f[L[i].p[c]]++;
    return f;
}

double chi_element(uint fi, double fm) {
    return ((double) fi - fm) * ((double) fi - fm) / fm;
}

void chi_test(char *s) {
    uint W, H, i;
    pixel *L = liniarizare(s, &W, &H);
    if (L == NULL)
        exit(EXIT_FAILURE);
    uint *frecv_r, *frecv_g, *frecv_b;
    double fm, R = 0, G = 0, B = 0;

    frecv_r = frecventa_canal(2, L, W * H);
    frecv_g = frecventa_canal(1, L, W * H);
    frecv_b = frecventa_canal(0, L, W * H);
    if (frecv_r == NULL || frecv_g == NULL || frecv_b == NULL) {
        free(frecv_r);
        free(frecv_g);
        free(frecv_b);
        free(L);
        exit(EXIT_FAILURE);
    }
    fm = (double) W * H / 256;
    for (i = 0; i < 256; i++) {
        R += chi_element(frecv_r[i], fm);
        G += chi_element(frecv_g[i], fm);
        B += chi_element(frecv_b[i], fm);
    }
    free(frecv_r);
    free(frecv_g);
    free(frecv_b);
    free(L);
    printf("R: %lf\nG: %lf\nB: %lf\n\n", R, G, B);
}
