#ifndef _ROSHELPER_H_
#define _ROSHELPER_H_

typedef struct RosPub rospub_t;
extern rospub_t* RosPubInit(void);
extern void RosPubPublish(rospub_t* _s, char* buffer);
extern void RosPubCleanup(rospub_t* _s);

#endif
