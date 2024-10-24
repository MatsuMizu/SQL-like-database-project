CC = g++

CPPFLAGS = -DM=10 -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format



all: a.out

a.out: task.o command_list.o command.o list2.o record.o
	$(CC) -g $(CPPFLAGS) $^ -lm -o $@

record.o: record.cpp record.h condition.h
	$(CC) -c $(CPPFLAGS) $<

list2.o: list2.cpp list2.h
	$(CC) -c $(CPPFLAGS) $<

command.o: command.cpp database.h command.h avl.h
	$(CC) -c $(CPPFLAGS) $<

command_list.o: command_list.cpp command_list.h
	$(CC) -c $(CPPFLAGS) $<

task.o: task.cpp
	$(CC) -c $(CPPFLAGS) $<

clean:
	rm -f *.out *.o
	rm -f data.h.gch a.out data.o