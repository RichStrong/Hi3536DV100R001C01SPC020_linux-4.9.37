/******************************************************************************

  Copyright (C), 2011-2014, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     :hi_drv_cipher.h
  Version       : Initial Draft
  Author        : Hisilicon hisecurity team
  Created       :
  Last Modified :
  Description   :
  Function List :
  History       :
******************************************************************************/
#ifndef __DRV_OSAL_LIB_LINUX_H__
#define __DRV_OSAL_LIB_LINUX_H__
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
#include <asm/atomic.h>
#include <asm/cacheflush.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/traps.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/page.h>
#include "hi_types.h"
#include "drv_osal_chip.h"
#include "drv_cipher_kapi.h"

#define memset_s(s, sn, c, n)    { if (sn >= n) memset(s, c, n); else HI_LOG_ERROR("error, memset overflow\n");}
#define memcpy_s(s, sn, c, n)    { if (sn >= n) memcpy(s, c, n); else HI_LOG_ERROR("error, memcpy overflow\n");}
#define memcmp_s(s1, n1, s2, n)  memcmp(s1, s2, n)

#define crypto_ioremap_nocache(addr, size)  ioremap_nocache(addr, size)
#define crypto_iounmap(addr, size)          iounmap(addr)

#define crypto_read(addr)         (*(volatile unsigned int *)(addr))
#define crypto_write(addr, val)   (*(volatile unsigned int *)(addr) = (val))

#define crypto_msleep(msec)         msleep(msec)
#define crypto_udelay(msec)         udelay(msec)

void *crypto_calloc(size_t n, size_t size);
#define crypto_malloc(x)          (0 < (x) ? kzalloc((x), GFP_KERNEL) : HI_NULL)
#define crypto_free(x)            {if (HI_NULL != (x) ) kfree((x));}

s32 crypto_copy_from_user(void *to, const void *from, unsigned long n);
s32 crypto_copy_to_user(void  *to, const void *from, unsigned long n);
u32 get_rand(void);

#define crypto_queue_head                          wait_queue_head_t
#define crypto_queue_init(x)                       init_waitqueue_head(x)
#define crypto_queue_wait_up(x)                    wake_up_interruptible(x)
#define crypto_queue_wait_timeout(head, con, time) wait_event_interruptible_timeout(head, *(con), time)

#define crypto_request_irq(irq, func, name) request_irq(irq, func, IRQF_SHARED, name, (void*)name)
#define crypto_free_irq(irq, name)          free_irq(irq, (void*)name)

typedef struct semaphore                     crypto_mutex;
#define crypto_mutex_init(x)                 sema_init(x, 1)
#define crypto_mutex_lock(x)                 down_interruptible(x)
#define crypto_mutex_unlock(x)               up(x)
#define crypto_mutex_destroy(x)

#define flush_cache()                        flush_cache_all()

#define crypto_owner                         pid_t
#define crypto_get_owner(x)                  *x = task_tgid_nr(current)

#define HI_PRINT(fmt...)                     printk(fmt)
#define HI_LOG_FATAL(fmt...) \
    do{ \
        printk("[FATAL-HI_CIPHER]:%s[%d]:",(HI_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
    }while(0)
#define HI_LOG_ERROR(fmt...) \
    do{ \
        printk("[ERROR-HI_CIPHER]:%s[%d]:",(HI_U8*)__FUNCTION__,__LINE__); \
        printk(fmt); \
    }while(0)

#define HI_LOG_WARN(fmt...)
#define HI_LOG_INFO(fmt...)
#define HI_LOG_DEBUG(fmt...)

#endif  /* End of #ifndef __HI_DRV_CIPHER_H__*/
