
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>



struct JobsQueueNode
{
    struct Job *info;
    struct JobsQueueNode *ptr;
}*JobsQueueFront,*JobsQueueRear,*JobsQueueTemp,*JobsQueueFront1;

struct Job* JobsQueueFrontelement();
void JobsQueueEnq(struct Job *info);
struct Job* JobsQueueDeq();
bool JobsQueueEmpty();
void JobsQueueDisplay();
void JobsQueueCreate();


 
int JobsQueuecount = 0;


struct Job* JobsQueueDelete(int thread_id)
{

    struct JobsQueueNode *temp, *prev;
    temp=JobsQueueFront;
    while(temp!=NULL)
    {
        if(JobsQueuecount==1)
        {
            if(JobsQueueFront->info->requesterID==thread_id)  
            {   
            	struct Job *ptr=JobsQueueFront->info;
                JobsQueueDeq();
                return ptr;
            }
            else
            {
                return NULL;
            }
        }


        if(temp->info->requesterID ==thread_id)
        {

            if(temp==JobsQueueFront)
            {
                JobsQueueFront=temp->ptr;
                JobsQueuecount--;
                return temp->info;
            }
            else if(temp==JobsQueueRear)
            {
                prev->ptr=NULL;
                JobsQueueRear=prev;
                JobsQueuecount--;
                return temp->info;
            }
            else
            {
                prev->ptr=temp->ptr;
                JobsQueuecount--;
                return temp->info;
            }
        }
        else
        {
            prev=temp;
            temp= temp->ptr;
        }
    }

    return NULL;
}



/* Create an empty queue */
void JobsQueueCreate()
{
    JobsQueueFront = JobsQueueRear = NULL;
}

 
/* Enqueing the queue */
void JobsQueueEnq(struct Job *data)
{
    if (JobsQueueRear == NULL)
    {
        JobsQueueRear = (struct JobsQueueNode *)malloc(1*sizeof(struct JobsQueueNode));
        JobsQueueRear->ptr = NULL;
        JobsQueueRear->info = data;
        JobsQueueFront = JobsQueueRear;
    }
    else
    {
        JobsQueueTemp=(struct JobsQueueNode *)malloc(1*sizeof(struct JobsQueueNode));
        JobsQueueRear->ptr = JobsQueueTemp;
        JobsQueueTemp->info = data;
        JobsQueueTemp->ptr = NULL;
        JobsQueueRear = JobsQueueTemp;
    }

    JobsQueuecount++;
}
 
/* Displaying the queue elements */
void JobsQueueDisplay()
{
    JobsQueueFront1 = JobsQueueFront;
 
    if ((JobsQueueFront1 == NULL) && (JobsQueueRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (JobsQueueFront1 != JobsQueueRear)
    {
        printf("%s ", JobsQueueFront1->info->requesterID);
        JobsQueueFront1 = JobsQueueFront1->ptr;
    }
    if (JobsQueueFront1 == JobsQueueRear)
        printf("%s", JobsQueueFront1->info->requesterID);

    printf("\n");
}
 
/* Dequeing the queue */
struct Job* JobsQueueDeq()
{
    JobsQueueFront1 = JobsQueueFront;
 
    if (JobsQueueFront1 == NULL)
    {
        return;
    }
    else
        if (JobsQueueFront1->ptr != NULL)
        {
            JobsQueueFront1 = JobsQueueFront1->ptr;
            struct Job *temp =JobsQueueFront->info;
            free(JobsQueueFront);
            JobsQueueFront = JobsQueueFront1;
            JobsQueuecount--;
            return temp;
        }
        else
        {
            struct Job *temp =JobsQueueFront->info;
            free(JobsQueueFront);
            JobsQueueFront = NULL;
            JobsQueueRear = NULL;
            JobsQueuecount--;
            return temp;
        }
        
}
 
/* Returns the front element of queue */
struct Job* JobsQueueFrontelement()
{
    if ((JobsQueueFront != NULL) && (JobsQueueRear != NULL))
        return(JobsQueueFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool JobsQueueEmpty()
{
     if ((JobsQueueFront == NULL) && (JobsQueueRear == NULL))
        return true;
    else
       return false;
}
