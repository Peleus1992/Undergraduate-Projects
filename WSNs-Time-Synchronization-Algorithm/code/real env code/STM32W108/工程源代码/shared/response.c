#include PLATFORM_HEADER
#include "hal/hal.h"
#include "serial_utils.h"
#include "hal/error.h"

// Verbosity options
boolean responseHeaderDisplay=TRUE;
boolean responseCommandDisplay=TRUE;
//boolean responseTagDisplay=TRUE;    disabling tag display not yet supported
boolean responseDelimDisplay=TRUE;
boolean responseVerboseDisplay=TRUE;

// --------------------------------
// A simple printf() implementation
// Supported format specifiers are:
//  %% - percent sign
//  %c - single byte character
//  %s - ram string
//  %p - flash string  (non-standard)
//  %u - 2-byte unsigned decimal
//  %d - 2-byte signed decimal
//  %x %2x %4x - 1, 2, 4 byte hex value (always 0 padded) (non-standard)
//  Special response format specifiers:
//  %n : command name const string</li>
//  %N : command name RAM string</li>
//  %t : tag const string</li>
//  %T : tag RAM string</li>
//  && : ampersand
//  additionally, if any of the response format specifiers are prefixed with 
//  '&' instead of '%' they will be auto-wrapped in curly braces {}
//  See the platformtest architecture spec for details.
// 
// A few macros and a function help make this readable:
//   - flush the local buffer to the output
//   - ensure that there is some room in the local buffer
//   - add a single byte to the local buffer
//   - convert a nibble to its ascii hex character
//   - convert an int16u to a decimal string
// Most of these only work within the emPrintfInternal() function.

// Current champion is %4x which writes 8 bytes.  (%s and %p can write
// more, but they do their own overflow checks).
#define LOCAL_BUFFER_SIZE 16
#define MAX_SINGLE_COMMAND_BYTES 10

typedef struct {
  printfFlushHandler *flushHandler; 
  int8u localBuffer[LOCAL_BUFFER_SIZE + MAX_SINGLE_COMMAND_BYTES];
  int8u *localBufferPointer;
  int8u *localBufferLimit;
  int8u count;
  int8u total;
} PrintfState; 

#define failIfNot(foo)  do { if(!(foo)) goto fail; } while(0)

static boolean flushBuffer(PrintfState *state)
{
  int8u count = state->localBufferPointer - state->localBuffer;
  if (!(state->flushHandler(state->localBuffer, count)))
    return FALSE;
  state->total += count;
  state->localBufferPointer = state->localBuffer;
  return TRUE;
}

#define addByte(byte) \
do { *(state->localBufferPointer++) = (byte); } while (FALSE)

int8u *writeHex(int8u *charBuffer, int16u value, int8u charCount);

static boolean printfWriteString(PrintfState *state,
                               int8u *str)
{
    int8u len;
    
    for (len=0; str[len] != '\0'; len++)
      ;
    if(state->flushHandler(str, len)) {
      state->total += len;
      return TRUE;
    } else
      return FALSE;  
}


// Returns number of characters written
int8u responsePrintfInternal(PrintfState *state, 
                             const char * string, 
                             va_list args)
{
  boolean processingTag;
  boolean addDelim;
  int16u decimalValue;
  boolean decimalValueAvailable;

  state->localBufferPointer = state->localBuffer;
  state->localBufferLimit = state->localBuffer + LOCAL_BUFFER_SIZE;
  state->total = 0;

  for (; *string; string++) {
    int8u next = *string;
    if ((next != '%') && (next != '&')) {
      if((next == '{') || (next == '}') || (next =='#')) { 
        if(responseDelimDisplay) addByte(next);
      } else {
        if(responseVerboseDisplay) addByte(next);
      }
    } else {
      string += 1;
      if((*string == '%') || (*string == '&')) {
        addByte(*string);
      } else {
        if(next == '&' && responseDelimDisplay) {
            addDelim = TRUE;
            // initial bracket for all but command name, which is handled separate
            if((*string != 'n') && (*string != 'N'))
              addByte('{');
        } else {
          addDelim = FALSE;
        }        
        processingTag = FALSE;
        switch (*string) {
          case 'n':
          case 'N': {
            int8u *str = va_arg(args, int8u *);
            if(responseCommandDisplay) {
              if(addDelim) {
                addByte('{'); addByte('(');
              }
              failIfNot(flushBuffer(state));
              failIfNot(printfWriteString(state, str));
              if(addDelim) {
                addByte(')'); addByte('}');
              }
            }
            addDelim = FALSE;  // suppress add at end of loop
            break; }
          case 't':
          case 'T':
              {
                int8u *str = va_arg(args, int8u *);
                failIfNot(flushBuffer(state));
                failIfNot(printfWriteString(state, str));
              }
            failIfNot(flushBuffer(state));
            addByte(':');
            processingTag = TRUE;
            string += 1;
            // fall through to default case for handling of specifier after tag
          default:
            // pass-through to standard identifiers
            
            {
              // Allow space padding for ints, up to five digits plus sign.
              int8u widthSpecifier = *string - '0';
              int8u formatSpecifier = *(string+1);
              if ( (widthSpecifier > 1)
                 &&(widthSpecifier <= 6)
                 &&( (formatSpecifier == 'd')
                   ||(formatSpecifier == 'u') ) ) {
                int8u numDigits = 0;
                int16u tempValue = va_arg(args, int);
                decimalValue = tempValue;
                decimalValueAvailable = TRUE;
                if ( (formatSpecifier == 'd') && (tempValue & 0x8000) ) {
                  // Account for the '-' sign.
                  numDigits++;
                  // Convert from 2's complement so we can count digits.
                  tempValue = ~tempValue + 1;
                }
                if (tempValue) {
                  while (tempValue) {
                    // Count the digits in the value.
                    numDigits++;
                    tempValue /= 10;
                  }
                }
                else {
                  // Zero is one digit.
                  numDigits++;
                }
                while (numDigits++ < widthSpecifier) {
                  // Pad.
                  addByte(' ');
                }
                string += 1;
              }
              else {
                decimalValueAvailable = FALSE;
              }
            }

            switch(*string) {
              case 'c':
                // character
                addByte(va_arg(args, unsigned int) & 0xFF);
                break;
              case 'p': 

              case 's': {
                // string
                int8u *arg = va_arg(args, int8u *);
                failIfNot(flushBuffer(state));
                failIfNot(printfWriteString(state, arg));
                break; }
              case 'u':         // unsigned 2-byte
              case 'd': {       // signed 2-byte
                int16u value = decimalValueAvailable
                             ? decimalValue
                             : va_arg(args, int) ;
                if (value == 0)
                  addByte('0');
                else {
                  int8u i = 0;
                  int8u numBuff[5]; // max int16u is 5 characters (65535)
                  if ( (*string == 'd') && (value & 0x8000) ) {
                    addByte('-');
                    if (value != 0x8000) {
                      value = (int16u) (- ((int16s) value));
                    }
                  }
                  while (value) {
                    numBuff[i++] = value % 10;
                    value /= 10;
                  }
                  while (i--)
                    addByte(numBuff[i] + '0');
                }
                break;
              }
              case 'h':
              case 'H': 
                // 0x prefix.
                addByte('0'); addByte('x');
                // fall through to processing of x
              case 'x':
              case 'X': {
                // single hex byte (always prints 4 chars, e.g. 0x1A)
                int8u data = va_arg(args, int);
                state->localBufferPointer = writeHex(state->localBufferPointer, data, 2);
                break; }
              case '2':
                // %2x only, 2 hex bytes (always prints 6 chars, e.g. 0x1A2B)
              case '4':
                // %4x only, 4 hex bytes (always prints 10 chars, e.g. 0x1A2B3C4D)
                string += 1;
                if (*string == 'h' || *string == 'H') {
                  // 0x prefix.
                  addByte('0'); addByte('x');
                } else if (*string != 'x' && *string != 'X') {
                  string -= 1;
                  break;
                } 
                if (*(string - 1) == '2') {
                  int16u data = va_arg(args, int);
                  state->localBufferPointer = writeHex(state->localBufferPointer, data, 4);
                } else {
                  int32u data = va_arg(args, int32u);
                  state->localBufferPointer = writeHex(state->localBufferPointer, 
                                                             (int16u) (data >> 16), 
                                                             4);
                  state->localBufferPointer = writeHex(state->localBufferPointer, 
                                                             (int16u) data, 
                                                             4);
                }
                break;
              case '\0':
                if(!processingTag) string -=1;  // allow top loop to catch it
              default:
                // unknown specifier, skip it unless..
                if(processingTag) {
                  // there was no specifier immediately after, back-out the ':'
                  state->localBufferPointer = state->localBuffer;
                  string -= 1;
                }

            }                          
        }
        if(addDelim) {
          addByte('}');
        }
      }
    }
    if (state->localBufferLimit <= state->localBufferPointer)
      failIfNot(flushBuffer(state));
  }
  
 //done:
  failIfNot(flushBuffer(state));
  return state->total;

 fail:
  return 0;
}


boolean responseHeaderPrintf(const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  if(!responseHeaderDisplay)
    return TRUE;

  state.flushHandler = serialWriteData;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 
  return status;
}

boolean responsePrintf(const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  state.flushHandler = serialWriteData;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 
  return status;
}

boolean responsePrintfFlexible(printfFlushHandler *flushapp, const char * formatString, ...)
{
  va_list ap;
  boolean status = TRUE;
  PrintfState state;

  state.flushHandler = flushapp;

  va_start (ap, formatString);
  if(0 == responsePrintfInternal(&state, formatString, ap))
    status = FALSE;
  va_end (ap); 

  return status;
}
