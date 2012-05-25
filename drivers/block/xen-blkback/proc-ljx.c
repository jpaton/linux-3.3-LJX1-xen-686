/*
 * proc-ljx.c -- procfs functions for xen-blkback-ljx driver
 */

#include <linux/seq_file.h>
#include <linux/list.h>
#include <linux/spinlock.h>

#include "proc-ljx.h"

#define DATA_SIZE 1024

#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct proc_dir_entry *proc_file;
static LIST_HEAD(buffer_list);
static struct buffer *current_buffer;
static spinlock_t lock;

struct buffer {
	struct list_head list;
	char *buf;
	loff_t len;
};

static struct buffer *init_buf(void) {
	struct buffer *ret = kmalloc(sizeof(char) * DATA_SIZE, 0);
	ret->len	= 0,
	ret->buf	= (char *)kmalloc(sizeof(char) * DATA_SIZE, 0),
	memset(ret->buf, 0, sizeof(char) * DATA_SIZE);

	return ret;
}

struct user_pos {
	struct buffer *buf;
	loff_t offset;
};

int ljx_print(const char *fmt, ...) {
	va_list args;
	int lefttocopy;
	int tocopy;
	int wanttowrite;
	int bufsize = 512;
	int spaceleft;
	char *buf;


	buf = kmalloc(bufsize, 0);
	va_start(args, fmt);
	wanttowrite = vsnprintf(buf, bufsize, fmt, args);
	va_end(args);
	if (wanttowrite > bufsize) {
		/* get bigger buffer, try again */
		kfree(buf);
		buf = kmalloc(wanttowrite, 0);
		va_start(args, fmt);
		wanttowrite = vsnprintf(buf, wanttowrite, fmt, args);
		va_end(args);
	}
	bufsize = wanttowrite;

	/* copy contents of buf into buffer list, one buffer at a time */
	lefttocopy = bufsize;
	spin_lock(&lock);
	while (lefttocopy > 0) {
		spaceleft = DATA_SIZE - current_buffer->len;
		tocopy = MAX(spaceleft, lefttocopy);
		memcpy(&current_buffer->buf[current_buffer->len], buf, tocopy);
		buf += tocopy;
		lefttocopy -= tocopy;
		if (lefttocopy > 0) {
			/* need to allocate a new buffer */
			current_buffer = init_buf();
			list_add_tail(&current_buffer->list, &buffer_list);
		}
	}
	spin_unlock(&lock);

	kfree(buf);
	return 0;
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

static void ljx_seq_stop(struct seq_file *s, void *v)
{
	kfree (v);
}

static void *ljx_seq_next(struct seq_file *s, void *v, loff_t *pos) {
	struct user_pos *upos;
	loff_t byte_in_buf;
       
	upos = (struct user_pos *) v;
	byte_in_buf = ++upos->offset % DATA_SIZE;

	if (byte_in_buf == 0) {
		if (upos->buf->list.next == &buffer_list)
			/* we have reached the end */
			return 0;
		/* must move to next buf */
		upos->buf = list_entry(upos->buf->list.next, struct buffer, list);
	}
	if (byte_in_buf >= upos->buf->len) {
		/* we have reached the very end! */
		return 0;
	}

	return upos;
}

static int ljx_seq_show(struct seq_file *s, void *v) {
	struct user_pos *upos;
	loff_t byte_in_buf;
       
	upos = (struct user_pos *) v;
	byte_in_buf = upos->offset % DATA_SIZE;

	seq_printf(s, "%c", upos->buf->buf[byte_in_buf]);

	return 0;
}

static struct seq_operations ljx_seq_ops = {
	.start = ljx_seq_start,
	.next  = ljx_seq_next,
	.stop  = ljx_seq_stop,
	.show  = ljx_seq_show
};

static int ljx_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ljx_seq_ops);
}

static struct file_operations ljx_file_ops = {
	.open    = ljx_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

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
	struct proc_dir_entry *entry;

	/* initialize buffer list */
	struct buffer *first_buf = init_buf();
	list_add_tail(&first_buf->list, &buffer_list);
	current_buffer = first_buf;

	entry = create_proc_entry("ljx", 0, NULL);
	if (entry)
		entry->proc_fops = &ljx_file_ops;
	spin_lock_init(&lock);
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
