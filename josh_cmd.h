#ifndef __JOSH_CMD_H_
#define __JOSH_CMD_H_ /* __JOSH_CMD_H_ */

#include "josh_ast.h"

/* DEFINES FOR josh_exec (SEE BELOW) */
#define JEXEC_FAILURE -1
#define JEXEC_SUCCESS 0

/**
 * \brief A structure representing what a single command looks like.
 */
struct command
{
  /** The program name (should == args[0]). */
  char* progname;
  /** The arguments for this program. */
  char** args;
};

/**
 * \brief Free the memory allocated for a command.
 * \param cmd The command whose memory to free (for the struct).
 */
void
cmd_delete (struct command* cmd);

/**
 * \brief Execute the commands that have been parsed and stored as an ast.
 * \param ast The ast representing the command to execute.
 * \return One of the above defines indicating the success or failure of the
 *         execution.
 */
int
josh_exec (struct josh_ast* ast);

#endif /* __JOSH_CMD_H_ */
