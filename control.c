#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

const int CREATE = 0;
const int REMOVE = 1;
const int VIEW = 2;

int printFile(int fd) {
}

int main(int argc, char *argv[]) {
    int mode = VIEW;  // default
    if(argc > 1) {
        if(!strcmp(argv[1], "-r")) {
            mode = REMOVE;
        }
        else if(!strcmp(argv[1], "-c")) {
            mode = CREATE;
        }
    }
    int key = ftok("a_file", 7);
    int shmdesc = shmget(key, sizeof(int), IPC_CREAT | 0644);
    int semdesc = semget(key, 1, IPC_CREAT | 0644);
    if(mode == CREATE) {
        printf("Creating file...\n");
        close(creat("story", 0644));  // creat has correct flags
        printf("Initializing shared memory segment...\n");
        int *linelen = shmat(shmdesc, 0, 0);
        *linelen = 0;
        shmdt(linelen);
        printf("Initializing semaphore...\n");
        union semun msg;
        msg.val = 1;
        semctl(semdesc, 0, SETVAL, msg);
    } else if(mode == REMOVE){
        printf("Removing semaphore...\n");
        // Remove the shm and sem
        semctl(semdesc, 0, IPC_RMID);
        printf("Removing shared memory segment...\n");
        struct shmid_ds meta;
        shmctl(shmdesc, IPC_RMID, &meta);
        printf("Story:\n");
        int filedesc = open("story", O_RDONLY);
        printFile(filedesc);
        close(filedesc);
    } else {  // mode == VIEW
        printf("Story:\n");
        int filedesc = open("story", O_RDONLY);
        printFile(filedesc);
        close(filedesc);
    }
    return 0;
}
