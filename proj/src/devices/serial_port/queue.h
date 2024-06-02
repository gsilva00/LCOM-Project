#ifndef QUEUE_H
#define QUEUE_H

#include <lcom/lcf.h>


/** 
 * @brief Declaration of the queue struct
 * @details Provides encapsulation by only declaring the struct and it's alias (queue_t) in the queue.h file. The actual implementation is out of reach of the rest of the project, in the queue.c file.
*/
struct queue;
/** @brief Alias for struct queue type */
typedef struct queue queue_t;

#define Q_INIT_SZ 128 /**< Queue's initial size */


/** 
 * @brief Constructor of the queue "class" (C-class == struct + functions)
 * @details Dynamically allocates in memory a queue_t instance, effectively initializing the queue
 *
 * @param ini_size Initial queue size. If ini_size is 0, the queue is initialized with a size of 1
 * @return Pointer to the queue_t struct dynamically allocated in memory. If memory allocation fails, returns NULL
 */
queue_t *new_queue(unsigned int ini_size);
/** 
 * @brief Destructor of the queue "class"
 * @details Frees the memory allocated for the queue_t instance and its buffer
 *
 * @param q Pointer to the queue_t struct to be deallocated. If q is NULL, no operation is performed
 */
void delete_queue(queue_t *q);

/**
 * @brief Enqueues a value into the queue
 * @details If the queue is full, it tries to resize the queue. After successfully enqueuing, it adjusts the queue
 *
 * @param q Pointer to the queue struct where the value will be enqueued
 * @param n 8-bit unsigned value to be enqueued (what is passed through the serial port)
 * @return 0 upon success, non-zero otherwise
 */
int put_queue(queue_t *q, uint8_t n); 

/**
 * @brief Dequeues a value from the queue
 * @details If the queue is empty, it returns an error. After successfully dequeuing, it adjusts the queue
 *
 * @param q Pointer to the queue struct from where the value will be dequeued
 * @param n Pointer to a variable where the dequeued value will be stored
 * @return 0 upon success, non-zero otherwise
 */
int get_queue(queue_t *q, uint8_t *n);


#endif
