CC=gcc
CXX=g++
CPPFLAGS=-g -std=c++11
RM=rm -f
LDFLAGS=-g

SRCS=inet_socket.c utility.c sws_q2.cpp
OBJS=inet_socket.o utility.o sws_q2.o

q2: $(OBJS)
	$(CXX) $(LDFLAGS) -o sws_q2 $(OBJS)

object: $(SRCS)
	$(CXX) $(CPPFLAGS) -c $(SRCS)

clean: 
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) sws_q2
