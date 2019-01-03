/*  Simion Cristian
 *  Fisierul contine definitiile complete ale tuturor subprogramelor
 *  din partea de recunoastere de pattern-uri intr-o imagine a proiectului
 */
#include "rec_pattern.h"

int filtru_grayscale(char *s, char *nume_fis_grayscale, char *path) {
    FILE *f, *h;
    f = fopen(s, "rb");
    if (f == NULL) {
        printf("Imagine de intrare lipsa!\n");
        return -1;
    }
    strcpy(nume_fis_grayscale, path);
    if (strchr(s, '/') != NULL) {
        strcat(nume_fis_grayscale, "grayscale_");
        strcat(nume_fis_grayscale, strrchr(s, '/') + 1);
    }
    else {
        strcat(nume_fis_grayscale, "grayscale_");
        strcat(nume_fis_grayscale, s);
    }
    h = fopen(nume_fis_grayscale, "wb");
    if (h == NULL) {
        fclose(f);
        printf("Crearea imaginii de iesire a esuat!\n");
        return -1;
    }
    pixel x;
    byte header[54], r, g, b;
    fread(header, sizeof(header), 1, f);
    fwrite(header, sizeof(header), 1, h);
    while (fread(&x, sizeof(x), 1, f) == 1) {
        b = x.p[0];
        g = x.p[1];
        r = x.p[2];
        x.p[0] = x.p[1] = x.p[2] = (byte) (0.299 * r + 0.587 * g + 0.114 * b);
        fwrite(&x, sizeof(x), 1, h);
        fflush(h);
    }
    fclose(f);
    fclose(h);
    return 0;
}

byte **matricizare_grayscale(char *nume_fis, uint *w, uint *h, uint offsetW, uint offsetH) {
    FILE *f = fopen(nume_fis, "rb");
    if (f == NULL) {
        printf("Imagine de intrare lipsa!\n");
        return NULL;
    }
    uint W, H, currentH, padding, i, j;
    byte **M;
    // cautam dimensiunile
    fseek(f, 18, SEEK_SET);
    fread(&W, sizeof(W), 1, f);
    fread(&H, sizeof(H), 1, f);
    M = (byte**) malloc((H + 2 * offsetH) * sizeof(byte*));
    if (M == NULL) {
        fclose(f);
        printf("Alocarea memoriei pentru matricizarea imaginii a esuat!\n");
        return NULL;
    }

    for (i = 0; i < H + 2 * offsetH; i++) {
        M[i] = (byte*) calloc((W + 2 * offsetW), sizeof(byte));
        if (M[i] == NULL) {
            fclose(f);
            for (j = 0; j < i; j++)
                free(M[j]);
            free(M);
            printf("Alocarea memoriei pentru matricizarea imaginii a esuat!\n");
            return NULL;
        }
    }

    padding = W % 4;
    currentH = H;
    fseek(f, 54, SEEK_SET);
    while (currentH > 0) {
        for (i = 0; i < W; i++) {
            fread(M[offsetH + currentH - 1] + offsetW + i, sizeof(byte), 1, f);
            fseek(f, 2, SEEK_CUR);
        }
        fseek(f, padding, SEEK_CUR);
        currentH--;
    }
    *w = W;
    *h = H;
    fclose(f);
    return M;
}

pixel **matricizare_color(char *nume_fis, uint *w, uint *h, uint offsetW, uint offsetH) {
    FILE *f = fopen(nume_fis, "rb");
    if (f == NULL) {
        printf("Imagine de intrare lipsa!\n");
        return NULL;
    }
    uint W, H, currentH, padding, i, j;
    pixel **M;
    // cautam dimensiunile
    fseek(f, 18, SEEK_SET);
    fread(&W, sizeof(W), 1, f);
    fread(&H, sizeof(H), 1, f);
    M = (pixel**) malloc((H + 2 * offsetH) * sizeof(pixel*));
    if (M == NULL) {
        fclose(f);
        printf("Alocarea memoriei pentru matricizarea imaginii a esuat!\n");
        return NULL;
    }

    for (i = 0; i < H + 2 * offsetH; i++) {
        M[i] = (pixel*) calloc((W + 2 * offsetW), sizeof(pixel));
        if (M[i] == NULL) {
            fclose(f);
            for (j = 0; j < i; j++)
                free(M[j]);
            free(M);
            printf("Alocarea memoriei pentru matricizarea imaginii a esuat!\n");
            return NULL;
        }
    }

    padding = W % 4;
    currentH = H;
    fseek(f, 54, SEEK_SET);
    while (currentH > 0) {
        fread(M[offsetH + currentH - 1] + offsetW, sizeof(pixel), W, f);
        fseek(f, padding, SEEK_CUR);
        currentH--;
    }
    *w = W;
    *h = H;
    fclose(f);
    return M;
}

void calculeaza_termeni_sablon(byte **S, uint n, uint Ws, uint Hs, double *sm, double *sigmaS) {
    uint suma_s = 0, i, j;
    for (i = 0; i < Hs; i++)
        for (j = 0; j < Ws; j++)
            suma_s += S[i][j];
    *sm = (double) suma_s / n;
    for (i = 0; i < Hs; i++)
        for (j = 0; j < Ws; j++)
            *sigmaS += (S[i][j] - *sm) * (S[i][j] - *sm);
    *sigmaS *= 1.0 / (n - 1);
    *sigmaS = sqrt(*sigmaS);
}

double calculeaza_corelatia(byte **F, byte **S, uint l, uint c, uint Hs, uint Ws, uint n, double sm, double sigmaS) {
    uint suma_f = 0, i, j;
    double sol = 0, fm, sigmaF = 0;
    for (i = 0; i < Hs; i++)
        for (j = 0; j < Ws; j++)
            suma_f += F[l + i][c + j];
    fm = (double) suma_f / n;
    for (i = 0; i < Hs; i++)
        for (j = 0; j < Ws; j++)
            sigmaF += (F[l + i][c + j] - fm) * (F[l + i][c + j] - fm);
    sigmaF *= 1.0 / (n - 1);
    sigmaF = sqrt(sigmaF);
    for (i = 0; i < Hs; i++)
        for (j = 0; j < Ws; j++)
            sol += (1.0 / (sigmaF * sigmaS)) * (F[l + i][c + j] - fm) * (S[i][j] - sm);
    sol *= 1.0 / n;
    return sol;
}

void template_matching(char *f, char *s, char *path, double ps, fereastra *D, uint *nr_ferestre, int cif, uint sterge_gs) {
    char fgs[101] = {0}, sgs[101] = {0};
    int a;
    a = filtru_grayscale(f, fgs, path);
    if (a == -1)
        exit(EXIT_FAILURE);
    a = filtru_grayscale(s, sgs, path);
    if (a == -1) {
        remove(fgs);
        exit(EXIT_FAILURE);
    }

    byte **F, **S;
    uint Ws, Hs, Wf, Hf, offsetW, offsetH, i, j;
    double corelatie;
    S = matricizare_grayscale(sgs, &Ws, &Hs, 0, 0);
    if (S == NULL) {
        remove(fgs);
        remove(sgs);
        exit(EXIT_FAILURE);
    }
    offsetW = Ws / 2;
    offsetH = Hs / 2;
    F = matricizare_grayscale(fgs, &Wf, &Hf, offsetW, offsetH);
    if (F == NULL) {
        remove(fgs);
        remove(sgs);
        for (i = 0; i < Hs; i++)
            free(S[i]);
        free(S);
        exit(EXIT_FAILURE);
    }
    /* calculam termenii expresiei care rezulta din sablon - acesta nu se schimba
     * pe parcursul testului deci nu este nevoie sa calculam mai mult de o data per sablon
     */
    uint n = Ws * Hs;
    double sm, sigmaS = 0;
    calculeaza_termeni_sablon(S, n, Ws, Hs, &sm, &sigmaS);
    for (i = 0; i < Hf; i++)
        for (j = 0; j < Wf; j++) {
            corelatie = calculeaza_corelatia(F, S, i, j, Hs, Ws, n, sm, sigmaS);
            if (corelatie > ps) {
                D[*nr_ferestre] = (fereastra) {.l1 = i, .c1 = j, .l2 = i + Hs - 1, .c2 = j + Ws - 1, .corr = corelatie, .cifra = cif, .valid = 1};
                (*nr_ferestre)++;
            }
        }
    for (i = 0; i < Hs; i++)
            free(S[i]);
    free(S);
    for (i = 0; i < Hf + 2 * offsetH; i++)
            free(F[i]);
    free(F);
    if (sterge_gs) {
        remove(fgs);
        remove(sgs);
    }
}

void preia_date(char *s, uint *W, uint *H) {
    FILE *f = fopen(s, "rb");
    if (f == NULL) {
        printf("Imagine de intrare lipsa!\n");
        exit(EXIT_FAILURE);
    }
    fseek(f, 18, SEEK_SET);
    fread(W, sizeof(uint), 1, f);
    fread(H, sizeof(uint), 1, f);
    fclose(f);
}

int cmp_corelatie(const void *a, const void *b) {
    fereastra A = *(fereastra*) a;
    fereastra B = *(fereastra*) b;
    if (A.corr > B.corr)
        return -1;
    if (A.corr < B.corr)
        return 1;
    return 0;
}

uint min(uint a, uint b) {
    if (a > b)
        return b;
    return a;
}

uint max(uint a, uint b) {
    if (a < b)
        return b;
    return a;
}

uint intersectie(fereastra A, fereastra B) {
    uint sus, jos, st, dr;
    sus = max(A.l1, B.l1);
    jos = min(A.l2, B.l2);
    st = max(A.c1, B.c1);
    dr = min(A.c2, B.c2);
    if (sus > jos || st > dr)
        return 0;
    return (jos - sus + 1) * (dr - st + 1);
}

uint reuniune(fereastra A, fereastra B) {
    uint ai = intersectie(A, B);
    uint arie_A = (A.l2 - A.l1 + 1) * (A.c2 - A.c1 + 1);
    uint arie_B = (B.l2 - B.l1 + 1) * (B.c2 - B.c1 + 1);
    return arie_A + arie_B - ai;
}

void elimina_non_maxime(fereastra *D, uint nr_f) {
    qsort(D, nr_f, sizeof(fereastra), cmp_corelatie);
    uint i, j, ai, ar;
    double sup;
    for (i = 0; i < nr_f; i++) {
        if (D[i].valid == 0)
            continue;
        for (j = i + 1; j < nr_f; j++) {
            if (D[j].valid == 0)
                continue;
            ai = intersectie(D[i], D[j]);
            ar = reuniune(D[i], D[j]);
            sup = (double) ai / ar;
            if (sup > 0.2)
                D[j].valid = 0;
        }
    }
}

void contur(pixel **M, fereastra F, pixel cul) {
    uint i, j;
    for (i = F.l1; i <= F.l2; i++)
        M[i][F.c1] = M[i][F.c2] = cul;
    for (j = F.c1; j <= F.c2; j++)
        M[F.l1][j] = M[F.l2][j] = cul;
}

void coloreaza(char *s, char *d, fereastra *D, uint nr_f) {
    pixel **M, culori[10] = {{{0, 0, 255}}, {{0, 255, 255}}, {{0, 255, 0}}, {{255, 255, 0}}, {{255, 0, 255}}, {{255, 0, 0}}, {{192, 192, 192}}, {{0, 140, 255}}, {{128, 0, 128}}, {{0, 0, 128}}};
    uint W, H, offsetW, offsetH, i;
    offsetW = (D[0].c2 - D[0].c1 + 1) / 2;
    offsetH = (D[0].l2 - D[0].l1 + 1) / 2;
    M = matricizare_color(s, &W, &H, offsetW, offsetH);
    if (M == NULL) {
        free(D);
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < nr_f; i++) {
        if (D[i].valid == 1)
            contur(M, D[i], culori[D[i].cifra]);
    }
    int rez = exporta_matrice(d, M, W, H, offsetW, offsetH);
    for (i = 0; i < H + 2 * offsetH; i++)
            free(M[i]);
    free(M);
    if (rez == -1) {
    	    printf("Salvarea imaginii pe care au fost desenate contururile ferestrelor a esuat!\n");
	    exit(EXIT_FAILURE);
    }
}

