/**
 * \file josh_parse.c
 * \brief Contains a parser for command line arguments given to josh.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "josh_ast.h"
#include "josh_cmd.h"
#include "josh_parse.h"
#include "uba.h"

int
_josh_parse_ws (struct josh_ast* ast, char* comstr)
{
  int i;
  /* For tokenizing. */
  char* pws;
  char* pwssave;
  /* A command. */
  struct command* cmd;
  /* Store things split on whitespaces. */
  struct uba* wsuba;

  /* Next split on spaces and store the command obtained in the ast. */
  wsuba = uba_new ();
  pws = strtok_r (comstr, " \\\n\r\t\v", &pwssave);
  while (pws != NULL)
    {
      /* Append the current token. */
      uba_add (wsuba, pws);

      /* Get the next token. */
      pws = strtok_r (NULL, " \\\n\r\t\v", &pwssave);
    }

  /* Create a new command and set that as the ast's data. */
  cmd = malloc (sizeof (struct command));
  cmd->progname = uba_get (wsuba, 0);
  cmd->args = malloc (sizeof (char*) * (wsuba->length + 1));
  for (i = 0; i < wsuba->length; i++)
    {
      cmd->args[i] = uba_get (wsuba, i);
    }
  cmd->args[wsuba->length] = NULL;
  /* Free the uba. */
  uba_delete (wsuba);

  /* Set the tree's data. */
  jast_set_data (ast, cmd);

  return JPARSE_SUCCESS;
}

int
_josh_parse_inredirect (struct josh_ast* orast, char* por)
{
  int retv = 0;
  /* For tokenizing. */
  char* pin;
  char* pinsave;
  char* pinf;
  /* Temporary ast. */
  struct josh_ast* inast = NULL;
  struct josh_ast* cmdast = NULL;
  struct josh_ast* inredast = NULL;

  /* Split on '<'. */
  pin = strtok_r (por, "<", &pinsave);

  /* If we do not have any tokens, next split on out redirection and add that to
     the or ast. */
  if (pinsave[0] == 0)
    {
      retv |= _josh_parse_ws (orast, por);
    }
  else
    {
      /* Else add it to the or tree. */
      //while (pin != NULL)
        //{
          /* Get the file to take in. */
          pinf = strtok_r (NULL, " ", &pinsave);
          if (pinf == NULL)
            {
              /* Do I want to return a syntax error or just return? */
              return JPARSE_INVALID_SYNTAX;
            }

          /* Create a new temporary ast to hold the result. */
          inast = jast_new ();
          jast_set_leaf_type (inast, JEXEC_INRED);
          jast_add_leaf (orast, inast);

          /* The first leaf of the inast should be the command, and the second
             is the file. */
          cmdast = jast_new ();
          inredast = jast_new ();
          jast_set_leaf_type (cmdast, JEXEC_ALWAYS);
          jast_set_leaf_type (inredast, JEXEC_NEVER);
          jast_add_leaf (inast, cmdast);
          jast_add_leaf (inast, inredast);

          retv |= _josh_parse_ws (cmdast, pin);
          /* If there was an error, propigate the error. */
          if (retv != JPARSE_SUCCESS)
            return retv;
          retv |= _josh_parse_ws (inredast, pinf);
          /* If there was an error, propigate the error. */
          if (retv != JPARSE_SUCCESS)
            return retv;

          /* Get the next token. */
          pin = strtok_r (NULL, "<", &pinsave);
        //}
    }

  return retv;
}

int
_josh_parse_outredirect (struct josh_ast* orast, char* por)
{
  int retv = 0;
  int ltype = JEXEC_NEVER;
  /* For tokenizing. */
  char* pout;
  char* poutsave;
  char* poutf;
  /* Temporary ast. */
  struct josh_ast* outast = NULL;
  struct josh_ast* cmdast = NULL;
  struct josh_ast* outredast = NULL;

  /* Split on '>'. */
  pout = strtok_r (por, ">", &poutsave);

  /* If we do not have any tokens, add the command to the or tree. */
  if (poutsave == NULL || poutsave[0] == 0)
    {
      retv |= _josh_parse_inredirect (orast, por);
    }
  else
    {
      /* Else do the parsing for out redirection. */
      //while (pout != NULL)
        //{
          poutf = strtok_r (NULL, " ", &poutsave);
          if (poutf == NULL)
            return JPARSE_INVALID_SYNTAX;

          /* If the first character is not '>', then we are just writing to that
             file, otherwise we append. */
          if (poutsave[0] == 0)
            {
              ltype = JEXEC_OUTRED;
            }
          else
            {
              ltype = JEXEC_OUTAPP;
              /* Take the extra '>' out. */
              poutf = strtok_r (NULL, ">", &poutsave);
            }

          /* Now create the ast. */
          outast = jast_new ();
          jast_set_leaf_type (outast, ltype);
          jast_add_leaf (orast, outast);

          /* Create the leaf for the program and for the file. */
          cmdast = jast_new ();
          outredast = jast_new ();
          jast_set_leaf_type (cmdast, JEXEC_ALWAYS);
          jast_set_leaf_type (outredast, JEXEC_NEVER);
          jast_add_leaf (outast, cmdast);
          jast_add_leaf (outast, outredast);

          retv |= _josh_parse_inredirect (cmdast, pout);
          /* If there was an error, propigate the error. */
          if (retv != JPARSE_SUCCESS)
            return retv;
          retv |= _josh_parse_ws (outredast, poutf);
          /* If there was an error, propigate the error. */
          if (retv != JPARSE_SUCCESS)
            return retv;

          /* Get the next token. */
          pout = strtok_r (NULL, ">", &poutsave);          
      //}
    }

  return retv;
}

int
_josh_parse_or (struct josh_ast* andast, char* pand)
{
  int retv = 0;
  /* For tokenizing. */
  char* por;
  char* porsave;
  char* ppipe;
  /* Temporary ast. */
  struct josh_ast* orast = NULL;
  struct josh_ast* pipeast = NULL;
  struct josh_ast* pipeast0 = NULL;
  struct josh_ast* pipeast1 = NULL;

  /* Next split on or ('|'). */
  por = strtok_r (pand, "|", &porsave);

  /* If we do not have any or tokens, add the command to the and
     tree. */
  if (porsave[0] == 0)
    {
      retv |= _josh_parse_outredirect (andast, pand);
    }
  else
    {
      /* Else add it to the or tree. */
      while (por != NULL && porsave[0] != 0)
        {
          /* If the first character of porsave is '|', then it is an or, else it
             is a pipe. */
          if (porsave[0] != '|')
            {
              /* Handle piping. */
              /* por holds the program whose stdout we want, so next we split on
                 or again to get the program whose stdin to redirect to. */
              ppipe = strtok_r (NULL, "|", &porsave);

              /* If ppipe == NULL, we have invalid syntax, so return that
                 error. */
              if (ppipe == NULL)
                return JPARSE_INVALID_SYNTAX;
              else
                {
                  /* Now that we have both programs, create the tree. */
                  pipeast = jast_new ();
                  jast_set_leaf_type (pipeast, JEXEC_PIPE);
                  /* If we have an or tree, add it. */
                  if (orast != NULL)
                    {
                      jast_add_leaf (orast, pipeast);
                    }
                  /* Add the two programs to it. */
                  pipeast0 = jast_new ();
                  pipeast1 = jast_new ();
                  jast_set_leaf_type (pipeast0, JEXEC_ALWAYS);
                  jast_set_leaf_type (pipeast1, JEXEC_ALWAYS);
                  jast_add_leaf (pipeast, pipeast0);
                  jast_add_leaf (pipeast, pipeast1);
                  retv |= _josh_parse_outredirect (pipeast0, por);
                  if (retv != JPARSE_SUCCESS)
                    return retv;
                  retv |= _josh_parse_outredirect (pipeast1, ppipe);
                  if (retv != JPARSE_SUCCESS)
                    return retv;
                }
            }
          else
            {
              /* Handle oring. */
              /* Create a new temporary tree to hold or values. */
              orast = jast_new ();
              jast_set_leaf_type (orast, JEXEC_OR);
              jast_add_leaf (andast, orast);

              /* If we have a pipe tree, add it. */
              if (pipeast != NULL)
                {
                  jast_add_leaf (orast, pipeast);
                  /* Mark pipeast as NULL so we do not continually add it. */
                  pipeast = NULL;
                }

              retv |= _josh_parse_outredirect (orast, por);

              /* Get the next or token. */
              por = strtok_r (NULL, "|", &porsave);
            }
        }
    }

  /* If we have a pipe tree but not an or tree, add the pipe tree to the and
     tree. */
  if (pipeast != NULL && orast == NULL)
    jast_add_leaf (andast, pipeast);

  return retv;
}

int
_josh_parse_and (struct josh_ast* semiast, char* psemi)
{
  int retv = 0;
  /* For tokenizing. */
  char* pand;
  char* pandsave;
  /* Temporary ast. */
  struct josh_ast* andast;

  /* Next split on and ('&'). */
  pand = strtok_r (psemi, "&", &pandsave);

  /* If we do not have any and tokens, add the command to the semi
     tree. */
  if (pandsave[0] == 0)
    {
      /* Split on or because we could still have ors. */
      retv |= _josh_parse_or (semiast, psemi);
    }
  else
    {
      /* Otherwise split on and and add the result to the semi tree. */
      while (pand != NULL)
        {
          /* Create a new temporary tree to hold and values. */
          andast = jast_new ();
          jast_set_leaf_type (andast, JEXEC_AND);
          jast_add_leaf (semiast, andast);

          retv |= _josh_parse_or (andast, pand);
          /* If a syntax error was encountered, then propigate the error. */
          if (retv != JPARSE_SUCCESS)
            return retv;

          /* Get the next and token. */
          pand = strtok_r (NULL, "&", &pandsave);
        }
    }

  return retv;
}

int
_josh_parse_semi (struct josh_ast* ast, char* cmdstr)
{
  int retv = JPARSE_SUCCESS;
  /* For tokenizing. */
  char* psemi;
  char* psemisave;
  /* Temporary asts. */
  struct josh_ast* semiast;

  /* First split on semicolons (';'). */
  psemi = strtok_r (cmdstr, ";", &psemisave);
  /* If we do not have any semicolon tokens, add the command to ast. */
  if (psemisave[0] == 0)
    {
      /* Split on and because we still could have ands. */
      retv = _josh_parse_and (ast, psemi);
    }
  else
    {
      /* Split on semicolons and add. */
      while (psemi != NULL)
        {
          /* Create a new temporary tree to hold semicolon values. */
          semiast = jast_new ();
          jast_set_leaf_type (semiast, JEXEC_ALWAYS);
          jast_add_leaf (ast, semiast);

          /* Split on and. */
          retv |= _josh_parse_and (semiast, psemi);

          /* Get the next semicolon token. */
          psemi = strtok_r (NULL, ";", &psemisave);
        }
    }

  return retv;
}

int
josh_parse_cmd (struct josh_ast* ast, char* cmdstr)
{
  /* If we do not have any input do not do anything. */
  if (strcmp (cmdstr, "") == 0)
    return JPARSE_NOINPUT;

  /* Otherwise parse cmdstr. */
  return _josh_parse_semi (ast, cmdstr);
}
