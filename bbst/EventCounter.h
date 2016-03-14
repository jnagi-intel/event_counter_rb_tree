//
// Main header file for the event counter project 
//

#ifndef _BBST_H_
#define _BBST_H_

#include "Types.h"
#include "RbTree.h"

//Args Declaration for event counter
typedef struct _EVENT_COUNTER_ARGS
{
    char*   InputFilename;
}EVENT_COUNTER_ARGS, *PEVENT_COUNTER_ARGS;

// Context Declaration for event counter 
typedef struct _EVENT_COUNTER_CONTEXT
{
    EVENT_COUNTER_ARGS  EventCounterArgs;
    FILE                *InputFileHandle;
}EVENT_COUNTER_CONTEXT, *PEVENT_COUNTER_CONTEXT;

// Function Prototypes


#endif
