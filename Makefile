island: island.c
	gcc -o island island.c protocol.c -lzmq

tester: tester.c
	gcc -o tester tester.c -lzmq
