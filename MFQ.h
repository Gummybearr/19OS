#ifndef MFQ_H
#define MFQ_H
#include <stdio.h>
#include <stdlib.h>

//node to store burst time
typedef struct _BNode {
	int burst;
	struct _BNode* next;
} BNode;

//linked list to store nodes containing birst time
typedef struct _BList {
	struct _BNode* head;
	struct _BNode* curr;
	struct _BNode* tail;
} BList;

//Process that 1)stores values from input 
// 2) works as node in double linked list
// 3) stores values for result report
typedef struct _Process {
	int PID; //Process ID
	int Q; //Queue to enter
	int AT; //Arival time
	int TT; //Turnaround time
	int WT; //Waiting time
	int cycles; //if cycle is n, then there would be 2*n-1 sequences of bursts
	struct _BList* bursts; //Linked list for bursts
	struct _Process* next; //next Process
	struct _Process* prev; //Previous Process
} Process;

//Process containers to be used as waiting queue, asleepList, zombieList
typedef struct _ProcessList {
	struct _Process* head; //First element of list
	struct _Process* curr; //Current element
	struct _Process* tail; //Last element
} ProcessList;

//RQ with time quantum of 2
typedef struct _Q0 {
	int TQ; 
	struct _ProcessList* waiting;
} Q0;

//RQ with time quantum of 6
typedef struct _Q1 {
	int TQ;
	struct _ProcessList* waiting;
} Q1;

//SRTN RQ
typedef struct _Q2 {
	struct _Process* shortestNow;
	struct _ProcessList* waiting;
} Q2;

//FCFS RQ
typedef struct _Q3 {
	struct _ProcessList* waiting;
} Q3;

//cpu
typedef struct _CPU {
	int preemptionTime; //used for Process from Q0 and Q1
	int runningTime; //timer for cpu to compare with preemptionTIme
	int originQ; //origin of process to determine its next queue
	Process* running; //process that occupies cpu now
} CPU;

//reads input file and convert it into series of process
//containing corresponding values
//and put all process into one process list
//to be allocated to each RQ
void init(FILE* f, ProcessList* plist);

//Makes ProcessList
ProcessList* makeProcessList();

//Makes Process using input stream from the text file
Process* makeProcess(char* chars);

//Makes Linked list for burst time
BList* makeBList();

//put burst time into the burst time list
void insert2BList(BList* blist, int burst);

//put process into the process list
void insert2PList(ProcessList* plist, Process* p);

//un-link current element in the Process list
void segregate(ProcessList* plist);

//Increase waiting time for all process that are in the RQ 
//and has not occupied cpu 
void incrementWT(ProcessList* waiting);

Q0* makeQ0(); //Makes Q0. TQ set to two and wait-list generated
Q1* makeQ1(); //Makes Q1. TQ set to six and wait-list generated
Q2* makeQ2(); //Makes Q2. Shortest Process ptr and wait-list generated
Q3* makeQ3(); //Makes Q3. wait-list generated
CPU* makeCPU(); //Makes cpu. 

//run function combines all functions below except for the last function
void run(ProcessList* plist, Q0* q0, Q1* q1, Q2* q2, 
	Q3* q3, CPU* mycpu, ProcessList* asleepList, ProcessList* zombieList);

//1) allocate processes whose arrival time <= Program time cycle
void allocate(int time, ProcessList* plist, Q0* q0, Q1* q1, Q2* q2, Q3* q3);

//2) determine which process will occupy cpu according to q0>q1>q2>q3 order
void Q0ProcessCheck(int time, Q0* q0, CPU* mycpu);
void Q1ProcessCheck(int time, Q1* q1, CPU* mycpu);
void Q2ShortestOneCheck(int time, Q2* q2, Q3* q3, CPU* mycpu);
void Q3ProcessCheck(int time, Q3* q3, CPU* mycpu);

//3) decrements I/O burst time and if I/O burst time is 0
//send the process the corresponding RQ
void IOInterruptCheck(int time, ProcessList* asleepList, Q0* q0, Q1* q1, Q2* q2);

//4) decrements cpu burst time of process that has occupied cpu
void runCPU(CPU* mycpu);

//5) if cpu burst time is 0, send the process to asleepList or zombieList
void goSleepOrOut(int time, CPU* mycpu, ProcessList* asleepList, 
	ProcessList* zombieList);

//6) after consuming time, check for preemption for Q0 ans Q1
//if preemption occus, q0->q1 waiting list, and q1->q2 waiting list
void preemptionCheckForQ01(int time, Q1* q1, Q2* q2, CPU* mycpu);

//show reports of process TT, WT, average TT, and average WT
void showReports(ProcessList* zombieList);

#endif