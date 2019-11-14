#include "challenge/challenge_lib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char* argv[])
{
  t_node left_left;
  t_node right;
  t_node left;
  t_node root;

  init_node(&left_left, "left.left", NULL, NULL);
  init_node(&left, "left", &left_left, NULL);
  init_node(&right, "right", NULL, NULL);
  init_node(&root, "root", &left, &right);

  char* serialized = serialize(&root);

  if (serialized != NULL)
  {
    printf("Serialized Node:\n%s\n", serialized);
    t_node* des_node = deserialize(serialized);
    int des_success  = 0;

    if (des_node != NULL)
    {
      if (node_cmp(&root, des_node) == 0)
      {
        printf("Deserialization was successful!\n");
        des_success = 1;
      }
      destroy_node(des_node);
    }
    if (des_success != 1)
    {
      printf("Deserialization failed!\n");
    }
    free(serialized);
  }
  else
  {
    printf("Serialization failed!\n");
  }

  return 0;
}
