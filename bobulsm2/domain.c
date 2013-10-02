/*
 * domain infomation is stored security field of sturuct cred
 * and accessed from bprm_set_creds() hook.
 * domain status is as below.
 *
 * ENFORCE mode:
 * 	domain->flag = BOBULSM_ENFORCE
 *
 * STUDY mode:
 * 	domain->flag = BOBULSM_STUDY
 *
 * FREE mode:
 * 	domain->flag = BOBULSM_FREE
 * 	domain = NULL
 */

#include "common.h"


/*
 * head of all domain tree
 */
struct domain_tree *trees = NULL;

/*
 * allocate new domain tree
 *
 * return new domain tree if success, NULL otherwise
 */
struct domain_tree *alloc_new_domain_tree()
{
	struct domain_tree *t, *p;
	t =  (struct domain_tree*)malloc(sizeof(struct domain_tree));

	if(!t)
		return NULL;

	t->root = NULL;
	t->next = NULL;

	if(!trees)
		return trees = t;	/* first domain tree */
	else{
		for(p=trees; p->next; p=p->next) ;
		p->next = t;
		return t;
	}
}

/*
 * allocate new domain
 * not update its content
 *
 * @parent: parent domain if exist, NULL otherwise
 *
 * return new domain if success, NULL otherwise
 */
struct domain *alloc_new_domain(struct domain *parent)
{
	struct domain_tree *tree;
	struct domain *domain;
	struct domain_list *p;

	if(!(domain=(struct domain*)malloc(sizeof(struct domain))))
		return NULL;

	domain->filename = NULL;
	domain->flag = BOBULSM_FREE;
	domain->parent = parent;
	domain->list = NULL;

	if(!parent){			/* root domain */
		if(tree=alloc_new_domain_tree())
			tree->root = domain;
		else 
			return NULL;
	}else{				/* child domain*/
		if(!parent->list){
			/* first child */
			if(!(parent->list=(struct domain_list*)malloc(sizeof(struct domain_list))))
				return NULL;
			p = parent->list;
		}else{
			/* 2,3... child */
			for(p=parent->list; p->next; p->next) ;

			if(!(p->next=(struct domain_list*)malloc(sizeof(struct domain_list))))
				return NULL;
			p=p->next;
		}
		p->domain = domain;
		p->next = NULL;
	}
		
	return domain;
}

/*
 * update domain content
 *
 * @domain: domain
 * @filename: filename
 * @flag: flag
 *
 * return domain if success, NULL otherwise
 */
struct domain *update_domain(struct domain *domain, char *filename, char flag)
{
	int len;

	len = strlen(filename);
	if(!domain)
		return NULL;
	if(!(domain->filename=(char*)malloc(len+1)))
		return NULL;

	strncpy(domain->filename, filename, len);
	domain->flag = flag;

	return domain;
}

/*
 * free self and all descendant
 *
 * @root: root you want to free
 */
void free_all_domain(struct domain *root)
{
	struct domain_list *p, *q;

	/* free all children */
	p = root->list;
	while(p){
		q = p->next;
		free_all_domain(p->domain);
		free(p);
		p = q;
	}

	free(root->filename);
	free(root);
}

/*
 * free all domain and all domain_tree
 * all memory will be free
 */
void free_all_domain_tree()
{
	struct domain_tree *p, *q;

	p = trees;
	while(p){
		q = p->next;
		free_all_domain(p->root);
		free(p);
		p = q;
	}
	trees = NULL;
}

/*
 * translate domain and alloc new child. 
 * called from bprm_set_creds() hook.
 *
 * @domain: domain
 *
 * return 
 * new domain if success, 
 * NULL if new domain is BOBULSM_FREE
 * DOMAIN_ERROR if ban domain trans
 */
struct domain *translate_domain(struct domain *domain, char *filename)
{
	struct domain *p;
	struct domain_tree *t;

	if(!domain || domain->flag == BOBULSM_FREE){
		/* FREE mode */
		t = trees;			// alloc new tree
		while(t){
			if(!strcmp(t->root->filename,filename)){
				return t->root;
			}
			t = t->next;
		}
		return NULL;

	}else if(domain->flag == BOBULSM_ENFORCE){
		/* ENFORCE mode */
		p = check_child(domain, filename);
		return (p ? p : DOMAIN_ERROR);

	}else if(domain->flag == BOBULSM_STUDY){
		/* STUDY mode */
		p = check_child(domain, filename);
		if(!p){
			p = alloc_new_domain(domain);
			if(!p)
				return DOMAIN_ERROR;
			update_domain(p, filename, BOBULSM_STUDY);
		}
		return p;

	}else{
		return DOMAIN_ERROR;
	}
}

/*
 * check whether domain has "filename" child.
 *
 * @domain: domain
 *
 * return child domain if success, NULL otherwise
 */
struct domain *check_child(struct domain *domain, char *filename)
{
	struct domain_list *p;
	struct domain *q;

	p = domain->list;
	while(p){
		q = p->domain;
		if(!strcmp(q->filename,filename))
			return q;
		p = p->next;
	}
	return NULL;
}
