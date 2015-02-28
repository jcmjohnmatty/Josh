/**
 * \file josh_ast.c
 * \brief Implementation of ast for josh.
 */

#include <stdlib.h>
#include "josh_ast.h"
#include "josh_cmd.h"

struct josh_ast*
jast_new (void)
{
  struct josh_ast* ast;

  ast = malloc (sizeof (struct josh_ast));
  ast->data = NULL;
  ast->leaves = uba_new ();
  ast->ltype = JEXEC_NEVER;

  return ast;
}

void
jast_delete (struct josh_ast* ast)
{
  int i;

  for (i = 0; i < ast->leaves->length; i++)
    {
      jast_delete ((struct josh_ast*) uba_get (ast->leaves, i));
    }

  uba_delete (ast->leaves);

  if (ast->data != NULL)
    cmd_delete (ast->data);

  free (ast);
}

void
jast_set_leaf_type (struct josh_ast* ast, int type)
{
  ast->ltype = type;
}

void
jast_set_data (struct josh_ast* ast, struct command* data)
{
  ast->data = data;
}

void
jast_add_leaf (struct josh_ast* ast, struct josh_ast* last)
{
  uba_add (ast->leaves, last);
}
