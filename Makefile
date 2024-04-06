BINDIR = bin
BINARY = flashcards

$(BINDIR)/$(BINARY): clean $(BINDIR) 
	cc -o bin/flashcards main.c -lmysqlclient -lcurses -lsqlite3

$(BINDIR):
	mkdir -p $@

clean:
	rm -rf bin
