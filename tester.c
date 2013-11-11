#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include "zhelpers.h"


int main(void)
{
    void *context = zmq_ctx_new();
    void *pusher = zmq_socket(context, ZMQ_PUSH);
    int rc = zmq_connect(pusher, "tcp://localhost:9001");

    assert(rc == 0);
    
    char string[10];
    int i;
    for(i = 0; i < 10; i++){
        sprintf(string, "hello=%d", i);
        s_send(pusher, string);
    }

    zmq_close(pusher);
    zmq_ctx_destroy(context);
    return 0;
}

