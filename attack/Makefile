all: attack_aes

attack_aes: attack.o ../src/aes.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

attack.o: attack.c ../src/aes.h ../src/tables.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

clean:
	@rm -f *.o generate_whitebox attack_aes tables.h whitebox 
