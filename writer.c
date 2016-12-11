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
        printf("about to semop\n");
        semop(semdesc, &hold, 1);
        // shm part
        int shmdesc = shmget(key, sizeof(int), 0644);
        printf("aboutt to shmat\n");
        int *linelen = shmat(shmdesc, 0, 0);
        // now we can do things
        printf("*linelen: %d\n", *linelen);
        if(*linelen) {
            int filedesc = open("story", O_RDONLY);
            lseek(filedesc, -(*linelen), SEEK_END);
            char *lastline = calloc(*linelen + 1, sizeof(char));
            read(filedesc, lastline, *linelen + 1);
            printf("len of lastline: %d\n", strlen(lastline));
            close(filedesc);  // done with reading from file
            printf("Last line:\n%s", lastline);
        } else {
            // As *linelen includes the \n, it will only be 0 when there was no previously stored line:
            printf("Starting a new story\n");
        }
        // Read a line and append it to the file:
        char line[256];
        fgets(line, 256, stdin);
        int fd = open("story", O_APPEND | O_WRONLY);
        write(fd, line, strlen(line));
        printf("errno: %d. strified: %s. fd: %d\n", errno, strerror(errno), fd);
        close(fd);
        // Update shared memory segment:
        *linelen = strlen(line);
        shmdt(linelen);
    } else {
        printf("'story' file does not exist or we do not have permission to use it.\n");
    }
}
