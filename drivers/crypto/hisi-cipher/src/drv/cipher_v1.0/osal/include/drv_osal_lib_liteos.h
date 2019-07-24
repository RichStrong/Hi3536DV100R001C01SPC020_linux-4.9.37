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
#ifndef __DRV_OSAL_LIB_LITEOS_H__
#define __DRV_OSAL_LIB_LITEOS_H__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include "securec.h"
#include "hi_types.h"
#include "hi_osal.h"
#include "drv_osal_chip.h"
#include "drv_cipher_kapi.h"

/* osal_ioremap_nocache is adpat to ioremap_wc wiith cache in Linux */
#define crypto_ioremap_nocache(addr, size)  osal_ioremap_nocache(addr, size)
#define crypto_iounmap(addr, size)          osal_iounmap(addr)

#define crypto_read(addr)                 (*(volatile unsigned int *)(addr))
#define crypto_write(addr, val)           (*(volatile unsigned int *)(addr) = (val))
#define crypto_read_fifo(id, addr)        crypto_read((u8*)(module_get_base_address(id) + addr))
#define crypto_write_fifo(id, addr, val)  crypto_write((u8*)(module_get_base_address(id)+ addr), val)

#define crypto_msleep(msec)         osal_msleep(msec)
#define crypto_udelay(msec)         osal_udelay(msec)

void *crypto_calloc(size_t n, size_t size);
#define crypto_malloc(x)          (0 < (x) ? osal_kmalloc(x, osal_gfp_atomic) : HI_NULL)
#define crypto_free(x)            {if (HI_NULL != (x) ) osal_kfree(x);}

#define copy_from_user     osal_copy_from_user
#define copy_to_user       osal_copy_to_user

s32 crypto_copy_from_user(void *to, const void *from, unsigned long n);
s32 crypto_copy_to_user(void  *to, const void *from, unsigned long n);
u32 get_rand(void);
s32 crypto_waitdone_callback(void *param);

#define crypto_queue_head                          osal_wait_t
#define crypto_queue_init(x)                       osal_wait_init(x)
#define crypto_queue_wait_up(x)                    osal_wakeup(x)
#define crypto_queue_wait_timeout(head, con, time) osal_wait_event_timeout(&head, crypto_waitdone_callback,con, time)

#define crypto_request_irq(irq, func, name) request_irq(irq, (irq_handler_t)func, IRQF_SHARED, name, (void*)name)
#define crypto_free_irq(irq, name)          osal_free_irq(irq, (void*)name)

#define crypto_mutex                         osal_mutex_t
#define crypto_mutex_init(x)                 osal_mutex_init(x)
#define crypto_mutex_lock(x)                 osal_mutex_lock_interruptible(x)
#define crypto_mutex_unlock(x)               osal_mutex_unlock(x)
#define crypto_mutex_destroy(x)

#define flush_cache()                        osal_flush_cache_all()

#define crypto_owner                         u32
#define crypto_get_owner(x)                  *x = 0

#define PROC_PRINT                           osal_seq_printf

#define irqreturn_t                          int
#define IRQ_HANDLED                          OSAL_IRQ_HANDLED
#define IRQ_NONE                             OSAL_IRQ_NONE

#define HI_PRINT(fmt...)                     osal_printk(fmt)
#define HI_LOG_FATAL(fmt...)
#define HI_LOG_ERROR(fmt...)                 osal_printk(fmt)
#define HI_LOG_WARN(fmt...)
#define HI_LOG_INFO(fmt...)
#define HI_LOG_DEBUG(fmt...)

#endif  /* End of #ifndef __HI_DRV_CIPHER_H__*/
