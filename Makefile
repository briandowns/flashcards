CC = cc

VERSION := 0.1.0
BINDIR = bin
BINARY = flashcards

LDFLAGS := -lulfius -ljansson -lyder -lorcania -ljson-c -lmysqlclient -lcurses -lsqlite3  
CFLAGS  := -Dapp_name=$(BINARY) -Dgit_sha=$(shell git rev-parse HEAD) $(LDFLAGS)

$(BINDIR)/$(BINARY): clean $(BINDIR) 
	$(CC) main.c database.c models.c api.c $(CFLAGS) -o $@

$(BINDIR):
	mkdir -p $@

clean:
	rm -rf $(BINDIR)
