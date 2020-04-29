CC := g++
CFLAGS := -g -lpcap

all: libpcap.cpp libpcap_filter.cpp
	$(CC) $(CFLAGS) libpcap.cpp -o libpcap.o
	$(CC) $(CFLAGS) libpcap_filter.cpp -o libpcap_filter.o

libpcap: libpcap.cpp
	$(CC) $(CFLAGS) libpcap.cpp -o libpcap.o

libpcap_filter: libpcap_filter.cpp
	$(CC) $(CFLAGS) libpcap_filter.cpp -o libpcap_filter.o