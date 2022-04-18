BUILD_DIR=build
SRC_DIR=src
TEST_DIR=test
LIBS=-pthread -lboost_system

.PHONY: all test clean

all: clean init rest_server test

rest_server: $(SRC_DIR)/helpers.cpp $(SRC_DIR)/handlers.cpp main.cpp
	@$(CXX) $? $(LIBS) -o $(BUILD_DIR)/$(SRC_DIR)/$@

test: $(SRC_DIR)/helpers.cpp $(SRC_DIR)/handlers.cpp $(TEST_DIR)/unittest.cpp
	@cp $(TEST_DIR)/db_tmp $(BUILD_DIR)/$(TEST_DIR)/db_tmp
	@cp $(TEST_DIR)/package.json $(BUILD_DIR)/$(TEST_DIR)
	@cp $(TEST_DIR)/apitest.js $(BUILD_DIR)/$(TEST_DIR)
	@$(CXX) $? $(LIBS) -o $(BUILD_DIR)/$(TEST_DIR)/unittest

init:
	@mkdir -p $(BUILD_DIR)/$(TEST_DIR) $(BUILD_DIR)/$(SRC_DIR)

clean:
	@rm -rf $(BUILD_DIR)