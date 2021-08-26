#ifndef _SERVICE_VERSION_H_
#define _SERVICE_VERSION_H_

#define SERVICE_VERSION			"0.2"
#define SERVICE_VERSION_NAME	"Version 0.2"
#define SERVICE_VERSION_DATE	"26/08/2021"
#define SERVICE_VERSION_TIME	"12:39 CST"
#define SERVICE_VERSION_AUTHOR	"Erick Salas"

// print full worker version information
extern void worker_version_print_full (void);

// print the version id
extern void worker_version_print_version_id (void);

// print the version name
extern void worker_version_print_version_name (void);

#endif