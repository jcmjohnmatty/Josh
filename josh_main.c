/**
 * \file josh_main.c
 * \brief Main driver program for JOhn's own SHell.
 */

#define _GNU_SOURCE

#include <features.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include "josh_ast.h"
#include "josh_cmd.h"
#include "josh_main.h"
#include "josh_parse.h"

void
josh_sighand (int signum)
{
  switch (signum)
    {
    case SIGINT:
      /* If we got a ^C, do nothing except print \n. */
      printf ("\n");
      break;

    case SIGQUIT:
      /* If we got a C-\, exit. */
      printf ("\n");
      exit (0);

    default:
      /* Otherwise exit. */
      break;
    }}


int
main (int argc, char* argv[])
{
  int res = -1;
  char* cdir;
  /* The string to hold our input. */
  char* cmdstr;
  char* cmdstrcpy;
  /* Our ast. */
  struct josh_ast* ast;
  //struct termios term;

  /* Install my signal handler. */
  signal (SIGINT, josh_sighand);
  signal (SIGQUIT, josh_sighand);

  /* TODO this breaks ^D for bash. */
  /* Change the QUIT character to ^D, not C-\ */
  //tcgetattr (0, &term);
  //term.c_cc[VQUIT] = 4;
  //tcsetattr (0, TCSANOW, &term);

  /* For now continuously get input and just call josh_parse_cmd. */
  while (1)
    {
      /* Print out our prompt. */
      cdir = get_current_dir_name ();
      printf ("%s$ ", cdir);
      fflush (stdout);

      /* Create a new tree. */
      ast = jast_new ();

      /* Get user input. */
      fflush (stdin);
      cmdstr = josh_fgetline (stdin);

      /* Parse the command. */
      cmdstrcpy = malloc (strlen (cmdstr) - 1);
      /* Avoid newline. */
      strncpy (cmdstrcpy, cmdstr, strlen (cmdstr) - 1);
      res = josh_parse_cmd (ast, cmdstr);

      switch (res)
        {
        case JPARSE_SUCCESS:
          /* Execute the command. */
          res = josh_exec (ast);
          switch (res)
            {
            case JEXEC_FAILURE:
              /* On failure to execute a command print a message. */
              printf ("josh: unable to execute command '%s'.\n", cmdstrcpy);

            case JEXEC_SUCCESS:
              /* Just break on success. */
              break;

            default:
              /* What should I do here? */
              break;
            }
          /* Finally free the tree. */
          free (cmdstr);
          free (cmdstrcpy);
          jast_delete (ast);
          free (cdir);
          break;

        case JPARSE_INVALID_SYNTAX:
          /* They entered a command wrong. */
          printf ("josh: invalid command '%s'.\n", cmdstrcpy);
          /* Finally free the tree. */
          free (cmdstr);
          free (cmdstrcpy);
          jast_delete (ast);
          free (cdir);
          break;

        case JPARSE_NOINPUT:
          /* Finally free the tree. */
          free (cmdstr);
          free (cmdstrcpy);
          jast_delete (ast);
          free (cdir);
          break;

        default:
          /* What hapened?. */
          break;
        }
    }

  return 0;
}

char*
josh_fgetline (FILE* stream)
{
  char* ret;
  char* line = NULL;

  /* Limit user input to 1 MiB. */
  line = malloc (1024 * 1024);

  /* If the last two characters are '\\' and '\n', or the last character is not
     '\n' and not '\0', continue to get input. */
  ret = fgets (line, 1024 * 1024, stream);
  if (ret == NULL)
    {
      if (line != NULL)
        free (line);
      return NULL;
    }

  return line;
}
