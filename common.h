#ifndef _SECURITY_BOBULSM_COMMON_H
#define _SECURITY_BOBULSM_COMMON_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/tracehook.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/xattr.h>
#include <linux/in.h>
#include <linux/cred.h>
#include <linux/binfmts.h>
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

#define BUFLEN	512	/* buffer size for d_absolute_path */

/********** Structure Definition **********/

/* domain information used via bprm->cred->security */

struct domain_list;
struct domain;

struct domain_list {
  struct domain_list  *next;
  struct domain *domain;
};

struct domain {
  struct domain_list *list; /* domain list of children */
  struct domain *parent;    /* domain of parent */
  char *filename;
  int flag;
}; 

/********** Function Prototype **********/

/* bobulsm.c*/
int bobulsm_inode_mkdir(struct inode *inode, struct dentry *dentry,umode_t mask);
int bobulsm_bprm_set_creds(struct linux_binprm *bprm);
int bobulsm_bprm_check_security(struct linux_binprm *bprm);
int bobulsm_cred_alloc_blank(struct cred *new, gfp_t gfp);
int bobulsm_cred_prepare(struct cred *new, const struct cred *old, gfp_t gfp);
void bobulsm_cred_transfer(struct cred *new, const struct cred *old);
void bobulsm_cred_free(struct cred *cred);

int __init bobulsm_init(void);
int bobulsm_securityfs_init(void);
void bobulsm_securityfs_exit(void);
ssize_t write_policy(struct file *filep,const char __user *buf,size_t count,loff_t *ppos);
ssize_t read_policy(struct file *filep,char __user *buf,size_t count,loff_t *ppos);

/* domain.c */
void free_domain(struct domain* domain);
struct domain *alloc_domain(struct domain *parent, char *filename, int flag);
void show_domain(struct domain *domain);
struct domain *write_domain(char *buf, int buflen);

/********** External Valiable **********/
extern struct dentry *bobulsm_dentry,*policy_dentry;
extern struct file_operations bobulsm_file_ops;
extern struct security_operations bobulsm_ops;
extern struct domain *domain_root;

#endif	/* #ifndef _SECURITY_BOBULSM_COMMON_H  */
