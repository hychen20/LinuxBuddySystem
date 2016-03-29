#include <iostream>
#include <vector>
#include "types.h"
#include "helper.h"
#include "list.h"

#define MAX_ORDER 11

using namespace std;

extern void *mem_map_base;

// Sets page size in KB
static const int page_size = 4;

// Ram size in GB
static const int ram_size = 16;

// Number of frames
// static const unsigned long
// nPages = int((ram_size * pow(2, 30))/(page_size * pow(2,10)));
static const unsigned long nPages = 4194304; // (2^22)

class Buddy {
  Buddy() {};
  Buddy(const std::vector<page> &mem_map);
private:
  struct free_area free_area[MAX_ORDER];

public:
  unsigned int fix_order(const unsigned int page_num);
  struct page * alloc_pages(const unsigned int order);
  void free_pages(struct page *page, unsigned int order);
};

unsigned int Buddy::fix_order(const unsigned int page_num) {
  //TODO
  return 0;
}

Buddy::Buddy(const std::vector<page> &mem_map) {
  int order;
  int pfn;
  for (order = 0; order < MAX_ORDER; order++) {
    // init empty list
    INIT_LIST_HEAD(&free_area[order].free_list);
  }
  for (pfn = 0; pfn < nPages; pfn++) {
    page = &mem_map[pfn];
    __free_page(page);// add page to buddy system
  }
};

struct page * Buddy::alloc_pages(const unsigned int order) {
	struct free_area * area;
	unsigned int current_order;
	struct page *page;
	for (current_order = order; current_order < MAX_ORDER;
       ++current_order) {
		area = &free_area[current_order];
		if (list_empty(&area->free_list))
			continue;
		//Get first page of the free_list
		page = list_entry(area->free_list.next, struct page, lru);
		list_del(&page->lru);
    set_page_order(page, 0);
		// area->nr_free--;
		expand(page, order, current_order, area);
		return page;
	}
	return NULL;
}

void Buddy::free_pages(struct page *page, unsigned int order) {
	unsigned long buddy_idx;
  unsigned long page_idx;
	int order_size = 1 << order;
	page_idx = page_to_pfn(page) & ((1 << MAX_ORDER) - 1); // ?? very confusing
	while (order < MAX_ORDER-1) {
		unsigned long combined_idx;
		struct free_area *area;
		struct page *buddy;
    buddy_idx = __find_buddy_index(page_idx, order);
		buddy = page + (page_idx^(1<<order)-page_idx);
		if (!page_is_buddy(page, buddy, order))
			break;		/* Move the buddy up one level. */
		list_del(&buddy->lru);
		area = &free_area[order];
		// area->nr_free--;
		rmv_page_order(buddy);
		// get page_idx after combining
    combined_idx = buddy_idx & page_idx;
		// get first page after combining
		page = page + (combined_idx - page_idx);
		page_idx = combined_idx;
		order++;
	}
	set_page_order(page, order);
	list_add(&page->lru, free_area[order].free_list);
	// free_area[order].nr_free++;
}

int main() {
  // Declare mockup of ram memory
  // struct page mem_map[nPages];
  std::vector<page> mem_map(nPages);
  mem_map_base = (void*)&mem_map;

  struct page * page;
  page = alloc_pages(0);
  free_pages(page, 0);
}

