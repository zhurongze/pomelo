#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

typedef struct entity {
    uint32_t type;
    uint32_t id;
}entity;

typedef int64_t     offset_t;
typedef uint32_t    op_t;
typedef int32_t     stat_t;

typedef struct msg_head {
    entity      src;
    entity      dest;
    entity      target;
    uint64_t    req_id;
    offset_t    offset;
    uint32_t    version;
    op_t        operate;
    stat_t      status;
    size_t      len;
}msg_head;

#define PML_OP_LOGIN    0x0001
#define PML_OP_READ     0x0002
#define PML_OP_WRITE    0x0003

#define PML_OP_LOGIN_REP    0x0100
#define PML_OP_READ_REP     0x0200
#define PML_OP_WRITE_REP    0x0300

#define PML_VERSION 0x00000001



void build_request(void *msg, entity src, entity dest, entity target, 
        uint64_t req_id, op_t operate, offset_t offset, size_t len, void *data);

void build_reply(void *msg, entity src, entity dest, entity target,
        uint64_t req_id, op_t operate, stat_t status, size_t len, void *data);

#endif
