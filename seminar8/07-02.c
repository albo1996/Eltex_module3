#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main()
{
	int* array;
	int shmid;
	int new = 1;
	char pathname[] = "08-07-01.c";
	key_t key;
	long i;
	int semid;
	key_t semkey;
	struct sembuf seminc;
	struct sembuf semdec;

	seminc.sem_num = 0;
	seminc.sem_op = 1;
	seminc.sem_flg = 0;

	semdec.sem_num = 0;
	semdec.sem_op = -1;
	semdec.sem_flg = 0;

	semkey = ftok(pathname, 1);
	if (semkey < 0) {
		printf("Ошибка получения ключа семафора\n");
		exit(1);
	}

	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
	if (semid < 0) {
		if (errno != EEXIST) {
			printf("Ошибка получения семафора\n");
			exit(2);
		}

		semid = semget(semkey, 1, 0);
		if (semid < 0) {
			printf("Ошибка получения семафора\n");
			exit(2);
		}
	}
	else {
		struct sembuf initop;
		initop.sem_num = 0;
		initop.sem_op = 1;
		initop.sem_flg = 0;

		if (semop(semid, &initop, 1) < 0) {
			printf("Ошибка инициализации семафора\n");
			exit(5);
		}
	}

	key = ftok(pathname, 0);
	if (key < 0) {
		printf("Ошибка получения ключа\n");
		exit(1);
	}

	shmid = shmget(key, 3 * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid < 0) {
		if (errno != EEXIST) {
			printf("Ошибка получения разделяемой памяти\n");
			exit(2);
		}

		shmid = shmget(key, 3 * sizeof(int), 0);
		if (shmid < 0) {
			printf("Ошибка получения разделяемой памяти\n");
			exit(2);
		}

		new = 0;
	}

	array = shmat(shmid, NULL, 0);
	if (array == (int*) -1) {
		printf("Ошибка отображения разделяемой памяти\n");
		exit(3);
	}

	if (new) {
		array[0] = 0;
		array[1] = 1;
		array[2] = 1;
	}
	else {
		array[1] += 1;

		for(i = 0; i < 10000000000L; i++)

		if (semop(semid, &semdec, 1) < 0) {
			printf("Ошибка занятия семафора\n");
			exit(6);
		}

		array[2] += 1;

		if (semop(semid, &seminc, 1) < 0) {
			printf("Ошибка освобождения семафора\n");
			exit(7);
		}
	}

	printf(	"Программа 1 была запущена %d раз\n"
		"Программа 2 была запущена %d раз\n"
		"Обе программы были запущены %d раз\n",
		array[0], array[1], array[2]);

	if (shmdt(array) < 0) {
		printf("Ошибка удаления отображения\n");
		exit(4);
	}

	return 0;
}