#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cerver/timer.h>

#include <cerver/threads/worker.h>

#include <cerver/http/http.h>

#include <cerver/utils/sha256.h>

#include "data.h"
#include "state.h"
#include "worker.h"

#include "controllers/transactions.h"

static Worker *worker = NULL;

static void worker_handler_method (void *data_ptr);

static unsigned int worker_current_init_internal (void) {

	unsigned int retval = 1;

	WorkerState worker_state = service_state_get_worker_state ();

	switch (worker_state) {
		case WORKER_STATE_NONE:
		case WORKER_STATE_AVAILABLE: {
			retval = worker_start_with_state (
				worker, WORKER_STATE_AVAILABLE
			);
		} break;

		case WORKER_STATE_STOPPED: {
			retval = worker_start_with_state (
				worker, WORKER_STATE_STOPPED
			);
		} break;

		default: break;
	}

	switch (worker_state) {
		case WORKER_STATE_NONE:
		case WORKER_STATE_AVAILABLE: {
			service_state_update_start_worker ();
		} break;

		default: break;
	}

	return retval;

}

unsigned int worker_current_init (void) {

	worker = worker_create ();
	worker_set_name (worker, "test");
	worker_set_work (worker, worker_handler_method);
	worker_set_delete_data (worker, service_trans_return);

	return worker_current_init_internal ();

}

void worker_current_register (HttpCerver *http_cerver) {

	http_cerver_register_admin_worker (http_cerver, worker);

}

unsigned int worker_current_end (void) {

	unsigned int retval = 1;

	retval = worker_end (worker);

	worker_delete (worker);

	return retval;

}

unsigned int worker_current_resume (void) {

	unsigned int retval = 1;

	if (!worker_resume (worker)) {
		service_state_update_start_worker ();

		retval = 0;
	}

	return retval;

}

unsigned int worker_current_stop (void) {

	unsigned int retval = 1;

	if (!worker_stop (worker)) {
		service_state_update_stop_worker ();

		retval = 0;
	}

	return retval;

}

unsigned int worker_current_push (void *trans_ptr) {

	Transaction *trans = (Transaction *) trans_ptr;

	trans->worker_time = timer_get_current_time ();

	// set worker time in transaction
	transaction_update_worker_time (&trans->oid, trans->worker_time);

	service_data_update_received_trans_count ();

	// update current trans in queue count and push to queue
	service_data_add_to_current_trans_in_worker_queue (trans->id);

	return worker_push_job (worker, trans_ptr);

}

unsigned int worker_current_push_from_backup (void *trans_ptr) {

	// Transaction *trans = (Transaction *) trans_ptr;

	service_data_add_to_current_trans_in_worker_queue_from_backup ();

	return worker_push_job (worker, trans_ptr);

}

static double worker_handler_method_actual_work (
	Transaction *trans
) {

	size_t hash_input_len = 0;
	char hash_input[WORKER_HASH_INPUT_SIZE] = { 0 };

	double start_time = timer_get_current_time ();

	hash_input_len = (size_t) snprintf (
		hash_input, WORKER_HASH_INPUT_SIZE - 1,
		"%s-%ld",
		trans->id, trans->date
	);

	#ifdef SERVICE_DEBUG
	(void) printf ("Hash input: %s\n", hash_input);
	#endif

	sha256_generate (trans->result, hash_input, hash_input_len);

	return timer_get_current_time () - start_time;

}

static void worker_handler_method (void *data_ptr) {

	Transaction *trans = (Transaction *) data_ptr;

	double start_time = timer_get_current_time ();
	double waiting_time = 0;
	double process_time = 0;
	double complete_time = 0;

	service_data_set_current_worker_trans (trans->id);

	cerver_log_success ("Got trans %s in worker!", trans->id);

	waiting_time = start_time - trans->worker_time;

	// update waiting time stats
	service_data_update_waiting_time (waiting_time);

	cerver_log_debug (
		"Trans %s waited for %fs in WORKER queue",
		trans->id, waiting_time
	);

	trans->waiting_time = waiting_time;

	// do actual work
	process_time = worker_handler_method_actual_work (trans);

	trans->process_time = process_time;

	// update service stats
	service_data_update_process_time (process_time);

	cerver_log_success (
		"Worker done with trans %s in %fs",
		trans->id, process_time
	);

	// remove current worker trans
	service_data_clean_current_worker_trans ();

	// set complete process time
	complete_time = timer_get_current_time () - start_time;

	trans->complete_time = complete_time;

	// update complete process times
	service_data_update_complete_time (complete_time);

	service_data_update_processed_trans_count ();

	// update trans values in db
	(void) transaction_update_result (trans);

	cerver_log_success (
		"Trans %s complete work took %fs", trans->id, complete_time
	);

}
