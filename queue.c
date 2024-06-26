#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *l = malloc(sizeof(struct list_head));
    if (!l)
        return NULL;

    INIT_LIST_HEAD(l);
    return l;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *current, *safe;
    list_for_each_entry_safe (current, safe, head, list)
        q_release_element(current);

    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;

    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }

    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    q_insert_head(head->prev, s);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *removed_element = list_first_entry(head, element_t, list);
    list_del(&removed_element->list);
    if (sp != NULL && bufsize > 0) {
        strncpy(sp, removed_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return removed_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *removed_element = list_last_entry(head, element_t, list);
    list_del(&removed_element->list);

    if (sp != NULL && bufsize > 0) {
        strncpy(sp, removed_element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return removed_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */

bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;


    struct list_head **indir = &head, *fast;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next) {
        indir = &(*indir)->next;
    }
    indir = &(*indir)->next;
    element_t *mid_element = list_entry(*indir, element_t, list);
    list_del(*indir);
    q_release_element(mid_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    element_t *curr, *safe;
    bool is_dup = false;
    list_for_each_entry_safe (curr, safe, head, list) {
        if (&safe->list != head && !strcmp(curr->value, safe->value)) {
            list_del(&curr->list);
            q_release_element(curr);
            is_dup = true;
        }

        else if (is_dup) {
            list_del(&curr->list);
            q_release_element(curr);
            is_dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *safe;
    list_for_each_safe (curr, safe, head)
        list_move(curr, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *safe, *tmp_head = head;
    LIST_HEAD(splited_list_head);
    int len = 0;
    list_for_each_safe (curr, safe, head) {
        len++;
        if (len == k) {
            list_cut_position(&splited_list_head, tmp_head, curr);
            q_reverse(&splited_list_head);
            list_splice(&splited_list_head, tmp_head);
            len = 0;
            tmp_head = safe->prev;
        }
    }
}

int mergeTwoList(struct list_head *L1, struct list_head *L2, bool descend)
{
    if (!L1 || !L2)
        return 0;

    LIST_HEAD(head);
    int count = 0;
    while (!list_empty(L1) && !list_empty(L2)) {
        element_t *E1 = list_first_entry(L1, element_t, list);
        element_t *E2 = list_first_entry(L2, element_t, list);
        int cmp = strcmp(E1->value, E2->value);
        if (descend)
            cmp = -cmp;
        if (cmp > 0) {
            list_move_tail(&E2->list, &head);
        } else
            list_move_tail(&E1->list, &head);
        count++;
    }
    count += q_size(L1) + q_size(L2);
    list_splice(&head, L1);
    list_splice_tail_init(L2, L1);
    return count;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    struct list_head **indir = &head, *fast;
    for (fast = head->next; fast != head && fast->next != head;
         fast = fast->next->next) {
        indir = &(*indir)->next;
    }

    LIST_HEAD(tmp);
    list_cut_position(&tmp, *indir, head->prev);
    q_sort(head, descend);
    q_sort(&tmp, descend);
    mergeTwoList(head, &tmp, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int len = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_entry(cur->list.prev, element_t, list);
        if (strcmp(prev->value, cur->value) > 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            len++;
            cur = prev;
        }
    }
    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    int len = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_entry(cur->list.prev, element_t, list);
        if (strcmp(prev->value, cur->value) < 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            len++;
            cur = prev;
        }
    }
    return len;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
