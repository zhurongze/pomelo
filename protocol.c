#include <assert.h>
#include <string.h>
#include "protocol.h"

static void build_head(void *msg, entity src, entity dest, entity target,
        uint64_t req_id, op_t operate, stat_t status, offset_t offset, 
        size_t len)
{
    msg_head *head = (msg_head *)msg;
    head->version = PML_VERSION;

    head->src = src;
    head->dest = dest;
    head->target = target;
    head->req_id = req_id;
    head->operate = operate;
    head->status = status;
    head->offset = offset;
    head->len = len;
}


void build_request(void *msg, entity src, entity dest, entity target, 
        uint64_t req_id, op_t operate, offset_t offset, size_t len, void *data)
{
    assert(msg != NULL);
    build_head(msg, src, dest, target, req_id, operate, 0, offset, len);

    if (len > 0) {
        memcpy(msg + sizeof(msg_head), data, len);
    }
}

void build_reply(void *msg, entity src, entity dest, entity target,
        uint64_t req_id, op_t operate, stat_t status, size_t len, void *data)
{
    assert(msg != NULL);
    build_head(msg, src, dest, target, req_id, operate, status, 0, len);

    if (len > 0) {
        memcpy(msg + sizeof(msg_head), data, len);
    }

}

