#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "two_power_n.h"
#define MAX 1024

struct node{
int free; //1 if free, 0 if not
int cont_size; 
};



///////////////// HELPER FUNCTIONS IN ALLOCATION //////////////

/// This function searches till it gets the least possible block to allocate process
int search_for_least(struct node *memory,int A){
 int i=0; int size_min=1024+1;  
 int i_min=-1; //so that i can know if no possible allocation
   while(i+A<=MAX){
     if(memory[i].free == 1 && memory[i].cont_size >= A && memory[i].cont_size < size_min){
         size_min = memory[i].cont_size;
         i_min = i;
     }
   i = i+memory[i].cont_size;
   } 
 return i_min;
}

/// This function performs the calculations and repservations in allocation
void allocate(struct node *memory,int A, int i){

     while (memory[i].cont_size > A){
      // printf("From %d there is %d free space \n",i,memory[i].cont_size);
       memory[i+memory[i].cont_size/2].cont_size = memory[i].cont_size/2;
       memory[i].cont_size = memory[i].cont_size/2;

     }

     if (memory[i].cont_size == A){
       memory[i].free = 0; //Fill it.
       printf("%d MB allocated from %d to %d \n",A,i,i+memory[i].cont_size-1);
     }
}
/////////////////////////////////////////////////////////////////////////////////

/// The function use for allocation in memory
int simulate_allocation(struct node *memory, int A){
int i=search_for_least(memory,A);
if(i== -1) printf("can't be allocated\n");
else allocate(memory,A,i);
return i;
}

/// The function used for deallocation from the memory
void deallocate(struct node *memory,int i){
 
 memory[i].free = 1;
 
 int my_cont_size = memory[i].cont_size;

 int index=i/my_cont_size;
 
 if(memory[i+my_cont_size].free==1 && memory[i+my_cont_size].cont_size == my_cont_size)
 {
     if (index % 2 == 0)
     {
         memory[i].cont_size = 2*my_cont_size;
         printf("%d MB merged after\n", my_cont_size);
         deallocate(memory,i);
         
     }
     
 }
  if(memory[i-my_cont_size].free==1 && memory[i-my_cont_size].cont_size == my_cont_size)
 {
     if (index % 2 != 0)
     {
         memory[i-my_cont_size].cont_size = 2*my_cont_size;
         printf("%d MB merged before\n", my_cont_size);
         deallocate(memory,i-my_cont_size);
         
     }
 }   
 
 else {printf("deallocated \n");}
 

}









