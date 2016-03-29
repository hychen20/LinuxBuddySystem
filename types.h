#ifndef _TYPES_H
#define _TYPES_H

#define PAGE_BUDDY_MAPCOUNT_VALUE (-128)

void *mem_map_base;

struct list_head {
  struct list_head *next, *prev;
};

struct page {
  struct list_head lru;
  unsigned long flag;
  unsigned long _private;
  int _mapcount;
};

struct free_area {
  struct list_head free_list;
  unsigned long nr_free;
};


#endif
