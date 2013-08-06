#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/tracehook.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/xattr.h>
#include <linux/in.h>
#include <linux/capability.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/tty.h>
#include <net/icmp.h>
#include <net/ip.h>	
#include <net/tcp.h>
#include <net/net_namespace.h>
#include <net/netlabel.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <asm/atomic.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/dccp.h>
#include <linux/quota.h>
#include <linux/un.h>
#include <net/af_unix.h>
#include <linux/parser.h>
#include <linux/nfs_mount.h>
#include <net/ipv6.h>
#include <linux/hugetlb.h>
#include <linux/personality.h>
#include <linux/sysctl.h>
#include <linux/audit.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>

static int __init bobulsm_init(void);
static int bobulsm_inode_mkdir(struct inode *inode, struct dentry *dentry,umode_t mask);
static int bobulsm_securityfs_init(void);
static void bobulsm_securityfs_exit(void);
static ssize_t write_policy(struct file *filep,const char __user *buf,size_t count,loff_t *ppos);
static ssize_t read_policy(struct file *filep,char __user *buf,size_t count,loff_t *ppos);

/*
 * dentry of /sys/kernel/security/bobulsm/policy
 */
static struct dentry *bobulsm_dentry, *policy_dentry;

/*
 * operations of policy file
 */ 
static struct file_operations bobulsm_file_ops = {
	.read  = read_policy,
	.write = write_policy,
};

/*
 * operations of lsm hooks
 */ 
static struct security_operations bobulsm_ops = {
	.name = "bobulsm",
	.inode_mkdir = bobulsm_inode_mkdir,
};





/*
 * make securityfs and policy file
 * /sys/kernel/security/bobulsm/policy
 */
static int bobulsm_securityfs_init(void){
	int rc = 0;
	
	// make "bobulsm" directory
	bobulsm_dentry = securityfs_create_dir("bobulsm",NULL);
	if( IS_ERR(bobulsm_dentry))
		return PTR_ERR(bobulsm_dentry);
	// make "policy" file	
	// read and write enabled.
	policy_dentry = securityfs_create_file(
		"policy",S_IRUGO|S_IWUSR,bobulsm_dentry,NULL,&bobulsm_file_ops);
	if( IS_ERR(policy_dentry)){
		rc  = PTR_ERR(policy_dentry);
		securityfs_remove(bobulsm_dentry);
	}
	return rc;
}

/*
 * remove securityfs and policy file
 * /sys/kernel/security/bobulsm/policy
 */
static void bobulsm_securityfs_exit(void){
	securityfs_remove(policy_dentry);
	securityfs_remove(bobulsm_dentry);
}


/*
 * read policy
 */ 
static ssize_t read_policy(struct file *filep,char __user *buf,
	size_t count,loff_t *ppos){
	
	char tmp[256];
	ssize_t len;
	
	len = scnprintf(tmp,sizeof(tmp),
		"This is bobulsm policy file.\nAmazinggggggg :D :D :D\n");	
	
	printk("bobulsm: policy read.\n");
	// copy from "tmp(kernel buffer)" to "buf(user buffer)"
	return simple_read_from_buffer(buf,count,ppos,tmp,len);
}

/*
 * write policy
 */ 
static ssize_t write_policy(struct file *filep,const char __user *buf,
	size_t count,loff_t *ppos){

	char tmp[256];
	ssize_t len = sizeof(tmp);
	int rc=0;

	printk("bobulsm: policy written.\n");
	if( (rc=simple_write_to_buffer(tmp,len,ppos,buf,count)) < 0 ){
		printk("bobulsm:  Occurred an error at write_policy.\n");
		return rc;
	}
	if(buf[rc-1] == '\n')
		tmp[rc-1] = '\0';
	else	
		tmp[rc] = '\0';
	printk("bobulsm: policy contained \"%s\"\n",tmp);
	return rc;
}
	
/*
 * lsm hook
 */ 
static int bobulsm_inode_mkdir(struct inode *inode, struct dentry *dentry,
			   umode_t mask){
	printk("bobulsm: inode_mkdir called.\n");

  	/* Return 0 if permission is granted. */
	return 0;
}



static int __init bobulsm_init(void){
	int rc = 0;

	if (! security_module_enable(&bobulsm_ops)){
		printk("bobulsm: did not selected.\n");
		return 0;
	}

	if (register_security(&bobulsm_ops)){
		panic("bobulsm: Occurred an error at register_security(&bobulsm_ops).\n");
	}else{
		printk("bobulsm: registered surely.\n");
	}

	/*
	if( rc = bobulsm_securityfs_init() ){
		printk("bobulsm: Occurred an error at bobulsm_securityfs_init()"); 
		return rc;
	}
	*/
	return 0;
}

security_initcall(bobulsm_init);
fs_initcall(bobulsm_securityfs_init);

MODULE_DESCRIPTION("BOBULSM");
