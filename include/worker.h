#ifndef _SERVICE_WORKER_H_
#define _SERVICE_WORKER_H_

struct _HttpCerver;

extern unsigned int worker_current_init (
	struct _HttpCerver *http_cerver
);

extern unsigned int worker_current_end (void);

extern unsigned int worker_current_resume (void);

extern unsigned int worker_current_stop (void);

extern unsigned int worker_current_push (void *trans_ptr);

extern unsigned int worker_current_push_from_backup (
	void *trans_ptr
);

#endif