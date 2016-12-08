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



main() {
	int key = ftok("a_file", 7);
	// sem part
	int semdesc = semget(key, 1, IPC_CREAT | 0644);
	struct sembuf hold = {0, -1, SEM_UNDO};  // will dec the sem, then inc on termination
	semop(semdesc, *hold, 1);
	// shm part
	int shmdesc = shmget(key, sizeof(int), 0644);
	int *linelen = shmat(shmdesc, 0, 0);
	// now we can do things
	if(*linelen) {
		int filedesc = open("story", O_RDONLY);
		lseek(filedesc, -(*linelen), SEEK_END);
		char *lastline = malloc(*linelen + 1);
		read(filedesc, lastline, *linelen + 1);
		close(filedesc);  // done with reading from file
		printf("Last line:\n%s\n", lastline);
	}
	else printf("Starting a new story\n");