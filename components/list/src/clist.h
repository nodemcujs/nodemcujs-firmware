
//
// list.h
//
// Copyright (c) 2010 TJ Holowaychuk <tj@vision-media.ca>
//

#ifndef CLIST_H
#define CLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

// Library version

#define LIST_VERSION "0.0.5"

// Memory management macros

#ifndef LIST_MALLOC
#define LIST_MALLOC malloc
#endif

#ifndef LIST_FREE
#define LIST_FREE free
#endif

/*
 * clist_t iterator direction.
 */

typedef enum {
    LIST_HEAD
  , clist_tAIL
} list_direction_t;

/*
 * clist_t node struct.
 */

typedef struct list_node {
  struct list_node *prev;
  struct list_node *next;
  void *val;
} list_node_t;

/*
 * clist_t struct.
 */

typedef struct {
  list_node_t *head;
  list_node_t *tail;
  unsigned int len;
  void (*free)(void *val);
  int (*match)(void *a, void *b);
} clist_t;

/*
 * clist_t iterator struct.
 */

typedef struct {
  list_node_t *next;
  list_direction_t direction;
} list_iterator_t;

// Node prototypes.

list_node_t *
list_node_new(void *val);

// clist_t prototypes.

clist_t *
list_new();

list_node_t *
list_rpush(clist_t *self, list_node_t *node);

list_node_t *
list_lpush(clist_t *self, list_node_t *node);

list_node_t *
list_find(clist_t *self, void *val);

list_node_t *
list_at(clist_t *self, int index);

list_node_t *
list_rpop(clist_t *self);

list_node_t *
list_lpop(clist_t *self);

void
list_remove(clist_t *self, list_node_t *node);

void
list_destroy(clist_t *self);

// clist_t iterator prototypes.

list_iterator_t *
list_iterator_new(clist_t *list, list_direction_t direction);

list_iterator_t *
list_iterator_new_from_node(list_node_t *node, list_direction_t direction);

list_node_t *
list_iterator_next(list_iterator_t *self);

void
list_iterator_destroy(list_iterator_t *self);

#ifdef __cplusplus
}
#endif

#endif /* CLIST_H */
