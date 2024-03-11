#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/

void on_usigterm(int signal) {
	printf("Otrzymałem term jako potomny i wysle usr1\n");
}
void on_usr1(int signal) {
	printf("Dziecko wyslalo sygnal usr1\n");
}

int main()
{
    pid_t pid;
    printf("Moj PID = %d\n", getpid());

    switch(pid = fork()){
        case -1:
            return EXIT_FAILURE;
        case 0: {

            sigset_t mask; 
            struct sigaction sigTerm;
            sigemptyset(&mask); 
            sigTerm.sa_handler = (&on_usigterm);
            sigTerm.sa_mask = mask;
            sigTerm.sa_flags = SA_SIGINFO;
            sigaction(SIGTERM, &sigTerm, NULL);
            pause();
            kill(getppid(), SIGUSR1);
            return EXIT_SUCCESS;
        }
        default: {
            sigset_t mask; 
            struct sigaction sigUsr1;
            sigemptyset(&mask); 
            sigUsr1.sa_handler = (&on_usr1);
            sigUsr1.sa_mask = mask;
            sigUsr1.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1, &sigUsr1, NULL);
            sleep(1);
            kill(pid, SIGTERM);
            pause();
            if(wait(0) == -1)
            {
                fprintf(stderr, "Blad w wait\n");
                return EXIT_FAILURE;

            }
            return EXIT_SUCCESS;

        } 
    }

}