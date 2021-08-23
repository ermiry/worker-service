#include <cerver/handler.h>

#include <cerver/http/http.h>
#include <cerver/http/response.h>

#include "errors.h"

#include "controllers/service.h"

const char *service_error_to_string (const ServiceError type) {

	switch (type) {
		#define XX(num, name, string) case SERVICE_ERROR_##name: return #string;
		SERVICE_ERROR_MAP(XX)
		#undef XX
	}

	return service_error_to_string (SERVICE_ERROR_NONE);

}

void service_error_send_response (
	const ServiceError error,
	const HttpReceive *http_receive
) {

	switch (error) {
		case SERVICE_ERROR_NONE:
			(void) http_response_send (oki_doki, http_receive);
			break;

		case SERVICE_ERROR_BAD_REQUEST:
			(void) http_response_send (bad_request_error, http_receive);
			break;

		case SERVICE_ERROR_MISSING_VALUES:
			(void) http_response_send (missing_values, http_receive);
			break;

		case SERVICE_ERROR_BAD_USER:
			(void) http_response_send (bad_user_error, http_receive);
			break;

		case SERVICE_ERROR_NOT_FOUND:
			(void) http_response_send (not_found_error, http_receive);
			break;

		case SERVICE_ERROR_SERVER_ERROR:
			(void) http_response_send (server_error, http_receive);
			break;

		default: break;
	}

}
