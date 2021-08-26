#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <signal.h>

#include <cerver/cerver.h>
#include <cerver/version.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

#include "service.h"
#include "version.h"
#include "worker.h"

#include "routes/data.h"
#include "routes/service.h"
#include "routes/state.h"
#include "routes/transactions.h"
#include "routes/worker.h"

static Cerver *worker_service = NULL;

void end (int dummy) {
	
	if (worker_service) {
		cerver_stats_print (worker_service, false, false);
		cerver_log_msg ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) worker_service->cerver_data);
		cerver_log_line_break ();
		cerver_teardown (worker_service);
	}

	(void) service_end ();

	cerver_end ();

	exit (0);

}

static void service_set_main_routes (HttpCerver *http_cerver) {

	/* register top level route */
	// GET /api/service
	HttpRoute *service_route = http_route_create (REQUEST_METHOD_GET, "api/service", service_handler);
	http_cerver_route_register (http_cerver, service_route);

	/* register service children routes */
	// GET /api/service/version
	HttpRoute *service_version_route = http_route_create (REQUEST_METHOD_GET, "version", service_version_handler);
	http_route_child_add (service_route, service_version_route);

	/*** state ***/

	// GET /api/service/state
	HttpRoute *state_route = http_route_create (REQUEST_METHOD_GET, "state", service_state_handler);
	http_route_child_add (service_route, state_route);

	HttpRoute *state_init_route = http_route_create (REQUEST_METHOD_GET, "state/init", service_state_init_handler);
	http_route_child_add (service_route, state_init_route);

	HttpRoute *state_reset_route = http_route_create (REQUEST_METHOD_GET, "state/reset", service_state_reset_handler);
	http_route_child_add (service_route, state_reset_route);

	/*** data ***/
	
	// GET /api/service/data
	HttpRoute *data_route = http_route_create (REQUEST_METHOD_GET, "data", service_data_handler);
	http_route_child_add (service_route, data_route);

	// GET /api/service/data/init
	HttpRoute *data_init_route = http_route_create (REQUEST_METHOD_GET, "data/init", service_data_init_handler);
	http_route_child_add (service_route, data_init_route);

	// GET /api/service/data/reset
	HttpRoute *data_reset_route = http_route_create (REQUEST_METHOD_GET, "data/reset", service_data_reset_handler);
	http_route_child_add (service_route, data_reset_route);

	// GET /api/service/data/worker/queue
	HttpRoute *data_queue_route = http_route_create (REQUEST_METHOD_GET, "data/worker/queue", service_data_worker_queue_handler);
	http_route_child_add (service_route, data_queue_route);

}

static void service_set_transactions_routes (HttpCerver *http_cerver) {

	// GET /api/service/transactions
	HttpRoute *transactions_route = http_route_create (REQUEST_METHOD_GET, "api/transactions", service_transactions_handler);
	http_cerver_route_register (http_cerver, transactions_route);

	// POST /api/service/transactions
	http_route_set_handler (transactions_route, REQUEST_METHOD_POST, service_transaction_create_handler);

	// GET /api/service/transactions/:id/info
	HttpRoute *trans_info_route = http_route_create (REQUEST_METHOD_GET, ":id/info", service_transaction_get_handler);
	http_route_child_add (transactions_route, trans_info_route);

	// PUT /api/service/transactions/:id/update
	HttpRoute *trans_update_route = http_route_create (REQUEST_METHOD_PUT, ":id/update", service_transaction_update_handler);
	http_route_child_add (transactions_route, trans_update_route);

	// DELETE /api/service/transactions/:id/remove
	HttpRoute *trans_delete_route = http_route_create (REQUEST_METHOD_DELETE, ":id/remove", service_transaction_delete_handler);
	http_route_child_add (transactions_route, trans_delete_route);

}

static void service_set_worker_routes (HttpCerver *http_cerver) {

	// GET /api/worker
	HttpRoute *worker_route = http_route_create (REQUEST_METHOD_GET, "api/worker", service_worker_handler);
	http_cerver_route_register (http_cerver, worker_route);

	// GET /api/worker/start
	HttpRoute *worker_start_route = http_route_create (REQUEST_METHOD_GET, "start", service_worker_start_handler);
	http_route_child_add (worker_route, worker_start_route);

	// GET /api/worker/stop
	HttpRoute *worker_stop_route = http_route_create (REQUEST_METHOD_GET, "stop", service_worker_stop_handler);
	http_route_child_add (worker_route, worker_stop_route);

}

static void start (void) {

	worker_service = cerver_create (
		CERVER_TYPE_WEB,
		"worker-service",
		PORT,
		PROTOCOL_TCP,
		false,
		CERVER_CONNECTION_QUEUE
	);

	if (worker_service) {
		/*** cerver configuration ***/
		cerver_set_alias (worker_service, "worker");

		cerver_set_receive_buffer_size (worker_service, CERVER_RECEIVE_BUFFER_SIZE);
		cerver_set_thpool_n_threads (worker_service, CERVER_TH_THREADS);
		cerver_set_handler_type (worker_service, CERVER_HANDLER_TYPE_THREADS);

		cerver_set_reusable_address_flags (worker_service, true);

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) worker_service->cerver_data;

		service_set_main_routes (http_cerver);

		service_set_transactions_routes (http_cerver);

		service_set_worker_routes (http_cerver);

		// add a catch all route
		http_cerver_set_catch_all_route (http_cerver, service_catch_all_handler);

		// admin configuration
		http_cerver_enable_admin_routes (http_cerver, true);

		worker_current_register (http_cerver);

		if (cerver_start (worker_service)) {
			cerver_log_error (
				"Failed to start %s!",
				worker_service->info->name
			);

			cerver_delete (worker_service);
		}
	}

	else {
		cerver_log_error ("Failed to create cerver!");

		cerver_delete (worker_service);
	}

}

int main (int argc, char const **argv) {

	srand (time (NULL));

	// register to the quit signal
	(void) signal (SIGINT, end);
	(void) signal (SIGTERM, end);

	// to prevent SIGPIPE when writting to socket
	(void) signal (SIGPIPE, SIG_IGN);

	cerver_init ();

	cerver_version_print_full ();

	worker_version_print_full ();

	if (!service_init ()) {
		start ();
	}

	else {
		cerver_log_error ("Failed to init service!");
	}

	(void) service_end ();

	cerver_end ();

	return 0;

}
