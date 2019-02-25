//
//  target.h  
//
//  This header holds Common Definitions of a
//     Target Board. 
//
//
#ifndef __target_h__
#define __target_h__

#include "cardinfo.h"     //  Common Board Description Structure
#include "alias.h"        //  Board Specific Function Aliases
//
//  Aliased Prototypes
//
#ifdef __cplusplus
extern "C"
{
#undef  EXPORT
#define EXPORT _stdcall
#else
#define EXPORT EXTERN_C _stdcall
#endif 
//
//   General Functions
//
int    EXPORT target_open(int instance);
int    EXPORT target_close(int target);
LPVOID EXPORT target_cardinfo(int target);
int    EXPORT iicoffld(char *, int target, HWND hParent);
//
//  Conversion Functions
//
int    EXPORT from_ieee(float x);       
float  EXPORT to_ieee(unsigned int i);      
//
//  Interrupt Functions
//
void   EXPORT host_interrupt_install(int target, void (*virtual_isr)(void *), void * context);
void   EXPORT host_interrupt_deinstall(int target);
BOOL   EXPORT host_interrupt_enable(int target);
BOOL   EXPORT host_interrupt_disable(int target);
//
//  Low Level I/O
//
void   EXPORT target_reset(int target);
void   EXPORT target_run(int target);
void   EXPORT target_outport(int target, int port, int value);
int    EXPORT target_inport(int target, int port);
void   EXPORT target_opreg_outport(int target, int port, int value);
int    EXPORT target_opreg_inport(int target, int port);
void   EXPORT target_control(int target, int bit, int state);
//
//  Mailbox I/O Functions
//
int    EXPORT read_mailbox(int target, int box_number);
void   EXPORT write_mailbox(int target, int box_number, int value);
BOOL   EXPORT check_outbox(int target, int box_number);
BOOL   EXPORT check_inbox(int target, int box_number);
int    EXPORT read_mb_terminate(int target, int box_number, int *value, int wide);
int    EXPORT write_mb_terminate(int target, int box_number, int value, int wide);
void   EXPORT clear_mailboxes(int target);
int    EXPORT target_key(int target);
void   EXPORT target_emit(int target, int value);
void   EXPORT target_Tx(int target, int value);
int    EXPORT target_Rx(int target);
void   EXPORT get_semaphore(int target, int semaphore);
//
//  Board Specific Functions
//
void         EXPORT target_interrupt(int target);
void         EXPORT request_semaphore(int target, int semaphore);
BOOL         EXPORT own_semaphore(int target, int semaphore);
void         EXPORT release_semaphore(int target, int semaphore);
void         EXPORT mailbox_interrupt(int target, unsigned int value);
unsigned int EXPORT mailbox_interrupt_ack(int target);
//
//  Talker Functions
//
int    EXPORT target_check(int target);
void   EXPORT start_app(int target);
int    EXPORT start_talker(int target);
int    EXPORT talker_revision(int target);
int    EXPORT talker_read_memory(int target, int page, int address);
void   EXPORT talker_write_memory(int target, int page, int address, int value);
void   EXPORT talker_section(int target, int page, int address, int count);
void   EXPORT talker_launch(int target, int address);
void   EXPORT talker_resume(int target);
int    EXPORT talker_registers(int target);
void   EXPORT target_slow(int target);
void   EXPORT target_fast(int target);
void   EXPORT flash_sector_erase(int target, int sector);
void   EXPORT talker_sector_erase(int target, int sector);
void   EXPORT talker_flash_init(int target);
void   EXPORT talker_flash_offset(int target, int offset);

typedef PVOID BULK_HANDLE;

   #ifndef IN
      #define IN
   #endif

   #ifndef OUT
      #define OUT
   #endif

   #ifndef INT32
      typedef int INT32;
   #endif

// Generic operations:
int EXPORT BULK_GetNumDevices();

// Device operations:
BOOL EXPORT BULK_OpenDevice(int iDevice, IN OUT HANDLE *phDevice);
BOOL EXPORT BULK_CloseDevice(IN HANDLE hDevice);
int  EXPORT BULK_GetNumChannels( IN HANDLE hDevice);


// Pipe operations:
BOOL EXPORT BULK_OpenChannel(
                             int iDevice, 
                             WORD wChannel, 
                             BOOL fOverlapped,
                             IN OUT BULK_HANDLE *pHandle);
BOOL EXPORT BULK_CloseChannel(IN BULK_HANDLE Handle);
BOOL EXPORT BULK_Read(
                         IN BULK_HANDLE    Handle,           // handle of file to read
                         IN OUT LPVOID lpBuffer,             // pointer to buffer that receives data
                         IN DWORD dwNumberOfBytesToRead,     // number of bytes to read
                         OUT LPDWORD lpNumberOfBytesRead,    // pointer to number of bytes read
                         IN OUT LPOVERLAPPED lpOverlapped    // pointer to structure for data);
                         );
BOOL EXPORT BULK_Write(
                          IN BULK_HANDLE Handle,             // handle of file to read
                          IN LPCVOID lpBuffer,                // pointer to data to write to file
                          IN DWORD dwNumberOfBytesToWrite,    // number of bytes to write
                          OUT LPDWORD lpNumberOfBytesWritten, // pointer to number of bytes written
                          IN OUT LPOVERLAPPED lpOverlapped    // pointer to structure for overlapped I/O); 
                          );
BOOL EXPORT BULK_GetOverlappedReadResult(
                                        IN BULK_HANDLE  Handle,              // handle of file to read
                                        IN LPOVERLAPPED lpOverlapped,       // address of overlapped structure
                                        OUT LPDWORD lpNumberOfBytesTransferred,
                                        IN BOOL   bWait );

BOOL EXPORT BULK_GetOverlappedWriteResult(
                                         IN BULK_HANDLE Handle,            // handle of file to read
                                         IN LPOVERLAPPED lpOverlapped,       // address of overlapped structure
                                         OUT LPDWORD lpNumberOfBytesTransferred,
                                         IN BOOL   bWait );

BOOL EXPORT BULK_CancelIo( IN BULK_HANDLE Handle );


BOOL EXPORT STREAM_Open(     
                             int iDevice, 
                             WORD wChannel, 
                             WORD wBufferSize,
                             WORD wBlockSize,
                             IN OUT BULK_HANDLE *pHandle );

BOOL EXPORT STREAM_Close(   IN BULK_HANDLE handle );

WORD EXPORT STREAM_ReadAvailable(    IN BULK_HANDLE handle );
WORD EXPORT STREAM_WriteAvailable(   IN BULK_HANDLE handle );
WORD EXPORT STREAM_Write( IN BULK_HANDLE handle, INT32 *pBuffer, WORD wElementCount );
void EXPORT STREAM_Read( IN BULK_HANDLE handle, INT32 *pBuffer, WORD wElementCount );
void EXPORT STREAM_Flush( IN BULK_HANDLE handle );

BOOL EXPORT USB_VendorCommand(   IN HANDLE   hDevice,
                                 IN BYTE     bRequest,
                                 IN WORD     wIndex,
                                 IN WORD     wValue,
                                 IN LPCVOID  lpBuffer,
                                 IN WORD     wLength,
                                 IN BOOL     fTransferDirectionIN,
                                 OUT DWORD   *pNumberOfBytesTransferred,
                                 OUT DWORD   *pdwUSBD_Status );



#ifdef __cplusplus
}
#endif

#endif
