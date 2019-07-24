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
#include <linux/mm_types.h>
#include <linux/mm.h>
#include "drv_osal_lib.h"
#include "drv_symc.h"
#include "drv_hash.h"

/************************ Internal Structure Definition *********************/

extern s32 crypto_ioctl(u32 cmd, HI_VOID *argp);
extern s32 crypto_entry(void);
extern s32 crypto_exit(void);
extern s32 crypto_recover_hdcp_key(void);

static osal_dev_t *   g_CipherDevice;

/** @}*/  /** <!-- ==== Structure Definition end ====*/

/******************************* API Code *****************************/
/** \addtogroup      link*/
/** @{*/  /** <!-- [link]*/

/******* proc function begin ********/
#if (1 == HI_PROC_SUPPORT)
HI_S32 symc_proc_read(struct osal_proc_dir_entry *p)
{
    symc_chn_status status[8];
    int i = 0;
    s32 ret = HI_SUCCESS;

    PROC_PRINT(p, "\n------------------------------------------"
                  "CIPHER STATUS-------------------------------"
                  "--------------------------------------------"
                  "--------------------\n");
    PROC_PRINT(p, "Chnid   Status   Decrypt   Alg   Mode   KeyLen    "
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
        PROC_PRINT(p, "CIPHER_ProcGetStatus failed!\n");
        return HI_FAILURE;
    }

    for(i=0; i< CRYPTO_HARD_CHANNEL_MAX; i++)
    {
        PROC_PRINT(p, " %d       %s      %d      %s  %s    %03d    %08x/%08x   "
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

static HI_VOID symc_proc_init(HI_VOID)
{
    osal_proc_entry_t *pstProcEntry = HI_NULL;

    pstProcEntry = osal_create_proc_entry(UMAP_DEVNAME_CIPHER, HI_NULL);
    if (NULL == pstProcEntry)
    {
        HI_LOG_ERROR("cipher: can't create proc.\n");
        return;
    }
    pstProcEntry->read = symc_proc_read;
}

static HI_VOID symc_proc_deinit(HI_VOID)
{
    osal_remove_proc_entry(UMAP_DEVNAME_CIPHER, NULL);
}
#endif
/******* proc function end ********/

static s32 hi_cipher_open(struct inode * inode, struct file * file)
{
    if ((!capable(CAP_SYS_RAWIO)) || (!capable(CAP_SYS_ADMIN)))
        return -EPERM;
    return HI_SUCCESS;
}

static long hi_cipher_ioctl(HI_U32 cmd,  unsigned long arg, void *private_data)
{
     return crypto_ioctl(cmd, (void*)arg);
}

static osal_fileops_t dev_cipher_fops=
{
    .open             = hi_cipher_open,
    .unlocked_ioctl   = hi_cipher_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl     = hi_cipher_ioctl,
#endif
    .release          = HI_NULL,
};

HI_S32 CIPHER_DRV_ModInit(HI_VOID)
{
    s32 ret = HI_SUCCESS;

    g_CipherDevice = osal_createdev(UMAP_DEVNAME_CIPHER);
    g_CipherDevice->fops = &dev_cipher_fops;
    g_CipherDevice->minor = UMAP_MIN_MINOR_CIPHER;


    ret = osal_registerdevice(g_CipherDevice);
    if (HI_SUCCESS != ret)
    {
        HI_LOG_ERROR("register CIPHER failed.\n");
        return HI_FAILURE;
    }

    ret = crypto_entry();
    if (HI_SUCCESS != ret)
    {
        ret = HI_FAILURE;
        goto error;
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

error:
    osal_deregisterdevice(g_CipherDevice);
    osal_destroydev(g_CipherDevice);

    return ret;
}

HI_VOID CIPHER_DRV_ModExit(HI_VOID)
{

/******* proc function begin ********/
#if (1 == HI_PROC_SUPPORT)
    symc_proc_deinit();
#endif
/******* proc function end ********/

    (HI_VOID)crypto_exit();
    osal_deregisterdevice(g_CipherDevice);
    osal_destroydev(g_CipherDevice);

    return ;
}

/** @}*/  /** <!-- ==== Structure Definition end ====*/
