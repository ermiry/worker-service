#ifndef _SERVICE_ROUTES_WORKER_H_
#define _SERVICE_ROUTES_WORKER_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/worker
extern void service_worker_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/worker/start
extern void service_worker_start_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/worker/stop
extern void service_worker_stop_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif