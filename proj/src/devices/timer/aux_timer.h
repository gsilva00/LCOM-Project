#ifndef AUX_TIMER_H
#define AUX_TIMER_H

#include <lcom/lcf.h>


// Declarations of functions to be shared between files and implemented in timer.c

/** 
 * @brief Getter for timer_intCounter static global variable in timer.c 
 *
 * @return Integer value with the current number of interrupts
 */
int get_timer_intCounter(); 

/** 
 * @brief Setter for timer_intCounter static global variable in timer.c 
 * 
 * @param newTime New integer value to change the timer_intCounter variable to
 */
void set_timer_intCounter(int newTime);


#endif /* AUX_TIMER_H */
