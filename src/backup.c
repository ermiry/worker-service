#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>

#include <cerver/utils/log.h>

#include <credis/crud.h>
#include <credis/redis.h>

#include "data.h"
#include "state.h"
#include "worker.h"

#include "controllers/transactions.h"

#pragma region state

static void backup_fetch_worker_state (void) {

	CredisClient *client = credis_client_get ();
	if (client) {
		redisReply *reply = (redisReply *) redisCommand (
			client->redis_context,
			"MGET worker_state "
			"worker_last_started "
			"worker_last_stopped"
		);

		if (reply) {
			// cerver_log_debug (
			// 	"backup_fetch_recon_state () - "
			// 	"Fetched %lu elements!", reply->elements
			// );
			// for (size_t idx = 0; idx < reply->elements; idx++) {
			// 	(void) printf (
			// 		"[%lu]: %d - %s\n",
			// 		idx, reply->element[idx]->type, reply->element[idx]->str
			// 	);
			// }

			int worker_state = atoi (reply->element[0]->str);
			time_t worker_last_started = (time_t) atol (reply->element[1]->str);
			time_t worker_last_stopped = (time_t) atol (reply->element[2]->str);

			service_state_restore_worker (
				worker_state,
				worker_last_started,
				worker_last_stopped
			);

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

#pragma endregion

#pragma region data

static void backup_fetch_trans_counts (void) {

	CredisClient *client = credis_client_get ();
	if (client) {
		redisReply *reply = (redisReply *) redisCommand (
			client->redis_context,
			"MGET n_trans "
			"n_bad_trans "
			"n_good_trans "
			"n_received_trans "
			"n_processed_trans"
		);

		if (reply) {
			// cerver_log_debug (
			// 	"backup_fetch_trans_counts () - "
			// 	"Fetched %lu elements!", reply->elements
			// );
			// for (size_t idx = 0; idx < reply->elements; idx++) {
			// 	(void) printf (
			// 		"[%lu]: %d - %s\n",
			// 		idx, reply->element[idx]->type, reply->element[idx]->str
			// 	);
			// }

			size_t n_trans = (size_t) atol (reply->element[0]->str);
			size_t n_bad_trans = (size_t) atol (reply->element[1]->str);
			size_t n_good_trans = (size_t) atol (reply->element[2]->str);
			size_t n_received_trans = (size_t) atol (reply->element[3]->str);
			size_t n_processed_trans = (size_t) atol (reply->element[4]->str);

			service_data_restore_trans_count (
				n_trans,
				n_bad_trans,
				n_good_trans,
				n_received_trans,
				n_processed_trans
			);

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

static void backup_fetch_waiting_time (void) {

	CredisClient *client = credis_client_get ();
	if (client) {
		redisReply *reply = (redisReply *) redisCommand (
			client->redis_context,
			"MGET n_waits "
			"min_waiting_time "
			"max_waiting_time "
			"sum_waiting_times "
			"average_waiting_time"
		);

		if (reply) {
			// cerver_log_debug (
			// 	"backup_fetch_waiting_time () - "
			// 	"Fetched %lu elements!", reply->elements
			// );
			// for (size_t idx = 0; idx < reply->elements; idx++) {
			// 	(void) printf (
			// 		"[%lu]: %d - %s\n",
			// 		idx, reply->element[idx]->type, reply->element[idx]->str
			// 	);
			// }

			size_t n_waits = (size_t) atol (reply->element[0]->str);
			double min_waiting_time = atof (reply->element[1]->str);
			double max_waiting_time = atof (reply->element[2]->str);
			double sum_waiting_times = atof (reply->element[3]->str);
			double average_waiting_time = atof (reply->element[4]->str);

			service_data_restore_waiting_time (
				n_waits,
				min_waiting_time,
				max_waiting_time,
				sum_waiting_times,
				average_waiting_time
			);

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

static void backup_fetch_process_time (void) {

	CredisClient *client = credis_client_get ();
	if (client) {
		redisReply *reply = (redisReply *) redisCommand (
			client->redis_context,
			"MGET n_processes "
			"min_process_time "
			"max_process_time "
			"sum_process_times "
			"average_process_time"
		);

		if (reply) {
			// cerver_log_debug (
			// 	"backup_fetch_process_time () - "
			// 	"Fetched %lu elements!", reply->elements
			// );
			// for (size_t idx = 0; idx < reply->elements; idx++) {
			// 	(void) printf (
			// 		"[%lu]: %d - %s\n",
			// 		idx, reply->element[idx]->type, reply->element[idx]->str
			// 	);
			// }

			size_t n_processes = (size_t) atol (reply->element[0]->str);
			double min_process_time = atof (reply->element[1]->str);
			double max_process_time = atof (reply->element[2]->str);
			double sum_process_times = atof (reply->element[3]->str);
			double average_process_time = atof (reply->element[4]->str);

			service_data_restore_process_time (
				n_processes,
				min_process_time,
				max_process_time,
				sum_process_times,
				average_process_time
			);

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

// get all trans in current_trans_in_worker_queue list
static void backup_fetch_current_trans_in_worker_queue (void) {

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

			// load each trans and push it to the queue
			for (size_t idx = 0; idx < reply->elements; idx++) {
				#ifdef SERVICE_DEBUG
				(void) printf ("[%lu]: %s\n", idx, reply->element[idx]->str);
				#endif

				// push to worker queue
				(void) worker_current_push_from_backup (
					service_trans_get_by_id (reply->element[idx]->str)
				);
			}

			#ifdef SERVICE_DEBUG
			(void) printf ("\n");
			#endif

			freeReplyObject (reply);
		}

		credis_client_return (client);
	}

}

#pragma endregion

#pragma region main

void backup_fetch_state_from_cache (void) {

	cerver_log_debug ("Loading state cache...");

	backup_fetch_worker_state ();

	cerver_log_debug ("Done fetching state from cache!");

}

void backup_fetch_data_from_cache (void) {

	cerver_log_debug ("Loading data cache...");

	/*** counts ***/
	backup_fetch_trans_counts ();

	/*** worker ***/
	// get the trans that was being processed
	// backup_fetch_current_trans_in_worker_thread ();

	// get all trans in current_trans_in_worker_queue list
	backup_fetch_current_trans_in_worker_queue ();

	/*** times ***/
	backup_fetch_waiting_time ();
	backup_fetch_process_time ();

	cerver_log_debug ("Done fetching state from cache!");

}

#pragma endregion
