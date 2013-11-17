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
#include "switch.h"
#include "device.h"




int main(void)
{

    pthread_t frontend,backend,switchend;
    void *context = zmq_ctx_new();
    routine_arg_t arg;
    arg.context = context;
    arg.switch_port = 9999;
    arg.direct_port = 10000;
    arg.device_port = 9000;

    s_sleep(100);

    pthread_create(&frontend, NULL, direct_routine, &arg);
    s_sleep(1000);
    pthread_create(&switchend, NULL, switch_routine, &arg);
    s_sleep(1000);
    pthread_create(&backend, NULL, device_routine, &arg);

    s_sleep(1000000);

    //我喜欢简单粗暴.. 所有没有关闭线程就退出了:)
    return 0;
}



