build:
	mkdir -p build bin
	gcc -g -o build/find.o -c src/find.c
	gcc -g -o build/matcher.o -c src/matcher.c
	gcc -g -o build/parser.o -c src/parser.c
	gcc -g -o build/pred.o -c src/pred.c
	gcc -g -o build/tree.o -c src/tree.c
	gcc -g -o build/util.o -c src/util.c
	gcc -g -o bin/myfind src/main.c build/*.o
clean:
	rm -f build/*.o
	rm -f bin/*

.PHONY: build clean