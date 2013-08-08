#include "common.h"

struct domain *domain_root = NULL;

/*
 * free domain and ALL child domain
 */
void free_domain(struct domain *domain)
{
	struct domain *parent;
	struct domain_list *p,*t;

	parent = domain->parent;

	/* remove child */
	while(p = domain->list){
		domain->list = p->next;
		p->domain->parent = NULL;
		free_domain(p->domain);
		kfree(p);
	}

	/* root domain */
	if(domain_root == domain){
		kfree(domain->filename);
		kfree(domain);
		domain_root = NULL;
		return;
	}

	/* adjust parent list */
	if(parent){
		p = parent->list;
		if(p->domain == domain){
			/* domain is first child */
			parent->list = p->next;
			kfree(p);
		}else{
			/* domain is 2,3, ... child */
			while(p->next->domain != domain)
				p = p->next;

			t = p->next->next;
			kfree(p->next);
			p->next = t;
		}
	}

	kfree(domain->filename);
	kfree(domain);
	return;
}

/*
 *  add new domain to parent domain tree as **DIRECT CHILD**
 *
 *  return new domain if success , NULL otherwise
 */
struct domain *alloc_domain(struct domain *parent, char *filename, int flag)
{
	struct domain_list *p;
	struct domain* dp;
	int buflen = strlen(filename)+1;

	/* root domain */
	if(!parent){
		domain_root = kmalloc(sizeof(struct domain), GFP_NOFS);
		if(!domain_root)
			return NULL;
		domain_root->filename = kmalloc(buflen, GFP_NOFS);
		if(!domain_root->filename)
			return NULL;
		strcpy(domain_root->filename,filename);
		domain_root->flag = flag;
		domain_root->list = NULL;
		domain_root->parent = NULL;
		return domain_root;
	}  


	if(!parent->list){
		/* first child */
		p = parent->list = kmalloc(sizeof(struct domain_list), GFP_NOFS); 
	}else{
		/* 2,3, ... child */
		p = parent->list;
		while(p->next)
			p = p->next;
		p = p->next = kmalloc(sizeof(struct domain_list), GFP_NOFS);
	}

	if(!p)
		return NULL;

	p->next = NULL;
	dp = p->domain = kmalloc(sizeof(struct domain), GFP_NOFS);
	if(!dp)
		return NULL;
	dp->filename = kmalloc(buflen, GFP_NOFS);
	if(!dp->filename)
		return NULL;
	strcpy(dp->filename,filename);
	dp->flag = flag;
	dp->list = NULL;
	dp->parent = parent;
	return dp; 
}


static void _show_domain(struct domain *domain, int n)
{
	int i;
	struct domain_list *p;

	if(!domain)
		return;

	/*  domain info */
	for(i=0;i<n;i++)
		printk("*");
	printk("%s %d\n",domain->filename,domain->flag);

	/* child domain info */
	p = domain->list;
	while(p){
		_show_domain(p->domain,n+1);
		p = p->next;
	}
}

/*
 * show domain and all child domain
 */
void show_domain(struct domain *domain)
{
	_show_domain(domain,1);
}

/*
 * read each line of policy_file and make domain
 * buf is pure string ('\0' end) and not contain CR and LF
 * 
 * return domain if success, NULL otherwise
 */
struct domain *write_domain(char *buf, int buflen)
{
	static int n = 0;                     /* depth of previous domain   */
	static struct domain *d = NULL;       /* address of previous domain */

	int i, j;
	char *p;

	if(strstr(buf,"#")==buf)
		return NULL;
	if(buflen<=1)
		return NULL;

	i = 0;
	while(buf[i] == '*')
		i++;

	/* root domain */
	if(i==1){
		n = 0;
		d = NULL;
	}

	/* search parent */
	if(i<=n){
		j = n-i+1;
		while(j--)
			d = d->parent;
	}

	j = 1;
	if(p = strstr(&buf[i],"&end&")){
		*p = '\0';
		j = 0;
	}

	d = alloc_domain(d,&buf[i],j);
	n = i;

	return d;
}

/*
 * check domain transion
 * 
 * return new domain if given permission
 * NULL otherwise
 */
struct domain *check_domain_trans(struct domain *domain, char *filename)
{
	struct domain_list *p;
	
	p = domain->list;
	while(p){
		if(!strcmp(p->domain->filename,filename)){
			return p->domain;
		}
		p = p->next;
	}
	return NULL;
}
