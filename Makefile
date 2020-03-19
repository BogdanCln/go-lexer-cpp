build:
	clear;
	@echo "Building go-lexer-cpp..."
	gcc main.cpp -lstdc++ -o go-lexer-cpp.out;
test:
	@echo "Testing on go_sources/hello.go"
	 ./go-lexer-cpp.out go_sources/hello.go 
clean:
	@echo "Cleaning up..."
	rm go-lexer-cpp.out