#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

char buf[4096];
int textfd;

void quit()
{
	if (close(textfd) < 0) {
		printf("Ошибка закрытия файла исходного кода\n");
		exit(2);
	}
}

int main()
{
	ssize_t read_cnt;
	int shmid;
	key_t key;
	char* text;

	textfd = open("06-10-01.c", O_RDONLY);
	if (textfd < 0) {
		printf("Ошибка открытия исходного кода");
		exit(1);
	}

	read_cnt = read(textfd, buf, 4096);
	if (read_cnt < 0) {
		printf("Ошибка чтения открытого файла\n");
		quit();
	}

	key = ftok("06-10-01.c", 0);
	if (key < 0) {
		printf("Ошибка создания ключа разделяемой памяти\n");
		quit();
	}

	shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666);
	if (shmid < 0) {
		printf("Ошибка создания разделяемой памяти\n");
		quit();
	}

	text = shmat(shmid, NULL, 0);
	if (text == (char*)-1) {
		printf("Ошибка отображения разделяемой памяти\n");
		quit();
	}

	for(int i = 0; i < 4096; i++)
		text[i] = 0;

	for(int i = 0; i < read_cnt; i++)
		text[i] = buf[i];

	if (shmdt((void*)text) < 0) {
		printf("Ошибка закрытия отображения разделяемой памяти\n");
		quit();
	}

	quit();
}