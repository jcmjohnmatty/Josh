/**
 * \file josh_cmd.c
 * \brief Provides the implementation of how to execute a command for josh.
 */

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "josh_cmd.h"
#include "uba.h"

void
cmd_delete (struct command* cmd)
{
  free (cmd->args);
}

/**
 * \brief Just execute one command.
 * \param progname The name of the program.
 * \param args The arguments for the program to have.
 * \return The exit code of the program.
 */
int
_josh_exec_cmd (char* progname, char** args)
{
  int status;
  int execvpret = 0;
  int waitret = -1;

  /* If it is a command we can handle do so. */
  if (strncmp (progname, "cd", 2) == 0
      && strlen (progname) == 2)
    {
      /* Change directory. */
      if (chdir (args[1]))
        {
          /* Do nothing. */
          perror (NULL);
        }
    }
  else if (strncmp (progname, "exit", 4) == 0
           && strlen (progname) == 4)
    {
      /* Exit. */
      /* Note that the OS will clean up after me. */
      exit (0);
    }

  else if (fork () == 0)
    {
      execvpret = execvp (progname, args);
      if (execvpret)
        {
          /* If execvp returned an error... */
          /* DIE!!! */
          perror (NULL);
          abort ();
        }
    }
  else
    {
      waitret = wait (&status);
      if (waitret == -1)
        return JEXEC_FAILURE;
      else
        return status;
    }

  /* We should not get here. */
  return JEXEC_FAILURE;
}

int
_josh_exec_pipe (struct uba* progs)
{
  int status1;
  int status2;
  int mypipe[2];
  struct josh_ast* tmpast;

  /* Get our ast. */
  tmpast = (struct josh_ast*) progs->data[0];

  /* First one comes from somewhere else. */
  /* Execute the first command. */
  if (fork () == 0)
    {
      if (pipe (mypipe))
        {
          perror (NULL);
          abort ();
        }

      /* We need to make another child for the second command. */
      if (fork () == 0)
        {
          /* Execute the first command. */
          tmpast = (struct josh_ast*) progs->data[0];

          if (close (mypipe[0]))
            {
              perror (NULL);
              abort ();
            }

          if (dup2 (mypipe[1], STDOUT_FILENO) == -1)
            {
              perror (NULL);
              abort ();
            }

          if (execvp (tmpast->data->progname, tmpast->data->args))
            {
              /* If execvp returned an error... */
              /* DIE!!! */
              perror (NULL);
              abort ();
            }
        }
      else
        {
          /* Wait for the second command and exit. */
          /* IF I HAVE AN ERROR I HAD !wait (&status1) == -1 */
          if (wait (&status1) == -1)
            {
              perror (NULL);
              abort ();
            }

          /* Now start the second process. */
          tmpast = (struct josh_ast*) progs->data[1];

          if (close (mypipe[1]))
            {
              perror (NULL);
              abort ();
            }

          if (dup2 (mypipe[0], STDIN_FILENO) == -1)
            {
              perror (NULL);
              abort ();
            }

          if (execvp (tmpast->data->progname, tmpast->data->args))
            {
              /* If execvp returned an error... */
              /* DIE!!! */
              perror (NULL);
              abort ();
            }

          return status1;
        }
      }
  else
    {
      if (wait (&status2) == -1)
        status2 = JEXEC_FAILURE;

      return status2;
    }

  /* We should not get here. */
  return JEXEC_FAILURE;
}

int
_josh_exec_redirect (struct uba* cmd, int inoutapp)
{
  /* Wait. */
  int status;
  /* File descriptor for the file. */
  int ifd = -1;
  int ofd = -1;
  /* Flags for open. */
  int flags = 0;
  /* For the different types of redirection. */
  char* infile = NULL;
  char* outfile = NULL;
  /* The first leaf is the command, and the second is the file. */
  struct josh_ast* cmdast;
  struct command* cmddata;
  /* We just want progname for this since that should be the file name. */
  struct command* filedata;

  /* Get the data. */
  cmdast = (struct josh_ast*) uba_get (cmd, 0);
  filedata = ((struct josh_ast*) uba_get (cmd, 1))->data;

  /* If we are just doing on or the other. */
  if (cmdast->data != NULL)
    {
      /* Set the data. */
      cmddata = cmdast->data;
      switch (inoutapp)
        {
        case 0:
          infile = filedata->args[0];
          break;

        case 1:
        case 2:
          outfile = filedata->args[0];
          break;

        default:
          break;
        }
    }
  else
    {
      /* We are combining an input and an output redirection in this case. */
      /* This gives the program name. */
      cmddata = ((struct josh_ast*) ((struct josh_ast*) cmdast->leaves->data[0])
                 ->leaves->data[0])->data;
      /* This gives the file for stdin. */
      infile = ((struct josh_ast*) ((struct josh_ast*) cmdast->leaves->data[0])
                ->leaves->data[1])->data->args[0];
      outfile = filedata->args[0];
    }

  /* First check for a built-in command.  If we have one, execute that else do
     the redirection. */
  if (strncmp (cmddata->progname, "cd", 2) == 0
      && strlen (cmddata->progname) == 2)
    {
      /* Execute the cd. */
      if (chdir (cmddata->args[1]))
        {
          /* Do nothing. */
          perror (NULL);
        }
    }

  else if (strncmp (cmddata->progname, "exit", 4) == 0
           && strlen (cmddata->progname) == 4)
    {
      /* Exit and thank the OS for cleaning up after me. */
      exit (0);
    }

  else
    {
      /* If we are doing input redirection. */
      if (infile != NULL)
        {
          ifd = open (infile, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
                      | S_IROTH);
          if (ifd == -1)
            {
              perror (NULL);
              abort ();
            }
        }

      /* If we are doing output redirection. */
      if (outfile != NULL)
        {
          flags |= O_WRONLY | O_CREAT;
          if (inoutapp > 1)
            flags |= O_APPEND;

          ofd = open (outfile, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
                      | S_IROTH);
          if (ofd == -1)
            {
              perror (NULL);
              abort ();
            }
        }

      if (fork () == 0)
        {
          /* Execute the command, first setting the file who we want to be stdin. */
          if (ifd != -1)
            {
              if (dup2 (ifd, STDIN_FILENO) == -1)
                {
                  perror (NULL);
                  abort ();
                }
            }
          if (ofd != -1)
            {
              if (dup2 (ofd, STDOUT_FILENO) == -1)
                {
                  perror (NULL);
                  abort ();
                }
            }

          if (execvp (cmddata->progname, cmddata->args))
            {
              /* If execvp returned an error... */
              /* DIE!!! */
              perror (NULL);
              abort ();
            }
        }
      else
        {
          /* Wait for the second command and exit. */
          if (wait (&status) == -1)
            status = JEXEC_FAILURE;

          /* Close the file. */
          if (ifd != -1)
            {
              if (close (ifd) == -1)
                {
                  perror (NULL);
                  abort ();
                }
            }

          if (ofd != -1)
            {
              if (close (ofd) == -1)
                {
                  perror (NULL);
                  abort ();
                }
            }
          return status;
        }
    }

  /* We should not get here. */
  return JEXEC_FAILURE;
}

int
josh_exec (struct josh_ast* ast)
{
  int i = 0;
  int retv = JEXEC_SUCCESS;

  /* Switch on command type. */
  switch (ast->ltype)
    {
    case JEXEC_NEVER:
    case JEXEC_ALWAYS:
      /* We always execute these commands or we are at the base. */
      if (ast->data != NULL)
        {
          /* Execute this command since it has no children. */
          i |= (retv = _josh_exec_cmd (ast->data->progname, ast->data->args));
        }
      /* Execute it's children if it has any. */
      for (i = 0; i < ast->leaves->length; i++)
        i |= (retv = josh_exec (uba_get (ast->leaves, i)));
      break;

    case JEXEC_AND:
      /* Execute these commands in a chain until one fails. */
      if (ast->data != NULL)
        {
          /* Execute this command since it has no children. */
          i |= (retv = _josh_exec_cmd (ast->data->progname, ast->data->args));
        }
      /* Execute it's children if it has any. */
      for (i = 0; i < ast->leaves->length; i++)
        {
          i |= (retv = josh_exec (uba_get (ast->leaves, i)));
          if (i)
            break;
        }
      break;

    case JEXEC_OR:
      /* Try to execute commands, stopping when one executes successfully. */
      if (ast->data != NULL)
        {
          /* Execute this command since it has no children. */
          i |= (retv = _josh_exec_cmd (ast->data->progname, ast->data->args));
        }
      /* Execute it's children if it has any. */
      for (i = 0; i < ast->leaves->length; i++)
        {
          i |= (retv = josh_exec (uba_get (ast->leaves, i)));
          if (!i)
            break;
        }
      break;

    case JEXEC_PIPE:
      /* Pipe the stdout of the first leaf to the stdout of the second leaf. */
      /* We should have NULL data. */
      assert (ast->data == NULL);
      i |= (retv = _josh_exec_pipe (ast->leaves));
      break;

    case JEXEC_OUTRED:
      /* Set the standard out of this command to the given file, not
         appending. */
      assert (ast->data == NULL);
      i |= (retv = _josh_exec_redirect (ast->leaves, 1));
      break;

    case JEXEC_OUTAPP:
      /* Set the standard out of this command to the given file, appending. */
      assert (ast->data == NULL);
      i |= (retv = _josh_exec_redirect (ast->leaves, 2));
      break;

    case JEXEC_INRED:
      /* We need to set the command's stdin to the file. */
      /* We should have NULL data. */
      assert (ast->data == NULL);
      i |= (retv = _josh_exec_redirect (ast->leaves, 0));
      break;

    default:
      /* If we do not know what type they are, then just stop. */
      break;
    }

  /* Finally return the return value. */
  return retv;
}
