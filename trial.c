#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "aloc_and_deloc.h"


int main(int argc, char * argv[])
{
struct node memory[1024];
//// Initialization
 memory[0].cont_size=1024;
 for (int i=0; i<1024; i++){
   memory[i].free=1;
  // memory[i].cont_size=1024-i;
 } 
//////

simulate_allocation(memory,32);
simulate_allocation(memory,64);
simulate_allocation(memory,64);
deallocate(memory,64);
deallocate(memory,0);
simulate_allocation(memory,16);
simulate_allocation(memory,128);
printf("%d\n",memory[0].cont_size);
}//end main;


