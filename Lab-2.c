﻿/*
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
	process = (table_typedef*)malloc(MAX_PROCS * sizeof(table_typedef)); // Memory is allocated for 'n' elements 
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
	/* declare (and initilize when appropriate) local variables */
	int earliestArrival = 0;
	int earlistArrivalIndex = 0;
	int currentProcArrival = 0;
	int procScheduled = 1;
	int start = 0;
	int end = 0;
	int totalTime = 0;
	bool firstScan = true;

	/* for each process, reset "done" field to 0 */
	for (int i = 1; i <= MAX_PROCS; i++) {
		process[i].done = 0;
	}
	// scan through each proc
	for (; procScheduled <= MAX_PROCS; procScheduled++) {
		/* initilize the earliest arrival time to INT_MAX (largest integer value) */
		earliestArrival = INT_MAX;
		// starting from ID[1], identify which proc we are going to start with (lowest arrival time)
		// scan through every proc in search of the lowest one and record it (starting from all the available procs
		for (int scanIndex = 1; scanIndex <= MAX_PROCS; scanIndex++) {	
			if (process[scanIndex].done == 0) {		// only scan through non-scheduleded processes
				//printf("\n\nChecking Process[%d] against all others to find lowest arrival time", scanIndex);
				//printf("\tearliestArrival is = %d", earliestArrival);
				currentProcArrival = process[scanIndex].arrival;	// record non-scheduled process
				//printf("\tcurrentProcArrival of Process[%d] = %d\n", scanIndex, currentProcArrival);
				earliestArrival = MinOfTwoInts(earliestArrival, currentProcArrival);
				//printf("earliestArrival is now = %d\n", earliestArrival);
				if (earliestArrival >= currentProcArrival) {
					earliestArrival = currentProcArrival;
					earlistArrivalIndex = scanIndex;
					//printf("What is the earliest arrival? = %d\n", earliestArrival);
				}
			}
		}
		process[earlistArrivalIndex].done = 1;
		//printf("\nEarliest Arrival Time List (Value, Index) = (%d, %d)\n", earliestArrival, earlistArrivalIndex);
		/* set start time, end time, turnaround time, done fields for unscheduled process with earliest arrival time */
		if (firstScan) {
			start = process[earlistArrivalIndex].arrival;
			end = process[earlistArrivalIndex].total_cpu;
			firstScan = false;
		}
		else {
			start = end;
			end += process[earlistArrivalIndex].total_cpu;
		}
		process[earlistArrivalIndex].start_time = start;
		process[earlistArrivalIndex].end_time = end;
		process[earlistArrivalIndex].turnaround_time = process[earlistArrivalIndex].end_time - process[earlistArrivalIndex].arrival;
		//printf("\nProcess[%d]: start_time = %d, end_time = %d\n", earlistArrivalIndex, process[earlistArrivalIndex].start_time, process[earlistArrivalIndex].end_time);
		/* update current cycle time and increment number of processes scheduled */
	}
	/* print contents of table */
	PrintTable();
	return;
}


// option #3 SJF algorithm (non-preemptive) Shortest-Job-First
/***************************************************************/
void SchedProcSJF() {
	/* while there are still processes to schedule */
		/* initilize the lowest total cycle time to INT_MAX (largest integer value) */
		/* for each process not yet scheduled */
			/* check if process has lower total cycle time than current lowest and has arrival time less than current cycle time and update */
		/* set start time, end time, turnaround time, done fields for unscheduled process with lowest (and available) total cycle time */
		/* update current cycle time and increment number of processes scheduled */
	/* print contents of table */

	/* declare (and initilize when appropriate) local variables */
	int lowestCycleTime = 0;
	int lowestCycleIndex = 0;
	int currentProcArrival = 0;
	int arrivalStart = 0;
	int procScheduled = 1;
	int start = 0;
	int end = 0;
	int totalTime = 0;
	bool firstScan = true;

	/* for each process, reset "done" field to 0 */
	for (int i = 1; i <= MAX_PROCS; i++) {
		process[i].done = 0;
		if (process[i].arrival == 0) arrivalStart = i;
	}
	// scan through each proc
	for (; procScheduled <= MAX_PROCS; procScheduled++) {
		/* initilize the earliest arrival time to INT_MAX (largest integer value) */
		lowestCycleTime = INT_MAX;
		// starting from ID[1], identify which proc we are going to start with (lowest arrival time)
		// scan through every proc in search of the lowest one and record it (starting from all the available procs
		for (int scanIndex = 1; scanIndex <= MAX_PROCS; scanIndex++) {
			if (process[scanIndex].done == 0) {		// only scan through non-scheduleded processes
				//printf("\n\nChecking Process[%d] against all others to find lowest arrival time", scanIndex);
				//printf("\tearliestArrival is = %d", earliestArrival);
				currentProcArrival = process[scanIndex].total_cpu;	// record non-scheduled process
				//printf("\tcurrentProcArrival of Process[%d] = %d\n", scanIndex, currentProcArrival);
				lowestCycleTime = MinOfTwoInts(lowestCycleTime, currentProcArrival);
				//printf("earliestArrival is now = %d\n", earliestArrival);
				if (lowestCycleTime >= currentProcArrival) {
					lowestCycleTime = currentProcArrival;
					lowestCycleIndex = scanIndex;
					//printf("What is the earliest arrival? = %d\n", earliestArrival);
				}
			}
		}
		//printf("\nEarliest Arrival Time List (Value, Index) = (%d, %d)\n", earliestArrival, lowestCycleIndex);
		/* set start time, end time, turnaround time, done fields for unscheduled process with earliest arrival time */
		if (firstScan) {
			lowestCycleIndex = arrivalStart;
			start = process[lowestCycleIndex].arrival;
			end = process[lowestCycleIndex].total_cpu;
			firstScan = false;
		}
		else {
			start = end;
			end += process[lowestCycleIndex].total_cpu;
		}
		process[lowestCycleIndex].done = 1;
		process[lowestCycleIndex].start_time = start;
		process[lowestCycleIndex].end_time = end;
		process[lowestCycleIndex].turnaround_time = process[lowestCycleIndex].end_time - process[lowestCycleIndex].arrival;
		//printf("\nProcess[%d]: start_time = %d, end_time = %d\n", lowestCycleIndex, process[lowestCycleIndex].start_time, process[lowestCycleIndex].end_time);
		/* update current cycle time and increment number of processes scheduled */
	}
	/* print contents of table */
	PrintTable();
	return;
}


// option #4 SRT algorithm (preemptive version of SJF) Shortest-remaining-time-first
/***************************************************************/
void SchedProcSRT() {
	/* declare (and initilize when appropriate) local variables */

	/* for each process, reset "done", "total_remaining" and "already_started" fields to 0 */

	/* while there are still processes to schedule */
		/* initilize the lowest total remaining time to INT_MAX (largest integer value) */
		/* for each process not yet scheduled */
			/* check if process has lower total remaining time than current lowest and has arrival time less than current cycle time and update */
		/* check if process already partially-scheduled */
			/* if so, set "start time", "already_started" fields of process with lowest (and available) total remaining cycle time */
		/* set end time, turnaround time of process with lowest (and available) total remaining cycle time */
		/* decrement total remaining time of process with lowest (and available) total remaining cycle time */
		/* if remaining time is 0, set done field to 1, increment cycle time and number of scheduled processes*/
	/* print contents of table */
	return;
}


// option #5
// free the memory being used by the program then quit it
/***************************************************************/
void FreeMemoryQuitProgram() {
	/* free the schedule table if not NULL */
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
	return 1; /* indicates success */
}