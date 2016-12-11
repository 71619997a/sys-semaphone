#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>


int main() {
    if (access("story", R_OK | W_OK) == 0) {
        int key = ftok("a_file", 7);
        // sem part
        int semdesc = semget(key, 1, IPC_CREAT | 0644);
        struct sembuf hold = {0, -1, SEM_UNDO};  // will dec the sem, then inc on termination
        printf("Waiting for other writers to finish...\n");
        semop(semdesc, &hold, 1);

        // shm part
        int shmdesc = shmget(key, sizeof(int), 0644);
        int *linelen = shmat(shmdesc, 0, 0);

        // As the \n is counted in *linelen, it will only be 0 when there was
        // no previously stored line:
        if(*linelen) {
            int filedesc = open("story", O_RDONLY);
            lseek(filedesc, -(*linelen), SEEK_END);
            char *lastline = calloc(*linelen + 1, sizeof(char));
            read(filedesc, lastline, *linelen + 1);
            close(filedesc);  // done with reading from file
            printf("Last line:\n%s", lastline);
        } else {
            printf("Starting a new story\n");
        }
        // Read a line and append it to the file:
        char line[256];
        fgets(line, 256, stdin);
        int fd = open("story", O_APPEND | O_WRONLY);
        write(fd, line, strlen(line));
        close(fd);
        // Update the shared memory segment:
        *linelen = strlen(line);
        shmdt(linelen);
    } else {
        printf("'story' file does not exist or we do not have permission to use it.\n");
    }
}
