#ifndef _SERVICE_ROUTES_STATE_H_
#define _SERVICE_ROUTES_STATE_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/service/state
extern void service_state_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/state/init
extern void service_state_init_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/service/state/reset
extern void service_state_reset_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif