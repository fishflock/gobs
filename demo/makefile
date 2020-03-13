CC=gcc

LDIR=../lib

demo2: $(LDIR)/read_c_vectors.h $(LDIR)/read_c_vectors.c demo2.c
	$(CC) -o demo2 demo2.c $(LDIR)/read_c_vectors.c $(LDIR)/create_c_matrix.c -I $(LDIR)

demo: demo.c
	$(CC) -o demo demo.c

generate_sample: generate_sample.c
	$(CC) -o generate_sample generate_sample.c

