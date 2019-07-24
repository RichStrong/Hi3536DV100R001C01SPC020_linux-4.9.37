
/* the total cipher hard channel which we can used*/
#define CIPHER_HARD_CHANNEL_CNT         (0x07)
/* mask which cipher channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_CHANNEL_MASK        (0xFE)

/* the total hash hard channel which we can used*/
#define HASH_HARD_CHANNEL_CNT           (0x01)

#ifdef __HuaweiLite__
/* liteos resource config */
/* mask which cipher channel we can used, bit0 means channel 0*/
#define HASH_HARD_CHANNEL_MASK          (0x04)
#define HASH_HARD_CHANNEL               (0x02)
#else
/* mask which cipher channel we can used, bit0 means channel 0*/
#define HASH_HARD_CHANNEL_MASK          (0x02)
#define HASH_HARD_CHANNEL               (0x01)
#endif

/* the total cipher hard key channel which we can used*/
#define CIPHER_HARD_KEY_CHANNEL_CNT     (0x04)

/* mask which cipher hard key channel we can used, bit0 means channel 0*/
#define CIPHER_HARD_KEY_CHANNEL_MASK    (0xF0)

/* support read IRQ number from DTS */
#define IRQ_DTS_SUPPORT

/* support OTP load key */
#define OTP_SUPPORT

/* support smmu*/
//#define CRYPTO_SMMU_SUPPORT

/* support interrupt*/
#define CRYPTO_OS_INT_SUPPORT

/* RSA RAND Mask*/
//#define RSA_RAND_MASK

/* secure cpu*/
//#define CRYPTO_SEC_CPU

/* the hardware version */
#define CHIP_SYMC_VER_V200
#define CHIP_HASH_VER_V200
#define CHIP_TRNG_VER_V200
#define CHIP_IFEP_RSA_VER_V100
//#define CHIP_SM2_VER_V100

/* supoort odd key */
//#define CHIP_SYMC_ODD_KEY_SUPPORT

/* supoort SM1 */
#define CHIP_SYMC_SM1_SUPPORT

/* the hardware capacity */
#define CHIP_AES_CCM_GCM_SUPPORT

/* the software capacity */
//#define SOFT_AES_SUPPORT
//#define SOFT_TDES_SUPPORT
//#define SOFT_AES_CCM_GCM_SUPPORT
//#define SOFT_SHA1_SUPPORT
//#define SOFT_SHA256_SUPPORT
//#define SOFT_SHA512_SUPPORT
//#define SOFT_SM2_SUPPORT
//#define SOFT_SM3_SUPPORT
//#define SOFT_ECC_SUPPORT
//#define SOFT_AES_CTS_SUPPORT

/* SMP version linux is sec config */
/* moudle unsupport, we need set the table*/
#define BASE_TABLE_NULL    {\
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 0, \
    .crg_valid = 0, \
    .ver_valid = 0, \
    .int_valid = 0, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define HARD_INFO_CIPHER {\
    .name = "cipher",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 125, \
    .reset_bit = 0, \
    .clk_bit = 1, \
    .version_reg = 0x308, \
    .version_val = 0x20170609, \
    .reg_addr_phy = 0x04060000, \
    .reg_addr_size = 0x4000,    \
    .crg_addr_phy = 0x0451016c, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define HARD_INFO_HASH {\
    .name = "hash",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 125, \
    .reset_bit = 0, \
    .clk_bit = 1, \
    .version_reg = 0x308, \
    .version_val = 0x20170609, \
    .reg_addr_phy = 0x04060000, \
    .reg_addr_size = 0x4000, \
    .crg_addr_phy = 0x0451016c, \
}

/* define initial value of struct sys_arch_boot_dts for HASH*/
#define HARD_INFO_TRNG {\
    .name = "trng",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 0, \
    .int_valid = 0, \
    .reset_bit = 12, \
    .clk_bit = 13, \
    .reg_addr_phy = 0x04090200,  \
    .reg_addr_size = 0x100,   \
    .crg_addr_phy = 0x04510194, \
}

/* define sec rsa1 for SMP VERSION */
#define HARD_INFO_IFEP_RSA {\
    .name = "rsa1",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .reg_addr_phy = 0x04088000,  \
    .reg_addr_size = 0x1000,\
    .crg_addr_phy = 0x04510194, \
    .reset_bit = 22, \
    .clk_bit = 23, \
    .int_num = 127, \
    .version_reg = 0x90, \
    .version_val = 0x20160907, \
}

#define KLAD_REG_BASE_ADDR_PHY          (0x04070000)
#define OTP_REG_BASE_ADDR_PHY           (0x040A0000)
#define KLAD_CRG_ADDR_PHY               (0x04510194)
#define REG_SYS_OTP_CLK_ADDR_PHY        (0x04510194)
#define SPACC_PROT_BYPASS_N             (0x04C50004)

#define SPACC_PROT_BYPASS_N_BIT         (0x01 << 12)
#define RSA1_PROT_BYPASS_N_BIT          (0x01 << 18)

#define OTP_CRG_CLOCK_BIT               (0x01 << 7)

#define KLAD_CRG_CLOCK_BIT              (0x01 << 11)
#define KLAD_CRG_RESET_BIT              (0x01 << 10)

#define HARD_INFO_SMMU                BASE_TABLE_NULL
#define HARD_INFO_SIC_RSA             BASE_TABLE_NULL
#define HARD_INFO_CIPHER_KEY          BASE_TABLE_NULL
#define HARD_INFO_SM4                 BASE_TABLE_NULL
#define HARD_INFO_SM2                 BASE_TABLE_NULL

/* AMP version linux is nonsec config */
#define CRYPTO_SWITCH_CPU

#define NSEC_HARD_INFO_CIPHER {\
    .name = "cipher",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 126, \
    .reset_bit = 0, \
    .clk_bit = 1, \
    .version_reg = 0x308, \
    .version_val = 0x20170609, \
    .reg_addr_phy = 0x04060000, \
    .reg_addr_size = 0x4000,    \
    .crg_addr_phy = 0x0451016c, \
}

/* define initial value of struct sys_arch_boot_dts for cipher*/
#define NSEC_HARD_INFO_HASH {\
    .name = "hash",  \
    .reset_valid = 0,  \
    .clk_valid = 0, \
    .phy_valid = 1, \
    .crg_valid = 0, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .int_num = 126, \
    .reset_bit = 0, \
    .clk_bit = 1, \
    .version_reg = 0x308, \
    .version_val = 0x20170609, \
    .reg_addr_phy = 0x04060000, \
    .reg_addr_size = 0x4000, \
    .crg_addr_phy = 0x0451016c, \
}

/* define initial value of struct sys_arch_boot_dts for IFEP RSA*/
#define NSEC_HARD_INFO_IFEP_RSA {\
    .name = "rsa0",  \
    .reset_valid = 1,  \
    .clk_valid = 1, \
    .phy_valid = 1, \
    .crg_valid = 1, \
    .ver_valid = 1, \
    .int_valid = 1, \
    .reg_addr_phy = 0x04080000,  \
    .reg_addr_size = 0x1000,\
    .crg_addr_phy = 0x04510194, \
    .reset_bit = 14, \
    .clk_bit = 15, \
    .int_num = 117, \
    .version_reg = 0x90, \
    .version_val = 0x20160907, \
}

#define NSEC_HARD_INFO_SMMU                BASE_TABLE_NULL
#define NSEC_HARD_INFO_SIC_RSA             BASE_TABLE_NULL
#define NSEC_HARD_INFO_CIPHER_KEY          BASE_TABLE_NULL
#define NSEC_HARD_INFO_SM4                 BASE_TABLE_NULL
#define NSEC_HARD_INFO_SM2                 BASE_TABLE_NULL
#define NSEC_HARD_INFO_TRNG                BASE_TABLE_NULL

