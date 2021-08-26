#ifndef _SERVICE_CONTROLLER_H_
#define _SERVICE_CONTROLLER_H_

struct _HttpResponse;

extern struct _HttpResponse *missing_values;

extern struct _HttpResponse *worker_works;
extern struct _HttpResponse *current_version;

extern struct _HttpResponse *catch_all;

extern unsigned int worker_service_init (void);

extern void worker_service_end (void);

#endif