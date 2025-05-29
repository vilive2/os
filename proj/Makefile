build:
	mkdir -p build bin
	gcc -o build/matcher.o -c src/matcher.c
	gcc -o build/find.o -c src/find.c
	gcc -o bin/myfind src/main.c build/*.o
clean:
	rm -f build/*.o
	rm -f bin/*

.PHONY: build clean