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

//items to initialize and start queue
static int queue_start = 0;
static int queue_next = 0;
static int queue_size = 0;
static int cmd_num = 0; 

/**
 * initializes queue, adds elements to queue, and updates queue
 * @param *cmd_line The command line that we want to add to our queue
 */
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

/**
 * This function prints out a list of most recent commands done by our shell
 *
 * This function takes no parameters, it only prints out information
 * for loops are used to iterate over our queue, to print out all commands
 */
void print_history(){
	int i;
	for(i = queue_start; i < queue_size; i++){
		printf("%d\t%s", history[i].cmd_num, history[i].cmdline);
	}
	for(i = 0; i < queue_start; i++){
		printf("%d\t%s", history[i].cmd_num, history[i].cmdline);
	}
}
/**
 * Returns the command string at the specified index and looks at our history
 * qeueue
 *
 * @param index the position in our history queue that returns the specified
 * string
 */

char* check_history(int index) {
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
char* repeatCmd() {
	int k;
	int n = 0;
	int m = 0;
	for(k = queue_start; k < queue_size; k++) {
		if(history[k].cmd_num > n) {
			m = k;
			n = history[k].cmd_num;
		}
	}
	for(k = 0; k < queue_start; k++) {
		if(history[k].cmd_num > n) {
			m = k;
			n = history[k].cmd_num;
		}	
	}
	return history[m].cmdline;
}
