#ifndef __JOSH_MAIN_H_
#define __JOSH_MAIN_H_ /* __JOSH_MAIN_H_ */

#include <stdio.h>

/**
 * \brief Get the input from the user.
 * \param stream The stream to read from.
 * \return The malloc()'d user input.
 */
char*
josh_fgetline (FILE* stream);

#endif /* __JOSH_MAIN_H_ */
