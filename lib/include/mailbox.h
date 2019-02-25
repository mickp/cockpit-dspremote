/*
 *     mailbox.h       --   mailbox i/o definitions
 *
 *     This version defines mailbox layouts, etc., for HPI-based machines 
 */

#ifndef  __mailbox_h__
#define  __mailbox_h__


/* 
    Mailboxes are memory mapped in the data memory region of Target memory
*/
#define Rcv			0
#define Ack			2
#define Xmt			1
#define	Req			3

/*
 *  MAILBOX structure -- 
 *  NOTE:  Not used, as PCI mailboxes are I/O mapped.
 */
typedef struct _mailbox
{
    volatile unsigned long     RCV;  /* RCV contains data transmitted from target to PC    */
    volatile unsigned long     ACK;  /* ACK signals that target has transmitted to PC (-1) */
    volatile unsigned long     XMT;  /* XMT contains data transmitted from PC to target    */
    volatile unsigned long     REQ;  /* REQ signals that PC has transmitted to target (-1) */
} MAILBOX;

#endif


