#ifndef _SERVICE_ERRORS_H_
#define _SERVICE_ERRORS_H_

struct _HttpReceive;

#define SERVICE_ERROR_MAP(XX)						\
	XX(0,	NONE, 				None)				\
	XX(1,	BAD_REQUEST, 		Bad Request)		\
	XX(2,	MISSING_VALUES, 	Missing Values)		\
	XX(3,	BAD_USER, 			Bad User)			\
	XX(4,	NOT_FOUND, 			Not found)			\
	XX(5,	SERVER_ERROR, 		Server Error)

typedef enum ServiceError {

	#define XX(num, name, string) SERVICE_ERROR_##name = num,
	SERVICE_ERROR_MAP (XX)
	#undef XX

} ServiceError;

extern const char *service_error_to_string (
	const ServiceError type
);

extern void service_error_send_response (
	const ServiceError error,
	const struct _HttpReceive *http_receive
);

#endif