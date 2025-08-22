# Makefile for Todo CLI App
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = todo
SOURCE = main.c
OBJECTS = $(SOURCE:.c=.o)

# Default target
all: $(TARGET)

# Compile the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)
	@echo "✅ Todo CLI compiled successfully!"
	@echo "Run './$(TARGET)' to start the app"

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "🧹 Build files cleaned"

# Install system-wide (requires sudo)
install: $(TARGET)
	@echo "Installing Todo CLI system-wide..."
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "✅ Todo CLI installed! Run 'todo' from anywhere"

# Uninstall
uninstall:
	@echo "Uninstalling Todo CLI..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "✅ Todo CLI uninstalled"

# Run the app
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -DNDEBUG
release: clean $(TARGET)

# Show help
help:
	@echo "🚀 Todo CLI Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Build the todo app (default)"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install system-wide (requires sudo)"
	@echo "  uninstall- Remove system-wide installation"
	@echo "  run      - Build and run the app"
	@echo "  debug    - Build with debug symbols"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make        # Build the app"
	@echo "  make run    # Build and run"
	@echo "  make install # Install system-wide"

# Phony targets
.PHONY: all clean install uninstall run debug release help
