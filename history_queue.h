/*
 * Header file for circular queue of HistoryListItems.
 */
#ifndef __HISTQUEUE_H__
#define __HISTQUEUE_H__

#include "parse_args.h"

#define MAXHIST 10   // max number of commands in history list

/*
 * A struct to keep information one command in the history of 
 * command executed
 */
struct HistoryEntry {              
   	int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

// You can use "HistoryEntry" instead of "struct HistoryEntry"
typedef struct HistoryEntry HistoryEntry;


//forward declarations of our functions that will be used in our shell
void add_queue(char *cmd_line);
void print_history();
char* check_history(int index);
char* repeatCmd();
#endif
