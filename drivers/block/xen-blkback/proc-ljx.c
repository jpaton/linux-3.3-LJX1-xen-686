/*
 * proc-ljx.c -- procfs functions for xen-blkback-ljx driver
 */

#include "proc-ljx.h"

struct proc_dir_entry *proc_file;

int procfile_read(char *buffer,
	char **buffer_location,
	off_t offset, int buffer_length, int *eof, void *data) {
	int ret;
	
	if (offset > 0) {
		ret = 0;
	} else {
		ret = sprintf(buffer, "Hello world!");
	}

	return ret;
}
