
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>



struct JobsCompletedQueueNode
{
    struct Job *info;
    struct JobsCompletedQueueNode *ptr;
}*JobsCompletedQueueFront,*JobsCompletedQueueRear,*JobsCompletedQueueTemp,*JobsCompletedQueueFront1;

struct Job* JobsCompletedQueueFrontelement();
void JobsCompletedQueueEnq(struct Job *info);
struct Job* JobsCompletedQueueDeq();
bool JobsCompletedQueueEmpty();
void JobsCompletedQueueDisplay();
void JobsCompletedQueueCreate();
struct Job* JobsCompletedQueueDelete( int thread_id);

int JobsCompletedQueuecount = 0;

struct Job* JobsCompletedQueueDelete(int thread_id)
{

    struct JobsCompletedQueueNode *temp, *prev;
    temp=JobsCompletedQueueFront;
    while(temp!=NULL)
    {
        if(JobsCompletedQueuecount==1)
        {
            
            if(JobsCompletedQueueFront->info->requesterID==thread_id)  
            {   struct Job *ptr=JobsCompletedQueueFront->info;

                JobsCompletedQueueDeq();
                return ptr;
            }
            else
            {
                return NULL;
            }

        }


        if(temp->info->requesterID ==thread_id)
        {

            if(temp==JobsCompletedQueueFront)
            {

                JobsCompletedQueueFront=temp->ptr;
                
                JobsCompletedQueuecount--;

                return temp->info;
            }
            else if(temp==JobsCompletedQueueRear)
            {

                prev->ptr=NULL;
                JobsCompletedQueueRear=prev;
                JobsCompletedQueuecount--;

                return temp->info;
            }
            else
            {

                prev->ptr=temp->ptr;

                JobsCompletedQueuecount--;

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
void JobsCompletedQueueCreate()
{
    JobsCompletedQueueFront = JobsCompletedQueueRear = NULL;
}

 
/* Enqueing the queue */
void JobsCompletedQueueEnq(struct Job *data)
{
    if (JobsCompletedQueueRear == NULL)
    {
        JobsCompletedQueueRear = (struct JobsCompletedQueueNode *)malloc(1*sizeof(struct JobsCompletedQueueNode));
        JobsCompletedQueueRear->ptr = NULL;
        JobsCompletedQueueRear->info = data;
        JobsCompletedQueueFront = JobsCompletedQueueRear;
    }
    else
    {
        JobsCompletedQueueTemp=(struct JobsCompletedQueueNode *)malloc(1*sizeof(struct JobsCompletedQueueNode));
        JobsCompletedQueueRear->ptr = JobsCompletedQueueTemp;
        JobsCompletedQueueTemp->info = data;
        JobsCompletedQueueTemp->ptr = NULL;
        JobsCompletedQueueRear = JobsCompletedQueueTemp;
    }

    JobsCompletedQueuecount++;
}
 
/* Displaying the queue elements */
void JobsCompletedQueueDisplay()
{
    JobsCompletedQueueFront1 = JobsCompletedQueueFront;
 
    if ((JobsCompletedQueueFront1 == NULL) && (JobsCompletedQueueRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (JobsCompletedQueueFront1 != JobsCompletedQueueRear)
    {
        printf("%s ", JobsCompletedQueueFront1->info->requesterID);
        JobsCompletedQueueFront1 = JobsCompletedQueueFront1->ptr;
    }
    if (JobsCompletedQueueFront1 == JobsCompletedQueueRear)
        printf("%s", JobsCompletedQueueFront1->info->requesterID);

    printf("\n");
}
 
/* Dequeing the queue */
struct Job* JobsCompletedQueueDeq()
{
    JobsCompletedQueueFront1 = JobsCompletedQueueFront;
 
    if (JobsCompletedQueueFront1 == NULL)
    {
        return;
    }
    else
        if (JobsCompletedQueueFront1->ptr != NULL)
        {
            JobsCompletedQueueFront1 = JobsCompletedQueueFront1->ptr;
            struct Job *temp =JobsCompletedQueueFront->info;
            free(JobsCompletedQueueFront);
            JobsCompletedQueueFront = JobsCompletedQueueFront1;
            JobsCompletedQueuecount--;
            return temp;
        }
        else
        {
            struct Job *temp =JobsCompletedQueueFront->info;
            free(JobsCompletedQueueFront);
            JobsCompletedQueueFront = NULL;
            JobsCompletedQueueRear = NULL;
            JobsCompletedQueuecount--;
            return temp;
        }
        
}
 
/* Returns the front element of queue */
struct Job* JobsCompletedQueueFrontelement()
{
    if ((JobsCompletedQueueFront != NULL) && (JobsCompletedQueueRear != NULL))
        return(JobsCompletedQueueFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool JobsCompletedQueueEmpty()
{
     if ((JobsCompletedQueueFront == NULL) && (JobsCompletedQueueRear == NULL))
        return true;
    else
       return false;
}
