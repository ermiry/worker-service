#ifndef _SERVICE_ROUTES_TRANSACTIONS_H_
#define _SERVICE_ROUTES_TRANSACTIONS_H_

struct _HttpReceive;
struct _HttpResponse;

// GET /api/transactions
// get all the authenticated user's transactions
extern void service_transactions_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// POST /api/transactions
// a user has requested to create a new transaction
extern void service_transaction_create_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/transactions/:id/info
// returns information about an existing transaction that belongs to a user
extern void service_transaction_get_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// PUT /api/transactions/:id/update
// a user wants to update an existing transaction
extern void service_transaction_update_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// DELETE /api/transactions/:id/remove
// deletes an existing user's transaction
extern void service_transaction_delete_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif