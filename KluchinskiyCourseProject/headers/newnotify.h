
#ifndef __NEWNOTIFY_H
#define __NEWNOTIFY_H

#include <cstdint>
#include <sys/inotify.h>
typedef struct _rnotify Notify;


Notify*	initNotify(char** path, const uint32_t mask, const char* exclude);
int	waitNotify(Notify* ntf, char** const path, uint32_t* mask, const int timeout, uint32_t* cookie);
void freeNotify(Notify* ntf);


#endif // __NEWNOTIFY_H
