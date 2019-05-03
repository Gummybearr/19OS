#include "MFQ.h"

int main() {
	FILE *f;
	f = fopen("input2.txt", "r");
	ProcessList* plist = makeProcessList();
	init(f, plist);
	Q0* q0 = makeQ0();
	Q1* q1 = makeQ1();
	Q2* q2 = makeQ2();
	Q3* q3 = makeQ3();
	CPU* mycpu = makeCPU();
	ProcessList* asleepList = makeProcessList();
	ProcessList* zombieList = makeProcessList();
	run(plist, q0, q1, q2, q3, mycpu, asleepList, zombieList);
	showReports(zombieList);
	free(q0);
	free(q1);
	free(q2);
	free(q3);
	free(mycpu);
	free(plist);
	free(asleepList);
	free(zombieList);
	return 0;
}