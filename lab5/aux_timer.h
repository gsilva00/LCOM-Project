#ifndef AUX_TIMER_H
#define AUX_TIMER_H
// Declarations of functions to be shared between files and implemented in timer.c

int get_timer_intCounter(); /** @brief Getter for timer_intCounter static global variable in timer.c */
void set_timer_intCounter(int newTime); /** @brief Setter for timer_intCounter static global variable in timer.c */

#endif /* AUX_TIMER_H */
