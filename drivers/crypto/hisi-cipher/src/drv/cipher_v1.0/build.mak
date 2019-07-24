CIPHER_DIR := $(CIPHER_SRC_BASE)

CIPHER_CFLAGS += -DHI_VERSION_TYPE_LINUX_SMP

include $(CIPHER_DIR)/drv/cipher_v1.0/drivers/build.mak
include $(CIPHER_DIR)/drv/cipher_v1.0/osal/build.mak
include $(CIPHER_DIR)/drv/cipher_v1.0/compat/build.mak

EXTRA_CFLAGS += $(CIPHER_CFLAGS)
