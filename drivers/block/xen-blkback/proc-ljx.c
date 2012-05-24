/*
 * proc-ljx.c -- procfs functions for xen-blkback-ljx driver
 */

#include <linux/seq_file.h>
#include <linux/list.h>

#include "proc-ljx.h"

#define DATA_SIZE 1024

struct proc_dir_entry *proc_file;
static loff_t offset = 0; 
static LIST_HEAD(buffer_list);

struct buffer {
	struct list_head list;
	char *buf;
	loff_t len;
};

struct user_pos {
	struct buffer *buf;
	loff_t offset;
};

int ljx_print(const char *fmt, ...) {
	/*va_list args;
	int written;
	char buf[DATA_SIZE];
	char * tail;

	va_start(args, fmt);
	str_size = scnprintf(buf, DATA_SIZE, fmt, args);
	va_end(args);

	return size;
	*/
	return 0;
}

/**
 * find_nth_buffer - Find the nth buffer
 * @n: Which buffer you want (0-indexed)
 *
 * Returns the nth buffer, or null if there weren't that many buffers
 */
static struct buffer *find_nth_buffer(int n) {
	struct buffer *ret;
	struct list_head *cur;
	int i = 0;
	list_for_each(cur, &buffer_list) {
		ret = list_entry(cur, struct buffer, list);
		if (i == n)
			return ret;
		i++;
	}
	return NULL;
}

static void *ljx_seq_start(struct seq_file *s, loff_t *pos) {
	struct user_pos *upos;
	
	upos = kmalloc(sizeof(struct user_pos), GFP_KERNEL);
	if (!upos)
		return NULL;
	upos->buf = list_entry(buffer_list.next, struct buffer, list);
	upos->offset = 0;

	return upos;
}

static void *ljx_seq_next(struct seq_file *s, void *v, loff_t *pos) {
	struct user_pos *upos;
	size_t buf_num;
	loff_t byte_in_buf;
       
	upos = (struct user_pos *) v;
	byte_in_buf = upos->offset % DATA_SIZE;

	if (byte_in_buf + 1 >= DATA_SIZE) {
		/* must move to next buf */
		upos->buf = list_entry(upos->buf->list.next, struct buffer, list);
	}

	upos->offset++;
	return upos;
}

static int ljx_seq_show(struct seq_file *s, void *v) {
	// TODO
	//loff_t *spos = (loff_t *) v;
	return 0;
}

static struct buffer *init_buf(void) {
	struct buffer *ret = kmalloc(sizeof(char) * DATA_SIZE, 0);
	ret->len	= 0,
	ret->buf	= (char *)kmalloc(sizeof(char) * DATA_SIZE, 0),
	memset(ret->buf, 0, sizeof(char) * DATA_SIZE);

	return ret;
}

void ljx_init() {
	/* initialize procfs file 
	proc_file = create_proc_entry(procfs_name, 0644, NULL);
	if (proc_file == NULL) {
		printk(KERN_ALERT "Error: could not initialize /proc/%s\n", procfs_name);
	}
	proc_file->read_proc	= procfile_read;
	proc_file->mode		= S_IFREG | S_IRUGO;
	proc_file->uid		= 0;
	proc_file->gid		= 0;
	proc_file->size		= 37;
	*/

	/* initialize buffer list */
	struct buffer *first_buf = init_buf();
	list_add_tail(&first_buf->list, &buffer_list);
}

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
