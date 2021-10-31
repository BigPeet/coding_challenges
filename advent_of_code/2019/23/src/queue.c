#include "challenge/queue.h"
#include "assert.h"
#include "challenge/packet.h"

static PacketQueueNode* create_node(Packet p)
{
    PacketQueueNode* created = (PacketQueueNode*) malloc(sizeof(PacketQueueNode));
    assert(created != NULL);
    if (created)
    {
        created->content = p;
        created->next    = NULL;
    }
    return created;
}

static void destroy_node(PacketQueueNode* node)
{
    if (node)
    {
        free(node);
    }
}

int queue_is_empty(PacketQueue const* q)
{
    if (q)
    {
        return q->size == 0;
    }
    return 1;
}

void queue_push(PacketQueue* q, Packet p)
{
    if (q)
    {
        PacketQueueNode* new_node = create_node(p);
        if (queue_is_empty(q))
        {
            q->head = new_node;
            q->tail = new_node;
            q->size = 1;
        }
        else
        {
            q->tail->next = new_node;
            q->tail       = new_node;
            q->size++;
        }
    }
}

Packet queue_pop(PacketQueue* q)
{
    Packet p = {0, 0};
    if (!queue_is_empty(q))
    {
        PacketQueueNode* first = q->head;
        p                      = first->content;
        if (first->next)
        {
            q->head = first->next;
            q->size--;
        }
        else
        {
            // queue had no other nodes and is now empty
            q->head = NULL;
            q->tail = NULL;
            q->size = 0;
        }
        destroy_node(first);
    }

    return p;
}

PacketQueue queue_create()
{
    PacketQueue q = {.head = NULL, .tail = NULL, .size = 0};
    return q;
}
void queue_destroy(PacketQueue* q)
{
    if (q)
    {
        if (!queue_is_empty(q))
        {
            PacketQueueNode* current = q->head;
            PacketQueueNode* next    = q->head->next;

            destroy_node(current);
            while (next)
            {
                current = next;
                next    = current->next;
                destroy_node(current);
            }
        }
    }
}
