#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "zhelpers.h"
#include "protocol.h"

typedef struct routine_arg_t {
    void *context;
    int switch_port;
    int volume_port;
    int device_port;
}routine_arg_t;

void *volume_directer_routine(void *arg)
{
    routine_arg_t *routine = arg;
    void *context = routine->context;
    char addr[128];

    sprintf(addr, "tcp://*:%d", routine->volume_port);
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
    entity test = {1, 1};
    while(1) {
        int size = zmq_recv(pull_sock, buf, 1024, 0);
        assert(size != -1);
        assert(size <= 1024);

        msg_head *head = (msg_head *)buf;
        switch (head->operate) {
            case PML_OP_LOGIN:
                assert(head->len > 0);
                printf("volume recv LOGIN request\n");

                //TODO read src/dest/target

                data = buf + sizeof(msg_head);

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
                printf("volume recv READ request\n");

                //TODO read src/dest/target

                //forward to switcher
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE:

                printf("volume recv WRITE request\n");
                //TODO read src/dest/target

                //forward to switcher
                zmq_send(switch_sock, buf, 1024, 0);
                break;
            case PML_OP_READ_REP:

                printf("volume recv READ_REP request\n");
                //TODO read src/dest/target

                //send to client
                zmq_send(client_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE_REP:

                printf("volume recv WRITE_REP request\n");
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

void *switcher_routine(void *arg)
{
    routine_arg_t *routine = arg;
    void *context = routine->context;
    char addr[128];

    sprintf(addr, "tcp://*:%d", routine->switch_port);
    void *pull_sock = zmq_socket(context, ZMQ_PULL);
    int rc = zmq_bind(pull_sock, addr);
    assert(rc == 0);

    sprintf(addr, "tcp://localhost:%d", routine->volume_port);
    void *volume_sock = zmq_socket(context, ZMQ_PUSH);
    rc = zmq_connect(volume_sock, addr);
    assert(rc == 0);

    sprintf(addr, "tcp://localhost:%d", routine->device_port);
    void *device_sock = zmq_socket(context, ZMQ_PUSH);
    rc = zmq_connect(device_sock, addr);
    assert(rc == 0);

    char buf[1024];
    while(1) {
        int size = zmq_recv(pull_sock, buf, 1024, 0);
        assert(size != -1);
        assert(size <= 1024);

        msg_head *head = (msg_head *)buf;

        switch (head->operate) {
            case PML_OP_READ:

                printf("switch recv READ request\n");
                //TODO read src/dest/target

                //send to deivce
                zmq_send(device_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE:

                printf("switch recv WRITE request\n");
                //TODO read src/dest/target

                //send to deivce
                zmq_send(device_sock, buf, 1024, 0);
                break;
            case PML_OP_READ_REP:

                printf("switch recv READ_REP request\n");
                //TODO read src/dest/target

                //send to volume
                zmq_send(volume_sock, buf, 1024, 0);
                break;
            case PML_OP_WRITE_REP:
                printf("switch recv WRITE_REP request\n");
                //TODO read src/dest/target

                //send to volume
                zmq_send(volume_sock, buf, 1024, 0);
                break;
            default:
                // never arrive here
                assert(0 == 1);
                break;
        }
    }

    return (void *)0;
}

void *storage_device_routine(void *arg)
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

        msg_head *head = (msg_head *)buf;

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

int main(void)
{

    pthread_t frontend,backend,switchend;
    void *context = zmq_ctx_new();
    routine_arg_t arg;
    arg.context = context;
    arg.switch_port = 9999;
    arg.volume_port = 10000;
    arg.device_port = 9000;

    s_sleep(100);

    pthread_create(&frontend, NULL, volume_directer_routine, &arg);
    s_sleep(1000);
    pthread_create(&switchend, NULL, switcher_routine, &arg);
    s_sleep(1000);
    pthread_create(&backend, NULL, storage_device_routine, &arg);

    s_sleep(1000000);

    //我喜欢简单粗暴.. 所有没有关闭线程就退出了:)
    return 0;
}



