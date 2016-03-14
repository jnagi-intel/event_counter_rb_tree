//
// Main source file for event counter project
//

#include "EventCounter.h"

// Local Function Declarations
PEVENT_COUNTER_CONTEXT  __createEventCounterContext();
VOID                    __destroyEventCounterContext(PEVENT_COUNTER_CONTEXT pEventCounterContext);
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

BOOLEAN __parseInputFile(PEVENT_COUNTER_CONTEXT pEventCounterArgs)
{

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
    
}

VOID __destroyEventCounterContext(PEVENT_COUNTER_CONTEXT pEventCounterContext)
{

}


