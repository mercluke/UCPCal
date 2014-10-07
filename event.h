/*
Events:
contains structs for events and dates
as well as functions for event file I/O
and building string representations of 
events as detailed in assignment spec
*/

#ifndef EVENT_H
#define EVENT_H
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**a calender date
*fairly self explanatory...
*holds, day, month, year, hour, min
*as ints...(not unsigned for reasons that i'm sure exist)**/
typedef struct{
	int day;
	int month;
	int year;
	int hour;
    int min;
}Date;

/**holds a date struct, 
*duration (in minutes), 
*pointer to name and location strings
*Note: while names and locations read
*in from file can be any length, name
*entered via the program will be limited to 255 chars
*Double Note: because of this a user
*can edit an event loaded from file that
*is > 255 chars but the maximum length will
*be whatever length the old string was :(**/
typedef struct{
	Date date;
	int duration;
	char* name;
	char* location;	
}Event;

/*maloc required space for an Event struct, 
blank out location 
(so we don't get random data in there)
and return a pointer to the new Event struct*/
Event* newEvent();

/*create a new event and populate it with 
values.  this is the one that will actually
be used from outside this file.  returns 
NULL if any inputs were "invalid" so that we 
have a way of checking input before adding it to the list*/
Event* newEvent_v(int day, 
		int month,
		int year,
		int hour,
		int min,
		int duration,
		char* name,
		char* location);

/*free event without leaving 
strings floating around in memory*/
void freeEvent(Event* event);

/*return malloc'd string representation of event for ui purposes*/
char* printEvent(Event* event);

/*read a single line from a file*/
char* readLine(FILE* file);

/*Read an event from file and return pointer to event struct*/
Event* readEvent(FILE* file);

/*write an event to file*/
void writeEvent(FILE* file, Event* event);

/*format values for use by printEvent*/
char* friendlyDuration(int duration);
char* friendlyDate(Date date);


#endif
