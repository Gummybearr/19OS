#include "MFQ.h"

void init(FILE* f, ProcessList* plist) {
	if (f != NULL) {
		char line[255]; char *chars; //buffer
		int pTotal = 0; //total number of process
		chars = fgets(line, sizeof(line), f); //read line by line
		int tmp = 0; //current reader place counter
		while (chars[0 + tmp] != 32 && chars[0 + tmp] >= 48 && 
			chars[0 + tmp] <= 57) { //if value is not " " and >='0' and <='9'
			pTotal = 10 * pTotal + (chars[0 + tmp] - '0');
			tmp++;
		} // have to write like this because the number can be larger than 10
		for (int i = 0; i < pTotal; i++) {
			chars = fgets(line, sizeof(line), f);
			Process* p = makeProcess(chars); //make process list
			insert2PList(plist, p); //put the process into the list
			chars = NULL;
		}
		fclose(f); //close the file
	}
	else {
		fprintf(stderr, "Blank File\n"); fclose(f);  exit(1);
	}
}

//make process list
ProcessList* makeProcessList() {
	ProcessList* plist = malloc(sizeof(ProcessList));
	plist->head = plist->curr = plist->tail = NULL;
	return plist;
}

//pake process
Process* makeProcess(char* chars) {
	Process* p = malloc(sizeof(Process));
	int tmp = 0;
	p->PID = 0;
	while (chars[tmp] != 32 && chars[tmp] >= 48 && chars[tmp] <= 57) {
		p->PID = (10 * p->PID) + (chars[tmp] - '0');
		tmp++;
	}
	tmp++;
	p->Q = chars[tmp] - '0';
	tmp += 2;
	p->AT = 0;
	while (chars[tmp] != 32 && chars[tmp] >= 48 && chars[tmp] <= 57) {
		p->AT = (10 * p->AT) + (chars[tmp] - '0');
		tmp++;
	}
	tmp++;
	p->cycles = 0;
	while (chars[tmp] != 32 && chars[tmp] >= 48 && chars[tmp] <= 57) {
		p->cycles = (10 * p->cycles) + (chars[tmp] - '0');
		tmp++;
	}
	tmp++;
	p->TT = 0-p->AT;
	p->WT = 0;
	BList* blist = makeBList();
	int tcnt = 0;
	int num;
	for (int i = 0; i < 2 * (p->cycles) - 1; i++) {
		num = 0;
		while (chars[tmp + tcnt] != 32 && chars[tmp + tcnt] >= 48 && chars[tmp + tcnt] <= 57) {
			num = 10 * num + (chars[tmp + tcnt] - '0');
			tcnt++;
		}
		tcnt++;
		insert2BList(blist, num);
	}
	p->bursts = blist;
	p->next = p->prev = NULL;
	return p;
}

//make burst list
BList* makeBList() {
	BList* blist = malloc(sizeof(BList));
	blist->head = blist->tail = blist->curr = NULL;
	return blist;
}

//insert burst into burst list
void insert2BList(BList* blist, int burst) {
	if (blist->head == NULL) {
		blist->head = malloc(sizeof(Process));
		blist->head->burst = burst;
		blist->head->next = NULL;
		blist->tail = blist->curr = blist->head;
	}
	else {
		blist->tail->next = malloc(sizeof(BNode));
		blist->tail = blist->tail->next;
		blist->tail->burst = burst;
		blist->tail->next = NULL;
	}
}

//insert process to process list
void insert2PList(ProcessList* plist, Process* p) {
	p->next = p->prev = NULL;
	if (plist->head == NULL)
		plist->head = plist->curr = plist->tail = p;
	else {
		plist->tail->next = p;
		p->prev = plist->tail;
		plist->tail = plist->tail->next;
	}
}

void segregate(ProcessList* plist) {
	if (plist->curr == plist->head&&plist->curr == plist->tail) {
		plist->curr->prev = NULL;
		plist->curr->next = NULL;
		plist->head = NULL;
		plist->curr = NULL;
		plist->tail = NULL;
		return;
	}
	if (plist->curr == plist->head && plist->curr != plist->tail) {
		plist->curr = plist->curr->next;
		plist->curr->prev->next = NULL;
		plist->curr->prev = NULL;
		plist->head = plist->curr;
		return;
	}
	if (plist->curr != plist->head&&plist->curr == plist->tail) {	
		plist->curr->prev->next = NULL;
		plist->tail = plist->curr->prev;
		plist->curr->prev = NULL;
		plist->curr = NULL;
		return;
	}
	if (plist->curr != plist->head&&plist->curr != plist->tail) {
		plist->curr = plist->curr->prev;
		plist->curr->next = plist->curr->next->next;
		plist->curr->next->prev = plist->curr;
		plist->curr = plist->curr->next;
		return;
	}
}

void incrementWT(ProcessList* waiting) {
	if (waiting->head != NULL) {
		while (waiting->curr != NULL) {
			//printf("WT increment for Process %d is successful!\n", waiting->curr->PID);
			waiting->curr->WT++;
			waiting->curr = waiting->curr->next;
		}
		waiting->curr = waiting->head;
	}
}

//make RR with time quantum of 2
Q0* makeQ0() {
	Q0* q0 = malloc(sizeof(Q0));
	q0->TQ = 2;
	q0->waiting = makeProcessList();
	return q0;
}

//make RR with time quantum of 6
Q1* makeQ1() {
	Q1* q1 = malloc(sizeof(Q0));
	q1->TQ = 6;
	q1->waiting = makeProcessList();
	return q1;
}

//make SRTN 
Q2* makeQ2() {
	Q2* q2 = malloc(sizeof(Q2));
	q2->shortestNow = malloc(sizeof(Process));
	q2->shortestNow->bursts = malloc(sizeof(BList));
	q2->shortestNow->bursts->curr = malloc(sizeof(BNode));
	q2->waiting = makeProcessList();
	return q2;
}

//make FCFS
Q3* makeQ3() {
	Q3* q3 = malloc(sizeof(Q3));
	q3->waiting = makeProcessList();
	return q3;
}

//make cpu
CPU* makeCPU() {
	CPU* mycpu = malloc(sizeof(CPU));
	mycpu->running = NULL;
	mycpu->preemptionTime = 0;
	mycpu->runningTime = 0;
	mycpu->originQ = 0;
	return mycpu;
}

//run 
void run(ProcessList* plist, Q0* q0, Q1* q1, Q2* q2, Q3* q3, CPU* mycpu, ProcessList* asleepList, ProcessList* zombieList) {
	int time = 0;
	while (plist->head != NULL || asleepList->head != NULL
		|| q0->waiting->head != NULL || q1->waiting->head != NULL
		|| q2->waiting->head != NULL || q3->waiting->head != NULL
		|| mycpu->running != NULL) {
		allocate(time, plist, q0, q1, q2, q3);
		Q0ProcessCheck(time, q0, mycpu);
		Q1ProcessCheck(time, q1, mycpu);
		Q2ShortestOneCheck(time, q2, q3, mycpu);
		Q3ProcessCheck(time, q3, mycpu);
		IOInterruptCheck(time, asleepList, q0, q1, q2);
		runCPU(mycpu);
		goSleepOrOut(time, mycpu, asleepList, zombieList);
		preemptionCheckForQ01(time, q1, q2, mycpu);
		printf("cycle %d ended!\n\n", time);
		time++;
	}
}

void allocate(int time, ProcessList* plist, Q0* q0, Q1* q1, Q2* q2, Q3* q3) {
	printf("cycle %d starting!\n", time);
	if (plist->head != NULL) {
		while (plist->curr != NULL) {
			//if Init queue match
			if (time >= plist->curr->AT) {
				Process* tmp = malloc(sizeof(Process));
				tmp = plist->curr;
				segregate(plist);
				if (tmp->Q == 0) {
					printf("process %d going into q0 waiting q!\n", tmp->PID);
					insert2PList(q0->waiting, tmp);
				}
				else if (tmp->Q == 1) {
					printf("process %d going into q1 waiting q!\n", tmp->PID);
					insert2PList(q1->waiting, tmp);
				}
				else if (tmp->Q == 2) {
					printf("process %d going into q2 waiting q!\n", tmp->PID);
					insert2PList(q2->waiting, tmp);
				}
				else if (tmp->Q == 3) {
					printf("process %d going into q3 waiting q!\n", tmp->PID);
					insert2PList(q3->waiting, tmp);
				}
			}
			else
				plist->curr = plist->curr->next;
		}
		plist->curr = plist->head;
	}
}

void IOInterruptCheck(int time, ProcessList* asleepList, Q0* q0, Q1* q1, Q2* q2) {
	if (asleepList->head != NULL) {
		//from head to tail
		while (asleepList->curr != NULL) {
			asleepList->curr->bursts->curr->burst--;
			//if IO burst is over
			if (asleepList->curr->bursts->curr->burst == 0) {
				printf("Process %d I/O burst time ended! going to wake up!\n", asleepList->curr->PID);
				asleepList->curr->bursts->curr = asleepList->curr->bursts->curr->next;
				Process* tmp;
				tmp = asleepList->curr;
				tmp->AT = time + 1;
				segregate(asleepList);
				if (tmp->Q == 0)
					insert2PList(q0->waiting, tmp);
				else if (tmp->Q == 1)
					insert2PList(q0->waiting, tmp);
				else if (tmp->Q == 2)
					insert2PList(q1->waiting, tmp);
				else
					insert2PList(q2->waiting, tmp);
			}
			//if IO burst is not over
			else
				asleepList->curr = asleepList->curr->next;
		}
		if(asleepList->head!=NULL)
			asleepList->curr = asleepList->head;
	}
}

void Q0ProcessCheck(int time, Q0* q0, CPU* mycpu) {
	//if q is not working and waiting list is not empty, run q
	//if cpu not occupied
	if (q0->waiting->head != NULL) {
		//no process running on cpu
		if (mycpu->running == NULL) {
			//arrived on time
			if (time >= q0->waiting->curr->AT) {
				printf("Process %d in Q0 is about to occupy cpu!\n", q0->waiting->head->PID);
				mycpu->running = q0->waiting->curr;
				mycpu->preemptionTime = q0->TQ;
				mycpu->runningTime = 0;
				mycpu->originQ = 0;
				segregate(q0->waiting);
			}
		}
		incrementWT(q0->waiting);
	}
}

void Q1ProcessCheck(int time, Q1* q1, CPU* mycpu) {
	//if q is not working and waiting list is not empty, run q
	//if cpu not occupied
	if (q1->waiting->head != NULL) {
		//no process running on cpu
		if (mycpu->running == NULL) {
			//arrived on time
			if (time >= q1->waiting->head->AT) {
				printf("Process %d in Q1 is about to occupy cpu!\n", q1->waiting->head->PID);
				mycpu->running = q1->waiting->head;
				mycpu->preemptionTime = q1->TQ;
				mycpu->runningTime = 0;
				mycpu->originQ = 1;
				segregate(q1->waiting);
			}
		}
		incrementWT(q1->waiting);
	}
}

void Q2ShortestOneCheck(int time, Q2* q2, Q3* q3, CPU* mycpu) {
	//if cpu is not scheduled from Q2, return
	if (mycpu->running != NULL) {
		if (mycpu->originQ != 2) {
			incrementWT(q2->waiting);
			return;
		}
	}
	if (q2->waiting->head != NULL) {
		//if cpu is occupied
		if (mycpu->running != NULL) {
			q2->shortestNow = mycpu->running;
		}
		//if cpu is not occupied
		else
			q2->shortestNow = q2->waiting->head;
		//now traverse to find the good value
		while (q2->waiting->curr != NULL) {
			if (time >= q2->waiting->curr->AT&&
				q2->waiting->curr->bursts->curr->burst < q2->shortestNow->bursts->curr->burst)
				q2->shortestNow = q2->waiting->curr;
			else
				q2->waiting->curr = q2->waiting->curr->next;
		}
		q2->waiting->curr = q2->shortestNow;
		//if found the one to preempt
		if (mycpu->running != NULL) {
			if (mycpu->running->bursts->curr->burst > q2->shortestNow->bursts->curr->burst) {
				printf("Preemption is going to happen in Q2\n");
				printf("Process %d is going to take cpu\n", q2->shortestNow->PID);
				q2->waiting->curr = q2->shortestNow;
				insert2PList(q3->waiting, mycpu->running);
				mycpu->running = q2->waiting->curr;
				mycpu->preemptionTime = 0;
				mycpu->runningTime = 0;
				mycpu->originQ = 2;
				segregate(q2->waiting);
			}
		}
		else {
			printf("Process %d from Q2 is going to take cpu\n", q2->shortestNow->PID);
			q2->waiting->curr = q2->shortestNow;
			mycpu->running = q2->waiting->curr;
			mycpu->preemptionTime = 0;
			mycpu->runningTime = 0;
			mycpu->originQ = 2;
			segregate(q2->waiting);
		}
		if (q2->waiting->head != NULL) {
			q2->waiting->curr = q2->waiting->head;
			incrementWT(q2->waiting);
		}
	}
}

void Q3ProcessCheck(int time, Q3* q3, CPU* mycpu) {
	//if q is not working and waiting list is not empty, run q
	//if cpu not occupied
	if (q3->waiting->head != NULL) {
		//no process running on cpu
		if (mycpu->running == NULL) {
			//arrived on time
			if (time >= q3->waiting->head->AT) {
				printf("Process %d in Q3 is about to occupy cpu!\n", q3->waiting->head->PID);
				mycpu->running = q3->waiting->head;
				mycpu->preemptionTime = 0;
				mycpu->runningTime = 0;
				mycpu->originQ = 3;
				segregate(q3->waiting);
			}
		}
		incrementWT(q3->waiting);
	}
}

void runCPU(CPU* mycpu) {
	if (mycpu->running != NULL) {
		printf("Process %d burst!\n", mycpu->running->PID);
		mycpu->running->bursts->curr->burst--;
		mycpu->runningTime++;
	}
}

void goSleepOrOut(int time, CPU* mycpu, ProcessList* asleepList, ProcessList* zombieList) {
	if (mycpu->running != NULL) {
		if (mycpu->running->bursts->curr->burst == 0) {
			printf("Process %d Cpu Burst Time is over! might sleep!\n", mycpu->running->PID);
			if (mycpu->running->bursts->curr->next != NULL) {
				mycpu->running->bursts->curr = mycpu->running->bursts->curr->next;
				mycpu->running->Q = mycpu->originQ;
				insert2PList(asleepList, mycpu->running);

			}
			else {
				printf("Process %d ended! going to zombie List!\n", mycpu->running->PID);
				mycpu->running->TT += time;
				insert2PList(zombieList, mycpu->running);
			}
			mycpu->originQ = 0;
			mycpu->preemptionTime = 0;
			mycpu->running = NULL;
			mycpu->runningTime = 0;
		}
	}
}

void preemptionCheckForQ01(int time, Q1* q1, Q2* q2, CPU* mycpu) {
	if (mycpu->running != NULL) {
		if (mycpu->originQ == 0 || mycpu->originQ == 1) {
			if (mycpu->runningTime == mycpu->preemptionTime) {
				printf("Process %d scheduled from Q%d has been Preempted!\n", mycpu->running->PID, mycpu->originQ);
				if (mycpu->originQ == 0)
					insert2PList(q1->waiting, mycpu->running);
				else if (mycpu->originQ == 1)
					insert2PList(q2->waiting, mycpu->running);
				mycpu->originQ = 0;
				mycpu->preemptionTime = 0;
				mycpu->running = NULL;
				mycpu->runningTime = 0;
			}
			else
					mycpu->running->PID, mycpu->originQ, mycpu->runningTime, mycpu->preemptionTime;
		}
	}
}

void showReports(ProcessList* zombieList) {
	float mtt = 0;
	float mwt = 0;
	int tpcnt = 0;
	if (zombieList->head != NULL) {
		printf("*************  Process reports  *************\n");
		printf("---------------------------------------------\n");
		while (zombieList->curr != NULL) {
			printf("Process %d's TT: %d\t WT: %d\n",
				zombieList->curr->PID, zombieList->curr->TT, zombieList->curr->WT);
			tpcnt++;
			mtt += zombieList->curr->TT;
			mwt += zombieList->curr->WT;
			zombieList->curr = zombieList->curr->next;
		}
		mtt /= (float)tpcnt;
		mwt /= (float)tpcnt;
		printf("---------------------------------------------\n");
		printf("Process Count: %d\n", tpcnt);
		printf("Average TT: %0.2f\t Average WT: %0.2f\n", mtt, mwt);
		printf("---------------------------------------------\n");
	}
	else
		printf("Nothing to report!\n");
}
