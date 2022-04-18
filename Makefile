BUILD_DIR=build
SRC_DIR=src
TEST_DIR=test
LIBS=-pthread -lboost_system

.PHONY: all test clean

all: clean init rest_server test

rest_server: $(SRC_DIR)/helpers.cpp $(SRC_DIR)/handlers.cpp main.cpp
	@$(CXX) $? $(LIBS) -o $(BUILD_DIR)/$(SRC_DIR)/$@

test: $(SRC_DIR)/helpers.cpp $(SRC_DIR)/handlers.cpp $(TEST_DIR)/unit/unittest.cpp
	@cp $(TEST_DIR)/api/package.json $(BUILD_DIR)/$(TEST_DIR)/api
	@cp $(TEST_DIR)/api/apitest.js $(BUILD_DIR)/$(TEST_DIR)/api
	@cp $(TEST_DIR)/unit/db_tmp $(BUILD_DIR)/$(TEST_DIR)/unit/db_tmp
	@$(CXX) $? $(LIBS) -o $(BUILD_DIR)/$(TEST_DIR)/unit/unittest

init:
	@mkdir -p $(BUILD_DIR)/$(TEST_DIR)/api $(BUILD_DIR)/$(TEST_DIR)/unit $(BUILD_DIR)/$(SRC_DIR)

clean:
	@rm -rf $(BUILD_DIR)