#############################################################################################
#                                              BUILD                                        #
#############################################################################################

SRC=copy.c mov.c flops.c MSC.c validation.c
OBJ=$(patsubst %.c, %.o, $(SRC))
CFLAGS+=-march=native -O0
#CFLAGS+=-D__FMA__ -D__AVX512__ -D__AVX2__ -D__SSE__ -D__SSE2__ -D__sse4_1__
CFLAGS+=-D__FMA__ -D__AVX2__ -D__AVX__ -D__SSE__ -D__SSE2__ -D__SSE4_1__


%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $^

%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $^

MSC.a: $(OBJ)
	ar rcs $@ $(OBJ)

clean:
	rm -rf $(OBJ) *.s MSC.a

