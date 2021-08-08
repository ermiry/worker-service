#ifndef _ERMIRY_SERVICE_H_
#define _ERMIRY_SERVICE_H_

struct _HttpResponse;

extern struct _HttpResponse *worker_works;
extern struct _HttpResponse *current_version;

extern struct _HttpResponse *catch_all;

extern unsigned int worker_service_init (void);

extern void worker_service_end (void);

#endif