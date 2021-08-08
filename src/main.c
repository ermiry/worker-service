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

#include "ermiry.h"
#include "version.h"

#include "routes/service.h"

static Cerver *worker_service = NULL;

void end (int dummy) {
	
	if (worker_service) {
		cerver_stats_print (worker_service, false, false);
		cerver_log_msg ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) worker_service->cerver_data);
		cerver_log_line_break ();
		cerver_teardown (worker_service);
	}

	(void) ermiry_end ();

	cerver_end ();

	exit (0);

}

static void worker_set_routes (HttpCerver *http_cerver) {

	/* register top level route */
	// GET /api/worker
	HttpRoute *worker_route = http_route_create (REQUEST_METHOD_GET, "api/worker", worker_handler);
	http_cerver_route_register (http_cerver, worker_route);

	/* register worker children routes */
	// GET api/worker/version
	HttpRoute *worker_version_route = http_route_create (REQUEST_METHOD_GET, "version", worker_version_handler);
	http_route_child_add (worker_route, worker_version_route);

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

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) worker_service->cerver_data;

		worker_set_routes (http_cerver);

		// add a catch all route
		http_cerver_set_catch_all_route (http_cerver, worker_catch_all_handler);

		if (cerver_start (worker_service)) {
			cerver_log_error (
				"Failed to start %s!",
				worker_service->info->name->str
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

	if (!ermiry_init ()) {
		start ();
	}

	else {
		cerver_log_error ("Failed to init ermiry!");
	}

	(void) ermiry_end ();

	cerver_end ();

	return 0;

}
