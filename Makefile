#
# CDH Makefile
#
# Ben Kempke
#

CC = gcc
CPPC = g++
THREAD_LIB = -pthread
SOCKET_HANDLERS = ./bin/obj/socket_handler.o
INCLUDE = -Ilib

all: uhdd

uhdd: $(SOCKET_HANDLERS) $(CLIENT_INT) $(BASE64_LIB) $(SHA1_LIB) $(UHD_INT)
	$(CPPC) $(THREAD_LIB) $(INCLUDE) src/uhd_daemon.cc $(LITHIUM_UTILS) $(SOCKET_HANDLERS) $(CLIENT_INT) $(UHD_INT) $(BASE64_LIB) $(SHA1_LIB) -luhd -o ./bin/uhdd

clean:
	rm -f ./bin/obj/*.o

