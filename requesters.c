
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>





struct RequesterNode
{
    struct RequesterClient *info;
    struct RequesterNode *ptr;
}*RequesterFront,*RequesterRear,*RequesterTemp,*RequesterFront1;

struct RequesterClient* RequesterFrontelement();
void RequesterEnq(struct RequesterClient *info);
struct RequesterClient* RequesterDeq();
bool RequesterEmpty();
void RequesterDisplay();
void RequesterCreate();
bool RequesterDelete(struct RequesterClient *data);

 
int Requestercount = 0;


/* Create an empty queue */
void RequesterCreate()
{
    RequesterFront = RequesterRear = NULL;
}

bool RequesterDelete(struct RequesterClient *data)
{

    struct RequesterNode *temp, *prev;
    temp=RequesterFront;
    while(temp!=NULL)
    {
        if(Requestercount==1)
        {
            struct RequesterClient *ptr= RequesterDeq();
            free(ptr);
            return true;
        }


        
        if(isEqual(temp->info->requester_cliaddr,data->requester_cliaddr))
        {

            if(temp==RequesterFront)
            {
                RequesterFront=temp->ptr;
                free(temp->info);
                free(temp);
                Requestercount--;
                
                return true;
            }
            else if(temp==RequesterRear)
            {
                prev->ptr=NULL;
                RequesterRear=prev;
                free(temp->info);
                free(temp);
                Requestercount--;
                return true;
            }
            else
            {
                prev->ptr=temp->ptr;
                free(temp->info);
                free(temp);
                Requestercount--;

                return true;
            }
        }
        else
        {
            prev=temp;
            temp= temp->ptr;
        }
    }

    return false;
}

 
/* Enqueing the queue */
void RequesterEnq(struct RequesterClient *data)
{
    if (RequesterRear == NULL)
    {
        RequesterRear = (struct RequesterNode *)malloc(1*sizeof(struct RequesterNode));
        RequesterRear->ptr = NULL;
        RequesterRear->info = data;
        RequesterFront = RequesterRear;

    }
    else
    {
        RequesterTemp=(struct RequesterNode *)malloc(1*sizeof(struct RequesterNode));
        RequesterRear->ptr = RequesterTemp;
        RequesterTemp->info = data;
        RequesterTemp->ptr = NULL;
        RequesterRear = RequesterTemp;
    }

    Requestercount++;
}
 
/* Displaying the queue elements */
void RequesterDisplay()
{
    RequesterFront1 = RequesterFront;
 
    if ((RequesterFront1 == NULL) && (RequesterRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (RequesterFront1 != RequesterRear)
    {
        printf("%u ", RequesterFront1->info->thread_id);
        RequesterFront1 = RequesterFront1->ptr;
    }
    if (RequesterFront1 == RequesterRear)
        printf("%u", RequesterFront1->info->thread_id);

    printf("\n");
}
 
/* Dequeing the queue */
struct RequesterClient* RequesterDeq()
{
    RequesterFront1 = RequesterFront;
 
    if (RequesterFront1 == NULL)
    {
        printf("\n Error: Trying to display elements from empty queue");
        return NULL;
    }
    else
        if (RequesterFront1->ptr != NULL)
        {
            RequesterFront1 = RequesterFront1->ptr;
            struct RequesterClient *temp =RequesterFront->info;
            free(RequesterFront);
            RequesterFront = RequesterFront1;
            Requestercount--;
            return temp;
        }
        else
        {
            struct RequesterClient *temp =RequesterFront->info;
            free(RequesterFront);
            RequesterFront = NULL;
            RequesterRear = NULL;
            Requestercount--;
            return temp;
        }

        return NULL;
        
}
 
/* Returns the front element of queue */
struct RequesterClient* RequesterFrontelement()
{
    if ((RequesterFront != NULL) && (RequesterRear != NULL))
        return(RequesterFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool RequesterEmpty()
{
     if ((RequesterFront == NULL) && (RequesterRear == NULL))
        return true;
    else
       return false;
}
