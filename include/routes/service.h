#ifndef _SERVICE_ROUTES_SERVICE_H_
#define _SERVICE_ROUTES_SERVICE_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/worker
extern void worker_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/worker/version
extern void worker_version_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET *
extern void worker_catch_all_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif