#ifndef __SERVICES_H
#define __SERVICES_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "system.h"

/* TM TC services*/
#define ECSS_VER_NUMBER             0
#define ECSS_DATA_FIELD_HDR_FLG     1
// never used
//#define TC_TM_SER_TC_VER            1

#define ECSS_PUS_VER            1
#define ECSS_SEC_HDR_FIELD_FLG  0

/*sequence definitions*/
/*we only support TC_TM_SEQ_SPACKET*/
#define TC_TM_SEQ_FPACKET 0x01
#define TC_TM_SEQ_CPACKET 0x00
#define TC_TM_SEQ_LPACKET 0x02
#define TC_TM_SEQ_SPACKET 0x03

/*services ack req*/
/*should confirm endianess*/
#define TC_ACK_NO           0x00
#define TC_ACK_ACC          0x01
#define TC_ACK_EXE_START    0x02
#define TC_ACK_EXE_PROG     0x04
#define TC_ACK_EXE_COMP     0x08
#define TC_ACK_ALL          0x0F

//needs to redifine
#define MAX_PKT_DATA        200  //random
#define TC_MAX_PKT_SIZE     200  //random
#define TC_MIN_PKT_SIZE     6    //random
#define MAX_PKT_EXT_DATA    2048
#define TC_MAX_PKT_EXT_SIZE 2048  //random

#define ECSS_HEADER_SIZE        6
#define ECSS_DATA_HEADER_SIZE   4
#define ECSS_CRC_SIZE           2

#define ECSS_DATA_OFFSET        ECSS_HEADER_SIZE + ECSS_DATA_HEADER_SIZE

#define MAX_APP_ID      20
#define MAX_SERVICES    20
#define MAX_SUBTYPES    26

#define TC 1
#define TM 0

#define HLDLC_START_FLAG        0x7E
#define HLDLC_CONTROL_FLAG      0x7D

typedef enum {  
    SATR_PKT_ILLEGAL_APPID     = 0,
    SATR_PKT_INV_LEN           = 1,
    SATR_PKT_INC_CRC           = 2,
    SATR_PKT_ILLEGAL_PKT_TP    = 3,
    SATR_PKT_ILLEGAL_PKT_STP   = 4,
    SATR_PKT_ILLEGAL_APP_DATA  = 5,
    SATR_OK                    = 6,
    SATR_ERROR                 = 7,
    SATR_EOT                   = 8,
    SATR_CRC_ERROR             = 9,
    SATR_PKT_ILLEGAL_ACK       = 10,
    SATR_ALREADY_SERVICING     = 11,
    SATR_MS_MAX_FILES          = 12,
    SATR_PKT_INIT              = 13,
    SATR_INV_STORE_ID          = 14,
    SATR_INV_DATA_LEN          = 15,
    /* Scheduling Service Error State Codes
    * from 
    */
    SATR_SCHEDULE_FULL         = 17, /* Schedule array is full */
    SATR_SSCH_ID_INVALID       = 18, /* Subschedule ID invalid */
    SATR_NMR_OF_TC_INVALID     = 19, /* Number of telecommands invalid */
    SATR_INTRL_ID_INVALID      = 20, /* Interlock ID invalid */
    SATR_ASS_INTRL_ID_INVALID  = 21, /* Assess Interlock ID invalid */
    SATR_ASS_TYPE_ID_INVALID   = 22, /* Assesment type id invalid*/        
    SATR_RLS_TIMET_ID_INVALID  = 23, /* Relese time type ID invalid */
    SATR_DEST_APID_INVALID     = 24, /* Destination APID in embedded TC is invalid */
    SATR_TIME_INVALID          = 25, /* Release time of TC is invalid */
    SATR_TIME_SPEC_INVALID     = 26, /* Release time of TC is specified in a invalid representation*/
    SATR_INTRL_LOGIC_ERROR     = 27, /* The release time of telecommand is in the execution window of its interlocking telecommand.*/
    SATR_SCHEDULE_DISABLED     = 28,
    /*FatFs*/
    SATRF_OK                   = 29, /* (0) Succeeded */
    SATRF_DISK_ERR             = 30, /* (1) A hard error occurred in the low level disk I/O layer */
    SATRF_INT_ERR              = 31, /* (2) Assertion failed */
    SATRF_NOT_READY            = 32, /* (3) The physical drive cannot work */
    SATRF_NO_FILE              = 33, /* (4) Could not find the file */
    SATRF_NO_PATH              = 34, /* (5) Could not find the path */
    SATRF_INVALID_NAME         = 35, /* (6) The path name format is invalid */
    SATRF_DENIED               = 36, /* (7) Access denied due to prohibited access or directory full */
    SATRF_EXIST                = 37, /* (8) Access denied due to prohibited access */
    SATRF_INVALID_OBJECT       = 38, /* (9) The file/directory object is invalid */
    SATRF_WRITE_PROTECTED      = 39, /* (10) The physical drive is write protected */
    SATRF_INVALID_DRIVE        = 40, /* (11) The logical drive number is invalid */
    SATRF_NOT_ENABLED          = 41, /* (12) The volume has no work area */
    SATRF_NO_FILESYSTEM        = 42, /* (13) There is no valid FAT volume */
    SATRF_MKFS_ABORTED         = 43, /* (14) The f_mkfs() aborted due to any parameter error */
    SATRF_TIMEOUT              = 44, /* (15) Could not get a grant to access the volume within defined period */
    SATRF_LOCKED               = 45, /* (16) The operation is rejected according to the file sharing policy */
    SATRF_NOT_ENOUGH_CORE      = 46, /* (17) LFN working buffer could not be allocated */
    SATRF_TOO_MANY_OPEN_FILES  = 47, /* (18) Number of open files > _FS_SHARE */
    SATRF_INVALID_PARAMETER    = 48, /* (19) Given parameter is invalid */
    /*LAST*/
    SATR_LAST                  = 49
}SAT_returnState;

/*services types*/
#define TC_VERIFICATION_SERVICE         1
#define TC_HOUSEKEEPING_SERVICE         3
#define TC_EVENT_SERVICE                5
#define TC_FUNCTION_MANAGEMENT_SERVICE  8
#define TC_SCHEDULING_SERVICE           11
#define TC_LARGE_DATA_SERVICE           13
#define TC_MASS_STORAGE_SERVICE         15
#define TC_TEST_SERVICE                 17

/*services subtypes*/
#define TM_VR_ACCEPTANCE_SUCCESS        1
#define TM_VR_ACCEPTANCE_FAILURE        2

#define TC_HK_REPORT_PARAMETERS         21
#define TM_HK_PARAMETERS_REPORT         23

#define TM_EV_NORMAL_REPORT         	1
#define TM_EV_ERROR_REPORT         		4

#define TC_FM_PERFORM_FUNCTION          1

#define TC_SC_ENABLE_RELEASE            1
#define TC_SC_DISABLE_RELEASE           2
#define TC_SC_RESET_SCHEDULE            3
#define TC_SC_INSERT_TC                 4
#define TC_SC_DELETE_TC                 5
#define TC_SC_TIME_SHIFT_SPECIFIC       7
#define TC_SC_TIME_SHIFT_SELECTED_OTP   8
#define TC_SC_TIME_SHIFT_ALL            15

#define TM_LD_FIRST_DOWNLINK            1
#define TC_LD_FIRST_UPLINK              9
#define TM_LD_INT_DOWNLINK              2
#define TC_LD_INT_UPLINK                10
#define TM_LD_LAST_DOWNLINK             3
#define TC_LD_LAST_UPLINK               11
#define TC_LD_ACK_DOWNLINK              5
#define TM_LD_ACK_UPLINK                14
#define TC_LD_REPEAT_DOWNLINK           6
#define TM_LD_REPEAT_UPLINK             15
#define TM_LD_REPEATED_DOWNLINK         7
#define TC_LD_REPEATED_UPLINK           12
#define TM_LD_ABORT_SE_DOWNLINK         4
#define TC_LD_ABORT_SE_UPLINK           13
#define TC_LD_ABORT_RE_DOWNLINK         8
#define TM_LD_ABORT_RE_UPLINK           16

#define TC_MS_ENABLE                    1
#define TC_MS_DISABLE                   2
#define TC_MS_CONTENT                   8
#define TC_MS_DOWNLINK                  9
#define TC_MS_DELETE                    11
#define TC_MS_REPORT                    12
#define TM_MS_CATALOGUE_REPORT          13
#define TC_MS_UPLINK                    14

#define TC_CT_PERFORM_TEST              1
#define TM_CT_REPORT_TEST               2

/*cubesat subsystem's timeouts*/
#define TIMEOUT_V_COMMS     5000
#define TIMEOUT_V_ADCS      5000
#define TIMEOUT_V_IAC       5000
#define TIMEOUT_V_EPS       5000
#define TIMEOUT_V_SU_BYTE   5000
#define TIMEOUT_V_DBG       5000

/*taken from stm32f4xx_hal_rtc.h*/
#define TM_MONTH_JANUARY              ((uint8_t)0x01U)
#define TM_MONTH_FEBRUARY             ((uint8_t)0x02U)
#define TM_MONTH_MARCH                ((uint8_t)0x03U)
#define TM_MONTH_APRIL                ((uint8_t)0x04U)
#define TM_MONTH_MAY                  ((uint8_t)0x05U)
#define TM_MONTH_JUNE                 ((uint8_t)0x06U)
#define TM_MONTH_JULY                 ((uint8_t)0x07U)
#define TM_MONTH_AUGUST               ((uint8_t)0x08U)
#define TM_MONTH_SEPTEMBER            ((uint8_t)0x09U)
#define TM_MONTH_OCTOBER              ((uint8_t)0x10U)
#define TM_MONTH_NOVEMBER             ((uint8_t)0x11U)
#define TM_MONTH_DECEMBER             ((uint8_t)0x12U)

#define UART_BUF_SIZE (MAX_PKT_DATA*2)

typedef enum {  
    OBC_APP_ID      = 1,
    EPS_APP_ID      = 2,
    ADCS_APP_ID     = 3,
    COMMS_APP_ID    = 4,
    IAC_APP_ID      = 5,
    GND_APP_ID      = 6,
    DBG_APP_ID      = 7,
    LAST_APP_ID     = 8
}TC_TM_app_id;

typedef enum {  
    HEALTH_REP      = 1,
    EX_HEALTH_REP   = 2,
    EVENTS_REP      = 3,
    WOD_REP         = 4,
    LAST_STRUCT_ID  = 5
}HK_struct_id;

typedef enum {
    P_OFF       = 0,
    P_ON        = 1,
    P_RESET     = 2,
    SET_TIME    = 3,
    LAST_FUN_ID = 4
}FM_fun_id;

typedef enum {
    OBC_DEV_ID      = 1,
    EPS_DEV_ID      = 2,
    ADCS_DEV_ID     = 3,
    COMMS_DEV_ID    = 4,
    IAC_DEV_ID      = 5,
    SU_DEV_ID       = 6,
    GPS_DEV_ID      = 7,
    OBC_SD_DEV_ID   = 8,
    ADCS_SD_DEV_ID  = 9,
    LAST_DEV_ID     = 10
}FM_dev_id;

/*Mass storage ids*/
typedef enum {  
    SU_SCRIPT_1     = 1,
    SU_SCRIPT_2     = 2,
    SU_SCRIPT_3     = 3,
    SU_SCRIPT_4     = 4,
    SU_SCRIPT_5     = 5,
    SU_SCRIPT_6     = 6,
    SU_SCRIPT_7     = 7,
    SU_LOG          = 8,
    WOD_LOG         = 9,
    EVENT_LOG       = 10,
    FOTOS           = 11,
    SCHS            = 12,
    LAST_SID        = 13
}MS_sid;

typedef enum {  
    ALL         = 0,
    TO          = 1,
    BETWEEN     = 2,
    SPECIFIC    = 3,
    LAST_PART   = 4,
    NO_MODE     = 5,
    LAST_MODE   = 6
}MS_mode;

typedef enum {  
    su_off        = 1,
    su_running    = 2,
    su_idle       = 3,
    su_finished   = 4,
    LAST_SU_STATE = 5
}SU_state;

typedef enum {  
    ev_free_1     = 1,
    ev_free_2     = 2,
    ev_wr_1       = 3,
    ev_wr_2       = 4,
    ev_owr_1      = 5,
    ev_owr_2      = 6,
    LAST_EV_STATE = 7
}EV_state;

#define C_ASSERT(e)    ((e) ? (true) : (tst_debugging((uint8_t *)__FILE__, __FILE_ID__, __LINE__, #e))) 

union _cnv {
    uint32_t cnv32;
    uint16_t cnv16[2];
    uint8_t cnv8[4];
};

extern void HAL_uart_tx(TC_TM_app_id app_id, uint8_t *buf, uint16_t size);
extern SAT_returnState event_log(uint8_t *buf, const uint16_t size);
extern SAT_returnState event_crt_pkt_api(uint8_t *buf, uint8_t *f, uint16_t fi, uint32_t l, uint8_t *e, uint16_t *size, SAT_returnState mode);

extern void cnv32_8(const uint32_t from, uint8_t *to);
extern void cnv16_8(const uint16_t from, uint8_t *to);
extern void cnv8_32(uint8_t *from, uint32_t *to);
extern void cnv8_16(uint8_t *from, uint16_t *to);

typedef struct {
    /* packet id */
    //uint8_t ver; /* 3 bits, should be equal to 0 */

    //uint8_t data_field_hdr; /* 1 bit, data_field_hdr exists in data = 1 */
    TC_TM_app_id app_id; /* TM: app id = 0 for time packets, = 0xff for idle packets. should be 11 bits only 8 are used though */
    uint8_t type; /* 1 bit, tm = 0, tc = 1 */

    /* packet sequence control */
    uint8_t seq_flags; /* 3 bits, definition in TC_SEQ_xPACKET */
    uint16_t seq_count; /* 14 bits, packet counter, should be unique for each app id */

    uint16_t len; /* 16 bits, C = (Number of octets in packet data field) - 1, on struct is the size of data without the headers. on array is with the headers */

    uint8_t ack; /* 4 bits, definition in TC_ACK_xxxx 0 if its a TM */
    uint8_t ser_type; /* 8 bit, service type */
    uint8_t ser_subtype; /* 8 bit, service subtype */

    /*optional*/
    //uint8_t pckt_sub_cnt; /* 8 bits*/
    TC_TM_app_id dest_id;   /*on TC is the source id, on TM its the destination id*/

    uint8_t *data; /* pkt data */

    /*this is not part of the header. it is used from the software and the verification service,
     *when the packet wants ack. 
     *the type is SAT_returnState and it either stores R_OK or has the error code (failure reason).
     *it is initiazed as R_ERROR and the service should be responsible to make it R_OK or put the coresponding error.     
     */
    SAT_returnState verification_state; 
/*  uint8_t padding;  x bits, padding for word alligment */

//  uint16_t crc; /* CRC or checksum, mission specific*/
}tc_tm_pkt;

/*Lookup table that returns if a service with its subtype with TC or TM is supported and valid*/
extern const uint8_t services_verification_TC_TM[MAX_SERVICES][MAX_SUBTYPES][2];

//ToDo
//  add reset counter, reset source finder.
//  add it uart
//  add assertions for pkt size related to the service type, subtype
//  check hldlc, its buggy.
//  define in unpack the MIN_PKT_SIZE and MAX_PKT_SIZE
//  need to check pkt len for not overruning to checksum
//  sort definitions relating to file system and packet sizes etc.
//  add verification steps in each service.
//  assert for 0 in modes, ids when applicable.
//  verify HK_struct_id modes
//  check that cnv functions are used correctly
//  function management set time.
//  finalize TC_MAX_PKT_SIZE
//  add event log book function
//  test assertion definition for stm
//  finish assertions
//  add assertions in each service for its subtype
//  architecture overview
//  add definitions for packet len calculations

//finished
//  update verification lookup table
//  CRC in 8bits instead of 16 but use it anyway. the high byte should be 0.
//  there is no support for verification for obc, do we need that?
//  SAT_returnState renaming to UPS_OK?
//  what to do with verification service, after route or after its service.
//  should we move all utilities functions, like pack, route etc in one big function file?
//  migrate verification on pkt status bit: add status byte in tc_tm pkt, add support for each service, make sure route works
//  when to free the packets.
//  definitions of subtypes.
//  modify route & verification.
//  add function management service.
//  add serial.
//  use cnv functions.
//  use packet len instead of individual service pack, for pack.
//  rename tc_tm.h, use it as a header only, move .c in service_utilities.
//  add seq count in pack and global memory.
//  use pkt->len for data?
//  add pack functions in each service.

struct uart_data {
    
    uint8_t uart_buf[UART_BUF_SIZE];
    uint8_t uart_unpkt_buf[UART_BUF_SIZE];
#ifdef POOL_PKT_EXT
    uint8_t deframed_buf[TC_MAX_PKT_EXT_SIZE];
#else
    uint8_t deframed_buf[TC_MAX_PKT_SIZE];
#endif
    uint8_t uart_pkted_buf[UART_BUF_SIZE];
    uint8_t framed_buf[UART_BUF_SIZE];
    uint16_t uart_size;
    
    uint32_t last_com_time;
};

struct _sys_data {
    uint8_t seq_cnt[LAST_APP_ID];
    uint8_t rsrc;
    uint32_t *boot_counter;
};

/* These values represent the time of last complete packet
 * received by the OBC subsystem.
 */
struct _subs_last_comm {
    uint32_t last_com_comms;
    uint32_t last_com_adcs;
    uint32_t last_com_iac;
    uint32_t last_com_eps;
};

struct time_utc {
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
};

extern struct _sys_data sys_data;

//stub
uint32_t time_now();

uint8_t tst_debugging(uint8_t *f, uint16_t fi, uint32_t l, uint8_t *e);

SAT_returnState sys_data_INIT();

#endif