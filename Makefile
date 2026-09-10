CC = g++
CCFLAGS = -Wall -Wextra -O2

BIN_DIR = bin


all: $(BIN_DIR)/k-means $(BIN_DIR)/generate_dataset

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/k-means: k-means.cpp | $(BIN_DIR)
	$(CC) $(CCFLAGS) k-means.cpp -o $(BIN_DIR)/k-means

$(BIN_DIR)/generate_dataset: generate_dataset.cpp | $(BIN_DIR)
	$(CC) $(CCFLAGS) generate_dataset.cpp -o $(BIN_DIR)/generate_dataset

clean:
	rm -rf $(BIN_DIR)
