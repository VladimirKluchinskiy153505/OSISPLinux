
all:MyProject
MyProject: main.o extract_dictionary_lib.o censor_text_lib.o
	gcc obj/main.o obj/extract_dictionary_lib.o obj/сensor_text_lib.o -o MyProject
main.o: src/main.c
	gcc -c src/main.c -o obj/main.o
extract_dictionary_lib.o: src/extract_dictionary_lib.c
	gcc -c src/extract_dictionary_lib.c -o obj/extract_dictionary_lib.o
censor_text_lib.o: src/сensor_text_lib.c
	gcc -c src/сensor_text_lib.c -o obj/сensor_text_lib.o
clean:
	rm obj/main.o obj/extract_dictionary_lib.o obj/сensor_text_lib.o