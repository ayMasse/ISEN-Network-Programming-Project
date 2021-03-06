CC=gcc
CXX=g++
CPPFLAGS=-std=c++11 -g
RM=rm -f

SRCS=inet_socket.cpp utility.cpp
OBJS=inet_socket.o utility.o

server: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o server server.cpp

q7: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q7 sws_q7.cpp

q6: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o sws_q6 sws_q6.cpp

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
