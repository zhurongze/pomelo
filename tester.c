#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "zhelpers.h"
#include "protocol.h"


int main(void)
{
    void *context = zmq_ctx_new();
    void *direct_sock = zmq_socket(context, ZMQ_PUSH);
    int rc = zmq_connect(direct_sock, "tcp://localhost:10000");
    assert(rc == 0);

    void *pull_sock = zmq_socket(context, ZMQ_PULL);
    rc = zmq_bind(pull_sock, "tcp://*:11001");
    char addr[128] = "tcp://localhost:11001";

    char buf[1024];
    entity_t test = {1, 1};

    printf("Send Login request to direct\n");
    build_request(buf, test, test, test, 1, PML_OP_LOGIN,
            0, strlen(addr) + 1, addr);
    zmq_send(direct_sock, buf, 1024, 0);

    memset(buf, 0, 1024);
    zmq_recv(pull_sock, buf, 1024, 0);
    msg_head_t *head = (msg_head_t *)buf;
    assert(head->operate == PML_OP_LOGIN_REP);

    int i;
    for(i = 0; i < 10; i++){
        printf("Send %d Read request to direct\n", i);
        memset(buf, 0, 1024);
        build_request(buf, test, test, test, i, PML_OP_READ,
                0, 512, NULL);
        zmq_send(direct_sock, buf, 1024, 0);
    }

    for(i = 0; i < 10; i++){
        printf("Recv %d Read reply from direct\n", i);
        memset(buf, 0, 1024);
        zmq_recv(pull_sock, buf, 1024, 0);
        head = (msg_head_t *)buf;
        assert(head->operate == PML_OP_READ_REP);
    }

    for(i = 0; i < 10; i++){
        printf("Send %d Write request to direct\n", i);
        memset(buf, 0, 1024);
        build_request(buf, test, test, test, i, PML_OP_WRITE,
                0, 512, NULL);
        zmq_send(direct_sock, buf, 1024, 0);
    }

    for(i = 0; i < 10; i++){
        printf("Recv %d Write reply from direct\n", i);
        memset(buf, 0, 1024);
        zmq_recv(pull_sock, buf, 1024, 0);
        head = (msg_head_t *)buf;
        assert(head->operate == PML_OP_WRITE_REP);
    }

    zmq_close(pull_sock);
    zmq_close(direct_sock);
    zmq_ctx_destroy(context);
    return 0;
}

