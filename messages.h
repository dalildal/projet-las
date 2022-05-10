#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1" /* localhost */
#define MAX_PSEUDO 256

#define VIREMENT 10
#define VIREMENT_REC 11
#define VIREMENT_OK 12
#define VIREMENT_KO 13

/* struct message used between server and client */
typedef struct
{
  char messageText[MAX_PSEUDO];
  int code;
} StructMessage;

#endif
