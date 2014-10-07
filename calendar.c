#include "calendar.h"

/*main function, to be run either with no arguments or supplied a single filename to load*/
int main(int argc, char* argv[])
{
    /*some declartions for the gui*/
    List* eventList = newList();
    Window* window = createWindow("UCP Calendar - Luke Mercuri (17365547)");
    Data data;
    
    /*window and list pointers that we
    willpass around to other functions*/
    data.list = eventList;
    data.window = window;

    /*user supplied too many arguments*/
    if(argc > 2)
    {
        messageBox(data.window, "Invalid syntax: expected \'./calendar <calendar file>\'");
    }   
    /*attempt to load file from name supplied*/
    else if(argc == 2)
    {
        /*could not load file*/
        if(loadListFile(argv[1], eventList))
        {
            messageBox(data.window, "Error opening calendar file");
        }
        /*file loaded, print list*/
        else
        {
            char* listStr = printList(eventList);
            setText(data.window, listStr);
            free(listStr);
        }
    }


    /*fill window with some buttons to use*/
    addButton(window, "Load Calendar File", uiLoadButton, (void*)&data);
    addButton(window, "Save Calendar File", uiSaveButton, (void*)&data);
    addButton(window, "Edit Calendar Entry", uiEditButton, (void*)&data);
    addButton(window, "New Calendar Entry", uiNewButton, (void*)&data);
    addButton(window, "Delete Calendar Entry", uiDelButton, (void*)&data);

    runGUI(window);
    
    /*program finished, free used memory*/
    freeList(eventList);
    freeWindow(window);
	return 0;
}


/*load calendar from file*/
void uiLoadButton(void* input)
{
    Data* data = (Data*)input;
    /*
    (apparently) the longest filepath/filename string allowable is 4095 chars
    http://www.redhat.com/archives/ext3-users/2001-October/msg00090.html
    */
    char* fName = (char*)calloc(4096, sizeof(char));
    char* listStr;

    

    /*present a dialog box to enter filename into*/
    if(fileOpenDialog(data->window, "Open File", fName))
    {
        /*make sure user actually entered a filename*/
        if(*fName)
        {
            /*try to populate list with specified file
            give user error if cannot open*/
            if(loadListFile(fName, data->list))
            {
                messageBox(data->window, "Error: unable to load file");
            }

            /*update text area with currently loaded calendar
            (or tell user list is empty)*/
            listStr = printList(data->list);
            setText(data->window, listStr);
            free(listStr);
        }
    }
    free(fName);
}


/*save currently loaded calendar to specified file*/
void uiSaveButton(void* input)
{
    Data* data = (Data*)input;
    char* fName = (char*)calloc(4096, sizeof(char));

    if(!isEmpty(data->list))
    {
        /*prompt user to inout a filename to save calendar to*/
        if(fileSaveDialog(data->window, "Save File", fName))
        {
            /*make sure name isn't blank*/
            if(*fName)
            {
                if(saveListFile(fName, data->list))
                {
                    /*may not have permission to write to this file*/
                    messageBox(data->window, "Error: unable to save file");
                }
            }
        }
    }
    /*list is empty, don't bother trying to save*/
    else
    {
        messageBox(data->window, "Error: calendar is empty");
    }

    free(fName);
}


/*edit an existing event enter an event name to select
the one you want and then edit values and update if valid*/
void uiEditButton(void* input)
{
	/*used by for loop, only here because 
    'ISO C90 forbids mixed declarations and code in C'*/
    int i;
    Data* data = (Data*)input;
    InputProperties prop = {"event name", 255, 0};
    /*name of event we want to edit*/
    char* eName = (char*)calloc(256, sizeof(char));

    if(!isEmpty(data->list))
    {
        /*present a dialog box to enter event name*/
        if(dialogBox(data->window, "Enter event name", 1, &prop, &eName))
        {
            /*fetch event by given name from within list*/
            Node* node = peekByName(data->list, eName);
            char* listStr = printList(data->list);

            /*check if event actually exists*/
            if(node != NULL)
            {
                /*temporarily store a copy of event details so
                we don't overwrite old event if invalid input*/
                int nameMaxLen = strlen(node->value->name);
                int locMaxLen = (node->value->location == NULL) ? 0 : strlen(node->value->location);
                char* eValues[8];

                /*fields for edit box*/
                InputProperties props[8] = {{"Day*", 2, 0},
                        {"Month*", 2, 0},
                        {"Year*", 4, 0},
                        {"Hour (24hr)*", 2, 0},
                        {"Minutes*", 2, 0},
                        {"Duration (in minutes)*", 9, 0},
                        {"Name*", 255, 0},
                        {"Location", 255, 0}};

                        /*stops gcc bawing about computed values breaking c spec*/
                        props[6].maxLength = nameMaxLen > 255 ? nameMaxLen : 255;
                        props[7].maxLength = locMaxLen > 255 ? locMaxLen : 255;
                

                /*we want to copy the values of the event we're editing so we
                don't lose the current event if the user inputs invalid values*/
                eValues[0] = (char*)calloc(3, sizeof(char));
                sprintf(eValues[0],"%d", node->value->date.day);
                eValues[1] = (char*)calloc(3, sizeof(char));
                sprintf(eValues[1],"%d", node->value->date.month);
                eValues[2] = (char*)calloc(5, sizeof(char));
                sprintf(eValues[2],"%d", node->value->date.year);
                eValues[3] = (char*)calloc(3, sizeof(char));
                sprintf(eValues[3],"%d", node->value->date.hour);
                eValues[4] = (char*)calloc(3, sizeof(char));
                sprintf(eValues[4],"%d", node->value->date.min);
                eValues[5] = (char*)calloc(11, sizeof(char));
                sprintf(eValues[5],"%d", node->value->duration);

                /*it is possible to have a name > the maximum if loaded from file
                so we want to make sure we don't truncate any strings that are*/
                eValues[6] = (char*)calloc((nameMaxLen > 255) ? (nameMaxLen+1) : 256, sizeof(char));
                sprintf(eValues[6], "%s", node->value->name);
                eValues[7] = (char*)calloc((locMaxLen > 255) ? (locMaxLen+1) : 256, sizeof(char));
		
                /*copy location if event has one*/
                if((node->value->location != NULL) && (*node->value->location))
        		{
                	strcpy(eValues[7], node->value->location);
	            }

                /*present a dialog box to edit specified event*/
                if(dialogBox(data->window, "(*) required", 8, props, eValues))
                {
                    Event* event;
                    /*make strings into ints we can put into an event*/
                    int day = atoi(eValues[0]);
                    int month = atoi(eValues[1]);
                    int year = atoi(eValues[2]);
                    int hour = atoi(eValues[3]);
                    int min = atoi(eValues[4]);
                    int duration = atoi(eValues[5]);
                    
                    /*i was getting some garbage at the end of strings when accessing
                    eValues [6], [7] directly for some reason...*/
                    char* name = (char*)calloc(sizeof(char), (strlen(eValues[6])+1));
                    char* location;

                    sprintf(name, "%s", eValues[6]);
                    if(strlen(eValues[7]))
                    {
                        location = (char*)calloc(sizeof(char), (strlen(eValues[7])+1)); 
                        sprintf(location, "%s", eValues[7]);  
                    }
                    else
                    {
                        location = NULL;
                    }

                    /*use inputted values to create a new event*/
                    event = newEvent_v(day, month, year, hour, min, duration, name, location);

                    /*replace old event with edited version if it is valid*/
                    if(event != NULL)
                    {
                        /*make sure user isn't naming event the same as an existing event*/
                        Node* oldNode = peekByName(data->list, event->name);
                        if(oldNode != NULL && oldNode != node)
                        {
                            messageBox(data->window, "Error: event with same name already exists");
                        }
                        else
                        {
                            /*free node's old value and assign new event to it*/
                            freeEvent(node->value);
                            node->value = event;
                            listStr = printList(data->list);
                            setText(data->window, listStr);  
                        }
                    }
                    /*else keep old event and let user know*/
                    else
                    {
                        messageBox(data->window, "Error: event not valid");
                    }
                }
                /*free strings that we passed to the dialog box*/
                for(i = 0; i < 8; i++)
                {
                    free(eValues[i]);
                }
            }
            /*event doesn't exist*/
            else
            {
                messageBox(data->window, "Error: event not found");
            }
            free(listStr);
        }
    }
    else
    {
        messageBox(data->window, "Error: calendar is empty");
    }

    free(eName);
}

/*creates a new event and adds it to calendar*/
void uiNewButton(void* input)
{
    Data* data = (Data*)input;
    int i;
    char* listStr;
    /*fields for add box*/
    InputProperties props[8] = {{"Day*", 2, 0},
                            {"Month*", 2, 0},
                            {"Year*", 4, 0},
                            {"Hour (24hr)*", 2, 0},
                            {"Minutes*", 2, 0},
                            {"Duration (in minutes)*", 9, 0},
                            {"Name*", 255, 0},
                            {"Location", 255, 0}};

    /*nicely format current time/date as default when adding new event*/
    time_t currtime = time(NULL);
    struct tm* timeinfo;

    /*strings to store entered values in*/
    char* eValues[8];
    eValues[0] = (char*)calloc(3, sizeof(char));
    eValues[1] = (char*)calloc(3, sizeof(char));
    eValues[2] = (char*)calloc(5, sizeof(char));
    eValues[3] = (char*)calloc(3, sizeof(char));
    eValues[4] = (char*)calloc(3, sizeof(char));
    eValues[5] = (char*)calloc(10, sizeof(char));
    eValues[6] = (char*)calloc(256, sizeof(char));
    eValues[7] = (char*)calloc(256, sizeof(char));



    timeinfo = localtime(&currtime);
    sprintf(eValues[0], "%i", timeinfo->tm_mday);
    sprintf(eValues[1], "%i", (1 + timeinfo->tm_mon));
    sprintf(eValues[2], "%i", (1900 + timeinfo->tm_year));
    sprintf(eValues[3], "%i", timeinfo->tm_hour);
    sprintf(eValues[4], "%i", timeinfo->tm_min);
    
    


    /*present a dialog box for user to enter new event details*/
    if(dialogBox(data->window, "(*) required", 8, props, eValues))
    {
        Event* event;
        /*make strings into ints we can put into an event*/
        int day = atoi(eValues[0]);
        int month = atoi(eValues[1]);
        int year = atoi(eValues[2]);
        int hour = atoi(eValues[3]);
        int min = atoi(eValues[4]);
        int duration = atoi(eValues[5]);


        /*i'm getting garbage in my string just passing 
        eValues[6] and [7] hopefully this will fix that...
        edit: yeah, it fixed it...*/
        char* name = (char*)calloc(sizeof(char), (strlen(eValues[6])+1));
        char* location;
        sprintf(name, "%s", eValues[6]);
        if(strlen(eValues[7]))
        {
            location = (char*)calloc(sizeof(char), (strlen(eValues[7])+1)); 
            sprintf(location, "%s", eValues[7]);  
        }
        else
        {
            location = NULL;
        }
        

        /*try to create an event from values supplied*/
        event = newEvent_v(day, month, year, hour, min, duration, name, location);

        /*check event is valid and try to add it to the calendar*/
        if(event != NULL && !addEvent(data->list, event))
        {
            /*update textarea with new value in list*/
            listStr = printList(data->list);
            setText(data->window, listStr);  
            free(listStr);
        }
        /*could not add event to list*/
        else
        {
            /*free memory if malloc'd*/
            if(event != NULL)
            {
                freeEvent(event);
            }
            /*alert user*/
            messageBox(data->window, "Error: event not valid");
        }

    }

    /*free strings that we passed to the text area*/
    for(i = 0; i < 8; i++)
    {
        free(eValues[i]);
    }
}


/*delete an event by name*/
void uiDelButton(void* input)
{
    Data* data = (Data*)input;
    InputProperties prop = {"event name", 255, 0};
    char* eName = (char*)calloc(255, sizeof(char));


    if(!isEmpty(data->list))
    {
        if(dialogBox(data->window, "Enter event name", 1, &prop, &eName))
        {
            /*find node ith specified name*/
            Node* node = peekByName(data->list, eName);
            /*if it exists, delete it*/
            if(node != NULL)
            {
                char* listStr;
                freeNode(data->list, node);

                /*update textarea to reflect deleted event*/
                listStr = printList(data->list);
                setText(data->window, listStr);  
                free(listStr);
            }
            /*no events with this name*/
            else
            {
                messageBox(data->window, "Error: event not found");
            }
        }
    }
    /*list is empty, don'y bother*/
    else
    {
        messageBox(data->window, "Error: calendar is empty");
    }

    free(eName);

}

/*rip out a substring until specified delimiter encountered*/
/*char* subStr(char* string, char del)
{
    char* subStr = (char*)malloc(sizeof(char)*strlen(string));
    int i = 0;
    
    while(*string != del)
    {
        subStr[i] = *string;
        i++;
        string++;
    }
    subStr[i] = '\0';

    
    return subStr;
}*/

