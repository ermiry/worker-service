#ifndef _SERVICE_DATA_H_
#define _SERVICE_DATA_H_

#include <stddef.h>
#include <pthread.h>

#include <cerver/types/types.h>

#include "models/transaction.h"

#define SERVICE_DATA_MAX		65536

struct _ServiceData {

	size_t n_trans;

	size_t n_bad_trans;				// bad requests
	size_t n_good_trans;			// all checks succeded

	size_t n_received_trans;		// ready to be processed
	size_t n_processed_trans;		// processed images

	size_t current_n_trans_in_worker_queue;

	char current_trans_in_worker_thread[TRANSACTION_ID_SIZE];

	// total amount of waiting trans
	size_t n_waits;

	// work queue waiting time
	double min_waiting_time;
	double max_waiting_time;
	double sum_waiting_times;
	double average_waiting_time;

	// total amount of works
	size_t n_processes;

	// work process time
	// after queue - recon - result
	double min_process_time;
	double max_process_time;
	double sum_process_times;
	double average_process_time;

	// total amount of completed works
	size_t n_completed_processes;

	// complete process time
	double min_complete_process_time;
	double max_complete_process_time;
	double sum_complete_process_times;
	double average_complete_process_time;

	pthread_mutex_t trans_count_mutex;

	pthread_mutex_t worker_trans_mutex;

	pthread_mutex_t trans_waiting_time_mutex;
	pthread_mutex_t trans_process_time_mutex;
	pthread_mutex_t censos_completed_time_mutex;

};

typedef struct _ServiceData ServiceData;

#pragma region init

extern void service_data_init_default_values (void);

extern void service_data_init (const bool first_time);

#pragma endregion

#pragma region reset

extern void service_data_reset (void);

#pragma endregion

#pragma region restore

extern void service_data_restore_trans_count (
	const size_t n_trans,
	const size_t n_bad_trans,
	const size_t n_good_trans,
	const size_t n_received_trans,
	const size_t n_processed_trans
);

extern void service_data_restore_waiting_time (
	const size_t n_waits,
	const double min_waiting_time,
	const double max_waiting_time,
	const double sum_waiting_times,
	const double average_waiting_time
);

extern void service_data_restore_process_time (
	const size_t n_processes,
	const double min_process_time,
	const double max_process_time,
	const double sum_process_times,
	const double average_process_time
);

#pragma endregion

#pragma region update

// updates total trans count
extern void service_data_update_trans_count (void);

// updates bad trans count
extern void service_data_update_bad_trans_count (void);

// updates good trans count
extern void service_data_update_good_trans_count (void);

// updates received trans count
extern void service_data_update_received_trans_count (void);

// updates processed trans count
extern void service_data_update_processed_trans_count (void);

// a new trans has been inserted into the worker's queue
// adds one to current_n_trans_in_worker_queue
extern void service_data_add_to_current_trans_in_worker_queue (
	const char *trans_id
);

extern void service_data_add_to_current_trans_in_worker_queue_from_backup (void);

// a trans is being handled by the worker thread
// set the worker's current trans id
// subtracts one from current_n_trans_in_worker_queue
extern void service_data_set_current_worker_trans (
	const char *trans_id
);

// the worker thread is done with the current trans
// cleans current_trans_in_worker_thread
extern void service_data_clean_current_worker_trans (void);

extern void service_data_update_waiting_time (
	const double waiting_time
);

extern void service_data_update_process_time (
	const double process_time
);

#pragma endregion

#pragma region json

extern char *service_data_to_json (void);

// get all trans in current_trans_in_worker_queue list
extern char *service_data_trans_in_worker_queue_to_json (void);

#pragma endregion

#pragma region end

extern void service_data_end (void);

#pragma endregion

#endif