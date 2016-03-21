//
// Main source file for event counter project
//

#include "EventCounter.h"

// Local Function Declarations
PEVENT_COUNTER_CONTEXT  __createEventCounterContext();
VOID                    __destroyEventCounterContext(PEVENT_COUNTER_CONTEXT *ppEventCounterContext);
BOOLEAN                 __parseEventCounterArgs(PEVENT_COUNTER_CONTEXT pEventCounterContext, CHAR* argv[]);
BOOLEAN                 __parseInputFile(PEVENT_COUNTER_CONTEXT pEventCounterContext);
VOID                    __increaseEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID, INT IncrementValue);
VOID                    __reduceEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID, INT DecrementValue);
VOID                    __getEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID);
VOID                    __getPrevEvent(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID);
VOID                    __getNextEvent(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID);
VOID                    __getTotalCountInRange(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID1, INT ID2);

// Main Function for the project
INT main(INT argc, CHAR *argv[])
{
    PEVENT_COUNTER_CONTEXT  pEventCounterContext = NULL;
    INT                     RetStatus = 1;
    CHAR                    CommandString[100];
    CHAR                    *Token = NULL;
    INT                     EventID = 0;
    INT                     CountValue = 0;

    do
    {
        // validate the number of arguements entered by user
        if (argc != 2)
        {
            printf("main : syntax -- bbst <filename>\r\n");
            RetStatus = -1;
            break;
        }

        // create the context for event counter
        pEventCounterContext =  __createEventCounterContext();

        // Parse the arguements and get the filename
        if (!__parseEventCounterArgs(pEventCounterContext, argv))
        {
            RetStatus = -1;
            break;
        }

        // parse the input file and get the event IDs & counts, also builds the red black tree
        if (!__parseInputFile(pEventCounterContext))
        {
            RetStatus = -1;
            break;
        }

        // Wait for commands, quit to exit the program
        do
        {
            // First get the command string from standard input
            fgets(CommandString, 20, stdin);

            // Make sure that string has an EOL character at the end, if its a new line, 
            // convert it into EOL
            if (CommandString[strlen(CommandString) - 1] == '\n')
            {
                CommandString[strlen(CommandString) - 1] = '\0';
            }

            // Get the First Token to select the command, tokenize with white spaces
            Token = strtok(CommandString, " ");

            if (strcmp(Token, "increase") == 0)
            {
                // Get the event ID, increment value and call the function 
                EventID = (int)strtol(strtok(NULL, " "), NULL, 10);
                CountValue = (int)strtol(strtok(NULL, " "), NULL, 10);
                __increaseEventCount(pEventCounterContext, EventID, CountValue);
            }
            else if (strcmp(Token, "reduce") == 0)
            {
                // Get the event ID, decrement value and call the function 
                EventID = (int)strtol(strtok(NULL, " "), NULL, 10);
                CountValue = (int)strtol(strtok(NULL, " "), NULL, 10);
                __reduceEventCount(pEventCounterContext, EventID, CountValue);
            }
            else if (strcmp(Token, "count") == 0)
            {
                // Get the Event ID and call function to get count
                __getEventCount(pEventCounterContext, (int)strtol(strtok(NULL, " "), NULL, 10));
            }
            else if (strcmp(Token, "inrange") == 0)
            {
                // Get the event ID1 and ID2, call the function to get Count
                EventID = (int)strtol(strtok(NULL, " "), NULL, 10);
                __getTotalCountInRange(pEventCounterContext, EventID, (int)strtol(strtok(NULL, " "), NULL, 10));
            }
            else if (strcmp(Token, "next") == 0)
            {
                // Get the event ID and call the function
                __getNextEvent(pEventCounterContext, (int)strtol(strtok(NULL, " "), NULL, 10));
            }
            else if (strcmp(Token, "previous") == 0)
            {
                // Get the event ID and call the function
                __getPrevEvent(pEventCounterContext, (int)strtol(strtok(NULL, " "), NULL, 10));
            }
            else if (strcmp(Token, "quit") == 0)
            {
                // End the program
                break;
            }
            else
            {
                // User entered an invalid command
                printf("Only the following commands are supported :\n\tincrease <ID> <Value>\n\treduce <ID> <Value>\n\tcount <ID>\n\tinrange <ID1> <ID2>\n\tnext <ID>\n\tprevious <ID>\n");
            }

        } while (TRUE);

    } while (FALSE);

    if (!RetStatus)
    {
        if (pEventCounterContext)
        {
            __destroyEventCounterContext(&pEventCounterContext);
        }
    }

    return RetStatus;
}

// __parseInputFile()
// This function parses the input file with event ID and count information 
// Also builds up the Red black tree from IDs and Counts.
BOOLEAN __parseInputFile(PEVENT_COUNTER_CONTEXT pEventCounterContext)
{
    PRB_TREE_CONTEXT    pRbTreeContext = pEventCounterContext->pRbTreeContext;
    UINT                EventID = 0;
    UINT                EventCount = 0;
    INT                 Count = 0;
    
    // Open with the file with the given filename in the command 
    pEventCounterContext->InputFileHandle = fopen(pEventCounterContext->EventCounterArgs.InputFilename, "r");
    if (pEventCounterContext->InputFileHandle == NULL)
    {
        printf("__parseInputFile: Unable to open Input file\n");
        return FALSE;
    }

    // Read the number of ID's from the first line of the file 
    fscanf(pEventCounterContext->InputFileHandle, "%u", &pEventCounterContext->NumEvents);

    // Read remaining events with their counts and insert them in the red black tree 
    Count = pEventCounterContext->NumEvents;
    while(Count-- > 0)
    {
        // Read the line and get the Event ID and count 
        fscanf(pEventCounterContext->InputFileHandle, "%u %u", &EventID, &EventCount);

        // Insert this to the red black tree
        if (!pRbTreeContext->stRbTreeFnTbl.insertRbTreeNode(pRbTreeContext, EventID, EventCount))
        {
            return FALSE;
        }
    }

    return TRUE;
}

// __parseEventCounterArgs()
// This function gets the filename from the args and saves it to the event counter context
BOOLEAN __parseEventCounterArgs(PEVENT_COUNTER_CONTEXT pEventCounterContext, CHAR *argv[])
{
    UINT    FilenameLength = 0;
    BOOLEAN bRetStatus = TRUE;

    do
    {
        // Get the Filename first
        FilenameLength = strlen(argv[1]);
        if (FilenameLength)
        {
            pEventCounterContext->EventCounterArgs.InputFilename = (CHAR*)malloc(sizeof(CHAR) * FilenameLength);
            strcpy(pEventCounterContext->EventCounterArgs.InputFilename, argv[1]);
        }
        else
        {
            printf("__parseEventCounterArgs: Illegal Filename\r\n");
            bRetStatus = FALSE;
            break;
        }
    } while (FALSE);

    return bRetStatus;
}

// __createEventCounterContext()
// This function allocates memory for event counter context and also initializes variables
PEVENT_COUNTER_CONTEXT __createEventCounterContext()
{
    PEVENT_COUNTER_CONTEXT  pEventCounterContext = NULL;

    // Allocate memory for the context 
    pEventCounterContext = (PEVENT_COUNTER_CONTEXT)malloc(sizeof(EVENT_COUNTER_CONTEXT));
    pEventCounterContext->pRbTreeContext = createRbTreeContext();

    return pEventCounterContext;
}

// __destroyEventCounterContext()
// This function deallocates and frees up the event counter context
VOID __destroyEventCounterContext(PEVENT_COUNTER_CONTEXT *ppEventCounterContext)
{
    // Destroy Rb Tree Context first 
    destroyRbTreeContext(&(*ppEventCounterContext)->pRbTreeContext);

    // Now free the Event Counter Context 
    if (*ppEventCounterContext)
    {
        free(*ppEventCounterContext);
        *ppEventCounterContext = NULL;
    }
}

// __increaseEventCount()
// This function increasea the count of the event ID by IncrementValue. If event with ID is not present, insert it. 
// Also prints the count of event after the addition.
VOID __increaseEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID, INT IncrementValue)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;

    // Prints the count, insert function takes care of adding count if event exists
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.insertRbTreeNode(pEventCounterContext->pRbTreeContext, ID, IncrementValue);

    if (pRbTreeNode)
    {
        printf("%d\n", pRbTreeNode->Count);
    }
}

// __reduceEventCount()
// This function decrease the count of event count by Decrement Value. If the event count becomes less than or equal 
// to 0, removes the event from the counter. Also prints the count of the event after the deletion, or 0 if the event 
// is removed or not present.
VOID __reduceEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID, INT DecrementValue)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;

    // First get the event from the red black tree to be deleted 
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.findRbTreeNode(pRbTreeContext, ID);

    if (pRbTreeNode == NULL || pRbTreeNode->ID != ID)
    {
        // Event doesnt exist!
        printf("0\n");
    }
    else
    {
        // Get the new value of the count
        pRbTreeNode->Count -= DecrementValue;
        
        // Delete the event from the tree of the new count <= 0 
        if (pRbTreeNode->Count <= 0)
        {
            pRbTreeContext->stRbTreeFnTbl.deleteRbTreeNode(pRbTreeContext, pRbTreeNode);
            printf("0\n");
        }
        else
        {
            // Print the new count
            printf("%d\n", pRbTreeNode->Count);
        }
    }
}

// __getEventCount()
// This function prints the count of the event. If not present, prints 0.
VOID __getEventCount(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;

    // Prints the count
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.findRbTreeNode(pRbTreeContext, ID);

    if (pRbTreeNode && pRbTreeNode->ID == ID)
    {
        printf("%d\n", pRbTreeNode->Count);
    }
    else
    {
        // Event not found in the Red Black Tree
        printf("0\n");
    }
}

// __getTotalCountInRange()
// This function prints the total count for IDs between ID1 and ID2 inclusively
VOID __getTotalCountInRange(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID1, INT ID2)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;
    INT                 TotalCount      = 0;

    // First search for the Event ID with the given ID1
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.findRbTreeNode(pRbTreeContext, ID1);

    if (pRbTreeNode->ID < ID1)
    {
        pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.getNextIDRbTreeNode(pRbTreeContext, pRbTreeNode);
    }

    // Add the Count and use getNextID to find the next ID till we reach ID2
    // Assuming ID2 exists! 
    if (pRbTreeNode)
    {
        while (pRbTreeNode && pRbTreeNode->ID <= ID2)
        {
            TotalCount += pRbTreeNode->Count;
            pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.getNextIDRbTreeNode(pRbTreeContext, pRbTreeNode);
        }
    }

    // Print the total Count 
    printf("%d\n", TotalCount);
}

// __getNextEvent()
// This function prints the ID and the count of the event with the lowest ID that is greater that theID.
// Prints "0 0", if there is no next ID.
VOID __getNextEvent(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;

    // First search for the Event ID with the given ID
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.findRbTreeNode(pRbTreeContext, ID);

    if (pRbTreeNode)
    {
        // Get the event with next ID
        if (pRbTreeNode->ID <= ID)
        {
            pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.getNextIDRbTreeNode(pRbTreeContext, pRbTreeNode);
        }

        if (pRbTreeNode)
        {
            // Print the ID and the count
            printf("%d %d\n", pRbTreeNode->ID, pRbTreeNode->Count);
        }
        else
        {
            // No event with greater ID
            printf("0 0\n");
        }
    }
    else
    {
        // shouldnt happen in this project, leaving a print to catch this 
        printf("__getNextEvent: Event with %d not found", ID);
    }
}

// __getPrevEvent()
// This function prints the ID and the count of the event with the greatest key that is less that theID.
// Prints "0 0", if there is no previous ID.
VOID __getPrevEvent(PEVENT_COUNTER_CONTEXT pEventCounterContext, INT ID)
{
    PRB_TREE_CONTEXT    pRbTreeContext  = pEventCounterContext->pRbTreeContext;
    PRB_TREE_NODE       pRbTreeNode     = NULL;

    // First search for the Event ID with the given ID
    pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.findRbTreeNode(pRbTreeContext, ID);

    if (pRbTreeNode)
    {
        // Get the event with previous ID
        if (pRbTreeNode->ID >= ID)
        {
            pRbTreeNode = pRbTreeContext->stRbTreeFnTbl.getPrevIDRbTreeNode(pRbTreeContext, pRbTreeNode);
        }

        if (pRbTreeNode)
        {
            // Print the ID and the count
            printf("%d %d\n", pRbTreeNode->ID, pRbTreeNode->Count);
        }
        else
        {
            // No event with lesser ID
            printf("0 0\n");
        }
    }
    else
    {
        // shouldnt happen in this project, leaving a print to catch this 
        printf("__getPrevEvent: Event with %d not found", ID);
    }
}

