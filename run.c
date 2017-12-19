#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>
#include <stdio.h>

#include "run.h"
#include "util.h"

#define NULL (void*)0

void *end = 0;
void *base = 0;
void *last = 0;

// RETURN 0 IF NOT EXIST PROPER SPACE

p_meta find_meta(p_meta *last, size_t size) {
	p_meta index = base;
	p_meta result = 0;

	if(last == base) return 0;

	switch(fit_flag){
		case FIRST_FIT:
			{
				while(index != 0){
					if(index->free != 0 && index->size >= size){
						result = index;
						break;
					}

					index = (index->next);
				}
			}
			break;

		case BEST_FIT:
			{
				p_meta best = 0;	

				while(index != 0){
					if(index->free != 0)
						if((index->size) >=size){
							if(best == 0)
								best = index;
							else if(best->size > index->size)
								best = index;
						}
					index = (index->next);
				}
				result = best;
			}
			break;

		case WORST_FIT:
			{
				p_meta worst = 0;

				while(index != 0){
					if(index->free !=0)
						if((index->size) >= size){
							if(worst == 0)
								worst = index;
							else if(worst->size < index->size)
								worst = index;
						}
					index = (index->next);
				}
				result = worst;
			}
			break;

	}
	return result;
}

void *m_malloc(size_t size) {

	p_meta find;

	size = ((size+3)/4)*4;

	find = find_meta(end, size);

	// find proper place

	if(find != 0){
		m_realloc(find->data, size);
		return find->data;
	}

	// new area
	else{
		if(base == 0)
			base = end = find = last = sbrk(0);

		if(sbrk(end+size+META_SIZE) == -1) 
			return (void*)0;

		p_meta list = end;
		p_meta cur = last;

		if(last != list){
			cur->next = list;
			list->prev = last;
		}
		else
			list->prev = 0;

		list->free = 0;
		list->next = 0;
		list->size = size;
		last = list;

		end += size + META_SIZE;

		return list->data;
	}
}

void m_free(void *ptr) {
	p_meta x = ptr - META_SIZE;

	ptr = 0;

	x->free = 1;

	if(x->prev != 0){
		if((x->prev)->free == 1){
			(x->prev)->size += x->size + META_SIZE;
			(x->prev)->next = x->next;
			x = x->prev;
		}
	}


	if(x->next != 0){
		if((x->next)->free == 1){
			x->size += (x->next)->size + META_SIZE;
			x->next = (x->next)->next;
		}
	}

	// totally empty
	if(x->next == 0 && x->prev == 0)
		end = last = base;
}

void* m_realloc(void* ptr, size_t size){
	p_meta x = ptr - META_SIZE;
	size = (size+3)/4*4;
	x->free = 0;

	if((x->next)->free == 1){
		x->size += (x->next)->size + META_SIZE;
		x->next = (x->next)->next;
		x->next = 0;
	}

	if(x->size < size){
		// divide if rest thing is bigger than META_SIZE

		if(x->size - size > META_SIZE){
			p_meta list = x + size + META_SIZE;

			list->prev = x;
			list->size = (x->size) - size - META_SIZE;
			list->next = x->next;

			x->next = list;
			x->size = size;
		}
		else return x->data;
	}
	else if((x->size) < size){
		x->free = 1;
		// make new thing like m_malloc
	}
	else return ptr;
}

