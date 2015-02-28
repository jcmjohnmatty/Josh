#ifndef __JOSH_PARSE_H_
#define __JOSH_PARSE_H_ /* __JOSH_PARSE_H_ */

#include "josh_ast.h"

/* RETURN VALUES FROM josh_parse_cmd */

/** \def We were successful parsing. */
#define JPARSE_SUCCESS 0
/** \def Invalid syntax was encountered. */
#define JPARSE_INVALID_SYNTAX 1
/** \def We did not recieve any input. */
#define JPARSE_NOINPUT 2

/**
 * \brief Parse the given command string.
 * \param ast A pointer to the ast to store in.
 * \param cmdstr The command string to parse.
 * \return JPARSE_SUCCESS on success, or one of the #defined errors mentioned
 *         above.
 */
int
josh_parse_cmd (struct josh_ast* ast, char* cmdstr);

#endif /* __JOSH_PARSE_H_ */
