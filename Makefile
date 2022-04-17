BUILD_DIR=build
SRC_DIR=src
LIBS=-pthread -lboost_system

all: clean init rest_server

rest_server: $(SRC_DIR)/helpers.cpp $(SRC_DIR)/handlers.cpp main.cpp
	@$(CXX) $? $(LIBS) -o $(BUILD_DIR)/$@

init:
	@mkdir build

clean:
	@rm -rf build
	@rm -f rest_server