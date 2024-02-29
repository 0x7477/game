BUILD_DIR := build
EXECUTABLE := game

# Main target
all: $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile using CMake
$(BUILD_DIR)/Makefile: CMakeLists.txt | $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..

# Build the executable
$(EXECUTABLE): $(BUILD_DIR)/Makefile
	$(MAKE) -j8 -C $(BUILD_DIR)

# Run the game
run: $(EXECUTABLE)
	./$(BUILD_DIR)/$(EXECUTABLE)

gdb: $(EXECUTABLE)
	gdb ./$(BUILD_DIR)/$(EXECUTABLE)

# Clean up
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all run clean