#include "common.h"

void dump_domain(struct domain* root, int depth)
{
	struct domain_list *p;
	printf("%c %d %s\n", root->flag, depth,  root->filename);

	for(p=root->list; p; p=p->next)
		dump_domain(p->domain, depth+1);
}

void dump_trees()
{
	int i=0;
	struct domain_tree *t;

	for(t=trees; t; t=t->next){
		printf("tree #%d\n",i);
		dump_domain(t->root, 0);
		i++;
	}
}


int main(void)
{
	struct domain *d1, *d2;

	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar", BOBULSM_STUDY);



	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge2",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo2", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar2", BOBULSM_STUDY);



	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge3",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo3", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar3", BOBULSM_STUDY);


	dump_trees();
	free_all_domain_tree();

	/********************************************************/

	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar", BOBULSM_STUDY);



	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge2",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo2", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar2", BOBULSM_STUDY);



	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge3",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo3", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar3", BOBULSM_STUDY);

	dump_trees();
	free_all_domain_tree();

	return 0;
}
