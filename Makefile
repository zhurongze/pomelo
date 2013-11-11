island: island.c
	gcc -o island island.c -lzmq

tester: tester.c
	gcc -o tester tester.c -lzmq
