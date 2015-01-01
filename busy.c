
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>





struct BusyHelperNode
{
    struct HelperClient *info;
    struct BusyHelperNode *ptr;
}*BusyHelperFront,*BusyHelperRear,*BusyHelperTemp,*BusyHelperFront1;

struct HelperClient* BusyHelperFrontelement();
void BusyHelperEnq(struct HelperClient *info);
struct HelperClient* BusyHelperDeq();
bool BusyHelperEmpty();
void BusyHelperDisplay();
void BusyHelperCreate();
struct HelperClient* BusyHelperDelete(struct HelperClient *data);



int BusyHelpercount = 0;




/* Create an empty queue */
void BusyHelperCreate()
{
    BusyHelperFront = BusyHelperRear = NULL;
}


struct HelperClient* BusyHelperDelete(struct HelperClient *data)
{

    struct BusyHelperNode *temp, *prev;
    temp=BusyHelperFront;
    while(temp!=NULL)
    {

        if(BusyHelpercount==1)
        {

            if(isEqual(BusyHelperFront->info->helper_cliaddr,data->helper_cliaddr))  
            {  
                struct HelperClient *ptr= BusyHelperDeq();
                return ptr;
            }
            else
            {
                return NULL;
            }
        }


        if(isEqual(temp->info->helper_cliaddr,data->helper_cliaddr))
        {
        	
            if(temp==BusyHelperFront)
            {

                BusyHelperFront=temp->ptr;
                BusyHelpercount--;
                return temp->info;
            }
            else if(temp==BusyHelperRear)
            {
                prev->ptr=NULL;
                BusyHelperRear=prev;
                BusyHelpercount--;
                return temp->info;
            }
            else
            {

                prev->ptr=temp->ptr;
                BusyHelpercount--;
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


// struct HelperClient* BusyHelperDelete(struct HelperClient *data)
// {


//     struct BusyHelperNode *temp, *prev;
//     temp=BusyHelperFront;
//     while(temp!=NULL)
//     {

//         if(BusyHelpercount==1)
//         {

//             if(isEqual(BusyHelperFront->info->helper_cliaddr,data->helper_cliaddr))  
//             {  
//                 struct HelperClient *ptr= BusyHelperDeq();

//                 return ptr;
//             }
//             else
//             {

//                 return NULL;
//             }
//         }


//         if(isEqual(temp->info->helper_cliaddr,data->helper_cliaddr))
//         {

//             if(temp==BusyHelperFront)
//             {
//                 BusyHelperFront=temp->ptr;
                
//                 BusyHelpercount--;

//                 return temp;
//             }
//             else if(temp==BusyHelperRear)
//             {
//                 prev->ptr=NULL;
//                 BusyHelperRear=prev;
//                 BusyHelpercount--;

//                 return temp;
//             }
//             else
//             {
//                 prev->ptr=temp->ptr;

//                 BusyHelpercount--;

//                 return temp;
//             }
//         }
//         else
//         {
//             prev=temp;
//             temp= temp->ptr;
//         }
//     }

//     return NULL;
// }

 
/* Enqueing the queue */
void BusyHelperEnq(struct HelperClient *data)
{
    if (BusyHelperRear == NULL)
    {
        BusyHelperRear = (struct BusyHelperNode *)malloc(1*sizeof(struct BusyHelperNode));
        BusyHelperRear->ptr = NULL;
        BusyHelperRear->info = data;
        BusyHelperFront = BusyHelperRear;
    }
    else
    {
        BusyHelperTemp=(struct BusyHelperNode *)malloc(1*sizeof(struct BusyHelperNode));
        BusyHelperRear->ptr = BusyHelperTemp;
        BusyHelperTemp->info = data;
        BusyHelperTemp->ptr = NULL;
        BusyHelperRear = BusyHelperTemp;
    }

    BusyHelpercount++;
}
 
/* Displaying the queue elements */
void BusyHelperDisplay()
{
    BusyHelperFront1 = BusyHelperFront;
 
    if ((BusyHelperFront1 == NULL) && (BusyHelperRear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (BusyHelperFront1 != BusyHelperRear)
    {
        printf("%u ", BusyHelperFront1->info->thread_id);
        BusyHelperFront1 = BusyHelperFront1->ptr;
    }
    if (BusyHelperFront1 == BusyHelperRear)
        printf("%u", BusyHelperFront1->info->thread_id);

    printf("\n");
}
 
/* Dequeing the queue */
struct HelperClient* BusyHelperDeq()
{

    BusyHelperFront1 = BusyHelperFront;
 
    if (BusyHelperFront1 == NULL)
    {

        return;
    }
    else
        if (BusyHelperFront1->ptr != NULL)
        {
            BusyHelperFront1 = BusyHelperFront1->ptr;
            struct HelperClient *temp =BusyHelperFront->info;
            free(BusyHelperFront);
            BusyHelperFront = BusyHelperFront1;
            BusyHelpercount--;
            return temp;
        }
        else
        {
            struct HelperClient *temp =BusyHelperFront->info;
            free(BusyHelperFront);
            BusyHelperFront = NULL;
            BusyHelperRear = NULL;
            BusyHelpercount--;
            return temp;
        }
        
}
 
/* Returns the front element of queue */
struct HelperClient* BusyHelperFrontelement()
{
    if ((BusyHelperFront != NULL) && (BusyHelperRear != NULL))
        return(BusyHelperFront->info);
    else
    {
        
        return NULL;
    }
}
 
/* Display if queue is empty or not */
bool BusyHelperEmpty()
{
     if ((BusyHelperFront == NULL) && (BusyHelperRear == NULL))
        return true;
    else
       return false;
}