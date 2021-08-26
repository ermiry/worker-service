#ifndef _SERVICE_WORKER_H_
#define _SERVICE_WORKER_H_

#define WORKER_WAIT_TIME			1

#define WORKER_HASH_INPUT_SIZE		256

struct _HttpCerver;

extern unsigned int worker_current_init (void);

extern void worker_current_register (
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