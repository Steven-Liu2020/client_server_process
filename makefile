#链接 -lpthread 进行编译
all : server.bin client1.bin client2.bin client3.bin client4.bin

server.bin : server.o
	cc -Wall -o $@ $<
server.o : server.c
	cc -c $<
client1.bin : client1.o
	cc -Wall -o $@ $<
client1.o : client1.c
	cc -c $<
client2.bin : client2.o
	cc -Wall -o $@ $<
client2.o : client2.c
	cc -c $<
client3.bin : client3.o
	cc -Wall -o $@ $<
client3.o : client3.c
	cc -c $<
client4.bin : client4.o
	cc -Wall -o $@ $< -lrt
client4.o : client4.c
	cc -c $<
.PHONY : clean
clean : 
	rm *.o *.bin
