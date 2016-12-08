#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};


main(int argc, char *argv[]) {
    int create = 1;
    if(argc > 1 && !strcmp(argv[1], "-r")) {
        create = 0;
    }
    int key = ftok("a_file", 57457);
    int shmdesc = shmget(key, sizeof(int), IPC_CREAT | 0644);
    int semdesc = semget(key, 1, IPC_CREAT | 0644);
    if(create) {
        int filedesc = creat("story", 0644);
        int *linelen;
        shmat(shmdesc, linelen, 0);
        *linelen = 0;
        shmdt(linelen);
        union semun msg;
        msg.val = 1;
        semctl(semdesc, 0, IPC_SET, msg);
    } else {
        // Remove the shm and sem
        semctl(semdesc, 0, IPC_RMID);
        struct shmid_ds meta;
        shmctl(shmdesc, IPC_RMID, &meta);
        int filedesc = open("story", O_RDONLY);
        // TODO: print story
        close(filedesc);
    }
    return 0;
}
