#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define MAX 1024
struct node{
int free; //1 if free, 0 if not
int cont_size; 
};
 struct node memory[1024];

void allocate(int A, int i);
int search_for_least(int A);
void simulate_allocation(int A);
void deallocate(int i);

int main(int argc, char * argv[])
{

//// Initialization
 memory[0].cont_size=1024;
 for (int i=0; i<1024; i++){
   memory[i].free=1;
  // memory[i].cont_size=1024-i;
 } 
//////

simulate_allocation(64);
simulate_allocation(256);
simulate_allocation(256);
deallocate(256);
deallocate(512);
deallocate(0);
printf("%d\n",memory[0].cont_size);
}//end main;


void simulate_allocation(int A){
int i=search_for_least(A);
if(i== -1) printf("can't be allocated\n");
else allocate(A,i);
}
// This function searches till it gets the least possible block to allocate process
int search_for_least(int A){
 int i=0; int size_min=1025;  
 int i_min=-1; //so that i can know if no possible allocation
   while(i+A<=MAX){
     if(memory[i].free == 1 && memory[i].cont_size >= A){
       if(memory[i].cont_size < size_min) {
         size_min = memory[i].cont_size;
         i_min = i;
       }
     }
   i = i+memory[i].cont_size;
   } 
 return i_min;
}

void allocate(int A, int i){

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


void deallocate(int i){
 printf("deallocate [%d]\n",i);
 memory[i].free = 1;
 int my_cont_size = memory[i].cont_size;
 printf("my_cont_size = %d\n",my_cont_size);
 int ii=i+my_cont_size;
if(i != 0 && (ii==0||ii==4||ii==8||ii==16||ii==32||ii==64||ii==128||ii==256||ii==512||ii==1024)){
   printf("The previous:%d it's cont_size %d\n",i-my_cont_size,memory[i-my_cont_size].cont_size);
 //SO, merge with the previous same size if possible
  if(memory[i-my_cont_size].free==1 && memory[i-my_cont_size].cont_size == my_cont_size) {
 printf("Merged\n");
 memory[i-my_cont_size].cont_size = 2*my_cont_size;
 deallocate(i-my_cont_size);
 }else printf("Can't be mergerd\n");
 }else{
printf("The next:%d it's cont_size %d\n",i+my_cont_size,memory[i+my_cont_size].cont_size);
 //SO, merge with the next same size if possible
  if(memory[i+my_cont_size].free==1 && memory[i+my_cont_size].cont_size == my_cont_size) {
 printf("Merged\n");
 memory[i].cont_size = 2*my_cont_size;
 deallocate(i);
 }else printf("Can't be mergerd\n");
}
printf("Out of function\n");
}



