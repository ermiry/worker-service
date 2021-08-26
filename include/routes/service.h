#ifndef _SERVICE_ROUTES_SERVICE_H_
#define _SERVICE_ROUTES_SERVICE_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/service
extern void service_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/version
extern void service_version_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET *
extern void service_catch_all_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif