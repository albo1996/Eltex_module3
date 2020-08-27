#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_MSG_LEN 4080
#define KEYPATH "09-10-server.c"
#define REQ_TYPE 1
#define MESSAGE "Hello from server"

int main()
{
	key_t key;
	int msqid;
	int len;
	
	struct request {
		long mtype;
		pid_t mpid;
	} myreq;

	struct response {
		long mtype;
		char mtext[MAX_MSG_LEN];
	} myres;

	key = ftok(KEYPATH, 0);
	if (key < 0) {
		printf("Ошибка получения ключа\n");
		exit(1);
	}

	msqid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);
	if (msqid < 0) {
		printf("Ошибка создания очереди сообщений\n");
		exit(2);
	}

	strcpy(myres.mtext, MESSAGE);

	while (1) {
		len = msgrcv(msqid, &myreq, sizeof(myreq.mpid), REQ_TYPE, 0);
		if (len < 0) {
			printf("Ошибка получения запроса\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
			exit(3);
		}

		myres.mtype = (long)myreq.mpid;
		if (msgsnd(msqid, &myres, sizeof(MESSAGE) + 1, 0) < 0) {
			printf("Ошибка отправки ответа\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
			exit(4);
		}
	}

	return 0;
}