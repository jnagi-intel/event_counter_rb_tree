//
// Main source file for event counter project
//

#include "EventCounter.h"

// Local Function Declarations
PEVENT_COUNTER_CONTEXT  __createEventCounterContext();
VOID                    __destroyEventCounterContext(PEVENT_COUNTER_CONTEXT *ppEventCounterContext);
BOOLEAN                 __parseEventCounterArgs(PEVENT_COUNTER_CONTEXT pEventCounterContext, CHAR* argv[]);
BOOLEAN                 __parseInputFile(PEVENT_COUNTER_CONTEXT pEventCounterContext);

INT main(INT argc, CHAR *argv[])
{
    PEVENT_COUNTER_CONTEXT  pEventCounterContext = NULL;
    INT                     bRetStatus = 1;

    do
    {
        // validate the number of arguements entered by user
        if (argc != 2)
        {
            printf("main : syntax -- bbst <filename>\r\n");
            bRetStatus = -1;
            break;
        }

        // create the context for event counter
        pEventCounterContext =  __createEventCounterContext();

        // Parse the arguements and get the filename
        if (!__parseEventCounterArgs(pEventCounterContext, argv))
        {
            bRetStatus = -1;
            break;
        }

        // parse the input file and get the event IDs & counts
        if (!__parseInputFile(pEventCounterContext))
        {
            bRetStatus = -1;
            break;
        }

    } while (FALSE);

    return bRetStatus;
}

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
        if (!pRbTreeContext->RbTreeFnTbl.insertRbTreeNode(pRbTreeContext, EventID, EventCount))
        {
            return FALSE;
        }
    }

    pRbTreeContext->RbTreeFnTbl.deleteRbTreeNode(pRbTreeContext, 15);
    pRbTreeContext->RbTreeFnTbl.deleteRbTreeNode(pRbTreeContext, 14);
    pRbTreeContext->RbTreeFnTbl.deleteRbTreeNode(pRbTreeContext, 8);

    return TRUE;
}

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

PEVENT_COUNTER_CONTEXT __createEventCounterContext()
{
    PEVENT_COUNTER_CONTEXT  pEventCounterContext = NULL;

    // Allocate memory for the context 
    pEventCounterContext = (PEVENT_COUNTER_CONTEXT)malloc(sizeof(EVENT_COUNTER_CONTEXT));
    pEventCounterContext->pRbTreeContext = createRbTreeContext();

    return pEventCounterContext;
}

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


