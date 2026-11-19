/**
 * Public API for fsm_cmd_parser
 */
#ifndef FSM_CMD_PARSER_H
#define FSM_CMD_PARSER_H

#include "main.h"

typedef enum
{
    STATE_CMD_PARSE_INIT = 0,
    STATE_WAIT_FOR_CMD,
    STATE_CMD_OK,  // successfully parsed "!OK#"
    STATE_CMD_RST, // successfully parsed "!RST#"
    STATE_PARSING
} fsm_state_cmdParse_t;

typedef struct
{
    char buffer[64]; // to hold the received string
    char last_parsed_char;
    char to_be_parsed_char;
    fsm_state_cmdParse_t current_state;
} fsm_context_cmdParse_t;

void fsm_cmdParse_init(void);
void fsm_cmdParse_load_msg(const char *msg);
void fsm_cmdParse_run(sys_event evt);

#endif