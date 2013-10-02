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

/*
 * test 
 * alloc_new_domain()
 * update_domain()
 * free_all_domain_tree()
 */
void test1()
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
}

/*
 * test check_child()
 */
void test2()
{
	struct domain *d1, *d2;
	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo", BOBULSM_STUDY);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"bar", BOBULSM_STUDY);

	printf("%d\n", ( check_child(d1, "foo") ? 1 : 0) );
	printf("%d\n", ( check_child(d1, "bar") ? 1 : 0) );
	printf("%d\n", ( check_child(d1, "baz") ? 1 : 0) );

	dump_trees();
	free_all_domain_tree();
}

/*
 * test translate_domain()
 */
void test3()
{
	struct domain *d1, *d2, *d3, *d4, *d5 , *d6, *d7;

	d1 = alloc_new_domain(NULL);
	update_domain(d1,"hogehoge",BOBULSM_ENFORCE);

	d2 = alloc_new_domain(d1);
	update_domain(d2,"foo", BOBULSM_STUDY);

	d3 = alloc_new_domain(d1);
	update_domain(d3,"bar", BOBULSM_FREE);

	dump_trees();

	/* STUDY mode */
	d4 = translate_domain(d2, "haaaa"); 		
	/* ENFORCE mode */
	printf("ENFORCE ok  %p\n", translate_domain(d1, "foo")); 
	printf("ENFORCE err %p\n", translate_domain(d1, "foo2"));
	/* FREE mode */
	printf("FREE        %p\n", translate_domain(d3, "hoge"));
	printf("FREE 	    %p[%s]\n", translate_domain(d3, "hogehoge"),
				translate_domain(d3,"hogehoge")->filename);
	dump_trees();

	free_all_domain_tree();
}


int main(void)
{
	test3();
	return 0;
}
