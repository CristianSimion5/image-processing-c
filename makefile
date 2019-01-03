CC=gcc
CFLAGS=-g -Wall

ODIR=obj

LIBS=-lm

_OBJ = proiect.o comun.o criptografie.o rec_pattern.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

creeaza:= $(shell mkdir -p $(ODIR))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

proiect: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

