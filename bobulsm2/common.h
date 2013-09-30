#ifndef _SECURITY_BOBULSM_COMMON_H
#define _SECURITY_BOBULSM_COMMON_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define BOBULSM_ENFORCE 'E'
#define BOBULSM_STUDY 	'S'	
#define BOBULSM_FREE 	'F'


/********** Structure Definition **********/

/* domain information used via bprm->cred->security */

struct domain_list;
struct domain;

struct domain_tree {
	struct domain *root;
	struct domain_tree *next;
};

struct domain {
	struct domain_list *list; /* domain list of children */
	struct domain *parent;    /* domain of parent */
	char *filename;
	char flag;
}; 

struct domain_list {
	struct domain_list  *next;
	struct domain *domain;
};

/********** Function Prototype **********/

/* domain.c */
struct domain_tree *alloc_new_domain_tree();
struct domain *alloc_new_domain(struct domain *parent);
struct domain *update_domain(struct domain *domain, char *filename, char flag);
void free_all_domain(struct domain *root);
void free_all_domain_tree();

/********** External Valiable **********/

extern struct domain_tree *trees;

#endif	/* #ifndef _SECURITY_BOBULSM_COMMON_H  */
