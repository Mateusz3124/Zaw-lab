#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/

void on_usr1(int signal) {
	printf("Otrzymałem USR1 jako potomny\n");
}


int main()
{
    pid_t pid;
    printf("Moj PID = %d\n", getpid());

    switch(pid = fork()){
        case -1:
            return EXIT_FAILURE;
        case 0: /*proces potomny*/;
            sigset_t mask; /* Maska sygnałów */
            struct sigaction usr1;
            sigemptyset(&mask); /* Wyczyść maskę */
            usr1.sa_handler = (&on_usr1);
            usr1.sa_mask = mask;
            usr1.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1, &usr1, NULL);
            pause();
            return EXIT_SUCCESS;
        default:
            /*Oczekiwanie na zakończenie procesu potomnego*/
            sleep(1);
            kill(pid, SIGUSR1);
            if(wait(0) == -1)
            {
                fprintf(stderr, "Blad w wait\n");
                return EXIT_FAILURE;

            }
            return EXIT_SUCCESS;

    }

}