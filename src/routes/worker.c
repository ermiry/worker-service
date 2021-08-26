#include <stdlib.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include "worker.h"

// GET /api/worker
void service_worker_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (oki_doki, http_receive);

}

// GET /api/worker/start
void service_worker_start_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	if (!worker_current_resume ()) {
		(void) http_response_send (oki_doki, http_receive);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}

// GET /api/worker/stop
void service_worker_stop_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	if (!worker_current_stop ()) {
		(void) http_response_send (oki_doki, http_receive);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}
