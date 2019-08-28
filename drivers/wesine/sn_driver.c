#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MAX_LENGTH 64

static char pData[MAX_LENGTH];
static size_t dataLength=0;

static int __init param_sn_setup(char *str)
{
	int i=0;
	while(str[i] && (i<MAX_LENGTH-1)){
		pData[i] = str[i];
		i++;
	}
	pData[i] = 0;
	dataLength=i+1;
	return 0;
}

__setup("sn=", param_sn_setup);

static int myopen(struct inode *ind,struct file *fl)
{
    return 0;
}

static ssize_t mywrite(struct file *fp,const char *__user buf,size_t len,loff_t *off)
{
 
    //copy_from_user(&stat,buf,sizeof(char));
 
    return len;
}

static ssize_t myread(struct file *fp,char *__user buf,size_t len,loff_t *off)
{
	unsigned long p = *off;
	unsigned int count = len;
	int ret = 0;

	if(p >= dataLength){
		return 0;
	}
	if(count > dataLength-p)
		count = dataLength-p;

	if(copy_to_user(buf,(void *)(pData + p),count)){
		ret = - EFAULT;
	} else {
		*off += count;
		ret = count;
	}

	return ret;
}

static loff_t myllseek(struct file *fp, loff_t offset, int orig)
{
	loff_t ret = 0;
	switch(orig){

	case 0:
		if(offset < 0) {
			ret = - EINVAL;
			break;
		}
		if(offset > dataLength){
			ret = - EINVAL;
			break;
		}
		fp->f_pos = (unsigned int)offset;
		ret = fp->f_pos;
		break;

	case 1:
		if( (fp->f_pos + offset) > dataLength ){
			ret = - EINVAL;
			break;
		}
		if((fp->f_pos + offset) < 0) {
			ret = - EINVAL;
			break;
		}
		fp->f_pos += offset;
		ret = fp->f_pos;
		break;

	default:
		ret = - EINVAL;
		break;
	}
	return ret;
}

static int myclose(struct inode *inode,struct file *fl)
{ 
    return 0;
}

static struct file_operations fops= {
    .owner = THIS_MODULE,
    .open  = myopen,
    .write = mywrite,
	.read = myread,
	.llseek = myllseek,
    .release= myclose,
};

static struct miscdevice sndev = {
    .minor = MISC_DYNAMIC_MINOR, // 255
    .name = "sn",
    .fops = &fops,
};

static int __init sn_init(void)
{
    return misc_register(&sndev);
}

static void __exit sn_exit(void)
{
    misc_deregister(&sndev);
}

module_init(sn_init);
module_exit(sn_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mark Zhang");
MODULE_DESCRIPTION("serial number driver");
MODULE_VERSION("v1.0");

