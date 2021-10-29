#ifndef INCLUDE_CHALLENGE_QUEUE
#define INCLUDE_CHALLENGE_QUEUE

#include "challenge/packet.h"

typedef struct PacketQueueNode
{
    Packet content;
    struct PacketQueueNode* next;
} PacketQueueNode;

typedef struct
{
    PacketQueueNode* head;
    PacketQueueNode* tail;
    int size;
} PacketQueue;

int queue_is_empty(PacketQueue const* q);
void queue_push(PacketQueue* q, Packet p);
Packet queue_pop(PacketQueue* q);
PacketQueue queue_create();
void queue_destroy(PacketQueue* q);

#endif /* ifndef INCLUDE_CHALLENGE_QUEUE */
