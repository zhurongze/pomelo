#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "zhelpers.h"


void *volume_routine(void *arg)
{
    void *context = arg;

    void *puller = zmq_socket(context, ZMQ_PULL);
    int rc = zmq_bind(puller, "tcp://*:9001");
    assert(rc == 0);

    void *pusher = zmq_socket(context, ZMQ_PUSH);
    rc = zmq_connect(pusher, "tcp://localhost:9002");
    assert(rc == 0);

    while(1) {
        char *str = s_recv(puller);
        s_send(pusher, str);
        free(str);
    }

    return (void *)0;
}

void *storage_routine(void *arg)
{
    void *context = arg;
    void *puller = zmq_socket(context, ZMQ_PULL);
    int rc = zmq_bind(puller, "tcp://*:9002");
    assert(rc == 0);

    while(1) {
        char *str = s_recv(puller);
        printf("I recv=%s\n", str);
        free(str);
    }

    return (void *)0;
}

int main(void)
{
    pthread_t frontend,backend;
    void *context = zmq_ctx_new();

    s_sleep(100);

    pthread_create(&frontend, NULL, volume_routine, context);
    s_sleep(1000);
    pthread_create(&backend, NULL, storage_routine, context);

    s_sleep(1000000);
    return 0;

}



