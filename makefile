# Directories
IDIR = headers
ODIR = out
SDIR = sources

# Compilation settings
CC = gcc
CFLAGS = -I$(IDIR) -Wall
LIBS =

# File list
_DEPS = 
_OBJ = main.o

#
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tsig: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~

.PHONY: clean
