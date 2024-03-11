#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct ThreadArgs_T
{
    int id;
    int *divisions;
    double **A;
    double **B;
    double **C;
    int row_a, col_b, col_a;
} ThreadArgs;

void mnoz_thread(ThreadArgs *args)
{
    int start = 0;
    for (int i = 0; i < args->id; i++)
    {
        start += args->divisions[i];
    }
    for (int i = start; i < start + args->divisions[args->id]; i++)
    {
        double s = 0;
        for (int k = 0; k < args->col_a; k++)
        {
            s += args->A[i][k] * args->B[k][i];
                
        }
        args->C[i][i] = s;
    }
}

void mnoz(int num_threads, double **A, int a, int b, double **B, int c, int d, double **C)
{
    int i, j, k;
    pthread_t threads[num_threads];
    int *divisions = (int *)malloc(num_threads * sizeof(int));
    int size_of_division = a / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        divisions[i] = size_of_division;
    }

    int rest = a - num_threads * size_of_division;
    int counter = 0;
    for (int i = rest; i > 0; i--)
    {
        divisions[counter] += 1;
        counter++;
        if (counter == num_threads)
        {
            counter = 0;
        }
    }

    ThreadArgs args[num_threads];
    for (i = 0; i < num_threads; i++)
    {
        args[i].id = i;
        args[i].divisions = divisions;
        args[i].A = A;
        args[i].B = B;
        args[i].C = C;
        args[i].row_a = a;
        args[i].col_a = b;
        args[i].col_b = d;
        pthread_create(&threads[i], NULL, mnoz_thread, &args[i]);
    }
    for (i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
}

print_matrix(double **A, int m, int n)
{
    int i, j;
    printf("[");
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
}
int main(int argc, char **argv)
{
    FILE *fpa;
    FILE *fpb;
    double **A;
    double **B;
    double **C;
    int ma, mb, na, nb;
    int i, j;
    double x;

    fpa = fopen("A.txt", "r");
    fpb = fopen("B.txt", "r");
    if (fpa == NULL || fpb == NULL)
    {
        perror("błąd otwarcia pliku");
        exit(-10);
    }

    fscanf(fpa, "%d", &ma);
    fscanf(fpa, "%d", &na);

    fscanf(fpb, "%d", &mb);
    fscanf(fpb, "%d", &nb);

    printf("pierwsza macierz ma wymiar %d x %d, a druga %d x %d\n", ma, na, mb, nb);

    if (na != mb)
    {
        printf("Złe wymiary macierzy!\n");
        return EXIT_FAILURE;
    }

    /*Alokacja pamięci*/
    A = malloc(ma * sizeof(double));
    for (i = 0; i < ma; i++)
    {
        A[i] = malloc(na * sizeof(double));
    }

    B = malloc(mb * sizeof(double));
    for (i = 0; i < mb; i++)
    {
        B[i] = malloc(nb * sizeof(double));
    }

    /*Macierz na wynik*/
    C = malloc(ma * sizeof(double));
    for (i = 0; i < ma; i++)
    {
        C[i] = malloc(nb * sizeof(double));
    }

    printf("Rozmiar C: %dx%d\n", ma, nb);
    for (i = 0; i < ma; i++)
    {
        for (j = 0; j < na; j++)
        {
            fscanf(fpa, "%lf", &x);
            A[i][j] = x;
        }
    }

    printf("A:\n");
    print_matrix(A, ma, mb);

    for (i = 0; i < mb; i++)
    {
        for (j = 0; j < nb; j++)
        {
            fscanf(fpb, "%lf", &x);
            B[i][j] = x;
        }
    }

    printf("B:\n");
    print_matrix(B, mb, nb);
    if (argc < 2)
    {
        printf("not enough arguments. ./a <num_threads>");
    }
    int number_threads = atoi(argv[1]);
    number_threads = 2;
    if (number_threads < 1)
    {
        printf("not enough arguments. ./a <num_threads>");
    }
    mnoz(number_threads, A, ma, na, B, mb, nb, C);

    printf("C:\n");
    print_matrix(C, ma, nb);

    for (i = 0; i < na; i++)
    {
        free(A[i]);
    }
    free(A);

    for (i = 0; i < nb; i++)
    {
        free(B[i]);
    }
    free(B);

    for (i = 0; i < nb; i++)
    {
        free(C[i]);
    }
    free(C);

    fclose(fpa);
    fclose(fpb);

    return 0;
}