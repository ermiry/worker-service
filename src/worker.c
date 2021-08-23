#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cerver/threads/worker.h>

#include <cerver/http/http.h>

#include "controllers/transactions.h"

static Worker *worker = NULL;

static void worker_handler_method (void *data_ptr);

unsigned int worker_current_init (HttpCerver *http_cerver) {

	worker = worker_create ();
	worker_set_name (worker, "test");
	worker_set_work (worker, worker_handler_method);
	worker_set_delete_data (worker, service_trans_return);

	http_cerver_register_admin_worker (http_cerver, worker);

	return worker_start (worker);

}

unsigned int worker_current_end (void) {

	return worker_end (worker);

}

unsigned int worker_current_resume (void) {

	return worker_resume (worker);

}

unsigned int worker_current_stop (void) {

	return worker_stop (worker);

}

unsigned int worker_current_push (void *trans_ptr) {

	return worker_push_job (worker, trans_ptr);

}

unsigned int worker_current_push_from_backup (void *trans_ptr) {

	// TODO:

	return 0;

}

static void worker_handler_method (void *data_ptr) {

	Transaction *trans = (Transaction *) data_ptr;

	// TODO: do actual work

}
