#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include "controllers/service.h"

// GET /api/worker
void worker_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (worker_works, http_receive);

}

// GET /api/worker/version
void worker_version_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (current_version, http_receive);

}

// GET *
void worker_catch_all_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	http_response_send (catch_all, http_receive);

}
