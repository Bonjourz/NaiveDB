CC = g++
CFLAGS = -Wall -g -std=gnu++14
LDFLAGS = -lm

naivedb : main.o Client.o DB.o BTree.o
	${CC} -o $@ $^ 

BTree.o : BTree.cpp BTree.h
	${CC} ${CFLAGS} -c $<

Client.o : Client.cpp Client.h
	${CC} ${CFLAGS} -c $<

DB.o : DB.cpp DB.h
	${CC} ${CFLAGS} -c $<

main.o : main.cpp Client.h
	${CC} ${CFLAGS} -c $<
	

clean:
	rm -rf *.o
	rm -rf naivedb
