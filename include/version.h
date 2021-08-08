#ifndef _ERMIRY_VERSION_H_
#define _ERMIRY_VERSION_H_

#define ERMIRY_VERSION			"0.1"
#define ERMIRY_VERSION_NAME		"Version 0.1"
#define ERMIRY_VERSION_DATE		"07/08/2021"
#define ERMIRY_VERSION_TIME		"23:13 CST"
#define ERMIRY_VERSION_AUTHOR	"Erick Salas"

// print full worker version information
extern void worker_version_print_full (void);

// print the version id
extern void worker_version_print_version_id (void);

// print the version name
extern void worker_version_print_version_name (void);

#endif