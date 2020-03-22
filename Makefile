build:
	clear;
	@echo "Building go-lexer-cpp..."
	gcc main.cpp -std=c++98 -lstdc++ --std=c++0x -o go-lexer-cpp.out;
test:
	@echo "Testing on go_sources/hello.go"
	 ./go-lexer-cpp.out go_sources/hello.go 
test-empty:
	@echo "Testing on go_sources/empty.go"
	 ./go-lexer-cpp.out go_sources/empty.go 
clean:
	@echo "Cleaning up..."
	rm go-lexer-cpp.out