#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "protocol.h"

void check_entity_t(entity_t a, entity_t b)
{
    assert(a.type == b.type);
    assert(a.id == b.id);
}

int main(void)
{
    char msg[1024] = {0};
    entity_t src = {1, 2};
    entity_t dest = {3, 4};
    entity_t target = {5, 6};
    uint64_t req_id = 123456789;
    op_t operate = 12300123;
    stat_t status = 8888;
    offset_t offset = 9999;
    size_t len = 128;

    char data[128];
    int i = 0;
    for (i = 0; i < 128; i++)
        data[i] = (char)i;


    memset(msg, 0, 1024);
    build_request(msg, src, dest, target, req_id, operate, offset, len, data);

    msg_head_t *head = (msg_head_t *)msg;
    assert(head->version == PML_VERSION);
    check_entity_t(head->src, src);
    check_entity_t(head->dest, dest);
    check_entity_t(head->target, target);
    assert(head->req_id == req_id);
    assert(head->operate == operate);
    assert(head->offset == offset);
    assert(head->len == len);
    for (i = 0; i < 128; i++)
        assert(data[i] == *((char *)(msg + sizeof(msg_head_t) + i)));

    memset(msg, 0, 1024);
    build_reply(msg, src, dest, target, req_id, operate, status, len, data);

    assert(head->version == PML_VERSION);
    check_entity_t(head->src, src);
    check_entity_t(head->dest, dest);
    check_entity_t(head->target, target);
    assert(head->req_id == req_id);
    assert(head->operate == operate);
    assert(head->status == status);
    assert(head->len == len);
    for (i = 0; i < 128; i++)
        assert(data[i] == *((char *)(msg + sizeof(msg_head_t) + i)));

    printf("Test protocol.c OK\n");

}



