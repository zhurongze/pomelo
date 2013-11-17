#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "zhelpers.h"
#include "protocol.h"
#include "device.h"

void *device_routine(void *arg)
{
    routine_arg_t *routine = arg;
    void *context = routine->context;
    char addr[128];

    sprintf(addr, "tcp://*:%d", routine->device_port);
    void *pull_sock = zmq_socket(context, ZMQ_PULL);
    int rc = zmq_bind(pull_sock, addr);
    assert(rc == 0);

    sprintf(addr, "tcp://localhost:%d", routine->switch_port);
    void *switch_sock = zmq_socket(context, ZMQ_PUSH);
    rc = zmq_connect(switch_sock, addr);
    assert(rc == 0);

    char buf[1024];
    while(1) {
        int size = zmq_recv(pull_sock, buf, 1024, 0);
        assert(size != -1);
        assert(size <= 1024);

        msg_head_t *head = (msg_head_t *)buf;

        switch (head->operate) {
            case PML_OP_READ:

                printf("device recv READ request\n");
                //TODO read src/dest/target

                //forward to switcher
                head->operate = PML_OP_READ_REP;
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE:

                printf("device recv WRITE request\n");
                //TODO read src/dest/target

                //forward to switcher
                head->operate = PML_OP_WRITE_REP;
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            default:
                // never arrive here
                assert(0 == 1);
                break;
        }
    }

    return (void *)0;
}


