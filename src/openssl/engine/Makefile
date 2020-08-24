
CC      := gcc

all:ykcrypto.o

ykcrypto.o: ykcrypto.c
	$(CC) -fPIC -o $(TMP_DIR)ykcrypto.o -c ykcrypto.c
clean:
	@rm -fr $(TMP_DIR)ykcrypto.o
