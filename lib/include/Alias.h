//
//  alias.h  
//
//  Board-Specific Definitions for M62 Target
//
#ifndef __alias_h__
#define __alias_h__

#define   TARGET_M62
#define   BOARD_NAME    "M62"
//
//  Board Specific Definitions
//
#define   PROM             0x300000
#define	  HEX_CONVERTER    "hex500"
#define   OUT_CONVERTER    "romfix"
#define   HASP_ADDR        0
//
//  DLL Aliases -- convert general name to board-specific
//                   "true name"
//
//   HASP Header Aliases
//
#define DSP_HASP                  M62_HASP
//
//
//   General Functions
//
#define target_open               M62_open
#define target_close              M62_close
#define target_cardinfo           M62_cardinfo
#define iicoffld                  M62_iicoffld
//
//  Conversion Functions
//
#define from_ieee                 M62_from_ieee       
#define to_ieee                   M62_to_ieee   
//
//  Interrupt Functions
//
#define host_interrupt_install    M62_host_interrupt_install
#define host_interrupt_deinstall  M62_host_interrupt_deinstall
#define host_interrupt_enable     M62_host_interrupt_enable
#define host_interrupt_disable    M62_host_interrupt_disable
//
//  Low Level I/O
//
#define target_reset              M62_reset
#define target_run                M62_run
#define target_outport            M62_outport
#define target_inport             M62_inport
#define target_opreg_outport      M62_opreg_outport
#define target_opreg_inport       M62_opreg_inport
#define target_control            M62_control
//
//  Mailbox I/O Functions
//
#define read_mailbox              M62_read_mailbox
#define write_mailbox             M62_write_mailbox
#define check_outbox              M62_check_outbox
#define check_inbox               M62_check_inbox
#define read_mb_terminate         M62_read_mb_terminate
#define write_mb_terminate        M62_write_mb_terminate
#define clear_mailboxes           M62_clear_mailboxes
#define target_key                M62_key
#define target_emit               M62_emit
#define target_Tx                 M62_Tx
#define target_Rx                 M62_Rx
#define get_semaphore             M62_get_semaphore
//
//  Board Specific Functions
//
#define target_interrupt          M62_interrupt
#define request_semaphore         M62_request_semaphore
#define own_semaphore             M62_own_semaphore
#define release_semaphore         M62_release_semaphore
#define mailbox_interrupt         M62_mailbox_interrupt
#define mailbox_interrupt_ack     M62_mailbox_interrupt_ack
//
//  Talker Functions
//
#define target_check              M62_check
#define start_app                 M62_start_app
#define start_talker              M62_start_talker
#define talker_revision           M62_talker_revision
#define talker_fetch              M62_talker_fetch
#define talker_store              M62_talker_store
#define talker_download           M62_talker_download
#define talker_launch             M62_talker_launch
#define talker_resume             M62_talker_resume
#define talker_registers          M62_talker_registers
#define target_slow               M62_slow
#define target_fast               M62_fast
#define talker_flash_sector_erase M62_flash_sector_erase
#define talker_flash_init         M62_flash_init
#define talker_flash_offset       M62_flash_offset

#endif
