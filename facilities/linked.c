/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

LIST_HEAD(my_list);

struct my_entry {
	struct list_head list;
	int intvar;
	char strvar[20];
};

static void mylist_init(void)
{
	struct my_entry *me;
	int j;
	for (j = 0; j < NENTRY; j++) {
		me = kmalloc(sizeof(struct my_entry), GFP_KERNEL);
		me->intvar = j;
		sprintf(me->strvar, "My_%d", j + 1);
		list_add(&me->list, &my_list);
	}
}

static int walk_list(void)
{
	int j = 0;
	struct list_head *l;

	if (list_empty(&my_list))
		return 0;

	list_for_each(l, &my_list) {
		struct my_entry *me = list_entry(l, struct my_entry, list);
		foobar(&me->intvar);
		j++;
	}
	return j;
}
