/*
Lab 2 - Batch Process Scheduling
Comp 322/L
Nima Shafie
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h> 

/*
	◦ id: the unique identifier of the process
	◦ arrival: the point in time when the process enters the ready list to be executed by the CPU
	◦ total_cpu: the amount of CPU time the process will consume between arrival and departure
	◦ total_remaining: the amount of CPU time remaining for a process to complete {used for SRT scheduling only}
	◦ done: a flag that indicates whether the process has been successfully completed (1) or not (0)
	◦ start_time: the time when the process has begun being executed by the CPU
	◦ already_started: a flag that indicated whether the process has already begun (1) or not (0) {used for SRT scheduling only}
	◦ end_time: the time when the process has been completed by the CPU
	◦ turnaround_time: the sum of the total CPU time and the waiting time (alternatively: the difference between the end time and the arrival time)
*/

/* declare global variables including a table structure to hold scheduling information */
struct TableType {
	int id;
	int arrival;
	int total_cpu;
	int total_remaining;
	bool done;
	int start_time;
	bool already_started;
	int end_time;
	int turnaround_time;
} *process = NULL;
typedef struct TableType table_typedef;	// pcb = struct Node

int MAX_PROCS;

// finds the minimum of two integers
int MinOfTwoInts(int num1, int num2) {
	if (num1 > num2) return num2;
	else return num1;
}

// prints out a formatted table with table_type variables
/***************************************************************/
void PrintTable() {
	/* declare local variables */

	/* print table header */
	printf("ID\t Arrival \t Total \t Start \t End \tTurnaround");
	printf("\n-------------------------------------------------------------\n");

	/* for each process */
	for (int i = 1; i <= MAX_PROCS; i++) {
		/* print the contents (id, arrival time, total_cycles) of each field of the table's index */
		printf("%d\t", process[i].id);
		printf(" %d\t", process[i].arrival);
		printf("\t %d", process[i].total_cpu);
		if (process[i].done == false) printf("\n");
		/* if process has been scheduled ("done" field is 1, print other contents (start time, end time, turnaround time) */
		else {
			printf("\t %d", process[i].start_time);
			printf("\t %d", process[i].end_time);
			printf("\t%d\n", process[i].turnaround_time);
		}
	}
	printf("\n");
	return;
}


// option #1
// used to receive user input and assign the values to the table_type struct
/***************************************************************/
void EnterParameters() {
	/* declare local variables */
	int id;
	int arrival;
	int total_cpu;
	bool validProcess;

	/* prompt for total number of processes */
	printf("Enter total number of processes: ");
	scanf("%d", &MAX_PROCS);

	/* allocate memory for table to hold process parameters */
	process = (table_typedef*)malloc(MAX_PROCS * sizeof(process)); // Memory is allocated for 'n' elements 
	if (process == NULL) {
		printf("\nNo memory is allocated.\n\n");
		exit(0);
	}

	// each process[i] is its own process, so we're still making a dynamic array of processes
	for (int i = 1; i <= MAX_PROCS; i++) {
		process[i].total_cpu = 0;
		process[i].id = i;
		process[i].arrival = 0;
		process[i].total_cpu = 0;
		process[i].total_remaining = 0;
		process[i].done = false;
		process[i].start_time = 0;
		process[i].already_started = false;
		process[i].end_time = 0;
		process[i].turnaround_time = 0;
	}

	/* for each process */
	for (int i = 1; i <= MAX_PROCS; i++) {
		/* prompt for process id, arrival time, and total cycle time */
		// only accept ID if it's between 1 and n
		do {
			validProcess = true;
			printf("\nEnter Process ID: ");      // i think the user enters which process id to work on here
			scanf("%d", &id);
			if (id <= 0 || id > MAX_PROCS) {
				printf("\nProcess ID must be greater than 0 and less than total processes\n");
				validProcess = false;
			}
			else {
				if (process[id].total_cpu != 0) {		// if the chosen process has already been selected, prompt a different process to select
					printf("\nThis Process ID has already been initialized earlier, choose a different one\n");
					validProcess = false;
				}
			}
		} while (!validProcess);
		do {
			printf("Enter arrival cycle for process P[%d", id);
			printf("]: ");
			scanf("%d", &arrival);
			if (arrival < 0) printf("\nArrival cycle must be at least 0 or greater\n");
		} while (arrival < 0);
		process[id].arrival = arrival;
		do {
			printf("Enter total cycles for process P[%d", id);
			printf("]: ");
			scanf("%d", &total_cpu);
			if (total_cpu <= 0) printf("\nTotal cycles must be at least 0 or greater\n");
		} while (total_cpu <= 0);
		process[id].total_cpu = total_cpu;
	}
	printf("\n");
	/* print contents of table */
	PrintTable();
	return;
}


// option #2
/***************************************************************/
void SchedProcFIFO() {
	int lowestArrivalTime = 0;
	int currentProcArrival = 0;
	int procScheduled = 0;
	int scanIndex = 1;
	int totalTimer = 0;
	bool foundProc = false;

	/* for each process, reset "done" field to 0 */
	for (int i = 1; i <= MAX_PROCS; i++) process[i].done = 0;
	// scan through each proc
	while (procScheduled < MAX_PROCS) {
		if (process[scanIndex].done == 0 && (process[scanIndex].arrival <= totalTimer)) {
			//printf("\nEarliest Arrival Time List (Value, Index) = (%d, %d)\n", earliestArrival, earliestArrivalIndex);
			process[scanIndex].start_time = totalTimer;
			totalTimer += process[scanIndex].total_cpu;
			process[scanIndex].done = true;
			process[scanIndex].end_time = totalTimer;
			process[scanIndex].turnaround_time = process[scanIndex].end_time - process[scanIndex].arrival;
			procScheduled++;
			scanIndex++;
			foundProc = false;
			//printf("\nProcess[%d]: start_time = %d, end_time = %d, turn_around = %d\n", earliestArrivalIndex, process[earliestArrivalIndex].start_time, process[earliestArrivalIndex].end_time, process[earliestArrivalIndex].turnaround_time);
		}
		else totalTimer++;
	}
	/* print contents of table */
	PrintTable();
	return;
}


// option #3 SJF algorithm (non-preemptive) Shortest-Job-First
/***************************************************************/
void SchedProcSJF() {
	int lowestCycleTime = 0;
	int lowestCycleIndex = 0;
	int currentProcArrival = 0;
	int procScheduled = 0;
	int scanIndex = 1;
	int totalTimer = 0;
	bool foundProc = false;

	/* for each process, reset "done", "total_remaining" and "already_started" fields to 0 */
	for (int i = 1; i <= MAX_PROCS; i++) {
		process[i].done = 0;
		process[i].total_remaining = process[i].total_cpu;
	}
	// scan through each proc
	while (procScheduled < MAX_PROCS) {
		/* initilize the earliest arrival time to INT_MAX (largest integer value) */
		lowestCycleTime = INT_MAX;
		scanIndex = 1;
		while (!foundProc) {
			for (; scanIndex <= MAX_PROCS; scanIndex++) {
				if (process[scanIndex].done == 0 && (process[scanIndex].arrival <= totalTimer)) {		// only scan through non-scheduleded procs & procs that have arrived on time compared to total timer
					currentProcArrival = process[scanIndex].total_cpu;	// record non-scheduled process
					lowestCycleTime = MinOfTwoInts(lowestCycleTime, currentProcArrival);
					if (lowestCycleTime >= currentProcArrival) {
						lowestCycleTime = currentProcArrival;
						lowestCycleIndex = scanIndex;
						foundProc = true;
						//printf("\nProcess[%d]\t arrival = %d\t time remaining = %d\n", lowestCycleIndex, process[lowestCycleIndex].arrival, process[lowestCycleIndex].total_remaining);
					}
				}
			}
			if (!foundProc) {		// we only reach here if the total timer has not yet reached the next proc's arrival time, so we just run this as idle time and extend the start and end timer
				totalTimer++;
				scanIndex = 1;
			}
		}
		//printf("\nEarliest Arrival Time List (Value, Index) = (%d, %d)\n", earliestArrival, lowestCycleIndex);
		process[lowestCycleIndex].start_time = totalTimer;
		totalTimer += process[lowestCycleIndex].total_cpu;
		process[lowestCycleIndex].total_remaining -= process[lowestCycleIndex].total_cpu;
		process[lowestCycleIndex].done = true;
		process[lowestCycleIndex].end_time = totalTimer;
		process[lowestCycleIndex].turnaround_time = process[lowestCycleIndex].end_time - process[lowestCycleIndex].arrival;
		procScheduled++;
		foundProc = false;
		//printf("\nProcess[%d]: start_time = %d, end_time = %d, turn_around = %d\n", lowestCycleIndex, process[lowestCycleIndex].start_time, process[lowestCycleIndex].end_time, process[lowestCycleIndex].turnaround_time);
	}
	/* print contents of table */
	PrintTable();
	return;
}


// option #4 SRT algorithm (preemptive version of SJF) Shortest-remaining-time-first
/***************************************************************/
void SchedProcSRT() {
	int lowestCycleTime = 0;
	int lowestCycleIndex = 0;
	int currentProcArrival = 0;
	int procScheduled = 0;
	int scanIndex = 1;
	int totalTimer = 0;
	bool foundProc = false;

	/* for each process, reset "done", "total_remaining" and "already_started" fields to 0 */
	for (int i = 1; i <= MAX_PROCS; i++) {
		process[i].done = 0;
		process[i].total_remaining = process[i].total_cpu;
		process[i].already_started = 0;
	}
	// scan through each proc
	while (procScheduled < MAX_PROCS) {
		/* initilize the earliest arrival time to INT_MAX (largest integer value) */
		lowestCycleTime = INT_MAX;
		scanIndex = 1;
		while (!foundProc) {
			for (; scanIndex <= MAX_PROCS; scanIndex++) {
				if (process[scanIndex].done == 0 && (process[scanIndex].arrival <= totalTimer)) {		// only scan through non-scheduleded procs & procs that have arrived on time compared to total timer
					currentProcArrival = process[scanIndex].total_remaining;	// record non-scheduled process
					lowestCycleTime = MinOfTwoInts(lowestCycleTime, currentProcArrival);
					if (lowestCycleTime >= currentProcArrival) {
						lowestCycleTime = currentProcArrival;
						lowestCycleIndex = scanIndex;
						foundProc = true;
						//printf("\nProcess[%d]\t arrival = %d\t time remaining = %d\n", lowestCycleIndex, process[lowestCycleIndex].arrival, process[lowestCycleIndex].total_remaining);
					}
				}
			}
			if (!foundProc) {		// we only reach here if the total timer has not yet reached the next proc's arrival time, so we just run this as idle time and extend the start and end timer
				totalTimer++;
				scanIndex = 1;
			}
		}
		//printf("\nEarliest Arrival Time List (Value, Index) = (%d, %d)\n", earliestArrival, lowestCycleIndex);
		// the logic is we're going to enter these blocks below for every 1 time cycle, we can then keep updating the process and see if there is an ealier one we want to start
		if (process[lowestCycleIndex].already_started == false) {		// we start the start timer
			process[lowestCycleIndex].already_started = true;
			process[lowestCycleIndex].start_time = totalTimer;
			process[lowestCycleIndex].total_remaining--;
		}
		else {		// we've already encountered this proc before, continue cycling through
			process[lowestCycleIndex].total_remaining--;
		}
		totalTimer++;
		if (process[lowestCycleIndex].total_remaining == 0) {		// we've finish the process
			process[lowestCycleIndex].done = true;
			process[lowestCycleIndex].end_time = totalTimer;
			process[lowestCycleIndex].turnaround_time = process[lowestCycleIndex].end_time - process[lowestCycleIndex].arrival;
			procScheduled++;
		}
		foundProc = false;
		//printf("\nProcess[%d]: start_time = %d, end_time = %d, turn_around = %d\n", lowestCycleIndex, process[lowestCycleIndex].start_time, process[lowestCycleIndex].end_time, process[lowestCycleIndex].turnaround_time);
	}
	/* print contents of table */
	PrintTable();
	return;
}


// option #5
// free the memory being used by the program then quit it
/***************************************************************/
void FreeMemoryQuitProgram() {
	/* free the schedule table if not NULL */
	if (process != NULL) printf("\nDestroying remaining processes\n");
	free(process);
	return;
}


/***************************************************************/
int main() {
	int i = 0;
	enum { PARAM, FIFO, SJF, SRT, QUIT, INVALID } menuChoice;

	while (i != 5) {
		printf("Batch Scheduling\n-------------------------------\n");
		printf("1) Enter parameters\n");
		printf("2) Schedule processes with FIFO alogirthm\n");
		printf("3) Schedule processes with SJF algorithm\n");
		printf("4) Schedule processes with SRT algorithm\n");
		printf("5) Quit program and free memory\n");
		printf("\nEnter selection: ");
		scanf("%d", &i);
		if (i == 1) menuChoice = PARAM;
		else if (i == 2) menuChoice = FIFO;
		else if (i == 3) menuChoice = SJF;
		else if (i == 4) menuChoice = SRT;
		else if (i == 5) menuChoice = QUIT;
		else menuChoice = INVALID;

		switch (menuChoice) {
		case PARAM:
			EnterParameters();
			break;
		case FIFO:
			SchedProcFIFO();
			break;
		case SJF:
			SchedProcSJF();
			break;
		case SRT:
			SchedProcSRT();
			break;
		case QUIT:
			FreeMemoryQuitProgram();
			break;
		case INVALID:
			printf("Invalid selection made, try again.\n\n");
		}
	};
	printf("\nThank you for using the Batch Process Scheduling program, have a good day!\n");
	return 1; /* indicates success */
}