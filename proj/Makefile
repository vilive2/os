build:
	mkdir -p build bin
	gcc -g -o build/matcher.o -c src/matcher.c
	gcc -g -o build/parser.o -c src/parser.c
	gcc -g -o build/find.o -c src/find.c
	gcc -g -o build/print.o -c src/print.c
	gcc -g -o build/help.o -c src/help.c
	gcc -g -o bin/myfind src/main.c build/*.o
clean:
	rm -f build/*.o
	rm -f bin/*

.PHONY: build clean