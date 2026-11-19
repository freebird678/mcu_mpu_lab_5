/**
 * Source code and private code for fsm_cmd_parser
 */
#include "fsm_cmd_parser.h"
#include "main.h"

static fsm_context_cmdParse_t fsm_cmdparse = {0};
static uint8_t parse_idx = 0;

static void fsm_cmdParse_transition(fsm_state_cmdParse_t next);

void fsm_cmdParse_init(void)
{
    fsm_cmdparse.current_state = STATE_CMD_PARSE_INIT;
    fsm_cmdparse.last_parsed_char = 0;
    fsm_cmdparse.to_be_parsed_char = 0;
    fsm_cmdparse.buffer[0] = '\0'; // set the first character to a null terminator to indicate an empty string
    parse_idx = 0;
}

void fsm_cmdParse_load_msg(const char *msg)
{
    if (msg == NULL)
        return;
    strncpy(fsm_cmdparse.buffer, msg, sizeof(fsm_cmdparse.buffer) - 1);
    fsm_cmdparse.buffer[sizeof(fsm_cmdparse.buffer) - 1] = '\0';
}

void fsm_cmdParse_run(sys_event evt)
{
    switch (fsm_cmdparse.current_state)
    {
    case STATE_CMD_PARSE_INIT:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
        case EVT_STATE_EXIT:
            break;
        default:
            fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
            break;
        }
        break;
    case STATE_WAIT_FOR_CMD:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            fsm_cmdparse.last_parsed_char = 0;
            fsm_cmdparse.to_be_parsed_char = 0;
            parse_idx = 0;
            break;
        case EVT_STATE_EXIT:
            break;
        case EVT_MSG_RCVD:
            fsm_cmdParse_transition(STATE_PARSING);
            break;
        default:
            break;
        }
        break;
    case STATE_PARSING:
        // parse char by char, handle incorrect command
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            parse_idx = 0;
            fsm_cmdparse.last_parsed_char = 0;
            fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[0];
            break;
        case EVT_STATE_EXIT:
            break;
        case EVT_NONE:
        {
            char ch = fsm_cmdparse.to_be_parsed_char;
            if (parse_idx == 0)
            {
                if (ch == '!')
                {
                    fsm_cmdparse.last_parsed_char = ch;
                    parse_idx++;
                    fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[parse_idx];
                }
                else
                {
                    fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
                }
            }
            else if (parse_idx == 1)
            {
                if (ch == 'R' || ch == 'O')
                {
                    fsm_cmdparse.last_parsed_char = ch;
                    parse_idx++;
                    fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[parse_idx];
                }
                else
                {
                    fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
                }
            }
            else if (fsm_cmdparse.last_parsed_char == 'R' && ch == 'S')
            {
                fsm_cmdparse.last_parsed_char = ch;
                parse_idx++;
                fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[parse_idx];
            }
            else if (fsm_cmdparse.last_parsed_char == 'S' && ch == 'T')
            {
                fsm_cmdparse.last_parsed_char = ch;
                parse_idx++;
                fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[parse_idx];
            }
            else if (fsm_cmdparse.last_parsed_char == 'T' && ch == '#' && fsm_cmdparse.buffer[parse_idx + 1] == '\0')
            {
                fsm_cmdparse.last_parsed_char = ch;
                fsm_cmdParse_transition(STATE_CMD_RST);
            }
            else if (fsm_cmdparse.last_parsed_char == 'O' && ch == 'K')
            {
                fsm_cmdparse.last_parsed_char = ch;
                parse_idx++;
                fsm_cmdparse.to_be_parsed_char = fsm_cmdparse.buffer[parse_idx];
            }
            else if (fsm_cmdparse.last_parsed_char == 'K' && ch == '#' && fsm_cmdparse.buffer[parse_idx + 1] == '\0')
            {
                fsm_cmdparse.last_parsed_char = ch;
                fsm_cmdParse_transition(STATE_CMD_OK);
            }
            else
            {
                fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
            }
            break;
        }
        default:
            break;
        }
        break;
    case STATE_CMD_RST:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            app_evt = EVT_CMD_RST;
            fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
            break;
        case EVT_STATE_EXIT:
            break;
        default:
            break;
        }
        break;
    case STATE_CMD_OK:
        switch (evt)
        {
        case EVT_STATE_ENTRY:
            app_evt = EVT_CMD_OK;
            fsm_cmdParse_transition(STATE_WAIT_FOR_CMD);
            break;
        case EVT_STATE_EXIT:
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void fsm_cmdParse_transition(fsm_state_cmdParse_t next)
{
    fsm_cmdParse_run(EVT_STATE_EXIT);
    fsm_cmdparse.current_state = next;
    fsm_cmdParse_run(EVT_STATE_ENTRY);
}