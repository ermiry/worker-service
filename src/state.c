#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <time.h>

#include <cerver/types/types.h>

#include <cerver/threads/worker.h>

#include <cerver/http/json/json.h>
#include <cerver/http/json/value.h>

#include <credis/crud.h>
#include <credis/redis.h>

#include "state.h"
#include "service.h"

static ServiceState *service_state = NULL;

#pragma region main

static ServiceState *service_state_create (void) {

	ServiceState *state = (ServiceState *) malloc (sizeof (ServiceState));
	if (state) {
		(void) memset (state, 0, sizeof (ServiceState));

		(void) pthread_mutex_init (&state->worker_mutex, NULL);
	}

	return state;

}

static void service_state_destroy (ServiceState *state) {

	if (state) {
		(void) pthread_mutex_destroy (&state->worker_mutex);

		free (state);
	}

}

WorkerState service_state_get_worker_state (void) {

	WorkerState result = WORKER_STATE_NONE;

	(void) pthread_mutex_lock (&service_state->worker_mutex);

	result = service_state->worker_state;

	(void) pthread_mutex_unlock (&service_state->worker_mutex);

	return result;

}

#pragma endregion

#pragma region init

void service_state_init_default_values (void) {

	(void) credis_command (
		"MSET worker_state %d "
		"worker_last_started %lu "
		"worker_last_stopped %lu",
		service_state->worker_state,
		service_state->worker_last_started,
		service_state->worker_last_stopped
	);

}

void service_state_init (const bool first_time) {

	service_state = service_state_create ();

	if (CONNECT_TO_REDIS && first_time) {
		service_state_init_default_values ();
	}

}

#pragma endregion

#pragma region reset

void service_state_reset (void) {

	(void) pthread_mutex_lock (&service_state->worker_mutex);

	service_state->worker_state = WORKER_STATE_NONE;

	service_state->worker_last_started = 0;

	service_state->worker_last_stopped = 0;

	(void) pthread_mutex_unlock (&service_state->worker_mutex);

}

#pragma endregion

#pragma region restore

void service_state_restore_worker (
	const WorkerState worker_state,
	const time_t worker_last_started,
	const time_t worker_last_stopped
) {

	service_state->worker_state = worker_state;

	service_state->worker_last_started = worker_last_started;

	service_state->worker_last_stopped = worker_last_stopped;

}

#pragma endregion

#pragma region update

void service_state_update_start_worker (void) {

	(void) pthread_mutex_lock (&service_state->worker_mutex);

	service_state->worker_state = WORKER_STATE_AVAILABLE;

	service_state->worker_last_started = time (NULL);

	if (CONNECT_TO_REDIS) {
		(void) credis_command (
			"MSET worker_state %d "
			"worker_last_started %lu",
			service_state->worker_state,
			service_state->worker_last_started
		);
	}

	(void) pthread_mutex_unlock (&service_state->worker_mutex);

}

void service_state_update_stop_worker (void) {

	(void) pthread_mutex_lock (&service_state->worker_mutex);

	service_state->worker_state = WORKER_STATE_STOPPED;

	service_state->worker_last_stopped = time (NULL);

	if (CONNECT_TO_REDIS) {
		(void) credis_command (
			"MSET worker_state %d "
			"worker_last_stopped %lu",
			service_state->worker_state,
			service_state->worker_last_stopped
		);
	}

	(void) pthread_mutex_unlock (&service_state->worker_mutex);

}

#pragma endregion

#pragma region json

char *service_state_to_json (void) {

	char *output_string = NULL;

	json_t *json = json_object ();
	if (json) {
		(void) pthread_mutex_lock (&service_state->worker_mutex);

		(void) json_object_set_new (
			json, "worker_state", json_string (worker_state_to_string (service_state->worker_state))
		);

		(void) json_object_set_new (
			json, "worker_last_started", json_integer ((json_int_t) service_state->worker_last_started)
		);

		(void) json_object_set_new (
			json, "worker_last_stopped", json_integer ((json_int_t) service_state->worker_last_stopped)
		);

		(void) pthread_mutex_unlock (&service_state->worker_mutex);

		output_string = json_dumps (json, 0);

		json_decref (json);
	}

	return output_string;

}

#pragma endregion

#pragma region end

void service_state_end (void) {

	service_state_destroy (service_state);

}

#pragma endregion
