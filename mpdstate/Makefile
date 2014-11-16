INC := -I/usr/include/mpd -I.
OBJS := conn.o state.o state-save.o libmpdclient.o
GXX := gcc

standard: $(OBJS)
	$(GXX) $(INC) -o state-save $(OBJS)
	mv state-save ../state-save

%.o: %.c
	$(GXX) $(INC) -c $*.c -o $*.o

clean:
	rm ../state-save $(OBJS) -f
