#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");

struct my_data_struct {
  int value;
  struct list_head full_list;
  struct list_head odd_list;
  struct list_head even_list;
};

LIST_HEAD( my_full_list );
LIST_HEAD( my_odd_list );
LIST_HEAD( my_even_list);


int init_module( void )
{
  int count;
  struct my_data_struct *obj;

  for (count = 1 ; count < 11 ; count++) {

    obj = (struct my_data_struct *)
            kmalloc( sizeof(struct my_data_struct), GFP_KERNEL );

    obj->value = count;

    list_add( &obj->full_list, &my_full_list );

    if (obj->value & 0x1) {
      list_add( &obj->odd_list, &my_odd_list );

	
    if (obj->value & 0x2) {
	list_add( &obj->even_list, &my_even_list);
	}
    }

  }

  return 0;
}


void cleanup_module( void )
{
  struct list_head *pos, *q;
  struct my_data_struct *my_obj;

  printk("Emit full list\n");
  list_for_each( pos, &my_full_list ) {
    my_obj = list_entry( pos, struct my_data_struct, full_list );
    printk( "%d\n", my_obj->value );
  }

  printk("Emit odd list\n");
  list_for_each_entry( my_obj, &my_odd_list, odd_list ) {
    printk( "%d\n", my_obj->value );
  }

  printk("Emit Even list\n");
  list_for_each_entry(my_obj, &my_even_list, even_list ) {
	printk("%d\n",my_obj->value );
 }

  printk("Cleaning up\n");
  list_for_each_safe( pos, q, &my_full_list ) {
    struct my_data_struct *tmp;
    tmp = list_entry( pos, struct my_data_struct, full_list );
    list_del( pos );
    kfree( tmp );
  }

  return;
}

