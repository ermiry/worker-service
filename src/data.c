#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include <cerver/http/json/json.h>
#include <cerver/http/json/value.h>

#include <cerver/utils/log.h>

#include <credis/crud.h>
#include <credis/redis.h>

#include "data.h"
#include "service.h"

static ServiceData *service_data = NULL;

#pragma region main

static ServiceData *service_data_create (void) {

	ServiceData *data = (ServiceData *) malloc (sizeof (ServiceData));
	if (data) {
		(void) memset (data, 0, sizeof (ServiceData));

		data->min_waiting_time = SERVICE_DATA_MAX;

		data->min_process_time = SERVICE_DATA_MAX;

		(void) pthread_mutex_init (&data->trans_count_mutex, NULL);

		(void) pthread_mutex_init (&data->worker_trans_mutex, NULL);

		(void) pthread_mutex_init (&data->trans_waiting_time_mutex, NULL);
		(void) pthread_mutex_init (&data->trans_process_time_mutex, NULL);
	}

	return data;

}

static void service_data_destroy (ServiceData *data) {

	if (data) {
		(void) pthread_mutex_destroy (&data->trans_count_mutex);

		(void) pthread_mutex_destroy (&data->worker_trans_mutex);

		(void) pthread_mutex_destroy (&data->trans_waiting_time_mutex);
		(void) pthread_mutex_destroy (&data->trans_process_time_mutex);

		free (data);
	}

}

#pragma endregion

#pragma region init

static void service_data_init_trans_counts (void) {

	(void) credis_command (
		"MSET n_trans %lu "
		"n_bad_trans %lu "
		"n_good_trans %lu "
		"n_received_trans %lu "
		"n_processed_trans %lu ",
		service_data->n_trans,
		service_data->n_bad_trans,
		service_data->n_good_trans,
		service_data->n_received_trans,
		service_data->n_processed_trans
	);

}

static void service_data_init_waiting_time (void) {

	(unsigned int) credis_command (
		"MSET n_waits %lu "
		"min_waiting_time %f "
		"max_waiting_time %f "
		"sum_waiting_times %f "
		"average_waiting_time %f",
		service_data->n_waits,
		service_data->min_waiting_time,
		service_data->max_waiting_time,
		service_data->sum_waiting_times,
		service_data->average_waiting_time
	);

}

static void service_data_init_process_time (void) {

	(unsigned int) credis_command (
		"MSET n_processes %lu "
		"min_process_time %f "
		"max_process_time %f "
		"sum_process_times %f "
		"average_process_time %f",
		service_data->n_processes,
		service_data->min_process_time,
		service_data->max_process_time,
		service_data->sum_process_times,
		service_data->average_process_time
	);

}

static void service_data_init_current_worker_transaction (void) {

	(unsigned int) credis_command (
		"SET current_trans_in_worker_thread 0"
	);

}

void service_data_init_default_values (void) {

	service_data_init_trans_counts ();

	service_data_init_waiting_time ();

	service_data_init_process_time ();

	service_data_init_current_worker_transaction ();

}

void service_data_init (const bool first_time) {

	service_data = service_data_create ();

	if (CONNECT_TO_REDIS && first_time) {
		service_data_init_default_values ();
	}

}

#pragma endregion

#pragma region reset

static void service_data_reset_trans_count (void) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	service_data->n_trans = 0;

	service_data->n_bad_trans = 0;
	service_data->n_good_trans = 0;

	service_data->n_received_trans = 0;
	service_data->n_processed_trans = 0;

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

static void service_data_reset_current_trans_in_worker_queue (void) {

	(void) pthread_mutex_lock (&service_data->worker_trans_mutex);

	service_data->current_n_trans_in_worker_queue = 0;

	(void) memset (
		service_data->current_trans_in_worker_thread,
		0,
		TRANSACTION_ID_SIZE
	);

	(void) pthread_mutex_unlock (&service_data->worker_trans_mutex);

}

static void service_data_reset_waiting_time (void) {

	(void) pthread_mutex_lock (&service_data->trans_waiting_time_mutex);

	service_data->n_waits = 0;

	service_data->min_waiting_time = SERVICE_DATA_MAX;

	service_data->max_waiting_time = 0;

	service_data->sum_waiting_times = 0;

	service_data->average_waiting_time = 0;

	(void) pthread_mutex_unlock (&service_data->trans_waiting_time_mutex);

}

static void service_data_reset_process_time (void) {

	(void) pthread_mutex_lock (&service_data->trans_process_time_mutex);

	service_data->n_processes = 0;

	service_data->min_process_time = SERVICE_DATA_MAX;

	service_data->max_process_time = 0;

	service_data->sum_process_times = 0;

	service_data->average_process_time = 0;

	(void) pthread_mutex_unlock (&service_data->trans_process_time_mutex);

}

void service_data_reset (void) {

	service_data_reset_trans_count ();

	service_data_reset_current_trans_in_worker_queue ();

	service_data_reset_waiting_time ();

	service_data_reset_process_time ();

}

#pragma endregion

#pragma region restore

void service_data_restore_trans_count (
	const size_t n_trans,
	const size_t n_bad_trans,
	const size_t n_good_trans,
	const size_t n_received_trans,
	const size_t n_processed_trans
) {

	service_data->n_trans = n_trans;

	service_data->n_bad_trans = n_bad_trans;
	service_data->n_good_trans = n_good_trans;

	service_data->n_received_trans = n_received_trans;
	service_data->n_processed_trans = n_processed_trans;

}

void service_data_restore_waiting_time (
	const size_t n_waits,
	const double min_waiting_time,
	const double max_waiting_time,
	const double sum_waiting_times,
	const double average_waiting_time
) {

	service_data->n_waits = n_waits;

	service_data->min_waiting_time = min_waiting_time;

	service_data->max_waiting_time = max_waiting_time;

	service_data->sum_waiting_times = sum_waiting_times;

	service_data->average_waiting_time = average_waiting_time;

}

void service_data_restore_process_time (
	const size_t n_processes,
	const double min_process_time,
	const double max_process_time,
	const double sum_process_times,
	const double average_process_time
) {

	service_data->n_processes = n_processes;

	service_data->min_process_time = min_process_time;

	service_data->max_process_time = max_process_time;

	service_data->sum_process_times = sum_process_times;

	service_data->average_process_time = average_process_time;

}

#pragma endregion

#pragma region update

// updates total trans count
void service_data_update_trans_count (void) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	service_data->n_trans += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large ("n_trans", service_data->n_trans);
	}

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

// updates bad trans count
void service_data_update_bad_trans_count (void) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	service_data->n_bad_trans += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"n_bad_trans",
			service_data->n_bad_trans
		);
	}

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

// updates good trans count
void service_data_update_good_trans_count (void) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	service_data->n_good_trans += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"n_good_trans",
			service_data->n_good_trans
		);
	}

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

// updates received trans count
void service_data_update_received_trans_count (void) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	service_data->n_received_trans += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"n_received_trans",
			service_data->n_received_trans
		);
	}

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

// updates processed trans count
void service_data_update_processed_trans_count (void) {

	service_data->n_processed_trans += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"n_processed_trans",
			service_data->n_processed_trans
		);
	}

}

// a new trans has been inserted into the worker's queue
// adds one to current_n_trans_in_worker_queue
void service_data_add_to_current_trans_in_worker_queue (
	const char *trans_id
) {

	(void) pthread_mutex_lock (&service_data->worker_trans_mutex);

	service_data->current_n_trans_in_worker_queue += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"current_n_trans_in_worker_queue",
			service_data->current_n_trans_in_worker_queue
		);

		(void) credis_right_push (
			"current_trans_in_worker_queue", trans_id
		);
	}

	(void) pthread_mutex_unlock (&service_data->worker_trans_mutex);

}

void service_data_add_to_current_trans_in_worker_queue_from_backup (void) {

	(void) pthread_mutex_lock (&service_data->worker_trans_mutex);

	service_data->current_n_trans_in_worker_queue += 1;

	if (CONNECT_TO_REDIS) {
		(void) credis_set_large (
			"current_n_trans_in_worker_queue",
			service_data->current_n_trans_in_worker_queue
		);
	}

	(void) pthread_mutex_unlock (&service_data->worker_trans_mutex);

}

// a trans is being handled by the worker thread
// set the worker's current trans id
// subtracts one from current_n_trans_in_worker_queue
void service_data_set_current_worker_trans (
	const char *trans_id
) {

	(void) pthread_mutex_lock (&service_data->worker_trans_mutex);

	service_data->current_n_trans_in_worker_queue -= 1;

	(void) strncpy (
		service_data->current_trans_in_worker_thread,
		trans_id,
		TRANSACTION_ID_SIZE - 1
	);

	if (CONNECT_TO_REDIS) {
		// (void) credis_command (
		// 	"MSET current_n_trans_in_worker_queue %lu "
		// 	"current_trans_in_worker_thread %s",
		// 	service_data->current_n_trans_in_worker_queue,
		// 	service_data->current_trans_in_worker_thread
		// );

		(void) credis_set_large (
			"current_n_trans_in_worker_queue",
			service_data->current_n_trans_in_worker_queue
		);

		(void) credis_list_remove (
			"current_trans_in_worker_queue", 1, trans_id
		);

		(unsigned int) credis_command (
			"SET current_trans_in_worker_thread %s",
			trans_id
		);
	}

	(void) pthread_mutex_unlock (&service_data->worker_trans_mutex);

}

// the worker thread is done with the current trans
// cleans current_trans_in_worker_thread
void service_data_clean_current_worker_trans (void) {

	(void) memset (
		service_data->current_trans_in_worker_thread,
		0,
		TRANSACTION_ID_SIZE
	);

	if (CONNECT_TO_REDIS) {
		(unsigned int) credis_command (
			"SET current_trans_in_worker_thread 0"
		);
	}

}

void service_data_update_waiting_time (
	const double waiting_time
) {

	if (waiting_time > 0) {
		(void) pthread_mutex_lock (&service_data->trans_waiting_time_mutex);

		service_data->n_waits += 1;

		if (waiting_time < service_data->min_waiting_time)
			service_data->min_waiting_time = waiting_time;

		if (waiting_time > service_data->max_waiting_time)
			service_data->max_waiting_time = waiting_time;

		service_data->sum_waiting_times += waiting_time;
		service_data->average_waiting_time = (
			service_data->sum_waiting_times / service_data->n_waits
		);

		if (CONNECT_TO_REDIS) {
			(void) credis_command (
				"MSET n_waits %lu "
				"min_waiting_time %f "
				"max_waiting_time %f "
				"sum_waiting_times %f "
				"average_waiting_time %f",
				service_data->n_waits,
				service_data->min_waiting_time,
				service_data->max_waiting_time,
				service_data->sum_waiting_times,
				service_data->average_waiting_time
			);
		}

		(void) pthread_mutex_unlock (&service_data->trans_waiting_time_mutex);
	}

}

void service_data_update_process_time (
	const double process_time
) {

	if (process_time > 0) {
		(void) pthread_mutex_lock (&service_data->trans_process_time_mutex);

		service_data->n_processes += 1;

		if (process_time < service_data->min_process_time)
			service_data->min_process_time = process_time;

		if (process_time > service_data->max_process_time)
			service_data->max_process_time = process_time;

		service_data->sum_process_times += process_time;
		service_data->average_process_time = (
			service_data->sum_process_times / service_data->n_processes
		);

		if (CONNECT_TO_REDIS) {
			(void) credis_command (
				"MSET n_processes %lu "
				"min_process_time %f "
				"max_process_time %f "
				"sum_process_times %f "
				"average_process_time %f",
				service_data->n_processes,
				service_data->min_process_time,
				service_data->max_process_time,
				service_data->sum_process_times,
				service_data->average_process_time
			);
		}

		(void) pthread_mutex_unlock (&service_data->trans_process_time_mutex);
	}

}

#pragma endregion

#pragma region json

static void service_data_trans_count_to_json (json_t *json) {

	(void) pthread_mutex_lock (&service_data->trans_count_mutex);

	(void) json_object_set_new (
		json, "n_trans", json_integer ((json_int_t) service_data->n_trans)
	);

	(void) json_object_set_new (
		json, "n_bad_trans", json_integer ((json_int_t) service_data->n_bad_trans)
	);

	(void) json_object_set_new (
		json, "n_good_trans", json_integer ((json_int_t) service_data->n_good_trans)
	);

	(void) json_object_set_new (
		json, "n_received_trans", json_integer ((json_int_t) service_data->n_received_trans)
	);

	(void) json_object_set_new (
		json, "n_processed_trans", json_integer ((json_int_t) service_data->n_processed_trans)
	);

	(void) pthread_mutex_unlock (&service_data->trans_count_mutex);

}

static void service_data_waiting_time_to_json (json_t *json) {

	(void) pthread_mutex_lock (&service_data->trans_waiting_time_mutex);

	(void) json_object_set_new (
		json, "n_waits", json_integer ((json_int_t) service_data->n_waits)
	);

	(void) json_object_set_new (
		json, "min_waiting_time", json_real (service_data->min_waiting_time)
	);

	(void) json_object_set_new (
		json, "max_waiting_time", json_real (service_data->max_waiting_time)
	);

	(void) json_object_set_new (
		json, "sum_waiting_times", json_real (service_data->sum_waiting_times)
	);

	(void) json_object_set_new (
		json, "average_waiting_time", json_real (service_data->average_waiting_time)
	);

	(void) pthread_mutex_unlock (&service_data->trans_waiting_time_mutex);

}

static void service_data_process_time_to_json (json_t *json) {

	(void) pthread_mutex_lock (&service_data->trans_process_time_mutex);

	(void) json_object_set_new (
		json, "n_processes", json_integer ((json_int_t) service_data->n_processes)
	);

	(void) json_object_set_new (
		json, "min_process_time", json_real (service_data->min_process_time)
	);

	(void) json_object_set_new (
		json, "max_process_time", json_real (service_data->max_process_time)
	);

	(void) json_object_set_new (
		json, "sum_process_times", json_real (service_data->sum_process_times)
	);

	(void) json_object_set_new (
		json, "average_process_time", json_real (service_data->average_process_time)
	);

	(void) pthread_mutex_unlock (&service_data->trans_process_time_mutex);

}

char *service_data_to_json (void) {

	char *json_string = NULL;

	json_t *json = json_object ();
	if (json) {
		service_data_trans_count_to_json (json);

		service_data_waiting_time_to_json (json);

		service_data_process_time_to_json (json);

		json_string = json_dumps (json, 0);

		json_decref (json);
	}

	return json_string;

}

static void service_data_trans_in_worker_queue_to_json_internal (
	json_t *trans_array
) {

	CredisClient *client = credis_client_get ();
	if (client) {
		redisReply *reply = (redisReply *) redisCommand (
			client->redis_context,
			"LRANGE current_trans_in_worker_queue 0 -1"
		);

		if (reply) {
			cerver_log_success (
				"Fetched %lu elements from current_trans_in_worker_queue!",
				reply->elements
			);

			for (size_t idx = 0; idx < reply->elements; idx++) {
				// #ifdef SERVICE_DEBUG
				// (void) printf ("[%lu]: %s\n", idx, reply->element[idx]->str);
				// #endif

				(void) json_array_append_new (
					trans_array, json_string (reply->element[idx]->str)
				);
			}

			// #ifdef SERVICE_DEBUG
			// (void) printf ("\n");
			// #endif

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

// get all trans in current_trans_in_worker_queue list
char *service_data_trans_in_worker_queue_to_json (void) {

	char *json_string = NULL;

	json_t *json = json_object ();
	if (json) {
		json_t *trans_array = json_array ();

		if (CONNECT_TO_REDIS) {
			service_data_trans_in_worker_queue_to_json_internal (
				trans_array
			);
		}

		(void) json_object_set_new (
			json, "trans", trans_array
		);

		json_string = json_dumps (json, 0);

		json_decref (json);
	}

	return json_string;

}

#pragma endregion

#pragma region end

void service_data_end (void) {

	service_data_destroy (service_data);

}

#pragma endregion
