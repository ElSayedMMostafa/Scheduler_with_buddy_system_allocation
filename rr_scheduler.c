#include "headers.h"
#include "priority_queue.h"
// The coming "includes" for IPC
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include "aloc_and_deloc.h"
#define MAXSIZE 100

struct msgbuff
{
    long mtype;
    struct Process_data data;
};

void int_handler();
void sigusr1_handler();

void run_process(int PS_ProcessingTime);
int pid; //The pid of the process..

FILE *f_log;
FILE *f_perf;
FILE *f_mem; 
int counter=-1;
int s_TA=0;
int s_WTA=0;
double WTA_arr[MAXSIZE];
int s_PT=0;
int count=0;
int first_start;

int start_now,end_now;
int main(int argc, char * argv[])
{
    signal(SIGINT,int_handler);
    signal(SIGUSR1,sigusr1_handler);
// FILE

    f_log = fopen("scheduler.log", "w");
    f_perf = fopen("scheduler.perf", "w");
    f_mem = fopen("memory.log","w");
//Variables
    struct Process_data Popped;
    int process_name=-1;
    int quantum=5;
    int status,pid_back;
    int TA;
    double WTA;
    int mem_index, mem_size;
// For IPC
    key_t msgqid,msgqid2;
    int msg_key = 12345;
    int msg_key2 = 12346;
    int recieving_status;
    struct msgbuff pg_message; //Instance of the buffer (Sent by the process generator)
    pg_message.mtype=0; //to recieve on all the tags.
    msgqid = msgget(msg_key,0644);
    msgqid2 = msgget(msg_key2,0644);

    if(msgqid == -1) perror("Error in recieving, No msg_queue exits");

// Intialize the priority queue
    heap_t *p_queue = (heap_t *)calloc(1, sizeof (heap_t));

////Memory Intialization///
    struct node memory[1024];
    memory[0].cont_size=1024;
    for (int i=0; i<1024; i++) memory[i].free=1;

//To intialize the clock
    initClk();

    while(getClk()<1); //Start form clock=1;
// The main functionality
    while(1) {
        process_name=-1;

// [ Continuous Recieving ]
        recieving_status = msgrcv(msgqid, &pg_message, sizeof(pg_message.data),14,IPC_NOWAIT);
        if(recieving_status != -1) push(p_queue,pg_message.data.arrival_time, pg_message.data);

// [ Pop a process ]
        Popped = pop(p_queue);
        process_name=Popped.name;

        if (process_name==-1) {
            recieving_status = msgrcv(msgqid2, &pg_message, sizeof(pg_message.data),14,IPC_NOWAIT);
            if(recieving_status != -1) break;
            printf("NO NEW processes to be run --> Gonna wait\n");
            recieving_status = msgrcv(msgqid, &pg_message, sizeof(pg_message.data),14,!IPC_NOWAIT);

            push(p_queue,pg_message.data.arrival_time,pg_message.data);
        }
        else {
             mem_index= search_for_least(memory,Get_nearest_2power_num(Popped.memory_size));
            if(Popped.remaining_time != Popped.processing_time) {
		start_now = getClk();
                kill(Popped.process_pid,SIGCONT);
                Popped.waiting_time= Popped.waiting_time +  start_now -Popped.stop_time;
                fprintf(f_log,"At time %d process %d resumed arr %d total %d remain %d wait %d\n", start_now, Popped.name, Popped.arrival_time, Popped.processing_time, Popped.remaining_time, Popped.waiting_time);
		
                sleep(quantum);
                if(Popped.remaining_time > quantum) {
                    //end_now = getClk();
                    kill(Popped.process_pid,SIGSTOP);

                    Popped.remaining_time = Popped.remaining_time - quantum;

                    Popped.stop_time=getClk();
                    fprintf(f_log,"At time %d process %d stopped arr %d total %d remain %d wait %d\n", getClk(), Popped.name, Popped.arrival_time, Popped.processing_time, Popped.remaining_time, Popped.waiting_time);
                    push(p_queue,getClk(), Popped);
                } else {
                    //end_now = getClk();
                    pid_back= wait(&status);
                    if(!(status & 0x00FF))  printf("FINISHED PROCESS\n");
                    TA= Popped.waiting_time+Popped.processing_time;
                    WTA= (double) TA/Popped.processing_time;
                    s_TA=s_TA + TA;
                    s_WTA=s_WTA + WTA;
                    WTA_arr[count]=WTA;
                    count++;
                    end_now=getClk();

                    fprintf(f_log,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f \n", end_now, Popped.name, Popped.arrival_time, Popped.processing_time, 0, Popped.waiting_time, TA, WTA);
		//memory deallocation
                    deallocate(memory, Popped.memory_index);
                    mem_size = Get_nearest_2power_num(Popped.memory_size);
		   fprintf(f_mem, "At time %d freed %d bytes from process %d from %d to %d\n", end_now, Popped.memory_size, Popped.name, Popped.memory_index, Popped.memory_index+mem_size-1);
                }
            } else if(mem_index != -1){

                pid=fork();
                if (pid == -1) perror("error in fork");
                else if (pid == 0) run_process(Popped.processing_time);
                else {
		   mem_size = Get_nearest_2power_num(Popped.memory_size);
                   mem_index= simulate_allocation(memory,mem_size);  
	           Popped.memory_index = mem_index;
                    start_now = getClk();
                    sleep(quantum);

                    s_PT=s_PT+Popped.processing_time;
                    Popped.waiting_time=start_now-Popped.arrival_time;
                    if(process_name==1)first_start=Popped.arrival_time;
                    fprintf(f_log,"At time %d Process %d started arr %d total %d remain %d wait %d\n",start_now,process_name,Popped.arrival_time,Popped.processing_time,Popped.processing_time,Popped.waiting_time);
	            fprintf(f_mem,"At time %d allocated %d bytes for process %d from %d to %d\n", start_now, Popped.memory_size,Popped.name, mem_index, mem_index+mem_size-1 );
                    Popped.process_pid = pid;
                    if(Popped.remaining_time > quantum) {
                        end_now = getClk();
                        kill(Popped.process_pid,SIGSTOP);

                        Popped.stop_time=end_now;
                        fprintf(f_log,"At time %d process %d stopped arr %d total %d remain %d wait %d\n", end_now, Popped.name, Popped.arrival_time, Popped.processing_time, Popped.remaining_time, Popped.waiting_time);
                        Popped.remaining_time = Popped.remaining_time - quantum;

                        push(p_queue,end_now, Popped);
                    } else {
                        end_now = getClk();
                        pid_back= wait(&status);
                        if(!(status & 0x00FF))  printf("FINISHED PROCESS\n");
                        TA= Popped.waiting_time+Popped.processing_time;
                        WTA= (double) TA/Popped.processing_time;
                        s_TA=s_TA + TA;
                        s_WTA=s_WTA + WTA;
                        WTA_arr[count]=WTA;
                        count++;
                        end_now=getClk();

                        fprintf(f_log,"At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f \n", end_now, Popped.name, Popped.arrival_time, Popped.processing_time, 0, Popped.waiting_time, TA, WTA);
		//memory deallocation
                      deallocate(memory, Popped.memory_index);
                      mem_size = Get_nearest_2power_num(Popped.memory_size);
		      fprintf(f_mem, "At time %d freed %d bytes from process %d from %d to %d\n", end_now, Popped.memory_size, Popped.name, Popped.memory_index, Popped.memory_index+mem_size-1);
                    }
                }
            }
		else{
		  printf("mem index = %d\n",mem_index);
		   printf("Process %d of size %d can't be allocated now\n",Popped.name, Popped.memory_size);
		   push(p_queue,getClk(), Popped);
		}
        }
    } //End of while loop
    raise(SIGINT);
    //destroyClk(true);
}

void run_process(int PS_ProcessingTime) {
    char pt_string[9]; //SOMETHING WEIRD HERE... less than 9 cause error
    sprintf(pt_string, "%d", PS_ProcessingTime); //convert to string
    char *argv[] = { "./process.out", pt_string , 0 }; //ZERO IS SHHHHHIIIIIIT
    execve(argv[0], &argv[0], NULL); //start the new process
}

void int_handler() {

    double std_WTA=0;
    double avg_WAITING = (double)((s_TA-s_PT)/count);
    double avg_WTA = (double)s_WTA/count;
    double ut_time= (double) (s_PT*100)/(end_now-first_start);
    fprintf(f_perf,"Utilization time =%.2f %%\n", ut_time);
    fprintf(f_perf,"Average WTA = %.2f\n",avg_WTA);
    fprintf(f_perf,"Average Waiting = %.2f\n",avg_WAITING);


    for(int i=0; i<count; i++) {
        std_WTA += pow((WTA_arr[i]-avg_WTA),2);
    }
    fprintf(f_perf,"std WTA = %f\n",sqrt(std_WTA/count));
    fclose(f_log);
    fclose(f_perf);
    exit(0);
}

void sigusr1_handler(int signum) {
// Empty
//printf("SIGUSR1 RECIEVED..\n");
    // DO nothing
}

