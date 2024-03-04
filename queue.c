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
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        free(head);
        return NULL;
    }

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *element, *element_safe;
    list_for_each_entry_safe (element, element_safe, head, list)
        q_release_element(element);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element) {
        free(element);
        return false;
    }
    int n = strlen(s) + 1;
    element->value = malloc(n * sizeof(char));
    if (!(element->value)) {
        q_release_element(element);
        return false;
    }
    memcpy(element->value, s, n);
    (element->value)[n - 1] = '\0';

    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *element = malloc(sizeof(element_t));
    if (!element) {
        free(element);
        return false;
    }
    int n = strlen(s) + 1;
    element->value = malloc(n * sizeof(char));
    if (!(element->value)) {
        q_release_element(element);
        return false;
    }
    memcpy(element->value, s, n);
    (element->value)[n - 1] = '\0';

    list_add_tail(&element->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    list_del(head->next);

    if (sp && bufsize) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_entry(head->prev, element_t, list);
    list_del(head->prev);

    if (sp && bufsize) {
        strncpy(sp, element->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return element;
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
    if (!head || list_empty(head))
        return false;
    struct list_head *front = head->next, *back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }
    element_t *element = list_entry(back, element_t, list);
    list_del(back);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;
    element_t *element, *element_dup;
    bool del_dup = false;
    list_for_each_entry_safe (element_dup, element, head, list) {
        if (&element->list == head)
            break;
        int find_dup = strcmp(element_dup->value, element->value);
        if (find_dup == 0) {
            del_dup = true;
            list_del(&element_dup->list);
            q_release_element(element_dup);
        }
        if (find_dup != 0 && del_dup) {
            list_del(&element_dup->list);
            q_release_element(element_dup);
            del_dup = false;
        }
    }
    if (del_dup) {
        list_del(&element_dup->list);
        q_release_element(element_dup);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *curr = head->next->next, *prev = head->next;
    while (prev != head && curr != head) {
        prev->next = curr->next;
        curr->next->prev = prev;
        curr->prev = prev->prev;
        prev->prev->next = curr;
        curr->next = prev;
        prev->prev = curr;
        prev = prev->next;
        curr = prev->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *curr, *next;
    list_for_each_safe (curr, next, head) {
        curr->next = curr->prev;
        curr->prev = next;
    }

    next = head->next;
    head->next = head->prev;
    head->prev = next;
}

struct list_head *reverse_seg(struct list_head *start, int k)
{
    struct list_head *end = start, *next, *prev;
    for (int i = 1; i < k; i++) {
        end = end->next;
    }
    next = end->next;
    prev = start->prev;

    end->next = start;
    start->prev = end;
    q_reverse(start);

    end->prev = prev;
    prev->next = end;
    start->next = next;
    next->prev = start;

    return start;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *current = head->next;
    int n = q_size(head);
    while (n >= k) {
        current = reverse_seg(current, k);
        current = current->next;
        n -= k;
    }
}

void mergeTwo(struct list_head *left,
              struct list_head *right,
              struct list_head *head,
              bool descend)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *element1 = list_entry(left->next, element_t, list);
        element_t *element2 = list_entry(right->next, element_t, list);
        if (descend) {
            if (strcmp(element1->value, element2->value) >= 0)
                list_move_tail(left->next, head);
            else
                list_move_tail(right->next, head);
        } else {
            if (strcmp(element1->value, element2->value) <= 0)
                list_move_tail(left->next, head);
            else
                list_move_tail(right->next, head);
        }
    }
    if (!list_empty(left))
        list_splice_tail(left, head);
    else
        list_splice_tail(right, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head, *fast = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != head && fast->next != head);

    LIST_HEAD(left);
    LIST_HEAD(right);
    list_splice_tail_init(head, &right);
    list_cut_position(&left, &right, slow);
    q_sort(&left, descend);
    q_sort(&right, descend);
    mergeTwo(&left, &right, head, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    bool find_greater = true;
    element_t *element1, *element2;
    while (find_greater) {
        find_greater = false;
        list_for_each_entry_safe (element1, element2, head, list) {
            if (strcmp(element1->value, element2->value) >= 0) {
                find_greater = true;
                list_del(&element1->list);
                break;
            }
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head) || list_is_singular(head))
        return q_size(head);
    bool find_smaller = true;
    element_t *element1, *element2;
    while (find_smaller) {
        find_smaller = false;
        list_for_each_entry_safe (element1, element2, head, list) {
            if (strcmp(element1->value, element2->value) <= 0) {
                find_smaller = true;
                list_del(&element1->list);
                break;
            }
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
