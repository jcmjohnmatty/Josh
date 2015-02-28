#ifndef __JOSH_AST_H_
#define __JOSH_AST_H_ /* __JOSH_AST_H */

/* Needed to be predeclared for the josh_cmd.h include. */
struct josh_ast;

#include "josh_cmd.h"
#include "uba.h"

/* TYPE OF LEAVES POSSIBLE FOR josh_ast LEAVES (SEE BELOW). */

/** \def All leaves are never executed. */
#define JEXEC_NEVER -1
/** \def All leaves are executed regardless (seperated by ';'). */
#define JEXEC_ALWAYS 0
/** \def Execute leaves in order, stopping when the first child does not
    execute successfully (combined by "&&"). */
#define JEXEC_AND 1
/** \def Execute leaves in order, executing the next child only if the
    previous child did not execute successfully (combined by "||"). */
#define JEXEC_OR 2
/** \def Pipe the first child's stdout into the second child's stdin. */
#define JEXEC_PIPE 3
/** \def Write the output of this command to a file. */
#define JEXEC_OUTRED 4
/** \def Append the output of this command to a file. */
#define JEXEC_OUTAPP 5
/** \def Read the contents of a file into the standard in of this command. */
#define JEXEC_INRED 6

/**
 * \brief A structre representing an ast.
 */
struct josh_ast
{
  /** An unbounded array to hold all of this node's leaves. */
  struct uba* leaves;
  /** Store the data for this node. */
  struct command* data;
  /** What type of leaves do we have. */
  int ltype;
};

/**
 * \brief Create a new ast.
 * \return A pointer to a newly created ast.
 */
struct josh_ast*
jast_new (void);

/**
 * \brief Free the memory allocated to an ast.
 * \param ast The ast whose memory to free.
 */
void
jast_delete (struct josh_ast* ast);

/**
 * \brief Set the type of children for this ast.
 * \param ast The ast to set the leaf type for.
 * \param type The type of the leaves.
 */
void
jast_set_leaf_type (struct josh_ast* ast, int type);

/**
 * \brief Set the data for this ast.
 * \param ast The ast.
 * \param data The data to set this ast's data to.
 */
void
jast_set_data (struct josh_ast* ast, struct command* data);

/**
 * \brief Add a leaf to this ast.
 * \param ast The ast to add a leaf to.
 * \param last The l(eaf) ast to add to ast.
 */
void
jast_add_leaf (struct josh_ast* ast, struct josh_ast* last);

#endif /* __JOSH_AST_H_ */
