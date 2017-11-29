/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
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

// Look at your lab 10 implementation of a circular queue of integers and
// reuse liberally.

// TODO: Add some more global variables for the history list.
//       These should be the only globals in your program
//       They should be static so that only functions in this file can
//       access them.


// TODO: implement your history queue functions here

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

