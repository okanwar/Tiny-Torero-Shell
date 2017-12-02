/*
 * The Tiny Torero Shell (TTSH)
 *
 * history_queue.c
 *
 * This file contains methods to interact with the history queue
 *
 * This file is part of COMP 280 Lab 7
 *
 * Authors:
 * 	1. Michael Dana
 * 	2. Om Kanwar
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "history_queue.h"

// global variables: add only globals for history list state
//                   all other variables should be allocated on the stack
// static: means these variables are only in scope in this .c module
static HistoryEntry history[MAXHIST]; 

static int queue_start = 0;
static int queue_next = 0;
static int queue_size = 0;
static int cmd_num = 0; 


void add_queue(char *cmd_line){
	
	if(queue_size == MAXHIST){
		queue_start++;
	}
	if(queue_start > MAXHIST){
		queue_start = 0;
	}
	if(queue_next == MAXHIST){
		queue_next = 0;
	}

	//Add new element to queue
	history[queue_next].cmd_num = cmd_num;
	strcpy( history[queue_next].cmdline, cmd_line );
	
	cmd_num++;
	queue_next++;

	if(queue_size < MAXHIST){
		queue_size++;
	}
}	

void print_history(){
	int i;
	for(i = queue_start; i < queue_size; i++){
		printf("%d\t%s", history[i].cmd_num, history[i].cmdline);
	}
	for(i = 0; i < queue_start; i++){
		printf("%d\t%s", history[i].cmd_num, history[i].cmdline);
	}
}

char* check_history(unsigned int index) {
	int k;
	for(k = queue_start; k < queue_size; k++) {
		if(history[k].cmd_num == index) {
		   return history[k].cmdline;
		}
	}
	for(k = 0; k < queue_start; k++) {
		if(history[k].cmd_num == index) {
			return history[k].cmdline;
		}
	}
	return NULL;
}
