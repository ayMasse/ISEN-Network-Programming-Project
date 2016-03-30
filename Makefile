CC=gcc
CXX=g++
CPPFLAGS=-std=c++11
RM=rm -f

SRCS=inet_socket.cpp utility.cpp
OBJS=inet_socket.o utility.o

q5: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q5 sws_q5.cpp

q4: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q4 sws_q4.cpp

q3: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q3 sws_q3.cpp

q2: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q2 sws_q2.cpp

object: $(SRCS)
	$(CXX) $(CPPFLAGS) -c $(SRCS)

clean: 
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) sws_q2
