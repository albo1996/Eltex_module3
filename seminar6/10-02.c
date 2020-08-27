#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int shmid;
	key_t key;
	char* buf;
	int i = 0;

	key = ftok("06-10-01.c", 0);
	if (key < 0) {
		printf("Ошибка генерации ключа\n");
		exit(1);
	}

	shmid = shmget(key, 4096, 0);
	if (shmid < 0) {
		printf("Ошибка получения дескриптора разделяемой памяти\n");
		exit(2);
	}

	buf = shmat(shmid, NULL, 0);
	if (buf == (char*)-1) {
		printf("Ошибка отображения разделяемой памяти\n");
		exit(3);
	}

	while(buf[i] != 0) {
		putchar(buf[i]);
		i++;
	}

	if (shmdt((void*)buf) < 0) {
		printf("Ошибка закрытия отображения памяти\n");
		exit(4);
	}

	if (shmctl(shmid, IPC_RMID, NULL) < 0) {
		printf("Ошибка удаления разделяемой памяти\n");
		exit(5);
	}
}