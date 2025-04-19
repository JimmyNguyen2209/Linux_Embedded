CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin

# Source files and corresponding object files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/p2p_chat

# Create directories if they don't exist
MKDIR_P = mkdir -p

all: directories $(EXECUTABLE)

directories:
	$(MKDIR_P) $(OBJDIR) $(BINDIR)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -rf $(OBJDIR)/*.o $(EXECUTABLE)

rebuild: clean all

# Include dependencies
-include $(OBJECTS:.o=.d)

# Generate dependency files
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@$(MKDIR_P) $(OBJDIR)
	@$(CC) -MM -MP -MT "$(OBJDIR)/$*.o $(OBJDIR)/$*.d" -I$(INCDIR) $< > $@

.PHONY: all clean directories rebuild