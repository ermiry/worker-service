#ifndef _MODELS_TRANSACTION_H_
#define _MODELS_TRANSACTION_H_

#include <time.h>

#include <mongoc/mongoc.h>
#include <bson/bson.h>

#define TRANSACTIONS_COLL_NAME         	"transactions"

#define	TRANSACTION_ID_SIZE				32
#define TRANSACTION_TITLE_SIZE			512
#define TRANSACTION_DESCRIPTION_SIZE	1024
#define TRANSACTION_RESULT_SIZE			128

extern unsigned int transactions_model_init (void);

extern void transactions_model_end (void);

#define TRANS_TYPE_MAP(XX)					\
	XX(0,	NONE, 		None)				\
	XX(1,	SINGLE, 	Single)				\
	XX(2,	RECURRENT, 	Recurrent)

typedef enum TransType {

	#define XX(num, name, string) TRANS_TYPE_##name = num,
	TRANS_TYPE_MAP (XX)
	#undef XX

} TransType;

extern const char *trans_type_to_string (const TransType type);

typedef struct Transaction {

	// transaction's unique id
	bson_oid_t oid;
	char id[TRANSACTION_ID_SIZE];

	TransType type;

	// the name of the transaction
	char title[TRANSACTION_TITLE_SIZE];
	
	// an optional description
	char description[TRANSACTION_DESCRIPTION_SIZE];

	// the actual value of the transaction
	double amount;

	// when the transaction was made
	time_t date;

	char result[TRANSACTION_RESULT_SIZE];

	double start_time;		// when the transaction was uploaded
	double worker_time;		// when pushed to worker queue
	double waiting_time;	// how long it waited on the queue
	double process_time;	// how long the actual work took
	double complete_time;	// from the start to the end of the work

} Transaction;

extern void *transaction_new (void);

extern void transaction_delete (void *transaction_ptr);

extern void transaction_print (
	const Transaction *transaction
);

extern unsigned int transaction_get_by_oid (
	Transaction *trans,
	const bson_oid_t *oid,
	const bson_t *query_opts
);

extern unsigned int transaction_get_by_oid_to_json (
	const bson_oid_t *oid,
	const bson_t *query_opts,
	char **json, size_t *json_len
);

// get all the transactions that are related to a user
extern mongoc_cursor_t *transactions_get_all_by_user (
	const bson_oid_t *user_oid, const bson_t *opts
);

extern unsigned int transactions_get_all_to_json (
	const bson_t *opts,
	char **json, size_t *json_len
);

extern unsigned int transaction_insert_one (
	const Transaction *transaction
);

extern unsigned int transaction_update_one (
	const Transaction *transaction
);

extern unsigned int transaction_update_worker_time (
	const bson_oid_t *trans_oid, const double worker_time
);

extern unsigned int transaction_update_result (
	const Transaction *trans
);

extern unsigned int transaction_delete_one_by_oid (
	const bson_oid_t *oid
);

#endif