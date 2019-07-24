/*****************************************************************************
 *  This is the driver for the host mci SOC.
 *
 *  Copyright (C) Hisilicon. All rights reserved.
 *
 ******************************************************************************/

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/platform_device.h>

#include <linux/mmc/card.h>

#include <linux/export.h>
#include <linux/mmc/host.h>
#include "himci.h"
#include "himci_reg.h"
#include "himci_proc.h"

#define MCI_PARENT       "mci"
#define MCI_STATS_PROC   "mci_info"
#define MAX_CLOCK_SCALE	(4)

static struct proc_dir_entry *proc_mci_dir;
static unsigned int mci_max_connections;
static char *card_type[MAX_CARD_TYPE + 1] = {
	"MMC card",
	"SD card",
	"SDIO card",
	"SD combo (IO+mem) card",
	"unknown"
};
static char *clock_unit[MAX_CLOCK_SCALE] = {
	"Hz",
	"KHz",
	"MHz",
	"GHz"
};

static char *mci_get_card_type(unsigned int sd_type)
{
	if (MAX_CARD_TYPE <= sd_type)
		return card_type[MAX_CARD_TYPE];
	else
		return card_type[sd_type];
}

static unsigned int analyze_clock_scale(unsigned int clock,
		unsigned int *clock_val)
{
	unsigned int scale = 0;
	unsigned int tmp = clock;

	while (1) {
		tmp = tmp / 1000;
		if (0 < tmp) {
			*clock_val = tmp;
			scale++;
		} else {
			break;
		}
	}
	return scale;
}

static void mci_stats_seq_printout(struct seq_file *s)
{
	unsigned int index_mci;
	unsigned int clock;
	unsigned int clock_scale;
	unsigned int clock_value = 0;
	const char *type;
	struct mmc_host *mmc;
	struct himci_host *host;
	struct mmc_card	*card;

	for (index_mci = 0; index_mci < HIMCI_SLOT_NUM; index_mci++) {
		host = mci_host[index_mci];
		if (!host || !host->mmc) {
			seq_printf(s, "MCI%d invalid\n", index_mci);
			continue;
		} else {
			seq_printf(s, "MCI%d", index_mci);
		}

		mmc = host->mmc;
		card = mmc->card;
		if (NULL == card) {
			seq_puts(s, " disconnected\n");
		} else {
			seq_puts(s, " connected\n");

			seq_printf(s,
					"\tType: %s",
					mci_get_card_type(card->type)
				  );

			if (mmc_card_blockaddr(card)) {
				if (mmc_card_ext_capacity(card))
					type = "SDXC";
				else
					type = "SDHC";
				seq_printf(s, "(%s)\n", type);
			}

			clock = host->hclk;
			clock_scale = analyze_clock_scale(clock, &clock_value);
			seq_printf(s, "\tHost work clock %d%s\n",
					clock_value, clock_unit[clock_scale]);

			clock = mmc->ios.clock;
			clock_scale = analyze_clock_scale(clock, &clock_value);
			seq_printf(s, "\tCard support clock %d%s\n",
					clock_value, clock_unit[clock_scale]);

			clock = host->cclk;
			clock_scale = analyze_clock_scale(clock, &clock_value);
			seq_printf(s, "\tCard work clock %d%s\n",
					clock_value, clock_unit[clock_scale]);
		}
	}
}

/* proc interface setup */
static void *mci_seq_start(struct seq_file *s, loff_t *pos)
{
	/*   counter is used to tracking multi proc interfaces
	 *  We have only one interface so return zero
	 *  pointer to start the sequence.
	 */
	static unsigned long counter;

	if (*pos == 0)
		return &counter;

	*pos = 0;
	return NULL;
}

/* proc interface next */
static void *mci_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos >= HIMCI_SLOT_NUM)
		return NULL;

	return NULL;
}

/* define parameters where showed in proc file */
static int mci_stats_seq_show(struct seq_file *s, void *v)
{
	mci_stats_seq_printout(s);
	return 0;
}

/* proc interface stop */
static void mci_seq_stop(struct seq_file *s, void *v)
{
}

/* proc interface operation */
static const struct seq_operations mci_stats_seq_ops = {
	.start = mci_seq_start,
	.next = mci_seq_next,
	.stop = mci_seq_stop,
	.show = mci_stats_seq_show
};

/* proc file open*/
static int mci_stats_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &mci_stats_seq_ops);
};

/* proc file operation */
static const struct file_operations mci_stats_proc_ops = {
	.owner = THIS_MODULE,
	.open = mci_stats_proc_open,
	.read = seq_read,
	.release = seq_release
};

int mci_proc_init(unsigned int max_connections)
{
	struct proc_dir_entry *proc_stats_entry;

	mci_max_connections = max_connections;

	proc_mci_dir = proc_mkdir(MCI_PARENT, NULL);
	if (!proc_mci_dir) {
		pr_err("%s: failed to create proc file %s\n",
				__func__, MCI_PARENT);
		return 1;
	}

	proc_stats_entry = proc_create(MCI_STATS_PROC,
			0, proc_mci_dir, &mci_stats_proc_ops);
	if (!proc_stats_entry) {
		pr_err("%s: failed to create proc file %s\n",
				__func__, MCI_STATS_PROC);
		return 1;
	}

	return 0;
}

int mci_proc_shutdown(void)
{
	if (proc_mci_dir) {
		if (mci_max_connections > 0)
			remove_proc_entry(MCI_STATS_PROC, proc_mci_dir);

		remove_proc_entry(MCI_PARENT, NULL);
		proc_mci_dir = NULL;
	}

	return 0;
}
