#include "common.h"

int bobulsm_file_fcntl(struct file *file,unsigned int cmd, unsigned long arg)
{
#if 0
	char *pos;
	char buf[BUFLEN];
	struct cred *cred = current_cred();

	/* get absolute path */	
	pos = d_absolute_path(&file->f_path,buf,BUFLEN);
	if(IS_ERR(pos))
		return pos;

	printk("bobulsm: file_fcntl %d %s\n",
		file->f_flags,
		pos);

	/* permission for self */
	if(cred->security && ((struct domain*)cred->security)->flag 
		&& !strcmp(pos,((struct domain*)cred->security)->filename)){
		printk("bobulsm: this is oneself fcntl. %d %s\n",
			file->f_flags,
			pos);
		return 0;
	}


	/* execve permission check */
	if( (file->f_flags && (O_RDONLY || O_LARGEFILE)) && cred->security && ((struct domain*)cred->security)->flag){
		if(check_domain_trans(cred->security,pos)){
			printk("bobulsm: flag:%d \"%s\" -> \"%s\" fcntl allowed.\n",
				file->f_flags,
				((struct domain*)cred->security)->filename,
				pos);
		}else{
			printk("bobulsm: flag:%d \"%s\" -> \"%s\" fcntl not allowed.\n",
				file->f_flags,
				((struct domain*)cred->security)->filename,
				pos);
		}
	}
#endif
	return 0;
}

int bobulsm_file_open(struct file *file, const struct cred *cred)
{
	char *pos;
	char buf[BUFLEN];

	if(current->in_execve)
		return 0;

	/* get absolute path */	
	pos = d_absolute_path(&file->f_path,buf,BUFLEN);
	if(IS_ERR(pos))
		return pos;

	printk("bobulsm: file_open %d %s\n",
		file->f_flags,
		pos);

	/* permission for self */
	if(cred->security && ((struct domain*)cred->security)->flag 
		&& !strcmp(pos,((struct domain*)cred->security)->filename)){
		printk("bobulsm: this is oneself. %d %s\n",
			file->f_flags,
			pos);
		return 0;
	}


	/* execve permission check */
	if( (file->f_flags && (O_RDONLY || O_LARGEFILE)) && cred->security && ((struct domain*)cred->security)->flag){
		if(check_domain_trans(cred->security,pos)){
			printk("bobulsm: flag:%d \"%s\" -> \"%s\" open allowed.\n",
				file->f_flags,
				((struct domain*)cred->security)->filename,
				pos);
		}else{
			printk("bobulsm: flag:%d \"%s\" -> \"%s\" open not allowed.\n",
				file->f_flags,
				((struct domain*)cred->security)->filename,
				pos);
			return -1;
		}
	}
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

	char tmp[256];
	ssize_t len = sizeof(tmp);
	int rc=0;
	struct domain *p;

	printk("bobulsm: policy written.\n");
	if( (rc=simple_write_to_buffer(tmp,len,ppos,buf,count)) < 0 ){
		printk("bobulsm:  Occurred an error at write_policy.\n");
		return rc;
	}
	if(rc>=1 && tmp[rc-1] == '\n')
		tmp[rc-1] = '\0';
	else	
		tmp[rc] = '\0';
	//printk("bobulsm: policy contained \"%s\"\n",tmp);
	
	p = write_domain(tmp, count);
	if(p)
		show_domain(domain_root);
	else
		printk("bobulsm: Occurred an error at write_domain\n");
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
	int rc;
	
	rc = cap_bprm_set_creds(bprm);
	if (rc)
		return rc;

	/* secondly call */
	if (bprm->cred_prepared)
		return 0;
	
	/* load policy when filename is /sbin/init.*/	
	if (!domain_root && !strcmp(bprm->filename,"/sbin/init")){
		if(bobulsm_policy_user_exist()){
			bobulsm_load_policy();
			printk("bobulsm: load policy using \"%s\".\n",BOBULSM_USER);
			show_domain(domain_root);
		}else{
			printk("bobulsm: can't find \"%s\".\n",BOBULSM_USER);
		}
	}

	/* get absolute path */	
	pos = d_absolute_path(&bprm->file->f_path,buf,BUFLEN);
	if(IS_ERR(pos))
		return pos;

	/* load root_domain */	
	if(domain_root && !old->security && !strcmp(pos,domain_root->filename)){
		new->security = domain_root;
		printk("bobulsm: domain_root assigned to \"%s\"\n",
			pos);
	}

	/* permission check */
	if(old->security && ((struct domain*)old->security)->flag){
		new->security = check_domain_trans(old->security,pos);
		if(new->security){
			printk("bobulsm: \"%s\" -> \"%s\" allowed.\n",
				((struct domain*)old->security)->filename,
				pos);
		}else{
			printk("bobulsm: \"%s\" -> \"%s\" not allowed.\n",
				((struct domain*)old->security)->filename,
				pos);
		}
		return (new->security ? 0 : -1);
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

	if (! security_module_enable(&bobulsm_ops)){
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
