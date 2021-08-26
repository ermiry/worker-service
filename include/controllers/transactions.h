#ifndef _SERVICE_TRANSACTIONS_H_
#define _SERVICE_TRANSACTIONS_H_

#include <bson/bson.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include "errors.h"

#include "models/transaction.h"

#define DEFAULT_TRANS_POOL_INIT			32

struct _HttpResponse;

extern Pool *trans_pool;

extern const bson_t *trans_no_user_query_opts;

extern struct _HttpResponse *no_user_trans;

extern struct _HttpResponse *trans_created_success;
extern struct _HttpResponse *trans_created_bad;
extern struct _HttpResponse *trans_deleted_success;
extern struct _HttpResponse *trans_deleted_bad;

extern unsigned int service_trans_init (void);

extern void service_trans_end (void);

extern Transaction *service_trans_get (void);

extern unsigned int service_trans_get_all (
	char **json, size_t *json_len
);

extern Transaction *service_trans_get_by_id (
	const char *trans_id
);

extern u8 service_trans_get_by_id_to_json (
	const char *trans_id,
	const bson_t *query_opts,
	char **json, size_t *json_len
);

extern ServiceError service_trans_create (
	const String *request_body
);

extern ServiceError service_trans_update (
	const String *trans_id,
	const String *request_body
);

extern ServiceError service_trans_delete (
	const String *trans_id
);

extern void service_trans_return (void *trans_ptr);

#endif