/**
 * File: command-interpreter.c
 * Description: processes commands incoming over the serial port.
 *
 */

#include PLATFORM_HEADER
#include "hal/hal.h"
#include "hal/micro/cortexm3/uart.h"
#include "serial_utils.h"
#include "command-interpreter2.h"

//------------------------------------------------------------------------------
// Forward declarations.
static void callCommandAction(void);
static int32u stringToUnsignedInt(int8u argNum, boolean swallowLeadingSign);
static int8u charDowncase(int8u c);

//------------------------------------------------------------------------------
// Command parsing state

typedef struct {

  // Finite-state machine's current state. 
  int8u state;

  // The command line is stored in this buffer.
  // Spaces and trailing '"' and '}' characters are removed,
  // and hex strings are converted to bytes. 
  int8u buffer[COMMAND_BUFFER_LENGTH];

  // Indices of the tokens (command(s) and arguments) in the above buffer.
  // The (+ 1) lets us store the ending index.
  int8u tokenIndices[MAX_TOKEN_COUNT + 1];

  // The number of tokens read in, including the command(s). 
  int8u tokenCount;

  // Used while reading in the command line. 
  int8u index;

  // First error found in this command. 
  int8u error; 

  // Storage for reading in a hex string. A value of 0xFF means unused. 
  int8u hexHighNibble;

  // The token number of the first true argument after possible nested commands.
  int8u argOffset;

} CommandState;

CommandState commandState;

enum {
  CMD_AWAITING_ARGUMENT,
  CMD_READING_ARGUMENT,
  CMD_READING_STRING,                  // have read opening " but not closing "
  CMD_READING_HEX_STRING,              // have read opening { but not closing }
  CMD_READING_TO_EOL                   // clean up after error
};

#ifdef TEST
char *stateNames[] =
  {
    "awaiting argument",
    "reading argument",
    "reading string",
    "reading hex string",
    "reading to eol"
  };
#endif

//----------------------------------------------------------------
// Initialize the state maachine.

void commandReaderInit(void)
{
  commandState.state = CMD_AWAITING_ARGUMENT;
  commandState.index = 0;
  commandState.tokenIndices[0] = 0;
  commandState.tokenCount = 0;
  commandState.error = CMD_SUCCESS;
  commandState.hexHighNibble = 0xFF;
}

// Returns a value > 15 if ch is not a hex digit.
static int8u hexToInt(int8u ch)
{
  return ch - (ch >= 'a' ? 'a' - 10
               : (ch >= 'A' ? 'A' - 10
                  : (ch <= '9' ? '0'
                     : 0)));
}

int8u tokenLength(int8u num)
{
  return (commandState.tokenIndices[num + 1] 
          - commandState.tokenIndices[num]);
}

int8u *tokenPointer(int8s tokenNum)
{
  return commandState.buffer + commandState.tokenIndices[tokenNum];
}


//----------------------------------------------------------------
// This is a state machine for parsing commands.  If 'input' is NULL
// 'sizeOrPort' is treated as a port and characters are read from there.
// 
// Goto's are used where one parse state naturally falls into another,
// and to save flash.

boolean processCommandString(int8u *input, int8u sizeOrPort)
{
  boolean isEol = FALSE;
  boolean isSpace, isQuote;

  while (TRUE) {
    int8u next;
    
    if (input == NULL) {
      switch (serialReadByte(&next)) {
      case TRUE:
        break;
      case FALSE:
        return isEol;
      default:
        commandState.error = CMD_ERR_PORT_PROBLEM;
        goto CMD_READING_TO_EOL;
      }
    } else if (sizeOrPort == 0) {
      return isEol;
    } else {
      next = *input;
      input += 1;
      sizeOrPort -= 1;
    }

    //   fprintf(stderr, "[processing '%c' (%s)]\n", next, stateNames[commandState.state]);

    if (next == '\r')
      continue;

    isEol =   (next == '\n');
    isSpace = (next == ' ');
    isQuote = (next == '"');

    switch (commandState.state) {

    case CMD_AWAITING_ARGUMENT:
      if (isEol) {
        callCommandAction();
      } else if (! isSpace) {
        if (isQuote) {
          commandState.state = CMD_READING_STRING;
        } else if (next == '{') {
          commandState.state = CMD_READING_HEX_STRING;
        } else {
          commandState.state = CMD_READING_ARGUMENT;
        }
        goto WRITE_TO_BUFFER;
      }
      break;

    case CMD_READING_ARGUMENT:
      if (isEol || isSpace) {
        goto END_ARGUMENT;
      } else {
        goto WRITE_TO_BUFFER;
      }

    case CMD_READING_STRING:
      if (isQuote) {
        goto END_ARGUMENT;
      } else { 
        goto WRITE_TO_BUFFER;
      }

    case CMD_READING_HEX_STRING: {
      boolean waitingForLowNibble = (commandState.hexHighNibble != 0xFF);
      if (next == '}') {
        if (waitingForLowNibble) {
          commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
          goto CMD_READING_TO_EOL;          
        }
        goto END_ARGUMENT;
      } else {
        int8u value = hexToInt(next);
        if (value < 16) {
          if (waitingForLowNibble) {
            next = (commandState.hexHighNibble << 4) + value;
            commandState.hexHighNibble = 0xFF;
            goto WRITE_TO_BUFFER;
          } else {
            commandState.hexHighNibble = value;
          }
        } else if (! isSpace) {
          commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
          goto CMD_READING_TO_EOL;
        }
      }
      break;
    }

    CMD_READING_TO_EOL:
      commandState.state = CMD_READING_TO_EOL;
      
    case CMD_READING_TO_EOL:
      if (isEol) {
        if (commandState.error != CMD_SUCCESS) {
          commandErrorHandler(commandState.error);
        }
        commandReaderInit();
      }
      break;

    END_ARGUMENT:
      if (commandState.tokenCount == MAX_TOKEN_COUNT) {
        commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
        goto CMD_READING_TO_EOL;
      }
      commandState.tokenCount += 1;
      commandState.tokenIndices[commandState.tokenCount] = commandState.index;
      commandState.state = CMD_AWAITING_ARGUMENT;
      if (isEol) {
        callCommandAction();
      }
      break;

    WRITE_TO_BUFFER:
      if (commandState.index == COMMAND_BUFFER_LENGTH) {
        commandState.error = CMD_ERR_STRING_TOO_LONG;
        goto CMD_READING_TO_EOL;
      }
      if (commandState.state == CMD_READING_ARGUMENT) {
        next = charDowncase(next);
      }
      commandState.buffer[commandState.index] = next;
      commandState.index += 1;
      break;
    }
  }
}

//----------------------------------------------------------------
// Command lookup and processing

static int8u charDowncase(int8u c)
{
  if ('A' <= c && c <= 'Z')
    return c + 'a' - 'A';
  else
    return c;
}

static int8u firstByteOfArg(int8u argNum)
{
  int8u tokenNum = argNum + commandState.argOffset;
  return commandState.buffer[commandState.tokenIndices[tokenNum]];
}

static CommandEntry *commandLookup(CommandEntry *commandFinger, 
                                        int8u tokenNum)
{
  int8u *command = tokenPointer(tokenNum);
  int8u length = tokenLength(tokenNum);
  int8u i;
  for (i = 0;
       commandFinger->action != NULL;
       i++, commandFinger++) {
    PGM_P name = commandFinger->name;
    int8u *finger = command;
    for (;; name++, finger++) {
      if (finger - command == length) {
        if (*name == 0) {
          return commandFinger;
        } else {
          break;
        }
      } else if (charDowncase(*finger) != charDowncase(*name)) {
        break;
      }
    }
  }
  return NULL;
}

static void callCommandAction(void)
{
  CommandEntry *commandFinger = CommandTable;
  int8u tokenNum = 0;
  int8u i;

  if (commandState.tokenCount == 0) {
    goto kickout2;
  }

  // Lookup the command.
  while (TRUE) {
    commandFinger = commandLookup(commandFinger, tokenNum);
    if (commandFinger != NULL) {
      tokenNum += 1;
      if (commandFinger->argumentTypes[0] == 'n') {
        // Nested commands are implemented by overloading the action
        // field of a command with a pointer to another table of
        // commands.  The intermediate (void *) cast is necessary
        // to appease the Phyton compiler, though Richard claims
        // the compiler should not distinguish between a function
        // pointer and any other pointer.
        commandFinger = (CommandEntry *)(void *)(commandFinger->action);
      } else {
        commandState.argOffset = tokenNum;
        break;
      }
    } else {
      commandState.error = CMD_ERR_NO_SUCH_COMMAND;
      goto kickout;
    }
  }

  // Validate the arguments.
  for(i = 0; tokenNum < commandState.tokenCount; tokenNum++, i++) {
    int8u type = commandFinger->argumentTypes[i];
    int8u firstChar = firstByteOfArg(i);
    switch(type) {
      
    // Integers
    case 'u':
    case 'v':
    case 'w':
    case 's': {
      int32u limit = (type == 'u' ? 0xFF
                      : (type == 'v' ? 0xFFFF
                         : (type =='s' ? 0x7F : -1)));
      if (stringToUnsignedInt(i, TRUE) > limit) {
        commandState.error = CMD_ERR_ARGUMENT_OUT_OF_RANGE;
      }
      break;
    }

    // String
    case 'b':
      if (firstChar != '"' && firstChar != '{') {
        commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
      }
      break;

    case 0:
      commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
      break;

    default:
      commandState.error = CMD_ERR_INVALID_ARGUMENT_TYPE;
      break;
    }

    if (commandState.error != CMD_SUCCESS) {
      goto kickout;
    }
  }

  if (0 != commandFinger->argumentTypes[commandState.tokenCount 
                                        - commandState.argOffset]) {
    commandState.error = CMD_ERR_WRONG_NUMBER_OF_ARGUMENTS;
  }

 kickout:

  if (commandState.error == CMD_SUCCESS) {
    (commandFinger->action)();
  } else {
    commandErrorHandler(commandState.error);
  }

 kickout2:

  commandReaderInit();
}


//----------------------------------------------------------------
// Retrieving arguments

static int32u stringToUnsignedInt(int8u argNum, boolean swallowLeadingSign)
{
  int8u tokenNum = argNum + commandState.argOffset;
  int8u *string = commandState.buffer + commandState.tokenIndices[tokenNum];
  int8u length = tokenLength(tokenNum);
  int32u result = 0;
  int8u base = 10;
  int8u i;
  for (i = 0; i < length; i++) {
    int8u next = string[i];
    if (swallowLeadingSign && i == 0 && next == '-') {
      // do nothing
    } else if ((next == 'x' || next == 'X')
        && result == 0) {
      base = 16;
    } else {
      int8u value = hexToInt(next);
      if (value < base) {
        result = result * base + value;
      } else {
        commandState.error = CMD_ERR_ARGUMENT_SYNTAX_ERROR;
        return 0;
      }
    }
  }
  return result;
}

int32u unsignedCommandArgument(int8u argNum) 
{
  return stringToUnsignedInt(argNum, FALSE);
}

int16s signedCommandArgument(int8u argNum)
{
  boolean negative = (firstByteOfArg(argNum) == '-');
  int16s result = (int16s) stringToUnsignedInt(argNum, negative);
  return (negative ? -result : result);
}

int8u *stringCommandArgument(int8s argNum, int8u *length)
{
  int8u tokenNum = argNum + commandState.argOffset;
  int8u leadingQuote = (argNum < 0 ? 0 : 1);
  if (length != NULL) {
    *length = tokenLength(tokenNum) - leadingQuote;
  }
  return tokenPointer(tokenNum) + leadingQuote;
}

int8u copyStringArgument(int8s argNum, 
                              int8u *destination, 
                              int8u maxLength,
                              boolean leftPad)
{
  int8u padLength;
  int8u argLength;
  int8u *contents = stringCommandArgument(argNum, &argLength);
  if (argLength > maxLength) {
    argLength = maxLength;
  }
  padLength = leftPad ? maxLength - argLength : 0;
  MEMSET(destination, 0, padLength);
  MEMCOPY(destination + padLength, contents, argLength);
  return argLength;
}

#if !defined(APPLICATION_HAS_COMMAND_ERROR_HANDLER)
#include <stdio.h>
PGM_NO_CONST PGM_P CommandErrorNames[] =
  {
    "no error",
    "serial port error",
    "no such command",
    "wrong number of arguments",
    "integer argument out of range",
    "argument syntax error",
    "string too long",
    "invalid argument type"
  };

void commandErrorHandler(CommandStatus status) {
  printf("%s\r\n", CommandErrorNames[status]);
}
#endif
