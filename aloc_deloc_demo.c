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

simulate_allocation(memory,512);
simulate_allocation(memory,256);
simulate_allocation(memory,8);

deallocate(memory,0);
deallocate(memory,768);
deallocate(memory,512);
printf("%d\n",memory[0].cont_size);

}//end main;


