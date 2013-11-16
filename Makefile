island: island.c protocol.c protocol.h
	gcc -o island island.c protocol.c -lzmq

tester: tester.c protocol.c protocol.h
	gcc -o tester tester.c protocol.c -lzmq

test_protocol: test/unit/test_protocol.c  protocol.c protocol.h
	gcc -o test_protocol test/unit/test_protocol.c protocol.c -I.
