#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cerver/http/response.h>

#include <cerver/utils/utils.h>

#include "version.h"

#define VERSION_BUFFER_SIZE		64

HttpResponse *worker_works = NULL;
HttpResponse *current_version = NULL;

HttpResponse *catch_all = NULL;

unsigned int worker_service_init (void) {

	unsigned int retval = 1;

	worker_works = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Worker service works!"
	);

	char version[VERSION_BUFFER_SIZE] = { 0 };
	(void) snprintf (
		version, VERSION_BUFFER_SIZE - 1,
		"%s - %s",
		ERMIRY_VERSION_NAME, ERMIRY_VERSION_DATE
	);

	current_version = http_response_json_key_value (
		HTTP_STATUS_OK, "version", version
	);

	catch_all = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Worker service!"
	);

	if (
		worker_works && current_version
		&& catch_all
	) retval = 0;

	return retval;

}

void worker_service_end (void) {

	http_response_delete (worker_works);
	http_response_delete (current_version);

	http_response_delete (catch_all);

}
