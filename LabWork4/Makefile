all:MyProject
MyProject: main.o file_scan_lib.o
	gcc obj/main.o obj/file_scan_lib.o -o MyProject -lstdc++
main.o: src/main.cpp
	gcc -c src/main.cpp -o obj/main.o
file_scan_lib.o: src/file_scan_lib.cpp
	gcc -c src/file_scan_lib.cpp -o obj/file_scan_lib.o

clean:
	rm -f obj/*