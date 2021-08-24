#ifndef _SERVICE_STATE_H_
#define _SERVICE_STATE_H_

#include <pthread.h>
#include <time.h>

#include <cerver/threads/worker.h>

struct _ServiceState {

	WorkerState worker_state;
	time_t worker_last_started;
	time_t worker_last_stopped;

	pthread_mutex_t worker_mutex;

};

typedef struct _ServiceState ServiceState;

extern WorkerState service_state_get_worker_state (void);

#pragma region init

extern void service_state_init_default_values (void);

extern void service_state_init (const bool first_time);

#pragma endregion

#pragma region reset

extern void service_state_reset (void);

#pragma endregion

#pragma region restore

extern void service_state_restore_worker (
	const WorkerState worker_state,
	const time_t worker_last_started,
	const time_t worker_last_stopped
);

#pragma endregion

#pragma region update

extern void service_state_update_start_worker (void);

extern void service_state_update_stop_worker (void);

#pragma endregion

#pragma region json

extern char *service_state_to_json (void);

#pragma endregion

#pragma region end

extern void service_state_end (void);

#pragma endregion

#endif