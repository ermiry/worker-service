#ifndef _ERMIRY_H_
#define _ERMIRY_H_

#include "runtime.h"

struct _HttpResponse;

extern RuntimeType RUNTIME;

extern unsigned int PORT;

extern unsigned int CERVER_RECEIVE_BUFFER_SIZE;
extern unsigned int CERVER_TH_THREADS;
extern unsigned int CERVER_CONNECTION_QUEUE;

// inits ermiry main values
extern unsigned int ermiry_init (void);

// ends ermiry main values
extern unsigned int ermiry_end (void);

#endif