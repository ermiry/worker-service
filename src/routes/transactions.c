#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/types/types.h>
#include <cerver/types/string.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

#include "errors.h"
#include "service.h"

#include "controllers/transactions.h"

// GET /api/worker/transactions
// get all the authenticated user's transactions
void service_transactions_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	size_t json_len = 0;
	char *json = NULL;

	if (!service_trans_get_all (
		&json, &json_len
	)) {
		if (json) {
			(void) http_response_json_custom_reference_send (
				http_receive,
				HTTP_STATUS_OK,
				json, json_len
			);

			free (json);
		}

		else {
			(void) http_response_send (no_user_trans, http_receive);
		}
	}

	else {
		(void) http_response_send (no_user_trans, http_receive);
	}

}

// POST /api/worker/transactions
// a user has requested to create a new transaction
void service_transaction_create_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	ServiceError error = service_trans_create (request->body);

	switch (error) {
		case SERVICE_ERROR_NONE: {
			// return success to user
			(void) http_response_send (
				trans_created_success,
				http_receive
			);
		} break;

		default: {
			service_error_send_response (error, http_receive);
		} break;
	}

}

// GET /api/worker/transactions/:id/info
// returns information about an existing transaction that belongs to a user
void service_transaction_get_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *trans_id = request->params[0];

	if (trans_id) {
		size_t json_len = 0;
		char *json = NULL;

		if (!service_trans_get_by_id_to_json (
			trans_id->str,
			trans_no_user_query_opts,
			&json, &json_len
		)) {
			if (json) {
				(void) http_response_json_custom_reference_send (
					http_receive, HTTP_STATUS_OK, json, json_len
				);

				free (json);
			}

			else {
				(void) http_response_send (server_error, http_receive);
			}
		}

		else {
			(void) http_response_send (no_user_trans, http_receive);
		}
	}

}

// PUT /api/worker/transactions/:id/update
// a user wants to update an existing transaction
void service_transaction_update_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	ServiceError error = service_trans_update (
		request->params[0], request->body
	);

	switch (error) {
		case SERVICE_ERROR_NONE: {
			(void) http_response_send (oki_doki, http_receive);
		} break;

		default: {
			service_error_send_response (error, http_receive);
		} break;
	}

}

// DELETE /api/worker/transactions/:id/remove
// deletes an existing user's transaction
void service_transaction_delete_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	const String *trans_id = request->params[0];

	switch (service_trans_delete (trans_id)) {
		case SERVICE_ERROR_NONE:
			(void) http_response_send (trans_deleted_success, http_receive);
			break;

		default:
			(void) http_response_send (trans_deleted_bad, http_receive);
			break;
	}

}