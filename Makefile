ars: dllist.o main.o 
	gcc --std=gnu99 dllist.o main.o -o ars 
main.o: main.c 
	gcc --std=gnu99 -c main.c 
dllist.o: dllist.c dllist.h
	gcc --std=gnu99 -c dllist.c
clean:
	rm -f ars c main.o dllist.o
install:
	cp ars /usr/local/bin/ars
uninstall: 
	rm -f /usr/local/bin/ars
