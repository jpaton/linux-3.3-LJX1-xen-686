#ifndef _PROC_LJX_H
#define _PROC_LJX_H

#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define procfs_name "blkback-ljx"

extern struct proc_dir_entry *proc_file;

extern void ljx_init(void);

#endif
