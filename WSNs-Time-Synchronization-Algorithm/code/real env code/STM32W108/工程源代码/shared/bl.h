
#include PLATFORM_HEADER

#define BOOTLOAD_COMMAND_SUCCESS 0
#define BOOTLOAD_COMMAND_NACKED  1
#define BOOTLOAD_COMMAND_TIMEOUT 2

// Bootloader command codes
#define GET         0x00
#define GET_VERSION 0x01
#define GET_ID      0x02
#define ERASE       0x43
#define WRITE       0x31
#define WRITE_INCREMENTAL       0x36
#define READ        0x11
#define GO          0x21

typedef enum blStateEnum {
  BL_STATE_INIT,
  BL_STATE_INIT_OK,
  BL_STATE_GET,
  BL_STATE_GET_ID,
  BL_STATE_ERASE,
  BL_STATE_WRITE_SETUP,
  BL_STATE_WRITE,
  BL_STATE_WRITE_INCREMENTAL,
  BL_STATE_IDLE,
  BL_STATE_GO,
  BL_STATE_FAILURE
} blStateType;

typedef struct btlCommandArgStruct {
  u8 length;
  u8 *data;
} btlCommandArgType;

typedef u8 (*writeFunctionPointerType)(u8 *, u8);
typedef u8 (*readFunctionPointerType)(u8 *);

blStateType blStateMachine(u32 *bytesWritten);

void blInit (const u8* imagePtr, u32 imageSizeValue, writeFunctionPointerType writeByteFunc, readFunctionPointerType readByteFunc);
u8 bootloadCommand(u8 cmdCode, btlCommandArgType *args, u8 replyLength, u8 *reply);
