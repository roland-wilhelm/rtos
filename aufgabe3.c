/*
 * aufgabe3.c
 *
 *  Created on: 07.12.2012
 *      Author: roland
 */

#include "headers.h"


static bool on_looping_flag = true;
static volatile unsigned int count_loops = 0;
static pthread_mutex_t count_loops_lock = PTHREAD_MUTEX_INITIALIZER;

void stop_looping_aufgabe3() {

	on_looping_flag = false;
}

bool set_count_loops(unsigned int a_count_loops){

	int ret = 0;

	ret = pthread_mutex_lock(&count_loops_lock);
	if(ret) {

		fprintf(stderr, "error: set_count_loops - pthread_mutex_lock.\n");
		print_error_code(ret);
		return false;
	}
	count_loops = a_count_loops;
	ret = pthread_mutex_unlock(&count_loops_lock);
	if(ret) {

		fprintf(stderr, "error: set_count_loops - pthread_mutex_unlock.\n");
		print_error_code(ret);
		return false;
	}
	dbg_h("Debug: Set loop counter %u.\n", count_loops);

	return true;
}

unsigned int get_count_loops(){

	unsigned int ret = 0, counter = 0;

	ret = pthread_mutex_lock(&count_loops_lock);
	if(ret) {

		fprintf(stderr, "error: get_count_loops - pthread_mutex_lock.\n");
		print_error_code(ret);
		return 0;
	}
	counter = count_loops;
	ret = pthread_mutex_unlock(&count_loops_lock);
	if(ret) {

		fprintf(stderr, "error: get_count_loops - pthread_mutex_unlock.\n");
		print_error_code(ret);
		return 0;
	}
	dbg_h("Debug: Get loop counter %u.\n", ret);

	return counter;
}


bool waste_time(unsigned int msec) {

	unsigned int i, j, counter = get_count_loops();

	for(i=0; ((i<msec) && (i<=100)); i++) {

		for(j=0; j<counter; j++) {

			if(!on_looping_flag)
				break;
		}

		if(!on_looping_flag)
			break;
	}

	if(i==100) {

		fprintf(stderr, "error: Waste time - exceed time limit.\n");
		return false;
	}
	return true;
}


bool init_waste_time() {

	struct _scheduler scheduler;
	clock_t start, end;
	unsigned int maximum_waste_time = 10;


	get_priority(pthread_self(), &scheduler);
	set_priority(pthread_self(), INIT_WASTE_TIME_SCHEDULER, INIT_WASTE_TIME_SCHEDULER_PRIOR);
	set_count_loops(100000000);

	start = clock();

	if(start == -1) {

		fprintf(stderr, "error: Check waste time - clock (start).\n");
		print_error_code(start);
		return false;
	}

	waste_time(maximum_waste_time);

	end = clock();

	if(end == -1) {

		fprintf(stderr, "error: Check waste time - clock (end).\n");
		print_error_code(end);
		return false;
	}

	set_count_loops((unsigned int)(((maximum_waste_time*(double)get_count_loops())/(end - start))*1000));
	set_priority(pthread_self(), scheduler.policy, scheduler.priority);

	printf("\tDeltaT [Ticks] : %u.\n", end - start);
	printf("\tDeltaT [s]: %f.\n", ((float)(end - start)) / CLOCKS_PER_SEC);
	printf("\tCalculated Loop counter: %u.\n", get_count_loops());

	return true;
}
