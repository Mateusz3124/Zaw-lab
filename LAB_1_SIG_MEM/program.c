#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

int *divisions;
int *pidTable;
int *num_vec;
int num_procs;
int num_numbers;

int *primeFind(int iteration)
{
    int number_of_all_numbers = num_numbers * 8 * iteration;
    int number_of_primes = 0;
    int *table_of_all_numbers = malloc(number_of_all_numbers * sizeof(int));

    for (int i = 0; i < number_of_all_numbers; i++)
    {
        table_of_all_numbers[i] = i + 2;
    }

    int max_value = table_of_all_numbers[number_of_all_numbers - 1];
    for (int i = 0; i < number_of_all_numbers; i++)
    {
        if (table_of_all_numbers[i] == -1)
        {
            continue;
        }
        int value = table_of_all_numbers[i];
        number_of_primes++;
        for (int j = value + value; j <= max_value; j += value)
        {
            table_of_all_numbers[j - 2] = -1;
        }
    }
    int *table_of_prime_numbers;
    if (number_of_primes < num_numbers)
    {
        table_of_prime_numbers = primeFind(iteration + 1);
        free(table_of_all_numbers);
        return table_of_prime_numbers;
    }
    table_of_prime_numbers = malloc(number_of_primes * sizeof(int));
    int count = 0;
    for (int i = 0; i < number_of_all_numbers; i++)
    {
        if (table_of_all_numbers[i] != -1)
        {
            table_of_prime_numbers[count] = table_of_all_numbers[i];
            count++;
        }
    }
    free(table_of_all_numbers);

    return table_of_prime_numbers;
}

long long unsigned sum(int itteration)
{
    long long unsigned s = 0;
    int offset = 0;
    for (int i = 0; i < itteration; i++)
    {
        offset += divisions[i];
    }
    for (int i = offset; i < offset + divisions[itteration]; ++i)
    {
        s += num_vec[i];
    }
    return s;
}

void on_usr1(int signal)
{
    printf("Otrzymałem USR1\n");
}

int spawn_n_procs(int master_pid, unsigned long long **shared_array)
{
    pid_t pid;
    for (int i = 0; i < num_procs; ++i)
    {
        switch (pid = fork())
        {
        case -1:
        {
            printf("Error in fork\n");
            return -1;
        }
        case 0:
        {
            struct sigaction usr1;
            sigset_t mask;
            sigemptyset(&mask);
            usr1.sa_handler = (&on_usr1);
            usr1.sa_mask = mask;
            usr1.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1, &usr1, NULL);
            pause();
            long long unsigned sum_result = sum(i);
            printf(" Hello from process: %d;sum: %lld\n", getpid(), sum_result);
            *shared_array[i] = sum_result;
            return 1;
        }
        default:
        {
            pidTable[i] = pid;
            break;
        }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    pid_t pid;
    pid = getpid();
    if (argc < 3)
    {
        printf("wrong input must be like this: ./a.out [number of processes] [number of prime numbers]");
        return -1;
    }

    num_procs = atoi(argv[1]);
    num_numbers = atoi(argv[2]);
    if(num_numbers > 20000000){
        printf("too many numbers to count");
        return -1;
    }
    if(num_procs > 2000){
        printf("too many processes");
        return -1;
    }
    if (num_procs <= 0 || num_numbers <= 0)
    {
        printf("wrong input must be like this: ./a.out [number of processes] [number of prime numbers]");
        return -1;
    }
    if (num_procs > num_numbers)
    {
        printf("too many processes, more than numbers");
        return -1;
    }

    pidTable = malloc(num_procs * sizeof(int));
    divisions = (int *)malloc(num_procs * sizeof(int));
    int size_of_division = num_numbers / num_procs;

    for (int i = 0; i < num_procs; i++)
    {
        divisions[i] = size_of_division;
    }

    int rest = num_numbers - num_procs * size_of_division;
    int counter = 0;
    for (int i = rest; i > 0; i--)
    {
        divisions[counter] += 1;
        counter++;
        if (counter == num_procs)
        {
            counter = 0;
        }
    }

    unsigned long long *shared_array[num_procs];
    for (int i = 0; i < num_procs; i++)
    {
        shared_array[i] = mmap(NULL, sizeof(unsigned long long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }
    num_vec = mmap(NULL, num_numbers * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int result = spawn_n_procs(pid, shared_array);
    if (result != 0)
    {
        free(divisions);
        free(pidTable);
        return result;
    }
    free(divisions);
    sleep(1);
    int *holder = primeFind(1);
    for (int i = 0; i < num_numbers; i++)
    {
        num_vec[i] = holder[i];
    }
    free(holder);
    for (int j = 0; j < num_procs; j++)
    {
        kill(pidTable[j], SIGUSR1);
    }

    for (int i = 0; i < num_procs; ++i)
    {
        if (wait(0) == -1)
            printf("Error in wait\n");
    }

    unsigned long long sum = 0;
    for (int i = 0; i < num_procs; ++i)
    {
        sum += *shared_array[i];
    }
    for (int i = 0; i < num_procs; i++) {
        munmap(shared_array[i], sizeof(unsigned long long));
    }
    munmap(num_vec, sizeof(int));
    free(pidTable);
    printf("sum of everything: %lld\n", sum);
}