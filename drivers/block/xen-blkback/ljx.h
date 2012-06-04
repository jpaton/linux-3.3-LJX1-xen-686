#ifndef _PROC_LJX_H
#define _PROC_LJX_H

#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/jbd.h>
#include <linux/capability.h>
#include <linux/ext3_fs.h>

#define procfs_name "blkback-ljx"

extern struct proc_dir_entry *proc_file;

extern void ljx_init(void);

#define LJX_DESC_PER_BLOCK(ls)		(ls->block_size / sizeof (struct ext3_group_desc))

/*
 * ljx_super_block: represents a superblock. Largely taken from ext3_super_block.
 */
struct ljx_ext3_super_block {
/*00*/	u32	inodes_count;		/* Inodes count */
	u32	blocks_count;		/* Blocks count */
	u32	r_blocks_count;	/* Reserved blocks count */
	u32	free_blocks_count;	/* Free blocks count */
/*10*/	u32	free_inodes_count;	/* Free inodes count */
	u32	first_data_block;	/* First Data Block */
	u32	log_block_size;	/* Block size */
	u32	log_frag_size;	/* Fragment size */
/*20*/	u32	blocks_per_group;	/* # Blocks per group */
	u32	frags_per_group;	/* # Fragments per group */
	u32	inodes_per_group;	/* # Inodes per group */
	u32	mtime;		/* Mount time */
/*30*/	u32	wtime;		/* Write time */
	u16	mnt_count;		/* Mount count */
	u16	max_mnt_count;	/* Maximal mount count */
	u16	magic;		/* Magic signature */
	u16	state;		/* File system state */
	u16	errors;		/* Behaviour when detecting errors */
	u16	minor_rev_level;	/* minor revision level */
/*40*/	u32	lastcheck;		/* time of last check */
	u32	checkinterval;	/* max. time between checks */
	u32	creator_os;		/* OS */
	u32	rev_level;		/* Revision level */
/*50*/	u16	def_resuid;		/* Default uid for reserved blocks */
	u16	def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT3_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	u32	first_ino;		/* First non-reserved inode */
	u16   	inode_size;		/* size of inode structure */
	u16	block_group_nr;	/* block group # of this superblock */
	u32	feature_compat;	/* compatible feature set */
/*60*/	u32	feature_incompat;	/* incompatible feature set */
	u32	feature_ro_compat;	/* readonly-compatible feature set */
/*68*/	u8	uuid[16];		/* 128-bit uuid for volume */
/*78*/	char	volume_name[16];	/* volume name */
/*88*/	char	last_mounted[64];	/* directory where last mounted */
/*C8*/	u32	algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT3_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	u8	prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	u8	prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	u16	reserved_gdt_blocks;	/* Per group desc for online growth */
	/*
	 * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
/*D0*/	u8	journal_uuid[16];	/* uuid of journal superblock */
/*E0*/	u32	journal_inum;		/* inode number of journal file */
	u32	journal_dev;		/* device number of journal file */
	u32	last_orphan;		/* start of list of inodes to delete */
	u32	hash_seed[4];		/* HTREE hash seed */
	u8	def_hash_version;	/* Default hash version to use */
	u8	reserved_char_pad;
	u16	reserved_word_pad;
	u32	default_mount_opts;
	u32	first_meta_bg;	/* First metablock block group */
	u32	mkfs_time;		/* When the filesystem was created */
	u32	jnl_blocks[17];	/* Backup of the journal inode */
	/* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/	u32	blocks_count_hi;	/* Blocks count */
	u32	r_blocks_count_hi;	/* Reserved blocks count */
	u32	free_blocks_count_hi;	/* Free blocks count */
	u16	min_extra_isize;	/* All inodes have at least # bytes */
	u16	want_extra_isize; 	/* New inodes should reserve # bytes */
	u32	flags;		/* Miscellaneous flags */
	u16  	raid_stride;		/* RAID stride */
	u16  	mmp_interval;         /* # seconds to wait in MMP checking */
	u64  	mmp_block;            /* Block for multi-mount protection */
	u32  	raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	u8	log_groups_per_flex;  /* FLEX_BG group size */

	/* additional information, mostly from ext3_sb_info */
	unsigned long groups_count;
	int db_count;
	struct buffer_head ** group_desc;
};

extern inline int load_super_block(struct ljx_ext3_super_block *lsb, struct ext3_super_block *sb) {
	int i; /* for loop counter */

	lsb->inodes_count		= le32_to_cpu(sb->s_inodes_count);
	lsb->blocks_count		= le32_to_cpu(sb->s_blocks_count);
	lsb->r_blocks_count		= le32_to_cpu(sb->s_r_blocks_count);
	lsb->free_blocks_count		= le32_to_cpu(sb->s_free_blocks_count);
	lsb->free_inodes_count		= le32_to_cpu(sb->s_free_inodes_count);
	lsb->first_data_block		= le32_to_cpu(sb->s_first_data_block);
	lsb->log_block_size		= le32_to_cpu(sb->s_log_block_size);
	lsb->log_frag_size		= le32_to_cpu(sb->s_log_frag_size);
	lsb->blocks_per_group		= le32_to_cpu(sb->s_blocks_per_group);
	lsb->frags_per_group		= le32_to_cpu(sb->s_frags_per_group);
	lsb->inodes_per_group		= le32_to_cpu(sb->s_inodes_per_group);
	lsb->mtime			= le32_to_cpu(sb->s_mtime);
	lsb->wtime			= le32_to_cpu(sb->s_wtime);
	lsb->mnt_count			= le16_to_cpu(sb->s_mnt_count);
	lsb->max_mnt_count		= le16_to_cpu(sb->s_max_mnt_count);
	lsb->magic			= le16_to_cpu(sb->s_magic);
	lsb->state			= le16_to_cpu(sb->s_state);
	lsb->errors			= le16_to_cpu(sb->s_errors);
	lsb->minor_rev_level		= le16_to_cpu(sb->s_minor_rev_level);
	lsb->lastcheck			= le32_to_cpu(sb->s_lastcheck);
	lsb->checkinterval		= le32_to_cpu(sb->s_checkinterval);
	lsb->creator_os			= le32_to_cpu(sb->s_creator_os);
	lsb->rev_level			= le32_to_cpu(sb->s_rev_level);
	lsb->def_resuid			= le16_to_cpu(sb->s_def_resuid);
	lsb->def_resgid			= le16_to_cpu(sb->s_def_resgid);
	lsb->first_ino			= le32_to_cpu(sb->s_first_ino);
	lsb->inode_size			= le16_to_cpu(sb->s_inode_size);
	lsb->block_group_nr		= le16_to_cpu(sb->s_block_group_nr);
	lsb->feature_compat		= le32_to_cpu(sb->s_feature_compat);
	lsb->feature_incompat		= le32_to_cpu(sb->s_feature_incompat);
	lsb->feature_ro_compat		= le32_to_cpu(sb->s_feature_ro_compat);
	strncpy(lsb->uuid, sb->s_uuid, 16);
	strncpy(lsb->volume_name, sb->s_volume_name, 16);
	strncpy(lsb->last_mounted, sb->s_last_mounted, 64);
	lsb->algorithm_usage_bitmap 	= le32_to_cpu(sb->s_algorithm_usage_bitmap);
	lsb->prealloc_blocks		= sb->s_prealloc_blocks;
	lsb->prealloc_dir_blocks	= sb->s_prealloc_dir_blocks;
	lsb->reserved_gdt_blocks	= le16_to_cpu(sb->s_reserved_gdt_blocks);
	strncpy(lsb->journal_uuid, sb->s_journal_uuid, 16);
	lsb->journal_inum		= le32_to_cpu(sb->s_journal_inum);
	lsb->journal_dev		= le32_to_cpu(sb->s_journal_dev);
	lsb->last_orphan		= le32_to_cpu(sb->s_last_orphan);
	for (i = 0; i < 4; i++)
		lsb->hash_seed[i]	= le32_to_cpu(sb->s_hash_seed[i]);
	lsb->def_hash_version		= sb->s_def_hash_version;
	lsb->reserved_char_pad		= sb->s_reserved_char_pad;
	lsb->reserved_word_pad		= sb->s_reserved_word_pad;
	lsb->default_mount_opts		= le32_to_cpu(sb->s_default_mount_opts);
	lsb->first_meta_bg		= le32_to_cpu(sb->s_first_meta_bg);
	lsb->mkfs_time			= le32_to_cpu(sb->s_mkfs_time);
	for (i = 0; i < 17; i++)
		lsb->jnl_blocks[i]	= le32_to_cpu(sb->s_jnl_blocks[i]);
	lsb->blocks_count_hi		= le32_to_cpu(sb->s_blocks_count_hi);
	lsb->r_blocks_count_hi		= le32_to_cpu(sb->s_r_blocks_count_hi);
	lsb->free_blocks_count_hi	= le32_to_cpu(sb->s_free_blocks_count_hi);
	lsb->min_extra_isize		= le16_to_cpu(sb->s_min_extra_isize);
	lsb->want_extra_isize 		= le16_to_cpu(sb->s_want_extra_isize);
	lsb->flags			= le32_to_cpu(sb->s_flags);
	lsb->raid_stride		= le16_to_cpu(sb->s_raid_stride);
	lsb->mmp_interval         	= le16_to_cpu(sb->s_mmp_interval);
	lsb->mmp_block            	= le64_to_cpu(sb->s_mmp_block);
	lsb->raid_stripe_width    	= le32_to_cpu(sb->s_raid_stripe_width);
	lsb->log_groups_per_flex  	= sb->s_log_groups_per_flex;

	/* fs/ext3/super.c:1904 */
	lsb->groups_count = (lsb->blocks_count - lsb->first_data_block - 1)
		/ lsb->blocks_per_group + 1;
	lsb->db_count = DIV_ROUND_UP(lsb->groups_count, LJX_DESC_PER_BLOCK(lsb));
	lsb->group_desc = kmalloc(lsb->db_count * sizeof(struct buffer_head *),
			GFP_KERNEL);
	if (lsb->group_desc == NULL) {
		printk(KERN_WARN "LJX: not enough memory to initialize ljx_super_block");
		return -ENOMEM;
	}

	return 0;
}

#endif
