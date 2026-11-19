/**
 * Following my experience with this course's major assignment,
 * this file will be the public API for fsm_app_control.
 */
#ifndef FSM_UART_COMM_H
#define FSM_UART_COMM_H

#include "main.h"

typedef enum
{
    STATE_APP_CTRL_INIT = 0,
    STATE_WAIT_RST,
    STATE_WAIT_OK
} fsm_state_appCtrl_t;

typedef struct
{
    char buffer[64]; // to hold the output message
    fsm_state_appCtrl_t current_state;
} fsm_context_appCtrl_t;

void fsm_appCtrl_init(void);
void fsm_appCtrl_run(sys_event evt);

#endif