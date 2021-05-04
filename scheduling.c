/*
	This program checks for valid input from the user and takes an input file containing
    the process information, number of processes, and extracts each process and its data. 
    It then calls the correct scheduling algorithm (First come first serve, round robin,
    or shortest remaining job first) to schedule the processes. 

	Author: Reema Amhaz
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// create a struct that contains the process data
typedef struct process {
  int pid;
  int cpu;
  int io;
  int arrival;
  int blocked;
  int running;
  int ready;
  int finished;
  int blocktime;
  int cputime;
  int halflife;
} process ;

void fcfs(char name[], process * processlist, int s)
{
    FILE *outputf;
    outputf = fopen(name, "w");

    int readylist[s];

    for (int i = 0; i < s; i++) 
    {
        readylist[i] = -1;
    }
    int timer = -1;
    int head = 0;
    int tail = 0;
    int wait = s;
    int blocked = 0;
    int running = -1;
    int terminated = 0;
    int cpuTime = 0;
    
    char * turnaround = malloc(2048 * sizeof(char));
    char * outputstring = malloc(512 * sizeof(char));
    // while the process is still running
    while (terminated < s) 
    {
        timer++;
        strcpy(outputstring, "");
        // enqueue the process
        if (wait != 0) 
        {
            for (int i = 0; i < s; i++) 
            {
                if (processlist[i].arrival == timer) 
                {
                    processlist[i].ready = timer;
                    wait--;
                    readylist[tail] = i;
                    tail = (tail + 1) % s;
                }
            }
        }

        if (blocked != 0) 
        {
        // enqueue
            for (int i = 0; i < s; i++) 
            {
                if (processlist[i].blocked == 1) 
                {
                    if (processlist[i].blocktime == processlist[i].io) 
                    {
                        processlist[i].ready = timer;
                        processlist[i].blocked = 0;
                        processlist[i].cputime = 0;
                        blocked--;
                        readylist[tail] = i;
                        tail = (tail + 1) % s;
                    }
                    else 
                    {
                        processlist[i].blocktime++;
                    }
                }
            }
        }
        int next = 0;

        if (running == -1) 
        {
            next = 1;
        } 
        else 
        {
            // see what the status of the current process is 
            process * node = &processlist[running];
            // block it 
            if (node->cputime == node->halflife) {
                running = -1;
                node->running = 0;
                if (node->blocktime > 0) 
                {
                    terminated++;
                    node->finished = timer;
                } 
                else 
                {
                    blocked++;
                    node->blocked = 1;
                    node->blocktime = 1;
                }
                next = 1;
            } 
            else 
            {
                node->cputime++;
                cpuTime++;
            }
        }

        if (next > 0 && head != tail) 
        {
            int ready = processlist[readylist[head]].ready;
            int nodeindex = head;
            int min = processlist[readylist[head]].pid;
            int i = 1;

            while(readylist[(head + i) % s] >= 0 && ready == processlist[readylist[(head + i) % s]].ready) 
            {
                if (processlist[readylist[(head + i) % s]].pid < min) 
                {
                    nodeindex = (head + i) % s;
                    min = processlist[readylist[nodeindex]].pid;
                }
                i++;
            }
            // swap
            if (nodeindex != head) 
            {
                int tmp = readylist[head];
                readylist[head] = readylist[nodeindex];
                readylist[nodeindex] = tmp;
            }
            running = readylist[head];
            processlist[running].cputime++;
            cpuTime++;
            processlist[running].running = 1;
            readylist[head] = -1;
            head = (head + 1) % s;
        }

        for (int i = 0; i < s; i++) 
        {
            if (processlist[i].arrival > timer || processlist[i].finished > 0) {
                continue;
            } 
            else 
            {
                char proc[30];
                sprintf(proc, "%d:", i);
                if (processlist[i].running == 1) 
                {
                    strcat(proc, "running ");
                }
                else if (processlist[i].blocked == 1)
                {
                    strcat(proc, "blocked ");
                } 
                else if (processlist[i].ready >= 0)
                {
                    strcat(proc, "ready ");
                } 
                strcat(outputstring, proc);
            }
        }

        if (terminated != s) 
        {
            fprintf(outputf, "%d: %s\n", timer, outputstring);
        }
    }

    for (int i = 0; i < s; i++) {
        char processstr[30];
        sprintf(processstr, "Turnaround process %d: %d\n", i, processlist[i].finished - processlist[i].arrival);
        strcat(turnaround, processstr);
    }

    fprintf(outputf, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaround);
    // stop mem leaks
    free(outputstring);
    free(turnaround);
    fclose(outputf);
}
void rr(char name[], process * processlist, int s)
{
    FILE *outputr;
    outputr = fopen(name, "w");

    int readylist[s];

    for (int i = 0; i < s; i++) 
    {
            readylist[i] = -1;
    }
    int timer = -1;
    int head = 0;
    int tail = 0;
    int wait = s;
    int blocked = 0;
    int running = -1;
    int timeRunning = 0;
    int terminated = 0;
    int cpuTime = 0;
    
    char * turnaround = malloc(2048 * sizeof(char));
    char * outputstring = malloc(512 * sizeof(char));
    // while the process is still running
    while (terminated < s) 
    {
        timer++;
        strcpy(outputstring, "");
        // enqueue the process
        if (wait != 0) 
        {
            for (int i = 0; i < s; i++) 
            {
                if (processlist[i].arrival == timer) 
                {
                    processlist[i].ready = timer;
                    wait--;
                    readylist[tail] = i;
                    tail = (tail + 1) % s;
                }
            }
        }

    if (blocked != 0) 
    {
    // enqueue
        for (int i = 0; i < s; i++) 
        {
            if (processlist[i].blocked == 1) 
            {
                if (processlist[i].blocktime == processlist[i].io) 
                {
                    // update the struct values
                    processlist[i].ready = timer;
                    processlist[i].blocked = 0;
                    processlist[i].cputime = 0;
                    blocked--;
                    readylist[tail] = i;
                    tail = (tail + 1) % s;
                }
                else 
                {
                    processlist[i].blocktime++;
                }
            }
        }
    }
    int next = 0;

    if (running == -1) 
    {
      next = 1;
    } 
    else 
    {
      // check if running process blocks or terminates
      process * node = &processlist[running];

      if (node->cputime == node->halflife) 
      {
            // block it
            timeRunning = 0;
            running = -1;
            node->running = 0;
            if (node->blocktime > 0) {
                terminated++;
                node->finished = timer;
            } 
            else 
            {
                blocked++;
                node->blocked = 1;
                node->blocktime = 1;
            }
            next = 1;
      } 
      // or enqueue it 
      else if (timeRunning == 2) 
      {
            readylist[tail] = running;
            tail = (tail + 1) % s;
            running = -1;
            timeRunning = 0;
            node->running = 0;
            node->ready = timer;
            next = 1;
      } 
      // otherwise keep running
      else 
      {
            node->cputime++;
            cpuTime++;
            timeRunning++;
      }
    }

    if (next > 0 && readylist[head] != -1) 
    {
        int ready = processlist[readylist[head]].ready;
        int nodeindex = head;
        int min = processlist[readylist[head]].pid;
        int i = 1;

        while(readylist[(head + i) % s] >= 0 && ready == processlist[readylist[(head + i) % s]].ready) 
        {
            if (processlist[readylist[(head + i) % s]].pid < min) 
            {
                nodeindex = (head + i) % s;
                min = processlist[readylist[nodeindex]].pid;
            }
            i++;
        }
        // swap
        if (nodeindex != head) 
        {
            int tmp = readylist[head];
            readylist[head] = readylist[nodeindex];
            readylist[nodeindex] = tmp;
        }

        running = readylist[head];
        processlist[running].cputime++;
        cpuTime++;
        timeRunning = 1;
        processlist[running].running = 1;
        readylist[head] = -1;
        head = (head + 1) % s;
    }

    for (int i = 0; i < s; i++) 
    {
        if (processlist[i].arrival > timer || processlist[i].finished > 0) {
            continue;
        } 
        else 
        {
            char proc[30];
            sprintf(proc, "%d:", i);
            if (processlist[i].running == 1) 
            {
                strcat(proc, "running ");
            }
            else if (processlist[i].blocked == 1)
            {
                strcat(proc, "blocked ");
            } 
            else if (processlist[i].ready >= 0)
            {
                strcat(proc, "ready ");
            } 
            strcat(outputstring, proc);
        }
    }

    if (terminated != s) 
    {
        fprintf(outputr, "%d: %s\n", timer, outputstring);
    }
  }

    for (int i = 0; i < s; i++) 
    {
        char processstr[30];
        sprintf(processstr, "Turnaround process %d: %d\n", i, processlist[i].finished - processlist[i].arrival);
        strcat(turnaround, processstr);
    }

    fprintf(outputr, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaround);
    // stop mem leaks
    free(outputstring);
    free(turnaround);
    fclose(outputr);
}
void srjf(char name[], process * processlist, int s)
{
    FILE *outputs;

    outputs = fopen(name, "w");

    int readylist[s];

    for (int i = 0; i < s; i++) 
    {
        readylist[i] = 0;
    }


    int wait = s;
    int blocked = 0;
    int running = -1;
    int terminated = 0;
    int cpuTime = 0;
    int timer = -1;
    char * turnaroundstring = malloc(2048 * sizeof(char));
    char * outstring = malloc(512 * sizeof(char));

    while (terminated < s) 
    {
        timer++;
        strcpy(outstring, "");

        if (wait != 0) 
        {
        // enqueue
        for (int i = 0; i < s; i++) {
            processlist[i].ready = timer;
            wait--;
            readylist[i] = 1;
            }
        }
        

        if (blocked != 0) {
        // enqueue
        for (int i = 0; i < s; i++) {
            if (processlist[i].blocked == 1) {
            if (processlist[i].blocktime == processlist[i].io) {
                processlist[i].ready = timer;
                processlist[i].blocked = 0;
                processlist[i].cputime = 0;
                blocked--;
                readylist[i] = 1;
            } else {
                processlist[i].blocktime++;
            }
            }
        }
        }

        if (running >= 0) 
        {
        // check if running process blocks or terminates
        process * node = &processlist[running];

        if (node->cputime == node->halflife) {
            // do blocking
            if (node->blocktime > 0) {
            // is terminated
            terminated++;
            readylist[running] = 0;
            running = -1;
            node->running = 0;
            node->finished = timer;
            } else {
            blocked++;
            readylist[running] = 0;
            running = -1;
            node->running = 0;
            node->blocked = 1;
            node->blocktime = 1;
            }
        } else {
            readylist[running] = 1;
            running = -1;
            node->running = 0;
        }
        }

        int mindex = -1;
        int minremain = 2147483647; // INT_MAX
        int minid = 2147483647;

        for (int i = 0; i < s; i++) 
        {
        if (readylist[i] < 1) continue;
        else if (processlist[i].arrival > timer) continue;
        else {
            int remain = 0;
            if (processlist[i].blocktime > 0) 
            {
                remain = processlist[i].halflife - processlist[i].cputime;
            } 
            else 
            {
                remain = processlist[i].halflife + processlist[i].halflife - processlist[i].cputime;
            }
            if (remain <= minremain) {
            if (remain == minremain) {
                if (processlist[i].pid < minid) {
                mindex = i;
                minremain = remain;
                minid = processlist[i].pid;
                }
            } else {
                mindex = i;
                minremain = remain;
                minid = processlist[i].pid;
            }
            }
        }
        }

        if (mindex >= 0) {
        running = mindex;
        processlist[running].cputime++;
        cpuTime++;
        processlist[running].running = 1;

        readylist[mindex] = -1;
        }

        for (int i = 0; i < s; i++) 
        {
        if (processlist[i].arrival > timer || processlist[i].finished > 0) 
        {
            continue;
        } else {
            char proc[25];
            sprintf(proc, "%d:", i);

            if (processlist[i].running == 1) strcat(proc, "running ");
            else if (processlist[i].blocked == 1) strcat(proc, "blocked ");
            else if (processlist[i].ready >= 0) strcat(proc, "ready ");

            strcat(outstring, proc);
        }
        }

        if (terminated != s)
        {
            fprintf(outputs, "%d: %s\n", timer, outstring);
        }
    }

    for (int i = 0; i < s; i++) 
    {
        char procstring[25];
        sprintf(procstring, "Turnaround process %d: %d\n", i, processlist[i].finished - processlist[i].arrival);
        strcat(turnaroundstring, procstring);
    }

    fprintf(outputs, "\nFinishing time: %d\nCPU Utilization: %.2f\n%s", timer - 1, cpuTime / (timer * 1.0), turnaroundstring);

    free(outstring);

    free(turnaroundstring);
    fclose(outputs);
}

/*
	This function checks for valid input from the user and takes an input file containing
    the process information, number of processes, and extracts each process and its data. 
    It then calls the correct scheduling algorithm to schedule the processes. 
	Parameters: an input file and algorithm number
	Return value: int (nothing)
*/
int main(int argc, char* argv[])
{
    // check for valid input
    if (argc != 3)
    {
        printf("This program requires an input file and algorithm type as input.\n");
        return -1;
    }

    // read the file for the processes
    char len[255];
    FILE *input;
    int processsize = 0;

    if((input = fopen(argv[1], "r"))){
        fscanf(input, "%s", len);
        processsize = atoi(len);
    }
    
    //declare structs for processes
    process *processlist = malloc(sizeof(process) * processsize);
    
    for (int i = 0; i < processsize; i++)
    {
        fscanf(input, "%d %d %d %d", &processlist[i].pid, &processlist[i].cpu, &processlist[i].io,&processlist[i].arrival);
        processlist[i].blocked = 0;
        processlist[i].running = 0;
        processlist[i].ready = -1;
        processlist[i].finished = -1;
        processlist[i].blocktime = 0;
        processlist[i].cputime = 0;
        processlist[i].halflife = ((processlist[i].cpu +1)/2);
    }
    // create the file name to pass to each function
    if(strlen(argv[1]) > 4)
    {
        argv[1][strlen(argv[1])-4] = '\0';
    }
    else
    {
        printf("Provide a valid file type./n");
        return -1;
    }


    // send to the correct function to execute scheduling algo
    if (strcmp(argv[2], "0") == 0)
    {
        char ext[7] = "-0.txt";
        char *name = strcat(argv[1],ext);
        fcfs(name, processlist, processsize);
    }
    else if (strcmp(argv[2], "1") == 0)
    {
        char ext[7] = "-1.txt";
        char *name = strcat(argv[1],ext);
        rr(name, processlist, processsize);
    }
    else if (strcmp(argv[2], "2") == 0)
    {
        char ext[7] = "-2.txt";
        char *name = strcat(argv[1],ext);
        srjf(name, processlist, processsize);
    }
    else
    {
        printf("This program does not support that algorithm.\n");
        return -1;
    }
    // so we don't have a memory leak
    free(processlist);
    return 0;
}