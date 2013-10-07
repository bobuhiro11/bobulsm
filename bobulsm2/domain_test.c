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

	d2 = alloc_new_domain(d1);
	update_domain(d2,"baz", BOBULSM_STUDY);



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

/*
 * test write_domain()
 */
void test4()
{
	write_domain("reset");
	write_domain("F 0 root");
	write_domain("F 1 app");
	write_domain("F 2 assets");
	write_domain("F 3 images");
	write_domain("F 3 javascripts");
	write_domain("F 3 stylesheets");
	write_domain("F 2 controllers");
	write_domain("F 2 helpers");
	write_domain("F 2 mailers");
	write_domain("F 2 models");
	write_domain("F 2 views");
	write_domain("F 3 layouts");
	write_domain("F 1 config");
	write_domain("F 2 environments");
	write_domain("F 2 initializers");
	write_domain("F 2 locales");
	write_domain("F 1 db");
	write_domain("F 1 doc");
	write_domain("F 1 lib");
	write_domain("F 2 assets");
	write_domain("F 2 tasks");
	write_domain("F 1 log");
	write_domain("F 1 public");
	write_domain("F 1 script");
	write_domain("F 1 test");
	write_domain("F 2 fixtures");
	write_domain("F 2 functional");
	write_domain("F 2 integration");
	write_domain("F 2 performance");
	write_domain("F 2 unit");
	write_domain("F 1 tmp");
	write_domain("F 2 cache");
	write_domain("F 3 assets");
	write_domain("F 1 vendor");
	write_domain("F 2 assets");
	write_domain("F 3 javascripts");
	write_domain("F 3 stylesheets");
	write_domain("F 2 plugins");

	printf("write_domain test.\n");
	dump_trees();
}


int main(void)
{
	test4();
	return 0;
}
