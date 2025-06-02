#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv){	
	int start, status, pid, n;
	int buffer[1];
	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
  	/* TO COMPLETE */
	n=atoi(argv[1]);
	buffer[0]=atoi(argv[2]);
	start=atoi(argv[3]);

	int pipefds[n+1][2];
    int result;
    int reading;
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer[0], start);

	for (int i = 0; i < n+1; i++) {
		if (pipe(pipefds[i]) == -1) {
			perror("pipe error");
			exit(1);
		}
	}
    write(pipefds[0][1], &buffer[0], sizeof(int));
	for (int i=1; i<n+1; i++) {	
        pid_t forkResult = fork();
        if (forkResult < 0) {
            perror("fork error");
            exit(1);
        }
        if (forkResult==0){
            for (int j = 0; j < n + 1; j++) {       //cerrar todas las pipes menos dos
                if (j != i - 1) close(pipefds[j][0]);
                if (j != i)     close(pipefds[j][1]);
            }
            read(pipefds[i-1][0], &reading, sizeof(int));
            printf("Proceso %i: Leo caracter %i del pipe anterior. Sumo 1 y escribo en mi pipe \n",start+i, reading);
            reading=reading + 1;
            int writing = write(pipefds[i][1], &reading, sizeof(int));
            close(pipefds[i - 1][0]);
            close(pipefds[i][1]);    
            exit(0);
        }

	}
    for (int i=0; i<n+1; i++) {
        wait(NULL);
    }
    // cerrar pipes
    for (int i = 0; i < n + 1; i++) {
        close(pipefds[i][1]);
        if (i != n)
            close(pipefds[i][0]);  // solo guardar este
    }
    
    read(pipefds[n][0], &result, sizeof(int));
    printf("Proceso %i: El resultado es %i\n", start, result);

}