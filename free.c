
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>


struct FreeHelperNode
{
    struct HelperClient *info;
    struct FreeHelperNode *ptr;
}*FreeHelperFront,*FreeHelperRear,*FreeHelperTemp,*FreeHelperFront1;

struct HelperClient* FreeHelperFrontelement();
void FreeHelperEnq(struct HelperClient *info);
struct HelperClient* FreeHelperDeq();
bool FreeHelperEmpty();
void FreeHelperDisplay();
void FreeHelperCreate();
bool FreeHelperDelete(struct HelperClient *data);


 
int FreeHelpercount = 0;


/* Create an empty queue */
void FreeHelperCreate()
{
    FreeHelperFront = FreeHelperRear = NULL;
}

bool FreeHelperDelete(struct HelperClient *data)
{

    struct FreeHelperNode *temp, *prev;
    temp=FreeHelperFront;
    while(temp!=NULL)
    {
        if(FreeHelpercount==1)
        {
            if(isEqual(FreeHelperFront->info->helper_cliaddr,data->helper_cliaddr))  
            {  
                struct HelperClient *ptr= FreeHelperDeq();
                return ptr;
            }
            else
            {
                return NULL;
            }
        }


        if(isEqual(temp->info->helper_cliaddr,data->helper_cliaddr))
        {

            if(temp==FreeHelperFront)
            {
                FreeHelperFront=temp->ptr;
                free(temp->info);
                free(temp);
                FreeHelpercount--;
                
                return true;
            }
            else if(temp==FreeHelperRear)
            {
                prev->ptr=NULL;
                FreeHelperRear=prev;
                free(temp->info);
                free(temp);
                FreeHelpercount--;
                return true;
            }
            else
            {
                prev->ptr=temp->ptr;
                free(temp->info);
                free(temp);
                FreeHelpercount--;

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
void FreeHelperEnq(struct HelperClient *data)
{

    if (FreeHelperRear == NULL)
    {
        FreeHelperRear = (struct FreeHelperNode *)malloc(1*sizeof(struct FreeHelperNode));
        FreeHelperRear->ptr = NULL;
        FreeHelperRear->info = data;
        FreeHelperFront = FreeHelperRear;

    }
    else
    {
        FreeHelperTemp=(struct FreeHelperNode *)malloc(1*sizeof(struct FreeHelperNode));
        FreeHelperRear->ptr = FreeHelperTemp;
        FreeHelperTemp->info = data;
        FreeHelperTemp->ptr = NULL;
        FreeHelperRear = FreeHelperTemp;
    }

    FreeHelpercount++;
}
 
/* Displaying the queue elements */
void FreeHelperDisplay()
{
    FreeHelperFront1 = FreeHelperFront;
    if ((FreeHelperFront1 == NULL) && (FreeHelperRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (FreeHelperFront1 != FreeHelperRear)
    {
        printf("%u ", FreeHelperFront1->info->thread_id);
        FreeHelperFront1 = FreeHelperFront1->ptr;
    }
    if (FreeHelperFront1 == FreeHelperRear)
        printf("%u", FreeHelperFront1->info->thread_id);

    printf("\n");
}
 
/* Dequeing the queue */
struct HelperClient* FreeHelperDeq()
{

    FreeHelperFront1 = FreeHelperFront;
    if (FreeHelperFront1 == NULL)
    {
        return NULL;
    }
    else
        if (FreeHelperFront1->ptr != NULL)
        {
            FreeHelperFront1 = FreeHelperFront1->ptr;
            struct HelperClient *temp =FreeHelperFront->info;
            free(FreeHelperFront);
            FreeHelperFront = FreeHelperFront1;
            FreeHelpercount--;
            return temp;
        }
        else
        {
            struct HelperClient *temp =FreeHelperFront->info;
            free(FreeHelperFront);
            FreeHelperFront = NULL;
            FreeHelperRear = NULL;
            FreeHelpercount--;
            return temp;
        }
        
    return NULL;
}
 
/* Returns the front element of queue */
struct HelperClient* FreeHelperFrontelement()
{
    if ((FreeHelperFront != NULL) && (FreeHelperRear != NULL))
        return(FreeHelperFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool FreeHelperEmpty()
{
     if ((FreeHelperFront == NULL) && (FreeHelperRear == NULL))
        return true;
    else
       return false;
}