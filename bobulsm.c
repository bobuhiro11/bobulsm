#include "common.h"

int bobulsm_file_fcntl(struct file *file,unsigned int cmd, unsigned long arg)
{
	return 0;
}

int bobulsm_file_open(struct file *file, const struct cred *cred)
{
	return 0;
}

/*
 * dentry of /sys/kernel/security/bobulsm/policy
 */
struct dentry *bobulsm_dentry, *policy_dentry;

/*
 * operations of policy file
 */ 
struct file_operations bobulsm_file_ops = {
	.read  = read_policy,
	.write = write_policy,
};

/*
 * operations of lsm hooks
 */ 
struct security_operations bobulsm_ops = {
	.name                = "bobulsm",
	.bprm_set_creds      = bobulsm_bprm_set_creds,
	.bprm_check_security = bobulsm_bprm_check_security,
	.file_open           = bobulsm_file_open,
	.file_fcntl          = bobulsm_file_fcntl,
	.cred_alloc_blank    = bobulsm_cred_alloc_blank,
	.cred_prepare        = bobulsm_cred_prepare,
	.cred_transfer       = bobulsm_cred_transfer,
	.cred_free           = bobulsm_cred_free,
};

/*
 * load policy from "/etc/bobulsm/policy" using "/sbin/bobulsm_user"
 */
void bobulsm_load_policy(void)
{
	char *argv[2];
	char *envp[1];
	
	argv[0] = BOBULSM_USER;
	argv[1] = NULL;
	envp[0] = NULL;
	
	call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

/*
 * check whether BOBULSM_USER exist or not.
 * return true if BOBUSLM_USER exist, false otherwise
 */
bool bobulsm_policy_user_exist(void)
{
	struct path path;
	if(kern_path(BOBULSM_USER,LOOKUP_FOLLOW,&path)){
		return false;
	}
	path_put(&path);
	return true;
}

/*
 * make securityfs and policy file
 * /sys/kernel/security/bobulsm/policy
 *
 * return 0 if success
 */
int bobulsm_securityfs_init(void)
{
	int rc = 0;
	
	/* make "bobulsm" directory */
	bobulsm_dentry = securityfs_create_dir("bobulsm",NULL);
	if( IS_ERR(bobulsm_dentry))
		return PTR_ERR(bobulsm_dentry);

	/* make "policy" file read and write enabled. */
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
void bobulsm_securityfs_exit(void)
{
	securityfs_remove(policy_dentry);
	securityfs_remove(bobulsm_dentry);
}


/*
 * read policy
 */ 
ssize_t read_policy(struct file *filep,char __user *buf,
	size_t count,loff_t *ppos)
{
	
	char tmp[256];
	ssize_t len;
	
	len = scnprintf(tmp,sizeof(tmp),
		"This is bobulsm policy file.\nAmazinggggggg :D :D :D\n");	
	
	printk("bobulsm: policy read.\n");
	/* copy from "tmp(kernel buffer)" to "buf(user buffer)" */
	return simple_read_from_buffer(buf,count,ppos,tmp,len);
}

/*
 * write policy
 */ 
ssize_t write_policy(struct file *filep,const char __user *buf,
	size_t count,loff_t *ppos) 
{
        static int domain_num = 0;      /* domain number  0...MAX_TREE_SIZE */
	char tmp[256];
	ssize_t len = sizeof(tmp);
	int rc=0;
	struct domain *p;
        char *endp;

	printk("bobulsm: policy written.\n");
	if( (rc=simple_write_to_buffer(tmp,len-1,ppos,buf,count)) < 0 ){
		printk("bobulsm:  Occurred an error at write_policy.\n");
		return rc;
	}
	if(rc>=1 && tmp[rc-1] == '\n')
		tmp[rc-1] = '\0';
	else	
		tmp[rc] = '\0';
        
        if(tmp[0] >= '0' && tmp[0] <= '9'){
                domain_num = simple_strtol(tmp, &endp, 10);
                printk("bobulsm: set domain_num = %d\n", domain_num);
                return;
        }

	p = write_domain(tmp, count, &domain_roots[domain_num]);

	if(p){
                printk("bobulsm: policy %d\n",domain_num);
		show_domain(domain_roots[domain_num]);
        }else{
		printk("bobulsm: Occurred an error at write_domain\n");
        }
	return rc;
}

/*
 * lsm hook for cred_alloc_blank
 */
int bobulsm_cred_alloc_blank(struct cred *new, gfp_t gfp)
{
	new->security = NULL;
	return 0;
}

/*
 * lsm hook for cred_prepare
 */
int bobulsm_cred_prepare(struct cred *new, const struct cred *old, gfp_t gfp)
{
	new->security = old->security;
	return 0;
}

/*
 * lsm hook for cred_transfer
 */
void bobulsm_cred_transfer(struct cred *new, const struct cred *old)
{
	bobulsm_cred_prepare(new,old,0);
}

/*
 * lsm hook for cred_free
 */
void bobulsm_cred_free(struct cred *cred)
{
	return;	
}

/*
 * lsm hook for bprm_set_creds
 */
int bobulsm_bprm_set_creds(struct linux_binprm *bprm)
{
	const struct cred *old = (struct cred *)current_cred();
	struct cred *new = bprm->cred;
	char buf[BUFLEN];
	char *pos = ERR_PTR(-ENOMEM);
	int rc,i;
	
	rc = cap_bprm_set_creds(bprm);
	if (rc){
                printk("cap not found\n");
		return rc;
        }

	/* secondly call */
	if (bprm->cred_prepared)
		return 0;
	
	/* load policy when filename is /sbin/init.*/	
	if (!domain_roots[0] && !strcmp(bprm->filename,"/sbin/init")){
		if(bobulsm_policy_user_exist()){
			bobulsm_load_policy();
			printk("bobulsm: load policy using \"%s\".\n",BOBULSM_USER);
                        for(i=0;i<DOMAIN_TREE_SIZE;i++)
                                if(domain_roots[i]){
                                        printk("bobulsm: policy %d\n",i);
			                show_domain(domain_roots[i]);
                                }
		}else{
			printk("bobulsm: can't find \"%s\".\n",BOBULSM_USER);
		}
	}

	/* get absolute path */	
	pos = d_absolute_path(&bprm->file->f_path,buf,BUFLEN);
	if(IS_ERR(pos)){
                printk(KERN_INFO "bobulsm: path not found.\n");
		return pos;
        }

	/* load root_domain */	
        for(i=0;i<DOMAIN_TREE_SIZE;i++){
                if(domain_roots[i] && !old->security && !strcmp(pos,domain_roots[i]->filename)){
                        new->security = domain_roots[i];
                        printk("bobulsm: domain_roots[%d] assigned to \"%s\"\n",
                                        i,pos);
                        break;
                }
        }

	/* permission check */
	if(old->security && ((struct domain*)old->security)->flag){
		new->security = check_domain_trans(old->security,pos);
		if(new->security){
			printk("bobulsm: \"%s\" -> \"%s\" allowed.\n",
				((struct domain*)old->security)->filename,
				pos);
                        if(!((struct domain*)new->security)->flag){
                                printk("bobulsm: \"%s\" lsm finished.\n",
                                pos);
                        }
		}else{
			printk("bobulsm: \"%s\" -> \"%s\" not allowed.\n",
				((struct domain*)old->security)->filename,
				pos);
		}
		return (new->security ? 0 : -EPERM);
	}else{
		/* out of check */
		return 0;
	}
}

/*
 * lsm hook for bprm_check_security
 */
int bobulsm_bprm_check_security(struct linux_binprm *bprm)
{
	return 0;	
}


int __init bobulsm_init(void)
{
	int rc = 0;
	struct cred *cred = (struct cred *) current_cred();

	if (!security_module_enable(&bobulsm_ops)){
		printk("bobulsm: did not selected.\n");
		return 0;
	}

	if (register_security(&bobulsm_ops))
		panic("bobulsm: Occurred an error at register_security(&bobulsm_ops).\n");
	else
		printk("bobulsm: registered surely.\n");

	return 0;
}

security_initcall(bobulsm_init);
fs_initcall(bobulsm_securityfs_init);

MODULE_DESCRIPTION("BOBULSM");
