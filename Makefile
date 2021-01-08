CC = g++
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS = -std=c++11 -g -Wall
INC = -I include

$(TARGETDIR)/proc_d: $(OBJECTS) dirs $(TARGETDIR)/proc_c $(TARGETDIR)/proc_b $(TARGETDIR)/proc_a
	g++ $(CFLAGS) $(BUILDDIR)/proc_d.o -o $(TARGETDIR)/proc_d -lrt

$(TARGETDIR)/proc_c: $(OBJECTS) dirs
	g++ $(CFLAGS) $(BUILDDIR)/proc_c.o -o $(TARGETDIR)/proc_c -lrt

$(TARGETDIR)/proc_b: $(OBJECTS) dirs
	g++ $(CFLAGS) $(BUILDDIR)/proc_b.o -o $(TARGETDIR)/proc_b -lrt

$(TARGETDIR)/proc_a: $(OBJECTS) dirs
	g++ $(CFLAGS) $(BUILDDIR)/proc_a.o -o $(TARGETDIR)/proc_a -lrt -lsfml-audio -pthread

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) dirs
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

dirs:
	mkdir -p $(TARGETDIR)
	mkdir -p $(BUILDDIR)

clean:
	@echo "Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(TARGETDIR)

.PHONY: clean
