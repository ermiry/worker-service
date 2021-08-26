#ifndef _SERVICE_ROUTES_DATA_H_
#define _SERVICE_ROUTES_DATA_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/service/data
extern void service_data_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/data/init
extern void service_data_init_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/data/reset
extern void service_data_reset_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/data/worker/queue
extern void service_data_worker_queue_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif