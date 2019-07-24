/******************************************************************************

    Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

******************************************************************************
  File Name     : ext_aead.c
  Version       : Initial Draft
  Created       : 2017
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/spinlock.h>
#include <linux/personality.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include "drv_osal_lib.h"
#include "drv_symc.h"
#include "drv_hash.h"

/************************ Internal Structure Definition *********************/

#define CIPHER_PROC_NAME    "driver/hi_cipher"

extern s32 crypto_ioctl(u32 cmd, HI_VOID *argp);
extern s32 crypto_entry(void);
extern s32 crypto_exit(void);
s32 crypto_release(void);

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      link*/
/** @{*/  /** <!-- [link]*/

/******* proc function begin ********/
#if (1 == HI_PROC_SUPPORT)
HI_S32 symc_proc_read(struct seq_file *p, HI_VOID *v)
{
    symc_chn_status status[8];
    int i = 0;
    s32 ret = HI_SUCCESS;

    seq_printf(p, "\n------------------------------------------"
                  "CIPHER STATUS-------------------------------"
                  "--------------------------------------------"
                  "--------------------\n");
    seq_printf(p, "Chnid   Status   Decrypt   Alg   Mode   KeyLen    "
                  "Addr in/out      KeyFrom  INT-RAW in/out  INT-EN "
                  "in/out INT_OCNTCFG    IVOUT\n");

    memset(&status, 0, sizeof(status));
    for(i = 0; i < 8; i++)
    {
        status[i].id = i;
    }

    ret = drv_symc_proc_status(status);
    if(HI_SUCCESS != ret)
    {
        seq_printf(p, "CIPHER_ProcGetStatus failed!\n");
        return HI_FAILURE;
    }

    for(i=0; i< CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        seq_printf(p, " %d       %s      %d      %s  %s    %03d    %08x/%08x   "
                      " %s           %d/%d            %d/%d        %08x     %s\n",
                        i,
                        status[i].open,
                        status[i].decrypt,
                        status[i].alg,
                        status[i].mode,
                        status[i].klen,
                        status[i].inaddr,
                        status[i].outaddr,
                        status[i].ksrc,
                        status[i].inraw,
                        status[i].outraw,
                        status[i].inten,
                        status[i].outen,
                        status[i].outintcnt,
                        status[i].iv);
    }

    return HI_SUCCESS;
}

static int symc_proc_open(struct inode* inode, struct file* file)
{
    return single_open(file, symc_proc_read, NULL);
}

static const struct file_operations DRV_CIPHER_ProcFops =
{
    .owner		= THIS_MODULE,
    .open		= symc_proc_open,
    .read		= seq_read,
    .llseek		= seq_lseek,
    .release	= single_release,
};

static HI_VOID symc_proc_init(HI_VOID)
{
    struct proc_dir_entry *pstProcEntry = HI_NULL;

    pstProcEntry = proc_create(CIPHER_PROC_NAME, 0, NULL, &DRV_CIPHER_ProcFops);
    if (NULL == pstProcEntry)
    {
        HI_LOG_ERROR("cipher: can't create %s.\n", CIPHER_PROC_NAME);
    }
}

static HI_VOID symc_proc_deinit(HI_VOID)
{
    remove_proc_entry(CIPHER_PROC_NAME, NULL);
}
#endif
/******* proc function end ********/


HI_S32 _crypto_ioctl(struct inode *inode, struct file *file,
                       HI_U32 cmd, HI_VOID *arg)
{
    return crypto_ioctl(cmd, arg);
}

static s32 hi_cipher_open(struct inode * inode, struct file * file)
{
    if ((!capable(CAP_SYS_RAWIO)) || (!capable(CAP_SYS_ADMIN)))
        return -EPERM;
    return HI_SUCCESS;
}

static long hi_cipher_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    long ret;
    HI_U8 unCmdParam[256] = {0};

    if ((HI_NULL == ffile) || (HI_NULL == ffile->f_path.dentry) || ( 0x00 == arg))
    {
        HI_LOG_ERROR("Invalid cmd param size!\n");
        return HI_ERR_CIPHER_INVALID_POINT;
    }

    if(_IOC_SIZE(cmd) > sizeof(unCmdParam))
    {
        HI_LOG_ERROR("Invalid cmd param size %d!\n", _IOC_SIZE(cmd));
        return HI_ERR_CIPHER_INVALID_PARA;
    }

    if (copy_from_user((HI_VOID*)unCmdParam, (void __user *)arg, _IOC_SIZE(cmd)))
    {
        HI_LOG_ERROR("copy data from user fail!\n");
        return HI_FAILURE;
    }

    ret = _crypto_ioctl(ffile->f_path.dentry->d_inode, ffile, cmd, (HI_VOID*)unCmdParam);

    if (copy_to_user((void __user *)arg, (const HI_VOID*)unCmdParam, _IOC_SIZE(cmd)))
    {
        HI_LOG_ERROR("copy data to user fail!\n");
        return HI_FAILURE;
    }

    return ret;
}

static s32 hi_cipher_release(struct inode * inode, struct file * file)
{
    return crypto_release();
}

static struct file_operations dev_cipher_fops=
{
    .owner            = THIS_MODULE,
    .open             = hi_cipher_open,
    .unlocked_ioctl   = hi_cipher_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl     = hi_cipher_ioctl,
#endif
    .release          = hi_cipher_release,
};

static struct miscdevice cipher_dev =
{
    .minor      = MISC_DYNAMIC_MINOR,
    .name       = UMAP_DEVNAME_CIPHER,
    .fops       = &dev_cipher_fops,
};

HI_S32 CIPHER_DRV_ModInit(HI_VOID)
{
    s32 ret = HI_SUCCESS;

    if (misc_register(&cipher_dev))
    {
        HI_LOG_ERROR("ERROR: could not register cipher devices\n");
        return -1;
    }

    ret = crypto_entry();
    if (HI_SUCCESS != ret)
    {
        misc_deregister(&cipher_dev);
        return HI_FAILURE;
    }

/******* proc function begin ********/
#if (1 == HI_PROC_SUPPORT)
    symc_proc_init();
#endif
/******* proc function end ********/

#ifdef MODULE
    HI_PRINT("Load hi_cipher.ko success.\n");
#endif

    return HI_SUCCESS;
}

HI_VOID CIPHER_DRV_ModExit(HI_VOID)
{
/******* proc function begin ********/
#if (1 == HI_PROC_SUPPORT)
    symc_proc_deinit();
#endif
/******* proc function end ********/
    misc_deregister(&cipher_dev);
    crypto_exit();

    return ;
}

/** @}*/  /** <!-- ==== Structure Definition end ====*/
