#include <stdlib.h>

#include <time.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/utils/log.h>

#include <cmongo/crud.h>
#include <cmongo/select.h>

#include "errors.h"

#include "models/transaction.h"

#include "controllers/transactions.h"

Pool *trans_pool = NULL;

const bson_t *trans_no_user_query_opts = NULL;
static CMongoSelect *trans_no_user_select = NULL;

HttpResponse *no_user_trans = NULL;

HttpResponse *trans_created_success = NULL;
HttpResponse *trans_created_bad = NULL;
HttpResponse *trans_deleted_success = NULL;
HttpResponse *trans_deleted_bad = NULL;

void service_trans_return (void *trans_ptr);

static unsigned int service_trans_init_pool (void) {

	unsigned int retval = 1;

	trans_pool = pool_create (transaction_delete);
	if (trans_pool) {
		pool_set_create (trans_pool, transaction_new);
		pool_set_produce_if_empty (trans_pool, true);
		if (!pool_init (trans_pool, transaction_new, DEFAULT_TRANS_POOL_INIT)) {
			retval = 0;
		}

		else {
			cerver_log_error ("Failed to init trans pool!");
		}
	}

	else {
		cerver_log_error ("Failed to create trans pool!");
	}

	return retval;

}

static unsigned int service_trans_init_query_opts (void) {

	unsigned int retval = 1;

	trans_no_user_select = cmongo_select_new ();
	(void) cmongo_select_insert_field (trans_no_user_select, "title");
	(void) cmongo_select_insert_field (trans_no_user_select, "amount");
	(void) cmongo_select_insert_field (trans_no_user_select, "date");

	trans_no_user_query_opts = mongo_find_generate_opts (trans_no_user_select);

	if (trans_no_user_query_opts) retval = 0;

	return retval;

}

static unsigned int service_trans_init_responses (void) {

	unsigned int retval = 1;

	no_user_trans = http_response_json_key_value (
		HTTP_STATUS_NOT_FOUND, "msg", "Failed to get transaction(s)"
	);

	trans_created_success = http_response_json_key_value (
		HTTP_STATUS_OK, "oki", "doki"
	);

	trans_created_bad = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Failed to create transaction!"
	);

	trans_deleted_success = http_response_json_key_value (
		HTTP_STATUS_OK, "oki", "doki"
	);

	trans_deleted_bad = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Failed to delete transaction!"
	);

	if (
		no_user_trans
		&& trans_created_success && trans_created_bad
		&& trans_deleted_success && trans_deleted_bad
	) retval = 0;

	return retval;

}

unsigned int service_trans_init (void) {

	unsigned int errors = 0;

	errors |= service_trans_init_pool ();

	errors |= service_trans_init_query_opts ();

	errors |= service_trans_init_responses ();

	return errors;

}

void service_trans_end (void) {

	cmongo_select_delete (trans_no_user_select);
	bson_destroy ((bson_t *) trans_no_user_query_opts);

	pool_delete (trans_pool);
	trans_pool = NULL;

	http_response_delete (no_user_trans);

	http_response_delete (trans_created_success);
	http_response_delete (trans_created_bad);
	http_response_delete (trans_deleted_success);
	http_response_delete (trans_deleted_bad);

}

unsigned int service_trans_get_all (
	char **json, size_t *json_len
) {

	return transactions_get_all_to_json (
		trans_no_user_query_opts,
		json, json_len
	);

}

Transaction *service_trans_get_by_id (
	const String *trans_id
) {

	Transaction *trans = NULL;

	if (trans_id) {
		trans = (Transaction *) pool_pop (trans_pool);
		if (trans) {
			bson_oid_init_from_string (&trans->oid, trans_id->str);

			if (transaction_get_by_oid (
				trans,
				&trans->oid,
				NULL
			)) {
				service_trans_return (trans);
				trans = NULL;
			}
		}
	}

	return trans;

}

u8 service_trans_get_by_id_to_json (
	const char *trans_id,
	const bson_t *query_opts,
	char **json, size_t *json_len
) {

	u8 retval = 1;

	if (trans_id) {
		bson_oid_t trans_oid = { 0 };
		bson_oid_init_from_string (&trans_oid, trans_id);

		retval = transaction_get_by_oid_to_json (
			&trans_oid,
			query_opts,
			json, json_len
		);
	}

	return retval;

}

static Transaction *service_trans_create_actual (
	const char *title,
	const char *description,
	const double amount
) {

	Transaction *trans = (Transaction *) pool_pop (trans_pool);
	if (trans) {
		bson_oid_init (&trans->oid, NULL);
		bson_oid_to_string (&trans->oid, trans->id);

		if (title)
			(void) strncpy (trans->title, title, TRANSACTION_TITLE_SIZE - 1);

		if (description)
			(void) strncpy (trans->description, description, TRANSACTION_DESCRIPTION_SIZE - 1);

		trans->amount = amount;

		trans->date = time (NULL);
	}

	return trans;

}

static void service_trans_parse_json (
	json_t *json_body,
	const char **title,
	const char **description,
	double *amount
) {

	// get values from json to create a new transaction
	const char *key = NULL;
	json_t *value = NULL;
	if (json_typeof (json_body) == JSON_OBJECT) {
		json_object_foreach (json_body, key, value) {
			if (!strcmp (key, "title")) {
				*title = json_string_value (value);
				#ifdef SERVICE_DEBUG
				(void) printf ("title: \"%s\"\n", *title);
				#endif
			}

			if (!strcmp (key, "description")) {
				*description = json_string_value (value);
				#ifdef SERVICE_DEBUG
				(void) printf ("description: \"%s\"\n", *description);
				#endif
			}

			else if (!strcmp (key, "amount")) {
				*amount = json_real_value (value);
				#ifdef SERVICE_DEBUG
				(void) printf ("amount: %f\n", *amount);
				#endif
			}
		}
	}

}

static ServiceError service_trans_create_parse_json (
	Transaction **trans, const String *request_body
) {

	ServiceError error = SERVICE_ERROR_NONE;

	const char *title = NULL;
	const char *description = NULL;
	double amount = 0;

	json_error_t json_error =  { 0 };
	json_t *json_body = json_loads (request_body->str, 0, &json_error);
	if (json_body) {
		service_trans_parse_json (
			json_body,
			&title, &description, &amount
		);

		if (title) {
			*trans = service_trans_create_actual (
				title, description, amount
			);

			if (*trans == NULL) error = SERVICE_ERROR_SERVER_ERROR;
		}

		else {
			error = SERVICE_ERROR_MISSING_VALUES;
		}

		json_decref (json_body);
	}

	else {
		cerver_log_error (
			"json_loads () - json error on line %d: %s\n",
			json_error.line, json_error.text
		);

		error = SERVICE_ERROR_BAD_REQUEST;
	}

	return error;

}

ServiceError service_trans_create (
	const String *request_body
) {

	ServiceError error = SERVICE_ERROR_NONE;

	if (request_body) {
		Transaction *trans = NULL;

		error = service_trans_create_parse_json (
			&trans, request_body
		);

		if (error == SERVICE_ERROR_NONE) {
			#ifdef SERVICE_DEBUG
			transaction_print (trans);
			#endif

			if (!transaction_insert_one (trans)) {
				cerver_log_success ("Created transaction %s", trans->id);
			}

			else {
				error = SERVICE_ERROR_SERVER_ERROR;
			}

			service_trans_return (trans);
		}
	}

	else {
		#ifdef SERVICE_DEBUG
		cerver_log_error ("Missing request body to create transaction!");
		#endif

		error = SERVICE_ERROR_BAD_REQUEST;
	}

	return error;

}

static ServiceError service_trans_update_parse_json (
	Transaction *trans, const String *request_body
) {

	ServiceError error = SERVICE_ERROR_NONE;

	const char *title = NULL;
	const char *description = NULL;
	double amount = 0;
	json_error_t json_error =  { 0 };
	json_t *json_body = json_loads (request_body->str, 0, &json_error);
	if (json_body) {
		service_trans_parse_json (
			json_body,
			&title, &description, &amount
		);

		if (title)
			(void) strncpy (trans->title, title, TRANSACTION_TITLE_SIZE - 1);

		if (description)
			(void) strncpy (trans->description, description, TRANSACTION_DESCRIPTION_SIZE - 1);

		trans->amount = amount;

		json_decref (json_body);
	}

	else {
		#ifdef SERVICE_DEBUG
		cerver_log_error (
			"json_loads () - json error on line %d: %s\n",
			json_error.line, json_error.text
		);
		#endif

		error = SERVICE_ERROR_BAD_REQUEST;
	}

	return error;

}

ServiceError service_trans_update (
	const String *trans_id,
	const String *request_body
) {

	ServiceError error = SERVICE_ERROR_NONE;

	if (request_body) {
		Transaction *trans = service_trans_get_by_id (trans_id);

		if (trans) {
			// get update values
			if (service_trans_update_parse_json (
				trans, request_body
			) == SERVICE_ERROR_NONE) {
				// update the transaction in the db
				if (transaction_update_one (trans)) {
					error = SERVICE_ERROR_SERVER_ERROR;
				}
			}

			service_trans_return (trans);
		}

		else {
			#ifdef SERVICE_DEBUG
			cerver_log_error ("Failed to get matching transaction!");
			#endif

			error = SERVICE_ERROR_NOT_FOUND;
		}
	}

	else {
		#ifdef SERVICE_DEBUG
		cerver_log_error ("Missing request body to update transaction!");
		#endif

		error = SERVICE_ERROR_BAD_REQUEST;
	}

	return error;

}

ServiceError service_trans_delete (
	const String *trans_id
) {

	ServiceError error = SERVICE_ERROR_NONE;

	bson_oid_t oid = { 0 };
	bson_oid_init_from_string (&oid, trans_id->str);

	if (!transaction_delete_one_by_oid (&oid)) {
		#ifdef SERVICE_DEBUG
		cerver_log_debug ("Deleted transaction %s", trans_id->str);
		#endif
	}

	else {
		error = SERVICE_ERROR_BAD_REQUEST;
	}

	return error;

}

void service_trans_return (void *trans_ptr) {

	if (trans_ptr) {
		(void) memset (trans_ptr, 0, sizeof (Transaction));
		(void) pool_push (trans_pool, trans_ptr);
	}

}
