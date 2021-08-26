#include <stdlib.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include "state.h"

// GET /api/service/state
void service_state_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	char *state_json = service_state_to_json ();

	if (state_json) {
		(void) http_response_render_json (
			http_receive, HTTP_STATUS_OK,
			state_json, strlen (state_json)
		);

		free (state_json);
	}

	else {
		(void) http_response_send (server_error, http_receive);
	}

}

// GET /api/service/state/init
void service_state_init_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	service_state_init_default_values ();

	(void) http_response_send (oki_doki, http_receive);

}

// GET /api/service/state/reset
void service_state_reset_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	service_state_reset ();

	(void) http_response_send (oki_doki, http_receive);

}
