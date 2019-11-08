#include "challenge/challenge_lib.h"
#include "stdlib.h"
#include "string.h"

#define START "["
#define END "]"
#define SEP ";"


static char* extract_node(const char* serialized, size_t* const continue_idx);
static char* extract_value(const char* serialized, size_t* const continue_idx);

char* serialize(const t_node* const root)
{
  char* serialized = NULL;
  if (root != NULL)
  {
    char* left    = serialize(root->left);
    char* right   = serialize(root->right);
    size_t length = strlen(START) + strlen(END) + (2 * strlen(SEP)) + 1;
    if (root->val != NULL)
    {
      length += strlen(root->val);
    }
    if (left != NULL)
    {
      length += strlen(left);
    }
    if (right != NULL)
    {
      length += strlen(right);
    }

    serialized = (char*)malloc(length * sizeof(char));
    if (serialized != NULL)
    {
      serialized[0] = '\0';
      strcat(serialized, START);
      if (root->val != NULL)
      {
        strcat(serialized, root->val);
      }
      strcat(serialized, SEP);
      if (left != NULL)
      {
        strcat(serialized, left);
        free(left);
      }
      strcat(serialized, SEP);
      if (right != NULL)
      {
        strcat(serialized, right);
        free(right);
      }
      strcat(serialized, END);
    }
  }
  else
  {
    size_t length = strlen(START) + strlen(END);
    serialized    = (char*)malloc(length * sizeof(char));
    if (serialized != NULL)
    {
      serialized[0] = '\0';
      strcat(serialized, START);
      strcat(serialized, END);
    }
  }
  return serialized;
}

static char* extract_value(const char* serialized, size_t* const continue_idx)
{
  char* val = NULL;
  if (serialized != NULL)
  {
    size_t val_end = strcspn(serialized + 1, SEP);
    val            = (char*)malloc((val_end + 1) * sizeof(char));
    if (val != NULL)
    {
      strncpy(val, serialized + 1, val_end);
      val[val_end]  = '\0';
      *continue_idx = val_end + 1;
    }
  }
  return val;
}

static char* extract_node(const char* serialized, size_t* const continue_idx)
{
  char* node_str = NULL;
  if ((serialized != NULL) && (strlen(serialized) > (strlen(START) + strlen(END))))
  {
    if (strncmp(serialized, START, strlen(START)) == 0)
    {
      /*Starts with START, as it should*/
      size_t idx    = 1;
      size_t closed = 1;
      while ((closed > 0) && (idx < strlen(serialized)))
      {
        if (strncmp(serialized + idx, END, strlen(END)) == 0)
        {
          closed--;
        }
        else if (strncmp(serialized + idx, START, strlen(START)) == 0)
        {
          closed++;
        }
        idx++;
      }
      if (closed == 0)
      {
        /*Found end of node substring*/
        node_str = (char*)malloc((idx + 1) * sizeof(char));
        strncpy(node_str, serialized, idx);
        node_str[idx] = '\0';
        *continue_idx += idx + 1;
      }
      else
      {
        /*something is malformed*/
        *continue_idx = 0;
      }
    }
    else
    {
      /*something is malformed*/
      *continue_idx = 0;
    }
  }
  return node_str;
}

t_node* deserialize(const char* serialized)
{
  t_node* root = NULL;
  if (serialized != NULL)
  {
    if ((strlen(serialized) != 0) && strcmp(serialized, "[]") != 0)
    {
      root = (t_node*)malloc(sizeof(t_node));
      init_node(root, NULL, NULL, NULL);
      if (root != NULL)
      {
        size_t subnode_idx = 0;
        char* val          = extract_value(serialized, &subnode_idx);
        root->val          = val;
        if (subnode_idx > 0)
        {
          char* left_str = extract_node(serialized + subnode_idx + 1, &subnode_idx);
          if (left_str != NULL)
          {
            root->left = deserialize(left_str);
            free(left_str);
          }
        }
        if (subnode_idx > 0)
        {
          char* right_str = extract_node(serialized + subnode_idx + 1, &subnode_idx);
          if (right_str != NULL)
          {
            root->right = deserialize(right_str);
            free(right_str);
          }
        }
      }
    }
  }
  return root;
}


void init_node(t_node* root, char* val, t_node* left, t_node* right)
{
  if (root != NULL)
  {
    root->val   = val;
    root->left  = left;
    root->right = right;
  }
}

int node_cmp(const t_node* const node1, const t_node* const node2)
{
  int cmp = 0;
  if ((node1 == NULL) && (node2 == NULL))
  {
    cmp = 0;
  }
  else if (node1 == NULL)
  {
    cmp = -1;
  }
  else if (node2 == NULL)
  {
    cmp = 1;
  }
  else
  {
    if ((node1->val == NULL) && (node2->val == NULL))
    {
      cmp = 0;
    }
    else if (node1->val == NULL)
    {
      cmp = 1;
    }
    else if (node2->val == NULL)
    {
      cmp = -1;
    }
    else
    {
      cmp = strcmp(node1->val, node2->val);
    }

    if (cmp == 0)
    {
      cmp = node_cmp(node1->left, node2->left);
    }
    if (cmp == 0)
    {
      cmp = node_cmp(node1->right, node2->right);
    }
  }
  return cmp;
}

void destroy_node(t_node* const node)
{
  if (node != NULL)
  {
    destroy_node(node->left);
    destroy_node(node->right);
    if (node->val != NULL)
    {
      free(node->val);
    }
    free(node);
  }
}
