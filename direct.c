#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "zhelpers.h"
#include "protocol.h"
#include "direct.h"

void *direct_routine(void *arg)
{
    routine_arg_t *routine = arg;
    void *context = routine->context;
    char addr[128];

    sprintf(addr, "tcp://*:%d", routine->direct_port);
    void *pull_sock = zmq_socket(context, ZMQ_PULL);
    int rc = zmq_bind(pull_sock, addr);
    assert(rc == 0);

    sprintf(addr, "tcp://localhost:%d", routine->switch_port);
    void *switch_sock = zmq_socket(context, ZMQ_PUSH);
    rc = zmq_connect(switch_sock, addr);
    assert(rc == 0);

    void *client_sock = NULL;

    char buf[1024];
    void *data;
    entity_t test = {1, 1};
    while(1) {
        int size = zmq_recv(pull_sock, buf, 1024, 0);
        assert(size != -1);
        assert(size <= 1024);

        msg_head_t *head = (msg_head_t *)buf;
        switch (head->operate) {
            case PML_OP_LOGIN:
                assert(head->len > 0);
                printf("direct recv LOGIN request\n");

                //TODO read src/dest/target

                data = buf + sizeof(msg_head_t);

                printf("Leng is %d, addr=%s\n", (int)(head->len), (char *)data);

                if (client_sock != NULL) {
                    zmq_close(client_sock);
                    client_sock = NULL;
                }
                client_sock = zmq_socket(context, ZMQ_PUSH);
                rc = zmq_connect(client_sock, data);
                assert(rc == 0);

                memset(buf, 0, 1024);
                build_reply(buf, test, test, test, 0, PML_OP_LOGIN_REP,
                        0, 0, NULL);
                //replay to client
                zmq_send(client_sock, buf, 1024, 0);
                break;
            case PML_OP_READ:
                printf("direct recv READ request\n");

                //TODO read src/dest/target

                //forward to switcher
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE:

                printf("direct recv WRITE request\n");
                //TODO read src/dest/target

                //forward to switcher
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            case PML_OP_READ_REP:

                printf("direct recv READ_REP request\n");
                //TODO read src/dest/target

                //send to client
                zmq_send(client_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE_REP:

                printf("direct recv WRITE_REP request\n");
                //TODO read src/dest/target
                //send to client
                zmq_send(client_sock, buf, 1024, 0);
                break;
            default:
                // never arrive here
                assert(0 == 1);
                break;
        }
    }

    return (void *)0;
}
