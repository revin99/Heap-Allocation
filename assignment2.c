#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#define MAXSIZE 1000
#define WORD 4

typedef struct allocnodetag
{
    char name[100];
    int loc;
    int size;
    int words;
    struct allocnodetag *next;
}allocnode;


typedef struct freenodetag
{
    int loc;
    int size;
    int words;
    struct freenodetag *next;
}freenode;


void displayalloc(allocnode *lptralloc)
{
        printf("Location\tName\tSize\tWords\n");
        while(lptralloc!=NULL)
        {
         printf("%d       \t%s   \t%d    \t%d\n",lptralloc->loc,lptralloc->name,lptralloc->size,lptralloc->words);
         lptralloc=lptralloc->next;
        }
}


void displayfree(freenode *lptrfree)
{
    printf("Location\tSize\tWords\n");
    while(lptrfree!=NULL)
    {
        printf("%d       \t%d   \t%d\n",lptrfree->loc,lptrfree->size,lptrfree->words);
        lptrfree= lptrfree->next;
    }

}
allocnode * insertinalloc(allocnode *afptr, allocnode* newptr) //same as INSERT_AT_END function
{
   // printf("coming in fnc\n");
    
    if(afptr==NULL) //if alloc list was empty
    {
        afptr = newptr;
        afptr->next = NULL;
    }
    else
    {
        allocnode *ptr = afptr;
        while(ptr->next!=NULL)
            ptr = ptr->next;

        ptr->next = newptr;
        newptr->next = NULL;
    }

    return afptr;
    
}

allocnode *freeinalloc(allocnode *afptr, allocnode *dptr)
{
   // printf("Coming in freealloc function to free ptrname %s\n",dptr->name); 
    int count=0;
    allocnode *ptr = afptr;
    allocnode *ptr2 = (allocnode*)malloc(sizeof(allocnode));
    ptr2->loc=-1;


    if(ptr == dptr) {//printf("first node\n");
    }
    while(ptr!=dptr)        //ptr points to dptr node and ptr2 points to one node before it 
    {
        count++;
        ptr2= ptr;
        ptr=ptr->next;
        
    }

  //  printf("prev node location%d\n",ptr2->loc);
    //printf("next node location%d\n",(ptr->next)->loc);

    if(ptr == afptr) //if mptrnode was first node only 
    {
        //printf("first node was the one to be deleted\n");
        //if(ptr->next == NULL) printf("ptr next is NULL\n");
        afptr = ptr->next;
        free(ptr);
    }
    else
    {
        //printf("%d node is to be deleted\n",count);
        ptr2->next = ptr->next;
        free(ptr);
    }

    return afptr;
    
}

freenode *insertinfree(freenode *ffptr,freenode *dptr) //insert dptr in freenode list but not actually insert since you just add to 1st node of freelist
{
    int wsize;
    freenode *ptr = ffptr;
    freenode *prevptr = NULL;
    freenode *newptr = dptr;
    if(dptr->size%WORD==0)
    {
        wsize=dptr->size/WORD;
    }
    else
    {
        wsize = dptr->size/WORD +1;
    }

    while(ptr!=NULL)
    {
        if(ptr->loc > dptr->loc)
        {
            break;
        }
        prevptr = ptr;
        ptr=ptr->next;
    }

    //need to insert node in between prevptr and ptr
    //ptr could be first element in freelist
    if(ptr==ffptr) //if first element
    {
            newptr->next = ptr;
            ffptr = newptr;
    }
    else //if not first element
    {
        prevptr->next = newptr;
        newptr->next =ptr;
    }
    
    

    return ffptr; 
}
freenode *freeinnode(freenode *ffptr, freenode* mptr) //need to free node mptr from the list
{
    freenode *ptr = ffptr;
    freenode *ptr2 = NULL;
    while(ptr!=mptr)        //ptr points to mptr node and ptr2 points to one node before it 
    {
        ptr=ptr->next;
        ptr2= ptr;
    }

    if(ptr == ffptr) //if mptrnode was first node only 
    {
        ffptr = ptr->next;
        free(ptr);
    }
    else
    {
        ptr2->next = ptr->next;
        free(ptr);
    }

    return ffptr;
    

}

freenode *compaction(freenode *ffptr) //function to compact free list and free zero sized pointers
{

    freenode *ptr =ffptr;
    freenode *prevptr = (freenode*)malloc(sizeof(freenode));
    freenode *prevprevptr = (freenode*)malloc(sizeof(freenode));
    prevptr=NULL;
    printf("comes into compaction func\n");

    while(ptr!=NULL){
        if(ptr->size==0) //need to remove this bug
        {
            if(ptr==ffptr) //if it is first ptr
            {
                ffptr = ptr->next;
                free(ptr);
            }
            else
            {
                prevptr->next= ptr->next;
                free(ptr);
            }
        }
        prevptr=ptr;
        ptr=ptr->next;
    }                             
    printf("finished deleting 0 sized nodes\n");


    ptr = ffptr; //now for compaction
    prevptr=NULL;
    prevprevptr=NULL;
    while(ptr!=NULL)
    {
        if(prevptr!=NULL && (prevptr->loc + prevptr->words == ptr->loc)) //then we need to compact
        {
            ptr->loc= prevptr->loc;
            ptr->size = prevptr->size+ptr->size;
            ptr->words = prevptr->words+ptr->words;
            if(prevptr == ffptr) {
                ffptr = ptr;
                free(prevptr);
            }
            else
            {
                prevprevptr->next=prevptr->next;
                free(prevptr);
            }
        }
        prevptr=ptr;
        ptr=ptr->next;
        if(prevptr==ffptr){}
        else
        {
            prevprevptr=prevptr;
        }
            
    }
    printf("finished compaction\n");

    return ffptr;




}

void mallocfunc(allocnode **lptralloc, freenode **lptrfree, int bytesize, char pname[])
{
    //printf("Comin into mallocfunction with ptr name %s\n",pname);
    allocnode *afptr,*newptr;
    freenode *ffptr, *mptr;
    
    afptr = *lptralloc;
    ffptr = *lptrfree;

    int msize = (*lptrfree)->size; int wsize; //msize is size in 1st node of freenode list 

    while((afptr!=NULL) && (strcmp(pname,afptr->name)!=0)) //to check if name already taken or not 
    {
        afptr = afptr->next;
    }

    if(afptr==NULL) //means name is not taken
    {
        afptr = *lptralloc; //give it its starting position again
        if(bytesize%WORD==0) //bytesize 5 needs 2 words of 4 each
        {
            wsize=bytesize/WORD;
        }
        else
        {
            wsize= bytesize/WORD +1;
        }
        int flag=0; //to check if heap is full or not 
        mptr = ffptr; // mptr points to free memory location which is going to be cut
        int diff,mindiff=100000000; //to store the difference of givennodesize and desired data size 
        while(mptr!=NULL) //this while-loop is used to find the best-fit freelist node for our alloc function
        {
            //we iterate through the available freelist to search for freelist nodes with sizes nearest to our desired size
            //the minimum size difference of a given nodesize and our desired size is stored which is used in future reference
            //if freelist is one big chunk, then we remove our desired size from there or if we have a freelist node with desired size, we fill that up
            diff = mptr ->size - bytesize;
            if(diff > 0) flag=1; //heap is not full 

            if(diff<mindiff && diff>=0)
            mindiff=diff;

            mptr = mptr->next;
        }
        //printf("%d\n",mindiff);
        if(flag==1){

              //printf("heap is not full\n");  

            //now we search for the particular node with mindiff
            mptr = ffptr;

            while(mptr!=NULL) //mptr will now point to chosen node 
            {
                diff = mptr ->size - bytesize;
               // printf("%d %d\n",diff,mindiff);
                if(diff == mindiff)
                    break;
            
                mptr = mptr->next;
                //printf("a\n");
            }

            //Now we need to add chosen node to alloclist and free the chosennode from freelist
            newptr = (allocnode*)malloc(sizeof(allocnode));
            newptr->loc = mptr->loc;
            newptr->size = bytesize;
            newptr->words = wsize;
            
            for(int i=0;i<strlen(pname);i++)
            {
                newptr->name[i] = pname[i];
            }
            //printf("Going to insertinalloc\n");

            afptr = insertinalloc(afptr,newptr); //insert this newptr node in allocated list
            *lptralloc = afptr;
            printf("Memory of size %d is allocated in position %d\n",bytesize, newptr->loc);

            if(mindiff==0) //if we'd gotten exact-size nodeptr in freelist
            {
                ffptr = freeinnode(ffptr,mptr); //free the mptr node in freenodelist
                *lptrfree=ffptr;
            }
            else //if mptr node remains but size is decreased so freelist is modified but not deleted entirely 
            {
                mptr->loc = mptr->loc + wsize;
                mptr->size = mptr->size -bytesize;
                mptr->words = mptr->words - wsize;
            }
        }
        else
        {
            printf("Sorry HEAP is full\n");
        }
        
    }
    else
    {
        printf("Sorry that name seems to have been taken\n");
    }

}

void freefunc(allocnode **lptralloc, freenode **lptrfree, char pname[])
{
           // printf("pname of ptr to be deletd is %s\n",pname);
            allocnode *afptr = *lptralloc;
            freenode *ffptr = *lptrfree;
            allocnode *ptr = (allocnode*)malloc(sizeof(allocnode));
            freenode *ptr_in_freelist = (freenode*)malloc(sizeof(allocnode));
            int flag=0;
            char *name;
          //  printf("pname is %s\n",pname);
            while(afptr!=NULL) //search for particular node with pointer name as pname and store in ptr
            {
                name = afptr->name;
              //  printf("%s =\n",name);
                //int len = strlen(afptr->name);
                int found=0;
              //  printf("%s %s\n",name,pname);
              //  printf("found value is %d\n",found);

                for(int i=0;i<strlen(pname);i++) //checking for equality of pname and name
                {
               //     printf("%c%c\n",name[i],pname[i]);
                    if(name[i]==pname[i]) 
                    {
               //         printf("it is equal\n");
                    }
                    else
                    {
              //          printf("makes found=1\n");
                        found=1; break;
                    }
                }

            //    printf("found value after check is %d\n",found);

               // printf("%d \n",strcmp(name,pname));
                if(found==0) //that name pointer was found 
                {
                    ptr = afptr; //ptr will point to the node containing that pointer's name
                    flag=1; 
                    break;
                }
                afptr = afptr->next;
            }
            afptr = *lptralloc; //make afptr point to headofnode again
            if(flag==1) //when found 
            {
                ptr_in_freelist->loc = ptr->loc;
                ptr_in_freelist->size = ptr->size;
                ptr_in_freelist->words = ptr->words;

                    //1.free the node in allocnode list
                    //2.insert the node memory space in freenodelist

                afptr = freeinalloc(afptr,ptr); //remove the node completely from alloclist
                ffptr = insertinfree(ffptr,ptr_in_freelist); //inserting in first node of free list. NOT SURE ABOUT THIS          
                printf("Location is freed\n");
                displayfree(ffptr);
                *lptralloc = afptr;

                 ffptr = compaction(ffptr);

                *lptrfree = ffptr;
            }
            else
            {
                printf("No pointer with that pointername found in alloclist\n");
            }
            
}


int main()
{

        allocnode *lptralloc =(allocnode*)malloc(sizeof(allocnode));
        lptralloc = NULL;

        freenode *lptrfree = (freenode *)malloc(sizeof(freenode));
        lptrfree->loc=0;
        lptrfree->next=NULL;
        lptrfree->size = MAXSIZE;
        lptrfree->words = MAXSIZE/WORD;


        printf("--------------------------------------------------------------------------------------------------------------------------\n");


	int choose=0;
	while(choose!=5){
 	    printf("Enter from following options\n1.To allocate memory\n2.To free up memory\n3.To print alloc list\n4.To print free list\n5.To quit\n");
        scanf("%d",&choose);
        switch(choose){
            case 1:
                    printf("Enter the type of memory to be allocated from options\n");
                    int choice; int bytesize;
                    char c; char str[100]; int a; long int b; double d; char pname[100];
                    printf("1.Character\n2.String\n3.Integer\n4.Long Integer\n5.Float\n6.Double\n7.User defined data type\n");
                    scanf("%d",&choice);
                    printf("Enter the pointer name\n");
                    scanf("%s",pname);
                    //printf("variable name is %s\n",pname);
                   // printf("%s",pname);
                    
                    switch(choice)
                    {
                        case 1:
                                bytesize=sizeof(char);
                                break;
                        case 2: 
                                printf("Enter the string variable name\n");
                                scanf("%s",str);
                                bytesize = strlen(str);
                                break;
                        case 3:
                                bytesize =sizeof(int);
                                break;
                        case 4:
                                bytesize =sizeof(long int);
                                break;
                        case 5: 
                                bytesize = sizeof(float);
                                break;
                        case 6: 
                                bytesize= sizeof(double);
                                break;
                        case 7: printf("Enter the byte size of your defined data type\n");
                                scanf("%d",&bytesize);
                                break;

                        default: printf("Wrong choice");

                    }

                    mallocfunc(&lptralloc,&lptrfree,bytesize,pname);
                    break;
            case 2:
                    printf("Enter the name of pointer to be freed\n");
                    scanf("%s",str);
                    freefunc(&lptralloc,&lptrfree,str);
                    break;
            case 3:
                    displayalloc(lptralloc);
                    break;
            case 4:
                    displayfree(lptrfree);
                    break;
            case 5:
                    printf("Exiting program\n");
                    break;

            default: printf("Wrong choice. Enter from numbers 1-5\n");
                
        }
        
	}
	
}