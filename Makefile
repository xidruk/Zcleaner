CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))

TARGET = $(BINDIR)/zclean

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJDIR) $(BINDIR)
	@mkdir -p conf

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@ln -sf $(TARGET) zclean

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

config:
	@mkdir -p conf
	@if [ ! -f conf/dirconf.dcon ]; then \
		echo "# Zcleaner configuration file" > conf/dirconf.dcon; \
		echo "# List directories to clean (one per line)" >> conf/dirconf.dcon; \
		echo "/tmp" >> conf/dirconf.dcon; \
	fi

install: all config
	mkdir -p $(HOME)/.config/zcleaner
	cp -r conf/dirconf.dcon $(HOME)/.config/zcleaner/
	sudo cp $(TARGET) /usr/local/bin/zclean

clean:
	rm -rf $(OBJDIR) $(BINDIR)
	rm -f zclean

rebuild: clean all

.PHONY: all directories clean rebuild config install