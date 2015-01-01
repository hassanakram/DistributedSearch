#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
pthread_mutex_t Messagelock;
pthread_mutex_t BusyHelperlock;
pthread_mutex_t FreeHelperlock;
pthread_mutex_t JobQueuelock;
pthread_mutex_t CompletedJobQueuelock;
pthread_mutex_t Requesterlock;


pthread_t message_thread_id;
pthread_t jobsassignengine_thread_id;


// This struct is for receiving message from clients via socket and sending it to processMessage
struct Message
{
    struct sockaddr_in addr;
    char mesg[10000];
    int size;
};


//This struct represents a job
struct Job
{
  int requesterID;
  int jobNum;
  int byteNum;
  int numOfLines;
  int patent_id;
  bool status;
  char result[10000];
};


//This struct is for helperCLient which will search the portion and return result via socket
struct HelperClient
{
    int helper_id;
    pthread_t thread_id;
    pthread_t status_thread;
    struct sockaddr_in helper_cliaddr;
    bool status;
    struct Job *job;
    bool state;
    //to be added
};

//This struct represents the clients who send queury to search
struct RequesterClient
{
  int patent_id;
  pthread_t thread_id;
  struct sockaddr_in requester_cliaddr;
  bool status;
  struct Part *parts;
  int numOfParts;
  pthread_t status_thread;
  //to be added
};


//To check equalance between two addresses 
bool isEqual(struct sockaddr_in a,struct sockaddr_in b)
{ 
  return ((a.sin_addr.s_addr == b.sin_addr.s_addr)&&(a.sin_port == b.sin_port));
}

#include "free.c"
#include "busy.c"
#include "messages.c"
#include "requesters.c"
#include "jobs.c"
#include "completedjobs.c"
#include "splitJob.c"



void * helper_thread(void * temp);

int sock,n;

struct sockaddr_in main_servaddr, main_cliaddr;


int helper_index=0, requester_index=0;



//This Function extracts jobs from Job (To do) Queue and assign these jobs to helper clients if available
void *jobsAssignEngine()
{
	while(1)
	{
		if(JobsQueuecount>0 && FreeHelpercount>0)
		{


	  		struct Job *jobb= NULL;
	  		do
		  	{
		  		pthread_mutex_lock(&JobQueuelock);
		  			jobb= JobsQueueDeq();
		  		pthread_mutex_unlock(&JobQueuelock);
		  	}
		  	while(jobb==NULL);

	  		struct HelperClient *helper=NULL;
	  		do
		  	{
		  		pthread_mutex_lock(&FreeHelperlock);
			  		helper= FreeHelperDeq();
			  	pthread_mutex_unlock(&FreeHelperlock);
		  	}
		  	while(helper==NULL);
		  	helper->state=true;
		  	helper->job=jobb;
		  	pthread_create (&helper->thread_id, NULL, helper_thread, helper);
		}
	}
}



//This thread divides the file into chunks according to available helpers and push all jobs to Job Queue
// and then wait for all parts to be completed
void * requester_thread(void * temp)
{

  struct RequesterClient *rq = ((struct RequesterClient *) temp); 
  
  pthread_t index = rq->thread_id;
  int total_jobs=FreeHelpercount+2;

  struct Part *dividedJobs=NULL;
  
  int input = rq->patent_id;
  dividedJobs=MakeFileChunks("file.txt" ,total_jobs);
  
  rq->parts=dividedJobs;
  rq->numOfParts=total_jobs;

  int i=0;

  while(i<total_jobs){ 
  	
  	struct Job * j = (struct Job *)malloc(1*sizeof(struct Job));
  	j->patent_id = input;
  	j->requesterID = index;
  	j->jobNum = i+1; 
  	j->byteNum = dividedJobs[i].start_address;
  	j->numOfLines = dividedJobs[i].total_lines;
  	i++;
  	pthread_mutex_lock(&JobQueuelock);
  		JobsQueueEnq(j);
  	pthread_mutex_unlock(&JobQueuelock);
  }

  char resp[10000];
  bool found_=false;
  int gotResultCount=0;
  while(gotResultCount < total_jobs)
  {
  	pthread_mutex_lock(&CompletedJobQueuelock);
  		struct Job *jj=JobsCompletedQueueDelete(index);
	  	if(jj!=NULL)
	  	{
	  		if(strcmp(jj->result, "Not Found")!=0)
	  		{
	  			strcat(resp, jj->result);
	  			found_=true;
	  		}
	  		gotResultCount++;
	  	}
	pthread_mutex_unlock(&CompletedJobQueuelock);
  }

  	if(!found_)
	{
		strcpy(resp, "Not Found");
	}

	sendto(sock,resp,10000,0,(struct sockaddr *)&rq->requester_cliaddr,sizeof(rq->requester_cliaddr));
	printf("sucessfully sent result back \n");

}


//This thread send search queury to helper
void * helper_thread(void * temp)
{

  struct HelperClient *hc = ((struct HelperClient *) temp); 
  struct Job *j=hc->job;

  pthread_mutex_lock(&BusyHelperlock);
      BusyHelperEnq(hc);
  pthread_mutex_unlock(&BusyHelperlock);

  char mesg[10000];

  sprintf(mesg, "%d#%d#%d#", j->byteNum, j->numOfLines, j->patent_id);


  printf("sending to helper for search  :  %s \n", mesg );

  sendto(sock,mesg,1000,0,(struct sockaddr *)&hc->helper_cliaddr,sizeof(hc->helper_cliaddr));

}

//This function is get port from an sockaddr_in struct
in_port_t get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}


//This function is for keeping the status alive of Helper.
void * statusOfHelper(struct HelperClient *me)
{
  printf("Helper client: connected to port : %i \n", ntohs(me->helper_cliaddr.sin_port));
  
  while(1)
  {
    sleep(3);
    if(!me->status)
    {
      me->status=false;
      printf("Helper going to Die\n");

      if(!me->state)
      {
      	  //If it was free and doing nothing 

	      pthread_mutex_lock(&FreeHelperlock);
	  		FreeHelperDelete(me);
	  	  pthread_mutex_unlock(&FreeHelperlock);
	  }
	  else
	  {
	  		// If this was doing some job, then push this job to Job queue and then delete it
	  		pthread_mutex_lock(&JobQueuelock);
	  			JobsQueueEnq(me->job);
	  		pthread_mutex_unlock(&JobQueuelock);

			pthread_mutex_lock(&BusyHelperlock);
				BusyHelperDelete(me);
			pthread_mutex_unlock(&BusyHelperlock);	
	  }
  	  printf("Helper Died\n");

      break;
    }
    else
    {
      printf("helper # : %u  is still alive \n",me->thread_id);
      me->status=false;
    }
  }
}


//This function is for keeping the status alive of Requeser.
void * statusOfRequester(struct RequesterClient *me)
{
  printf("Requester client: connected to port : %i \n", ntohs(me->requester_cliaddr.sin_port));
  while(1)
  {
    sleep(3);
    if(!me->status)
    {
      me->status=false;
      

      int requester_id=me->thread_id;
      
      /*   This stops all jobs requested by this client */

      struct Job *jobss=JobsQueueDelete(requester_id);
	  while(jobss!=NULL)
      {
      	free(jobss);
		jobss=JobsQueueDelete(requester_id);      	
      }

      /****************************************************/

      printf("Jobs Queue after : %d \n",JobsQueuecount);



      /*  Exiting the requester thread  */

      pthread_cancel(me->thread_id);

      /****************************************************/

      pthread_mutex_lock(&Requesterlock);
	    RequesterDelete(me);
	  pthread_mutex_unlock(&Requesterlock);


      printf("Done dequeing !\n");
      break;
    }
    else
    {
      printf("Requester # : %u  is still waiting  \n",me->thread_id);
      me->status=false;
    }
  }
}



//This function process all the messages coming from different clients
bool processSignalMessage(char mesg[],struct sockaddr_in incoming)
{
   char temp1[10000];
   strcpy(temp1,mesg);
   char *token=NULL;
   token=strtok(temp1,"#");

  if(strcmp(token,"result")==0)
  {

  	token=strtok(NULL,"#");
  	struct HelperClient *temp=(struct HelperClient *)malloc(1*sizeof(struct HelperClient));
  	struct HelperClient *hc;
  	temp->helper_cliaddr=incoming;



  	BusyHelperDisplay();

	pthread_mutex_lock(&BusyHelperlock);
      hc=BusyHelperDelete(temp);
  	pthread_mutex_unlock(&BusyHelperlock);


  	free(temp);

  	hc->job->status=true;
  	strcpy(hc->job->result,token);

  	struct Job *completed=hc->job;

  	//to do deinitialize hc struct

    hc->job=NULL;
    hc->status=true;
    hc->state=false;
    hc->helper_cliaddr= incoming;

    

    pthread_mutex_lock(&FreeHelperlock);
      FreeHelperEnq(hc);
  	pthread_mutex_unlock(&FreeHelperlock);


  	pthread_mutex_lock(&CompletedJobQueuelock);
      JobsCompletedQueueEnq(completed);
  	pthread_mutex_unlock(&CompletedJobQueuelock);

  	return true; 	

  }
  else if(strcmp(token,"signal")==0)
  {
    int i=0;
    struct FreeHelperNode *temp=FreeHelperFront;
    


    while(i<FreeHelpercount)
    {
      if(isEqual(temp->info->helper_cliaddr,incoming))
      {
        temp->info->status=true;
        return true;
      }
      temp=temp->ptr;
      i++;
    }

    i=0;
    struct BusyHelperNode *tempp=BusyHelperFront;
    while(i<BusyHelpercount)
    {
      if(isEqual(tempp->info->helper_cliaddr,incoming))
      {
        tempp->info->status=true;
        return true;
      }
      tempp=tempp->ptr;
      i++;
    }

    i=0;
    struct RequesterNode *temp1=RequesterFront;
    while(i<Requestercount)
    {
      if(isEqual(temp1->info->requester_cliaddr,incoming))
      {
        temp1->info->status=true;

        return true;
      }
      temp1=temp1->ptr;
      i++;
    }

    return false;
  }
  else if(strcmp(token,"requester")==0)
  {

    int input,patent;
    token = strtok(mesg, "#");
    token = strtok(NULL, "\0");
    patent=atoi(token);

    struct RequesterClient *temp=(struct RequesterClient *)malloc(1*sizeof(struct RequesterClient));
    temp->requester_cliaddr=main_cliaddr;
    temp->patent_id=patent;
    temp->status=true;
    pthread_create (&temp->thread_id, NULL, requester_thread, temp);
    pthread_create (&temp->status_thread, NULL, statusOfRequester,temp);
    
    pthread_mutex_lock(&Requesterlock);
	    RequesterEnq(temp);
	    RequesterDisplay();
	pthread_mutex_unlock(&Requesterlock);

    return true;
  }
  else if (strcmp(token,"helper")==0)
  {

    struct HelperClient *temp=(struct HelperClient *)malloc(1*sizeof(struct HelperClient));
    temp->helper_cliaddr=main_cliaddr;
    temp->status=true;
    
    pthread_create (&temp->status_thread, NULL, statusOfHelper, (void *)temp);
    printf("helper# %u attached\n",temp->thread_id);
    temp->state=false;
    pthread_mutex_lock(&FreeHelperlock);
  		FreeHelperEnq(temp);
  		FreeHelperDisplay();
  	pthread_mutex_unlock(&FreeHelperlock);
    
    return true;
  }

  
  return false;
}

void * processMessage()
{
  struct Message * inf=NULL;
  while(1)
  { 
    if(MessagesQueuecount>0)
    {
      pthread_mutex_lock(&Messagelock);
        inf=MessagesQueueDeq();
      pthread_mutex_unlock(&Messagelock);
      
      if(!processSignalMessage(inf->mesg,inf->addr))
      {
        printf("Invalid Message : %s\n",inf->mesg);
      }
      free(inf);
      inf=NULL;
    }
  }
}



int main(int argc, char**argv)
{

  /********   Initializinf queues ****************/
    FreeHelperCreate();
    BusyHelperCreate();
    MessagesQueueCreate();
    RequesterCreate();
    JobsQueueCreate();
    JobsCompletedQueueCreate();
  /**********************************************/



  /********   For syncronization ****************/

    if (pthread_mutex_init(&Messagelock, NULL) != 0)
    {
        printf("\n mutex1 init failed\n");
        return 1;
    }

    if (pthread_mutex_init(&FreeHelperlock, NULL) != 0)
    {
        printf("\n mutex2 init failed\n");
        return 1;
    }

    if (pthread_mutex_init(&BusyHelperlock, NULL) != 0)
    {
        printf("\n mutex3 init failed\n");
        return 1;
    }

    if (pthread_mutex_init(&JobQueuelock, NULL) != 0)
    {
        printf("\n mutex4 init failed\n");
        return 1;
    }

    

	if (pthread_mutex_init(&CompletedJobQueuelock, NULL) != 0)
    {
        printf("\n mutex4 init failed\n");
        return 1;
    }   

    if (pthread_mutex_init(&Requesterlock, NULL) != 0)
    {
        printf("\n mutex4 init failed\n");
        return 1;
    }   
     
/*************************************************************/

    socklen_t len;
    char mesg[10000];
       
    sock=socket(AF_INET,SOCK_DGRAM,0);

    main_servaddr.sin_family = AF_INET;
    main_servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    main_servaddr.sin_port=htons(32000);
    memset(main_servaddr.sin_zero, '\0', sizeof main_servaddr.sin_zero);
    bind(sock,(struct sockaddr *)&main_servaddr,sizeof(main_servaddr));

    pthread_create (&message_thread_id, NULL, processMessage,NULL);

    pthread_create (&jobsassignengine_thread_id, NULL, jobsAssignEngine,NULL);

    while(1)
    {
        len = sizeof(main_cliaddr);
        n = recvfrom(sock,mesg,10000,0,(struct sockaddr *)&main_cliaddr, &len);
        mesg[n]='\0';
	    struct Message * msg=(struct Message *)malloc(1*sizeof(struct Message));
        strcpy(msg->mesg,mesg);
        msg->size=n;
        msg->addr=main_cliaddr;

        pthread_mutex_lock(&Messagelock);
          printf("Received : %s\n",msg->mesg );
          MessagesQueueEnq(msg);
        pthread_mutex_unlock(&Messagelock);
    }
}
