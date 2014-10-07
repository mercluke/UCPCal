#include "event.h"

/*maloc required space for an Event struct, 
blank out location 
(so we don't get random data in there)
and return a pointer to the new Event struct*/
Event* newEvent()
{
	Event* event = (Event*)malloc(sizeof(Event));
	event->location = NULL;
    return event;
}


/*create a new event and populate it with 
values.  this is the one that will actually
be used from outside this file.  returns 
NULL if any inputs were "invalid" so that we 
have a way of checking input before adding it to the list*/
Event* newEvent_v(int day, int month, int year, int hour,
    int min, int duration, char* name, char* location)
{
    /*start with NULL so we have a way to
    check whether event is valid or not*/
    Event* event = NULL;

    /*
    Validity checking. unfortunately not advanced enough
    to for dates like "30th february" but it'll do...
    */
    if(day > 0 && day < 32 &&
        month > 0 && month < 13 &&
        year > 0 &&
        hour >= 0 && hour < 25 &&
        min >= 0 && min < 61 &&
        duration > 0 &&
        *name)
    {
        /*if values pass the test then malloc space for
        a new event and populate it with passed values*/
        event = newEvent();

        event->date.day = day;
        event->date.month = month;
        event->date.year = year;
        event->date.hour = hour;
        event->date.min = min;
        event->duration = duration;
        event->name = name; 

        /*location is optional so only assign if string not null*/
        if(location != NULL && *location)
        {
            event->location = location;
        }
    }  

    return event;
}


/*read a single line from file*/
char* readLine(FILE* file)
{
    /*starter length for line buffer size*/
    int lineBufferLength = 20;
    /*current line position*/
    int count = 0;
    /*create some space for the linebuffer to store text until eol*/
    char* lineBuffer = (char*)malloc(sizeof(char)*lineBufferLength);
    /*read first character on line*/
    int currCh = fgetc(file);

    /*until either end of line or end of file*/
    while(currCh != '\n' && currCh != EOF)
    {
        if(currCh != '\r') /*windows blahblahblah*/
        {
            /*double size of line buffer if too short...*/
            if(count == lineBufferLength-1)
            {
                lineBufferLength *= 2;
                lineBuffer = realloc(lineBuffer, lineBufferLength);
            }
            
            /*need to typecast to char because we are
            storing as int in order to check for eof*/
            lineBuffer[count] = (char)currCh;
            count++;
        }
        /*move to next character*/
       currCh = fgetc(file);
    }   

    /*terminate the string*/
    lineBuffer[count] = '\0';
    
    /*free any extra space at the end of the string*/
    lineBuffer = realloc(lineBuffer, (sizeof(char)*++count));
    
    return lineBuffer;
}


/*free event without leaving 
strings floating around in memory*/
void freeEvent(Event* event)
{
    free(event->name);
    
    if(event->location != NULL)
    {
        free(event->location);
    }
    free(event);
}

/*return malloc'd string representation
of event for ui purposes*/
char* printEvent(Event* event)
{
    char* retStr;

    /*make sure we're actually working with
    a valid event before proceeding*/
    if(event != NULL)
    {
        /*call helper functions to build nicely
        formatted date and time strings*/
        char* duration = friendlyDuration(event->duration);
        char* date = friendlyDate(event->date);

        /*reserve enough space in memory to hold date, duration
        name, location and formatting characters*/
        retStr = (char*)calloc((
            strlen(event->name) + 
            /*don't bother making space for location + ' @ ' if location is blank*/
            ((event->location == NULL) ? 0: (strlen(event->location) + 3)) +
            strlen(duration) +
            strlen(date) +
            /*magic numbers! this is for brackets,
            hyphens and linebreaks*/
            + 12 
        ), sizeof(char));

        /*place name into string*/
        sprintf(retStr, "%s", event->name);

        /*place ' @ <location>' into string if there one*/
        if(event->location != NULL)
        {
           retStr = strcat(retStr, " @ ");
           retStr = strcat(retStr, event->location);
        }
        /*duration in minutes/hours and some formatting characters*/
        retStr = strcat(retStr, " (");
        retStr = strcat(retStr, duration);
        retStr = strcat(retStr, ")\n");
        retStr = strcat(retStr, date);
        retStr = strcat(retStr, "\n---\n\n");
        
        /*free malloc'd strings we got from 'friendly' functions*/
        free(duration);
        free(date);
    }

    /*invalid event as returned by newEvent_v
    when passed invalid values*/
    else
    {
        retStr = "invalid event\n---\n\n";
    }

    return retStr;
}


/*Read an event from file and return pointer to event struct*/
Event* readEvent(FILE* file)
{
    Event* event = newEvent();
    char* currLine;
    /*scan info from first line*/
    fscanf(file, "%d-%d-%d %d:%d %d ",
        &event->date.year,
        &event->date.month,
        &event->date.day,
        &event->date.hour,
        &event->date.min,
        &event->duration);


    
    /*read name from rest of line*/
    currLine = readLine(file);
    event->name = currLine;

    /*read location or empty line*/
    currLine = readLine(file);

    /*is there a location?*/
    if(*currLine) /*null byte is 0, null byte is returned if line was empty, so if(<not zero>) will run if there is a location*/
    {
        event->location = currLine;
        /*blank line is end of event entry*/
        currLine = readLine(file);
    }
    free(currLine);
    
    /*check our event for validity before we return it
    return null incase of failed validity checks*/
    if(event->date.day < 1 || event->date.day > 31 ||
        event->date.month < 1 || event->date.month > 12 ||
        event->date.year < 1 ||
        event->date.hour < 0 || event->date.hour > 24 ||
        event->date.min < 0 || event->date.min > 60 ||
        event->duration < 1 ||
        !*event->name)
    {
        freeEvent(event);
        event = NULL;
    }

    return event;
}

/*write an event to file*/
void writeEvent(FILE* file, Event* event)
{
    /*print first line*/
    fprintf(file, "%04d-%02d-%02d %02d:%02d %d %s\n",
        event->date.year,
        event->date.month,
        event->date.day,
        event->date.hour,
        event->date.min,
        event->duration,
        event->name);
    /*if location exists print it on second line*/
    if(event->location != NULL)
        {
           fprintf(file, "%s\n", event->location);
        }
    /*print event seperator line*/
    fprintf(file, "\n");
}

/*format duration as hours, minutes or 
hours and minutes for use by printEvent*/
char* friendlyDuration(int duration)
{
    /*allocate some space for strings*/
    char* retStr = (char*)malloc(sizeof(char)*32);
    int hrs = duration / 60;
    int mins = duration % 60;
   
   /*choose how to display the duration*/
    if(hrs && mins)
    {
        sprintf(retStr,"%d Hour%s, %d minute%s",
            hrs, hrs == 1 ? "" : "s",
            mins, mins == 1 ? "" : "s");
    }
    else if(hrs)
    {
        sprintf(retStr,"%d Hour%s",
            hrs, hrs == 1 ? "" : "s");
    }
    else 
    {
        sprintf(retStr,"%d Minute%s",
            mins, mins == 1 ? "" : "s");
    }

    return retStr;
}


/*format date as 'dd Month yyyy, hh:mm[A,P]M'
or 'dd Month yyyy, hh[A,P]M' for use by printEvent*/
char* friendlyDate(Date date)
{
    /*allocate some space for the date string
    oooohhhh  magic numbers, so cool*/
    char* retStr = (char*)malloc(sizeof(char)*32);

    /*some month names (blank first so i don't
    need to bother subtracting one from month)*/
    static char* enumMonths[] = { "", "January", "February", "March",
                "April", "May", "June",
                "July", "August", "September",
                "October", "November", "December"};

    /*spec states that we should only display minutes if they are not zero*/
    if(date.min)
    {
        sprintf(retStr, "%d %s %d, %02d:%02d%cM",
                date.day,
                enumMonths[date.month],
                date.year,
                (date.hour%12) ? (date.hour%12) : 12,
                date.min,
                (date.hour/12 == 1) ? 'P': 'A');
    }
    else
    {
       sprintf(retStr, "%d %s %d, %02d%cM",
                date.day,
                enumMonths[date.month],
                date.year,
                (date.hour%12) ? (date.hour%12) : 12,
                (date.hour/12 == 1) ? 'P': 'A');
    }
    return retStr;
}


