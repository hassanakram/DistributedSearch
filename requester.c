
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

int sockfd,n;
struct sockaddr_in servaddr,cliaddr;
char sendline[1000];
char recvline[1000];
pthread_t thread_id;
char signal[100]={"signal"};
void * sendSignal()
{
   while(1)
   {
      sendto(sockfd,signal,strlen(signal),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
      sleep(1);
   }
}

int main(int argc, char**argv)
{
	if (argc != 2)
	{
		printf("usage:  udpcli <IP address>\n");
		exit(1);
	}

	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_port=htons(32000);

	/*fgets(sendline, 10000, stdin);
	sendto(sockfd,sendline,strlen(sendline),0,
	     (struct sockaddr *)&servaddr,sizeof(servaddr));
	n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
	recvline[n]=0;
	fputs(recvline,stdout);
	*/
	
	strcpy(sendline,"requester#");

	char patent[20];
	printf("Enter patent ID : ");
	fgets(patent, 20,stdin);
	printf("\n");

	strcat(sendline,patent);
	int size=strlen(sendline);
	printf("length : %d \n",size );
	sendline[size]='\0';

	sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));

	pthread_create (&thread_id, NULL, sendSignal, NULL);

	n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);
	recvline[n]=0;

	fputs(recvline,stdout);

	printf("/nhahahahhahahahahhahahahahhahahah :P :P :P :P\n");
}
