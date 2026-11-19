/**
 * Source code and private code for fsm_app_control
 */
#include "fsm_app_control.h"
#include "main.h"
#include "adc.h"
#include "usart.h"

static fsm_context_appCtrl_t fsm_appctrl = {0};

static void fsm_appCtrl_transition(fsm_state_appCtrl_t next);

void fsm_appCtrl_init(void)
{
    fsm_appctrl.current_state = STATE_APP_CTRL_INIT;
    fsm_appctrl.buffer[0] = '\0';
}

void fsm_appCtrl_run(sys_event evt)
{
    switch (fsm_appctrl.current_state)
    {
    case STATE_APP_CTRL_INIT:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
        case EVT_STATE_EXIT:
            break;
        default:
            fsm_appCtrl_transition(STATE_WAIT_RST);
            break;
        }
        break;
    case STATE_WAIT_RST:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            break;
        case EVT_STATE_EXIT:
        {
            uint32_t adc_val = HAL_ADC_GetValue(&hadc1);
            uint32_t voltage_mv = (adc_val * 3300UL) / 4095UL;
            snprintf(fsm_appctrl.buffer, sizeof(fsm_appctrl.buffer), "!ADC=%lumV#\r\n", (unsigned long)voltage_mv);
            uart2_send(fsm_appctrl.buffer);
            break;
        }
        case EVT_CMD_RST:
            fsm_appCtrl_transition(STATE_WAIT_OK);
            break;
        default:
            break;
        }
        break;
    case STATE_WAIT_OK:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            swTim_init(&arq_tim, ARQ_TIMEOUT);
            break;
        case EVT_STATE_EXIT:
            swTim_clear(&arq_tim);
            break;
        case EVT_CMD_OK:
            fsm_appCtrl_transition(STATE_WAIT_RST);
            break;
        case EVT_TIMEOUT:
            uart2_send(fsm_appctrl.buffer);
            swTim_init(&arq_tim, ARQ_TIMEOUT);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void fsm_appCtrl_transition(fsm_state_appCtrl_t next)
{
    fsm_appCtrl_run(EVT_STATE_EXIT);
    fsm_appctrl.current_state = next;
    fsm_appCtrl_run(EVT_STATE_ENTRY);
}