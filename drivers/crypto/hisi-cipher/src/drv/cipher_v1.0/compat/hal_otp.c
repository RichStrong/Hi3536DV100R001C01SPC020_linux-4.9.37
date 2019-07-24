#include "hi_types.h"
#include "drv_osal_lib.h"
#include "hal_otp.h"

#ifdef OTP_SUPPORT

HI_U8 *g_u32EfuseOtpRegBase = HI_NULL;

/* OTP init */
HI_S32 HAL_Efuse_OTP_Init(HI_VOID)
{
    HI_U32 CrgValue = 0;
    HI_U32 *u32SysAddr = HI_NULL;

    u32SysAddr = crypto_ioremap_nocache(REG_SYS_OTP_CLK_ADDR_PHY, 0x100);
    if (u32SysAddr == HI_NULL)
    {
        HI_LOG_ERROR("ERROR: u32SysAddr ioremap with nocache failed!!\n");
        return HI_FAILURE;
    }

    HAL_CIPHER_ReadReg(u32SysAddr, &CrgValue);
#if defined(CHIP_TYPE_hi3559av100)
    CrgValue |= OTP_CRG_RESET_BIT;   /* reset */
    CrgValue |= OTP_CRG_CLOCK_BIT;   /* set the bit 0, clock opened */
    HAL_CIPHER_WriteReg(u32SysAddr, CrgValue);

    /* clock select and cancel reset 0x30100*/
    CrgValue &= (~OTP_CRG_RESET_BIT); /* cancel reset */
#endif
    CrgValue |= OTP_CRG_CLOCK_BIT;   /* set the bit 0, clock opened */
    HAL_CIPHER_WriteReg(u32SysAddr, CrgValue);

    crypto_iounmap(u32SysAddr, 0x100);

    g_u32EfuseOtpRegBase = crypto_ioremap_nocache(OTP_REG_BASE_ADDR_PHY, 0x100);
    if (g_u32EfuseOtpRegBase == HI_NULL)
    {
        HI_LOG_ERROR("ERROR: osal_ioremap_nocache for OTP failed!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* CRC16 */
unsigned short CalculateCRC16 (const unsigned char* data_array_ptr, unsigned long data_array_length)
{
    unsigned short crc_value = 0xffff;    //init value 0xffff
    unsigned short polynomial = 0x8005;    //polynomial 0x8005
    unsigned short data_index = 0;
    int l = 0;

    if (data_array_ptr == 0)
    {
        return crc_value;
    }
    if (data_array_length == 0)
    {
        return crc_value;
    }

    for (data_index = 0; data_index < data_array_length; data_index++)//data_array_length=17
    {
        unsigned char byte0 = data_array_ptr [data_index];
        crc_value ^= byte0 * 256;
        for (l=0;l<8;l++)
        {
            HI_BOOL flag = ((crc_value & 0x8000)==32768);
            crc_value = (crc_value & 0x7FFF)*2;
            if (flag == HI_TRUE)
            {
                crc_value ^= polynomial;
            }
        }
    }
    return crc_value;
};

HI_S32 HAL_OTP_WaitFree(HI_VOID)
{
    HI_U32 u32TimeOutCnt = 0;
    HI_U32 u32RegValue = 0;

    while(1)
    {
        HAL_CIPHER_ReadReg(OTP_USER_CTRL_STA, &u32RegValue);
        if((u32RegValue&0x1)==0)//bit0:otp_op_busy 0:idle, 1:busy
        {
            return HI_SUCCESS;
        }

        u32TimeOutCnt++;
        if(u32TimeOutCnt >= 10000)
        {
            HI_LOG_ERROR("OTP_WaitFree TimeOut!\n");
            break;
        }
    }
    return HI_FAILURE;
}

HI_S32 HAL_OTP_SetMode(OTP_USER_WORK_MODE_E enOtpMode)
{
    HI_U32 u32RegValue = enOtpMode;

    if(enOtpMode >= OTP_UNKOOWN_MODE)
    {
        HI_LOG_ERROR("Mode Unknown!\n");
        return  HI_FAILURE;
    }

    (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_WORK_MODE, u32RegValue);
    return HI_SUCCESS;
}

HI_VOID HAL_OTP_OP_Start(HI_VOID)
{
    HI_U32 u32RegValue = 0x1acce551;
    (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_OP_START, u32RegValue);
}

HI_S32 HAL_OTP_Wait_OP_done(HI_VOID)
{
    HI_U32 u32TimeOutCnt = 0;
    HI_U32 u32RegValue = 0;

    while(1)
    {
        HAL_CIPHER_ReadReg(OTP_USER_CTRL_STA, &u32RegValue);
        if(u32RegValue & 0x2)
        {
            return HI_SUCCESS;
        }

        u32TimeOutCnt++;
        if(u32TimeOutCnt >= 10000)
        {
            HI_LOG_ERROR("OTP_Wait_OP_done TimeOut!\n");
            break;
        }
    }
    return HI_FAILURE;
}

HI_U32 HAL_Get_OTP_Lock_status_byType(OTP_LOCK_STA_TYPE_E enLockStatusType)
{
    HI_U8 *Regddr = 0;
    HI_U32 RegValue = 0;

    Regddr = (HI_U8 *)((enLockStatusType == OTP_USER_LOCK_STA0_TYPE)?OTP_USER_LOCK_STA0:OTP_USER_LOCK_STA1);

    HAL_CIPHER_ReadReg(Regddr, &RegValue);

    return RegValue;
}

HI_VOID HAL_CHOOSE_OTP_key(OTP_USER_KEY_INDEX_E enWhichKey)
{
    HI_U32 RegValue = 0;

    RegValue = enWhichKey;
    (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_INDEX, RegValue);
}

/* write data to otp */
HI_S32 HAL_Write_key(OTP_USER_KEY_LENGTH_E enKeyLength,HI_U32 *KeyDataBuf,HI_U32 UseCRC)
{
    HI_U32 crc16 = 0;

    switch(enKeyLength)
    {
        case OTP_KEY_LENGTH_64BIT:
        {
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA0, KeyDataBuf[0]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA1, KeyDataBuf[1]);
            if(UseCRC==HI_TRUE)
            {
                crc16 = CalculateCRC16((HI_U8*)KeyDataBuf,8);//computer user key CRC16
                crc16&=0xffff;
                (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA2, crc16);
            }
            break;
        }
        case OTP_KEY_LENGTH_128BIT:
        {
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA0, KeyDataBuf[0]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA1, KeyDataBuf[1]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA2, KeyDataBuf[2]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA3, KeyDataBuf[3]);
            if(UseCRC==HI_TRUE)
            {
                crc16 = CalculateCRC16((HI_U8*)KeyDataBuf,16);//computer user key CRC16
                crc16&=0xffff;
                (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA4, crc16);
            }
            break;
        }
        case OTP_KEY_LENGTH_256BIT:
        {
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA0, KeyDataBuf[0]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA1, KeyDataBuf[1]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA2, KeyDataBuf[2]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA3, KeyDataBuf[3]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA4, KeyDataBuf[4]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA5, KeyDataBuf[5]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA6, KeyDataBuf[6]);
            (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA7, KeyDataBuf[7]);
            if(UseCRC==HI_TRUE)
            {
                crc16 = CalculateCRC16((HI_U8*)KeyDataBuf,32);
                crc16&=0xffff;
                HI_LOG_ERROR("crc: 0x%x\n",crc16);
                (HI_VOID)HAL_CIPHER_WriteReg(OTP_USER_KEY_DATA8, crc16);
            }
            break;
        }
        default:
            break;
    }
    return HI_SUCCESS;
}

HI_S32 HAL_Get_OTP_Lock_status(OTP_LOCK_STA_TYPE_E enLockStatusType,HI_U32 *status)
{
    HI_U32 LockStatusType = enLockStatusType;

    if(LockStatusType >= OTP_USER_LOCK_UNKNOWN_STA)
    {
        return HI_FAILURE;
    }

    if(HI_FAILURE == HAL_OTP_WaitFree())
    {
        return HI_FAILURE;
    }

    if(HAL_OTP_SetMode(OTP_READ_LOCK_STA_MODE))
    {
        return HI_FAILURE;
    }

    HAL_OTP_OP_Start();

    if(HI_FAILURE == HAL_OTP_Wait_OP_done())
    {
        return HI_FAILURE;
    }

    *status = HAL_Get_OTP_Lock_status_byType(LockStatusType);
    return HI_SUCCESS;
}

HI_S32 Is_Locked(OTP_USER_KEY_INDEX_E enWhichKey,HI_U32 lock_sta)
{
    HI_U32 status = lock_sta;

    if(enWhichKey>OTP_USER_KEY3)
    {
        HI_LOG_ERROR("Unsupport Key!\n");
        return HI_FAILURE;
    }

    if((status>>(enWhichKey+5))&0x1)
    {
        HI_LOG_ERROR("Key%d was locked!\n",enWhichKey);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

/* set user key to otp */
HI_S32 HAL_Write_key_To_OTP(OTP_USER_KEY_INDEX_E enWhichKey, HI_U32 *KeyDataBuf, HI_U32 UseCRC)
{
    HI_U32 lock_sta0 = 0;

    if(enWhichKey > OTP_USER_KEY3)
    {
        return HI_FAILURE;
    }
    HAL_Get_OTP_Lock_status(OTP_USER_LOCK_STA0_TYPE,&lock_sta0);

    if(Is_Locked(enWhichKey,lock_sta0))
    {
        return HI_FAILURE;
    }

    if(HI_FAILURE == HAL_OTP_WaitFree())
    {
        return HI_FAILURE;
    }

    HAL_CHOOSE_OTP_key(enWhichKey);

    HAL_Write_key(OTP_KEY_LENGTH_128BIT, KeyDataBuf, HI_TRUE);

    if(HAL_OTP_SetMode(OTP_WRITE_KEY_ID_OR_PASSWD_MODE))
    {
        return HI_FAILURE;
    }
    HAL_OTP_OP_Start();

    if(HI_FAILURE == HAL_OTP_Wait_OP_done())
    {
        return HI_FAILURE;
    }
    return  HI_SUCCESS;
}

/* check otp key crc */
HI_S32 HAL_Is_CRC_Check_OK(HI_VOID)
{
    HI_U32 u32RegValue = 0;

    HAL_CIPHER_ReadReg(OTP_USER_CTRL_STA, &u32RegValue);
    if(u32RegValue&0x10)
    {
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

HI_S32 HAL_Key_CRC_CheckOut(OTP_USER_KEY_INDEX_E enWhichKey,HI_U32 *result)
{

    if(enWhichKey > OTP_USER_KEY3)
    {
        return HI_FAILURE;
    }

    if(HI_FAILURE == HAL_OTP_WaitFree())
    {
        return HI_FAILURE;
    }

    HAL_CHOOSE_OTP_key(enWhichKey);

    if(HAL_OTP_SetMode(OTP_KEY_ID_OR_PASSWD_CRC_MODE))
    {
        return HI_FAILURE;
    }
    HAL_OTP_OP_Start();

    if(HI_FAILURE == HAL_OTP_Wait_OP_done())
    {
        return HI_FAILURE;
    }

    *result = HAL_Is_CRC_Check_OK();
    if((*result) != HI_SUCCESS)
    {
        HI_LOG_ERROR("CRC Error!!!!!!!!!!!!\n");
        return  HI_FAILURE;
    }

    return  HI_SUCCESS;
}

/* set otp key to klad */
HI_S32 HAL_Efuse_OTP_LoadCipherKey(HI_U32 chn_id, HI_U32 opt_id)
{
    if(opt_id > OTP_USER_KEY3)
    {
        opt_id = OTP_USER_KEY0;
    }

    if(HI_FAILURE == HAL_OTP_WaitFree())
    {
        return HI_FAILURE;
    }
    HAL_CHOOSE_OTP_key(opt_id);

    if(HAL_OTP_SetMode(OTP_LOCK_CIPHER_KEY_MODE))
    {
        return HI_FAILURE;
    }

    HAL_OTP_OP_Start();

    if(HI_FAILURE == HAL_OTP_Wait_OP_done())
    {
        return HI_FAILURE;
    }

    return  HI_SUCCESS;
}
#endif

