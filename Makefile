island: island.c protocol.c protocol.h direct.c direct.h switch.c switch.h device.c device.h
	gcc -o island island.c protocol.c direct.c switch.c device.c -lzmq -I.

tester: tester.c protocol.c protocol.h
	gcc -o tester tester.c protocol.c -lzmq

test_protocol: test/unit/test_protocol.c  protocol.c protocol.h
	gcc -o test_protocol test/unit/test_protocol.c protocol.c -I.
