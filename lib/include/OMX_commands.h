#define PKTSIZE 4096

// Commands

//#define READ_POSITION	 0x01
#define WRITE_SHUTTER	 0x02
#define READ_SHUTTER	 0x03
#define TRIGGER_SPEC	 0x04
//#define SET_BASE		 0x05
//#define SET_AMPLITUDE	 0x06
//#define MOVE_ABSOLUTE	 0x07
#define DOWNLOAD_PROFILE 0x08
#define INIT_PROFILE	 0x09
#define COLLECT			 0x0a
#define EXPOSE			 0x0b
//neverUsed #define MOVE_RELATIVE	 0x0c
#define FLASH			 0x0d
#define READ_DIGITAL	 0x0e
#define WRITE_DIGITAL	 0x0f
#define ABORT			 0x10
#define POST_STATUS		 0x11
#define INIT_DMA		 0x12
#define READ_ACTUAL		 0x13

#define SET_BASE_0		 0x20
#define SET_BASE_1		 0x21
#define SET_BASE_2		 0x22
#define SET_BASE_3		 0x23
//20060829  #define SET_AMPLITUDE_0	 0x24
//20060829  #define SET_AMPLITUDE_1	 0x25
//20060829  #define SET_AMPLITUDE_2	 0x26
//20060829  #define SET_AMPLITUDE_3	 0x27
#define MOVE_ABSOLUTE_0	 0x28
#define MOVE_ABSOLUTE_1	 0x29
#define MOVE_ABSOLUTE_2	 0x2a
#define MOVE_ABSOLUTE_3	 0x2b
#define READ_POSITION_0	 0x30
#define READ_POSITION_1	 0x31
#define READ_POSITION_2	 0x32
#define READ_POSITION_3	 0x33
#define READ_ACTUAL_0	 0x34
#define READ_ACTUAL_1	 0x35
#define READ_ACTUAL_2	 0x36
#define READ_ACTUAL_3	 0x37

#define UPDATE_NREPS	 0x41

#define UNKNOWN_COMMAND   0xfefe

#define DONE    0x444F4E45
#define READY   0x52454459
#define NEXT    0x4E455854


/* OLD - OBSOLETE
#define TIMO    0x54494D4F
#define HERE    0x48455245
#define ERR	    0x455252     // only used for "not ready"


#define READ_POSITION	0x5250
#define WRITE_SHUTTER	0x5753
#define READ_SHUTTER	0x5253
#define TRIGGER_SPEC	0x5453
#define SET_BASE		0x5342
#define SET_AMPLITUDE	0x5341
#define MOVE_ABSOLUTE	0x4D41
#define DOWNLOAD_PROFILE	0x4450
#define INIT_PROFILE	0x4950
#define COLLECT			0x474F
#define EXPOSE			0x4558
#define MOVE_RELATIVE	0x4D52
#define FLASH			0x464C
#define READ_DIGITAL	0x5244
#define WRITE_DIGITAL	0x5744
#define ABORT			0x4142
#define POST_STATUS		0x5053
#define INIT_DMA		0x4944
#define READ_ACTUAL		0x5241



*/
