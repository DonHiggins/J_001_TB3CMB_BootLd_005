// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//     TimeStamp.H
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef TIMESTAMPx_H
#define TIMESTAMPx_H

#include "CanOpen.H"

//void ts_displayBuildDateTimestamp(void);

extern const Uint16 timeStamp_t1;
extern const Uint16 timeStamp_t2;
extern const Uint16 timeStamp_t3;
extern const Uint16 revision_rv1;
extern const Uint16 revision_rv2;

enum CANOPEN_STATUS ts_sendTimeStampClassic(struct CAN_COMMAND* can_command, Uint16* data);
enum CANOPEN_STATUS ts_sendDateStampClassic(struct CAN_COMMAND* can_command, Uint16* data);

#endif /* TIMESTAMPx_H */
