CC=gcc
CFLAGS=-ansi -Wall -pedantic -g `pkg-config --cflags gtk+-2.0`
LDLIBS=`pkg-config --libs gtk+-2.0`
OBJ=gui.o gui_luke.o calendar.o linklist.o event.o

calendar: $(OBJ)
	$(CC) -o calendar $(OBJ) $(LDLIBS)

gui.o: gui.c gui.h
	$(CC) $(CFLAGS) -c -o gui.o gui.c

gui_luke.o: gui_luke.c gui_luke.h gui.h
	$(CC) $(CFLAGS) -c -o gui_luke.o gui_luke.c

calendar.o: calendar.c calendar.h event.h linklist.h
	$(CC) $(CFLAGS) -c -o calendar.o calendar.c

linklist.o: linklist.c linklist.h event.h
	$(CC) $(CFLAGS) -c -o linklist.o linklist.c

event.o: event.c event.h
	$(CC) $(CFLAGS) -c -o event.o event.c

clean:
	rm -rfv $(OBJ) calendar
