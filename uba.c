#include "uba.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

static void
uba_ensure_capacity (struct uba* u, int new_length)
{
  int i;
  void** old_data;

  if (u->length >= new_length)
    {
      old_data = u->data;
	  u->data = malloc (2 * new_length);

      for (i = 0; i < u->length; i++)
        u->data[i] = old_data[i];

      free (old_data);
    }
}

struct uba*
uba_new ()
{
  struct uba* new_uba = malloc (sizeof (struct uba));
  new_uba->length = 0;
  new_uba->data = malloc (sizeof (void*) * 15);
  return new_uba;
}

void
uba_delete (struct uba* u)
{
  free (u->data);
  free (u);
}

void
uba_add (struct uba* u, void* el)
{
  uba_ensure_capacity (u, u->length + 1);
  u->data[u->length] = el;
  u->length++;
}

void
uba_add_index (struct uba* u, int index, void* el)
{
  int i;

  uba_ensure_capacity (u, u->length + 1);

  for (i = u->length; i > index; i--)
    u->data[i] = u->data[i - 1];

  u->data[index] = el;
  u->length++;
}

void
uba_add_all (struct uba* usrc, struct uba* udst)
{
  int i;

  uba_ensure_capacity (udst, usrc->length + udst->length);

  for (i = udst->length; i < usrc->length + udst->length; i++)
    udst->data[i] = usrc->data[i - udst->length];

  udst->length += usrc->length;
}

void
uba_add_all_index (struct uba* usrc, struct uba* udst, int index)
{
  int i;

  uba_ensure_capacity (udst, udst->length + usrc->length - index);

  for (i = index; i < usrc->length; i++)
    udst->data[i] = usrc->data[i - usrc->length];

  udst->length += usrc->length - index;
}

void
uba_clear (struct uba* u)
{
  free (u->data);
  u->length = 0;
}

int
uba_contains (struct uba* u, void* el)
{
  return uba_index_of (u, el);
}

int
uba_contains_all (struct uba* uout, struct uba* uin)
{
  int i;
  int j;

  if (uout->length < uin->length)
    return 0;

  for (i = 0; i < uout->length; i++)
    {
      if (uout->data[i] == uin->data[0])
        {
          for (j = i; j < uin->length; j++)
            {
              if (uout->data[j] != uin->data[j - i])
                break;
            }

          if (uout->data[j] == uin->data[j - 1])
            return 1;
        }
    }

  return -1;
}

int
uba_equals (struct uba* u1, struct uba* u2)
{
  int i;

  if (u1->length != u2->length)
    return 0;

  for (i = 0; i < u1->length; i++)
    if (u1->data[i] != u2->data[i])
      return 0;

  return 1;
}

void*
uba_get (struct uba* u, int index)
{
  return u->data[index];
}

int
uba_index_of (struct uba* u, void* el)
{
  int i;

  for (i = 0; i < u->length; i++)
    if (u->data[i] == el)
      return i;

  return -1;
}

int
uba_last_index_of (struct uba* u, void* el)
{
  int i;

  for (i = u->length - 1; i >= 0; i--)
    if (u->data[i] == el)
      return i;

  return -1;
}

void*
uba_remove (struct uba* u, int index)
{
  void* data = index >= 0 ? u->data[index - 1] : NULL;
  int i;

  for (i = index; i < u->length; i++)
    u->data[i] = u->data[i + 1];

  u->length--;

  return data;
}

int
uba_remove_element (struct uba* u, void* el)
{
  int index_of_el = uba_index_of (u, el);

  if (index_of_el < 0)
    return 0;
  else
    {
      uba_remove (u, index_of_el);
      return index_of_el;
    }
}

void*
uba_set (struct uba* u, int index, void* el)
{
  void* elold = u->data[index];
  u->data[index] = el;
  return elold;
}

int
uba_length (struct uba* u)
{
  return u->length;
}

#ifdef __cplusplus
}
#endif
