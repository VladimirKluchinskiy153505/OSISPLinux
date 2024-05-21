all: AsyncSort
AsyncSort: main.o merge_sort.o
	g++ obj/main.o obj/merge_sort.o -o AsyncSort
main.o: src/main.cpp
	g++ -c src/main.cpp -o obj/main.o
merge_sort.o: src/merge_sort.cpp
	g++ -c src/merge_sort.cpp -o obj/merge_sort.o
clean:
	rm obj/main.o obj/merge_sort.o