
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

char * FindinFilePortion(int start_pos, int max_lines, int patent_id);

int sockfd,n;
struct sockaddr_in servaddr,cliaddr;
char sendline[10000]={"helper"};
char recvline[10000];
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
	char * token=NULL;

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

   
   sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
   
   pthread_create (&thread_id, NULL, sendSignal, NULL);

   while(1)
   {

	n=recvfrom(sockfd,recvline,10000,0,NULL,NULL);

	printf("Received Query to search : %s \n",recvline);
	int start_pos=0, max_lines=0, patent_id=0;
		
	// Tokenization
	token = strtok(recvline, "#");
	start_pos=atoi(token);
		
	token = strtok(NULL, "#");
	max_lines=atoi(token);
	
	token = strtok(NULL, "#");
	patent_id=atoi(token);
	
	
	token=FindinFilePortion(start_pos, max_lines, patent_id);
	
	printf(" helper finished\n%s\n", token);
	
	sendto(sockfd,token,strlen(token),0,(struct sockaddr *)&servaddr,sizeof(servaddr));
      
	}
	return 0;
}


char * FindinFilePortion(int start_pos, int max_lines, int patent_id)
{
	char replytoclinet[10000];

	FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int index=0;
    char* token = NULL;	
    bool found=false;

    fp = fopen("file.txt", "r");
    if (fp == NULL)
    {
        printf("No file found");
        return;
    }
    
	fseek(fp, start_pos, SEEK_SET);
    bool first_time=true;

    strcpy(replytoclinet, "result#");
    while (index<max_lines) 
    {
    	if((read = getline(&line, &len, fp)) != -1)
    	{
    		char temp[1000];
			strcpy(temp, line);
    		token = strtok(line, ",");
    		
    		if(patent_id==atoi(token))
    		{
    			found=true;
    			strcat(replytoclinet, temp);
    		}

	    	/*printf("pos for testing=%d\n", start_pos+ read );
	    	printf("Retrieved line of length %zu :\n", read);
	    	printf("%s", line);*/
		}
	    index++;
    }

    sleep(10);

	fclose(fp);
	if(found==false)
	{
		strcpy(replytoclinet, "result#Not Found");
		return replytoclinet;
	}
	printf("in helper my result:::::%s\n", replytoclinet);

	return replytoclinet;
}
