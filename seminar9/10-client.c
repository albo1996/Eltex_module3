#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_MSG_LEN 4080
#define KEYPATH "09-10-server.c"
#define REQUEST_TYPE 1

int main()
{
	pid_t pid;
	key_t key;
	int msqid;
	int len;
	
	struct request {
		long mtype;
		pid_t mpid;
	} myrequest;

	struct response {
		long mtype;
		char mtext[MAX_MSG_LEN];
	} myresponse;

	pid = getpid();

	key = ftok(KEYPATH, 0);
	if (key < 0) {
		printf("Ошибка получения ключа\n");
		exit(1);
	}

	msqid = msgget(key, 0);
	if (msqid < 0) {
		printf("Не удалось установить связь с сервером\n");
		exit(2);
	}

	myrequest.mtype = REQUEST_TYPE;
	myrequest.mpid = pid;

	if (msgsnd(msqid, &myrequest, sizeof(myrequest.mpid), 0) < 0) {
		printf("Ошибка отправки запроса\n");
		exit(3);
	}

	len = msgrcv(msqid, &myresponse, MAX_MSG_LEN, pid, 0);
	if (len < 0) {
		printf("Ошибка получения ответа от сервера\n");
		exit(4);
	}

	printf("Получено: %s\n", myresponse.mtext);

	return 0;
}