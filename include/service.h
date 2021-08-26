#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <stdbool.h>

#include "runtime.h"

#define MONGO_URI_SIZE					256
#define MONGO_APP_NAME_SIZE				32
#define MONGO_DB_SIZE					32

#define REDIS_HOSTNAME_SIZE				128

struct _HttpResponse;

extern bool running;

extern RuntimeType RUNTIME;

extern unsigned int PORT;

extern unsigned int CERVER_RECEIVE_BUFFER_SIZE;
extern unsigned int CERVER_TH_THREADS;
extern unsigned int CERVER_CONNECTION_QUEUE;

extern bool CONNECT_TO_REDIS;
extern const char *REDIS_HOSTNAME;

// inits service main values
extern unsigned int service_init (void);

// ends service main values
extern unsigned int service_end (void);

#endif