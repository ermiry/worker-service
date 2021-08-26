#include <stdlib.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include "data.h"

// GET /api/service/data
void service_data_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	char *data_json = service_data_to_json ();

	if (data_json) {
		(void) http_response_render_json (
			http_receive, HTTP_STATUS_OK,
			data_json, strlen (data_json)
		);

		free (data_json);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}

// GET /api/service/data/init
void service_data_init_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	service_data_init_default_values ();

	(void) http_response_send (oki_doki, http_receive);

}

// GET /api/service/data/reset
void service_data_reset_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	service_data_reset ();

	(void) http_response_send (oki_doki, http_receive);

}

// GET /api/service/data/worker/queue
void service_data_worker_queue_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	char *data_json = service_data_trans_in_worker_queue_to_json ();

	if (data_json) {
		(void) http_response_render_json (
			http_receive, HTTP_STATUS_OK,
			data_json, strlen (data_json)
		);

		free (data_json);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}
