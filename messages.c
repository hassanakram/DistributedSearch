
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>





struct MessagesQueueNode
{
    struct Message *info;
    struct MessagesQueueNode *ptr;
}*MessagesQueueFront,*MessagesQueueRear,*MessagesQueueTemp,*MessagesQueueFront1;

struct Message* MessagesQueueFrontelement();
void MessagesQueueEnq(struct Message *info);
struct Message* MessagesQueueDeq();
bool MessagesQueueEmpty();
void MessagesQueueDisplay();
void MessagesQueueCreate();



 
int MessagesQueuecount = 0;


/* Create an empty queue */
void MessagesQueueCreate()
{
    MessagesQueueFront = MessagesQueueRear = NULL;
}

 
/* Enqueing the queue */
void MessagesQueueEnq(struct Message *data)
{
    if (MessagesQueueRear == NULL)
    {
        MessagesQueueRear = (struct MessagesQueueNode *)malloc(1*sizeof(struct MessagesQueueNode));
        MessagesQueueRear->ptr = NULL;
        MessagesQueueRear->info = data;
        MessagesQueueFront = MessagesQueueRear;
    }
    else
    {
        MessagesQueueTemp=(struct MessagesQueueNode *)malloc(1*sizeof(struct MessagesQueueNode));
        MessagesQueueRear->ptr = MessagesQueueTemp;
        MessagesQueueTemp->info = data;
        MessagesQueueTemp->ptr = NULL;
        MessagesQueueRear = MessagesQueueTemp;
    }

    MessagesQueuecount++;
}
 
/* Displaying the queue elements */
void MessagesQueueDisplay()
{
    MessagesQueueFront1 = MessagesQueueFront;
 
    if ((MessagesQueueFront1 == NULL) && (MessagesQueueRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (MessagesQueueFront1 != MessagesQueueRear)
    {
        printf("%s ", MessagesQueueFront1->info->mesg);
        MessagesQueueFront1 = MessagesQueueFront1->ptr;
    }
    if (MessagesQueueFront1 == MessagesQueueRear)
        printf("%s", MessagesQueueFront1->info->mesg);

    printf("\n");
}
 
/* Dequeing the queue */
struct Message* MessagesQueueDeq()
{
    MessagesQueueFront1 = MessagesQueueFront;
 
    if (MessagesQueueFront1 == NULL)
    {
        return NULL;
    }
    else
        if (MessagesQueueFront1->ptr != NULL)
        {
            MessagesQueueFront1 = MessagesQueueFront1->ptr;
            struct Message *temp =MessagesQueueFront->info;
            free(MessagesQueueFront);
            MessagesQueueFront = MessagesQueueFront1;
            MessagesQueuecount--;
            return temp;
        }
        else
        {
            struct Message *temp =MessagesQueueFront->info;
            free(MessagesQueueFront);
            MessagesQueueFront = NULL;
            MessagesQueueRear = NULL;
            MessagesQueuecount--;
            return temp;
        }
        
}
 
/* Returns the front element of queue */
struct Message* MessagesQueueFrontelement()
{
    if ((MessagesQueueFront != NULL) && (MessagesQueueRear != NULL))
        return(MessagesQueueFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool MessagesQueueEmpty()
{
     if ((MessagesQueueFront == NULL) && (MessagesQueueRear == NULL))
        return true;
    else
       return false;
}