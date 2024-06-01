#include "queue.h"


/**
 * @struct queue
 * @brief Implementation of the queue struct as a circular queue.
 * 
 * @var queue::buf
 * Pointer to array that stores queue elements.
 * 
 * @var queue::in
 * Index of array pointed by buf to insert elements.
 * 
 * @var queue::out
 * Index of array pointed by buf to remove elements.
 * 
 * @var queue::size
 * Size of the array.
 * 
 * @var queue::count
 * Number of elements in queue.
 */
struct queue {
  int *buf;
  int in,out;
  int size;
  int count;
};


queue_t *new_queue(unsigned int in_size) {
  // allocate queue object
  queue_t *q = malloc(sizeof(*q));
  if (q == NULL) return NULL;

  // allocate space to store queue elements
  q->size = in_size ? in_size : 1;
  q->buf = malloc(q->size * sizeof(int));
  if (q->buf == NULL) {
    free(q);
    return NULL;
  }

  // initialize state of queue
  q->in = q->out = q->count = 0;
  return q;
}

void delete_queue(queue_t *q) {
  if (q == NULL) return;
  free(q->buf);
  free(q);
  q = NULL;
}


// Private functions: can be invoked only in queue.c
// Adjusts the indices to provide the circularity of the queue
static void adjust_queue(queue_t *q) {
  if (q == NULL) return;
  q->in %= q->size;
  q->out %= q->size;
}
// Resizes the queue to double its size when it's full
static int resize_queue(queue_t *q) {
  if (q == NULL) return -1;

  int *p = (int *)realloc(q->buf, 2*(q->size)*sizeof(int));
  if (p == NULL) {
    printf("Queue object was NULL!\n");
    return -1;
  }
  q->buf = p;

  for (int i = 0; i < q->in; i++) {
    // Don't know the case in which this runs once, so I need to know
    printf("The for loop inside resize_queue was called\n");
    q->buf[q->size + i] = q->buf[i];
  }

  q->in += q->size;
  q->size *= 2;
  return 0;
}

int put_queue(queue_t *q, int n) {
  if (q == NULL) return 1;

  if (q->count == q->size) {
    if (resize_queue(q)) {
      printf("Error while resizing the queue!\n");
      return -1;
    }
  }
  q->buf[q->in++] = n;
  q->count++;

  adjust_queue(q);
  return 0;
}

int get_queue(queue_t *q, int *n) {
  if (q == NULL) return 1;

  if (q->count != 0) {
    *n = q->buf[q->out++];
    q->count--;

    adjust_queue(q);
    return 0;
  }
  printf("Queue is empty! No item to get.\n");
  return -1;
}
