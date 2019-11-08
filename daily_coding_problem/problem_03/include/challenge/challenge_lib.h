#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stddef.h"

typedef struct t_node
{
  char* val;
  struct t_node* left;
  struct t_node* right;
} t_node;


void init_node(t_node* root, char* val, t_node* left, t_node* right);
char* serialize(const t_node* const root);
t_node* deserialize(const char* serialized);
int node_cmp(const t_node* const node1, const t_node* const node2);
void destroy_node(t_node* const node);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
