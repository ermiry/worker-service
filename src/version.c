#include <cerver/utils/log.h>

#include "version.h"

// print full worker version information
void worker_version_print_full (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nWorker Service Version: %s", SERVICE_VERSION_NAME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Release Date & time: %s - %s", SERVICE_VERSION_DATE, SERVICE_VERSION_TIME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Author: %s\n", SERVICE_VERSION_AUTHOR
	);

}

// print the version id
void worker_version_print_version_id (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nWorker Service Version ID: %s\n", SERVICE_VERSION
	);

}

// print the version name
void worker_version_print_version_name (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"\nWorker Service Version: %s\n", SERVICE_VERSION_NAME
	);

}
