#include "list.h"

/* Initialise a lsit head to an emply list */
void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 * 
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
			      struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
int list_empty(const struct list_head *head)
{
	return head->next == head;
}

/* Take an element out of its current list, with or without
 * reinitialising the links.of the entry*/ 
void list_del(struct list_head *entry)
{
	struct list_head *list_next = entry->next;
	struct list_head *list_prev = entry->prev;

	list_next->prev = list_prev;
	list_prev->next = list_next;
}
