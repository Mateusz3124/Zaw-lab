#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct ThreadResults_T
{
    double sum_of_squares;
} ThreadResults;

typedef struct ThreadArgs_T
{
    int thread_num;
    int* works_per_group;
    double** mat_A;
    double** mat_B;
    double** mat_Results;
    int rows_A, cols_A, cols_B;
    ThreadResults results;
} ThreadArgs;

double sum_of_Elements_Of_Matrix_Result = 0;

void mnoz_thread(ThreadArgs *args)
{
    int start_row_A = 0;
    for (int i = 0; i < args->thread_num; i++)
    {
        start_row_A += args->works_per_group[i];
    }

    double sum_of_squares = 0.0;
    for (int i = start_row_A; i < start_row_A + args->works_per_group[args->thread_num]; i++)
    {
        for(int column = 0; column < args->cols_B; column++){
            double sum = 0;
            for (int k = 0; k < args->cols_A; k++)
            {
                sum += args->mat_A[i][k] * args->mat_B[k][column];
            }
            args->mat_Results[i][column] = sum;
            sum_of_Elements_Of_Matrix_Result += sum;
            sum_of_squares += sum * sum;
        }
    }
    args->results.sum_of_squares = sum_of_squares;
}

double mnoz(int num_threads, double** mat_A, int rows_A, int cols_A, double** mat_B, int rows_B, int cols_B, double** mat_Results)
{
    pthread_t threads[num_threads];
    int* works_per_group = (int*)malloc(num_threads * sizeof(int));
    int work_size = rows_A / num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        works_per_group[i] = work_size;
    }

    int rest = rows_A - num_threads * work_size;
    for (int i = 0; i < rest; i++)
    {
        works_per_group[i % num_threads]++;
    }

    ThreadArgs args[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        ThreadArgs arg = {
            .thread_num = i,
            .works_per_group = works_per_group,
            .mat_A = mat_A,
            .mat_B = mat_B,
            .mat_Results = mat_Results,
            .rows_A = rows_A,
            .cols_A = cols_A,
            .cols_B = cols_B,
            .results = {
                .sum_of_squares = 0.0
            }
        };
        args[i] = arg;
        pthread_create(&threads[i], NULL, mnoz_thread, &args[i]);
    }

    double sum_of_squares = 0.0;
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
        sum_of_squares += args[i].results.sum_of_squares;
    }

    return sqrt(sum_of_squares);
}

void print_matrix(double** A, int m, int n)
{
    printf("[");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%f ", A[i][j]);
        }
        printf("\n");
    }
    printf("]\n");
}

int main(int argc, char** argv)
{
    FILE* file_a;
    FILE* file_b;
    double** mat_A;
    double** mat_B;
    double** mat_Results;
    int rows_A, rows_B, cols_A, cols_B;

    file_a = fopen("A.txt", "r");
    file_b = fopen("B.txt", "r");
    if (file_a == NULL || file_b == NULL)
    {
        perror("błąd otwarcia pliku");
        exit(-10);
    }

    fscanf(file_a, "%d", &rows_A);
    fscanf(file_a, "%d", &cols_A);

    fscanf(file_b, "%d", &rows_B);
    fscanf(file_b, "%d", &cols_B);

    printf("pierwsza macierz ma wymiar %d x %d, a druga %d x %d\n", rows_A, cols_A, rows_B, cols_B);
    printf("Rozmiar macierzy wynikowej: %dx%d\n", rows_A, cols_B);

    if (argc < 2)
    {
        printf("złe wywołanie. ./a <num_threads>");
        return -1;
    }
    int num_threads = atoi(argv[1]);
    if (num_threads < 1 || num_threads > rows_A)
    {
        printf("złe wywołanie. ./a <num_threads>");
        return -1;
    }

    if (cols_A != rows_B)
    {
        printf("Złe wymiary macierzy!\n");
        return EXIT_FAILURE;
    }

    mat_A = malloc(rows_A * sizeof(double));
    mat_Results = malloc(rows_A * sizeof(double));
    for (int i = 0; i < rows_A; i++)
    {
        mat_A[i] = malloc(cols_A * sizeof(double));
        mat_Results[i] = malloc(cols_B * sizeof(double));
    }

    mat_B = malloc(rows_B * sizeof(double));
    for (int i = 0; i < rows_B; i++)
    {
        mat_B[i] = malloc(cols_B * sizeof(double));
    }

    for (int i = 0; i < rows_A; i++)
    {
        for (int j = 0; j < cols_A; j++)
        {
            double x;
            fscanf(file_a, "%lf", &x);
            mat_A[i][j] = x;
        }
    }

    for (int i = 0; i < rows_B; i++)
    {
        for (int j = 0; j < cols_B; j++)
        {
            double x;
            fscanf(file_b, "%lf", &x);
            mat_B[i][j] = x;
        }
    }

    double frobenius_norm = mnoz(num_threads, mat_A, rows_A, cols_A, mat_B, rows_B, cols_B, mat_Results);

    printf("A:\n");
    print_matrix(mat_A, rows_A, rows_B);
    printf("B:\n");
    print_matrix(mat_B, rows_B, cols_B);
    printf("Wynik:\n");
    print_matrix(mat_Results, rows_A, cols_B);
    printf("Suma elementów wyniku: %f\n", sum_of_Elements_Of_Matrix_Result);
    printf("Frobenius norm: %f\n", frobenius_norm);
    

    for (int i = 0; i < cols_B; i++)
    {
        free(mat_B[i]);
        free(mat_Results[i]);
    }
    for (int i = 0; i < cols_A; i++)
    {
        free(mat_A[i]);
    }
    free(mat_A);
    free(mat_B);
    free(mat_Results);
    fclose(file_a);
    fclose(file_b);

    return EXIT_SUCCESS;
}
