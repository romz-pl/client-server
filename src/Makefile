#
# Zbigniew ROMANOWSKI
# e-mail: romz@wp.pl
#

.PHONY : all clean

CC := gcc -Wall -Wextra -Wconversion -Wsign-conversion -DNDEBUG -O2


VPATH := ../src/


SOURCE := tcp_client.c
SOURCE += tcp_echo_client.c
SOURCE += tcp_echo_server.c
SOURCE += tcp_server.c
SOURCE += udp_echo_client.c
SOURCE += udp_echo_client_timeout.c
SOURCE += udp_echo_server.c
SOURCE += tcp_echo_server_fork.c
SOURCE += udp_echo_server_sigio.c
SOURCE += tcp_echo_server_thread.c
SOURCE += udp_client.c
SOURCE += udp_server.c

BINOUT := $(SOURCE:.c=.x)



all : $(BINOUT)
	
%.x : %.c
	$(CC) -o $@ $< -lpthread


clean :
	rm -f *.o $(BINOUT)
