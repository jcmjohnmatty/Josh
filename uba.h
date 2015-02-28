#ifndef __UBA_H_
#define __UBA_H_ /* __UBA_H_ */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * \brief A structure representing an unbounded array.
 */
struct uba
{
  /** The data. */
  void** data;
  /** The length. */
  int length;
};

/**
 * \brief Create a new empty uba with inital size 15.
 * \return A new empty uba.
 */
struct uba*
uba_new ();

/**
 * \brief Free the memory used for an uba.
 * \param u The uba to delete.
 */
void
uba_delete (struct uba* u);

/**
 * \brief Add an element to the end of the list.
 * \param u The uba to add to.
 * \param el The element to add.
 */
void
uba_add (struct uba* u, void* el);

/**
 * \brief Add an element to the specified index.
 * \param u The uba to add to.
 * \param index The index to add at.
 * \param el The element to add.
 */
void
uba_add_index (struct uba* u, int index, void* el);

/**
 * \brief Add all the contents of one uba to the end of another.
 * \param usrc The uba that's contents will be added.
 * \param udst The uba that will be appended onto.
*/
void
uba_add_all (struct uba* usrc, struct uba* udst);

/**
 * \brief Add all the contents of one uba to another starting at an index.
 * \param usrc The uba that's contents will be added.
 * \param udst The uba that will be appended onto.
 * \param index The index to start adding at in udst.
 */
void
uba_add_all_index (struct uba* usrc, struct uba* udst, int index);

/**
 * \brief Clear the contents of an uba.
 * \param u The uba to clear the contents of.
 */
void
uba_clear (struct uba* u);

/**
 * \brief Check if an uba contains an element.
 * \param u The uba to check in.
 * \param el The element to check for in u.
 * \return Nonzero if u contains el.
 */
int
uba_contains (struct uba* u, void* el);

/**
 * \brief Check if an uba contains another uba.
 * \param uout The uba to check in.
 * \param uin The (sub)uba to check for in uout.
 * \return Nonzero if uin is in uout.
 */
int
uba_contains_all (struct uba* uout, struct uba* uin);

/**
 * \brief Check if two ubas are ==.
 * \pararm u1 The first uba.
 * \param u2 The second uba.
 * \return Nonzero if the ubas are ==.
 */
int
uba_equals (struct uba* u1, struct uba* u2);

/**
 * \brief Get an item at an index in the uba.
 * \param u The uba to get from.
 * \param index The index of the item to get.
 * \return u[index].
 */
void*
uba_get (struct uba* u, int index);

/**
 * \brief Get the index of an element.
 * \param u The uba to get the index from.
 * \param el The element to get the index of in u.
 * \return The index of el in u, or -1 if el is not found in u.
 */
int
uba_index_of (struct uba* u, void* el);

/**
 * \brief Get the last index of an element.
 * \param u The uba to get the index from.
 * \param el The element to get the last index of in u.
 * \return The last index of el in u, or -1 if el is not found in u.
 */
int
uba_last_index_of (struct uba* u, void* el);

/**
 * \brief Remove an index form the uba.
 * \param u The uba to remove from.
 * \param index The index to remove.
 * \return The element that was removed, or (void**)0 if the element was not
 *         found.
 */
void*
uba_remove (struct uba* u, int index);

/**
 * \brief Remove the first occurance of an element from the uba.
 * \param u The uba to remove from.
 * \param el The element to remove.
 * \return Nonzero if the remove was successful.
 */
int
uba_remove_element (struct uba* u, void* el);

/**
 * \brief Set an index to an element.
 * \param u The uba to set an index.
 * \param index The index to set.
 * \param el The element to set u[index] to.
 * \return Whatever was at u[index] before executing u[index] = el.
 */
void*
uba_set (struct uba* u, int index, void* el);

/**
 * \brief Get the length of an uba.
 * \param u The uba to get the length of.
 * \return The length of u.
 */
int
uba_length (struct uba* u);

#ifdef __cplusplus
}
#endif

#endif /* __UBA_H_ */
