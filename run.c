#include <sys/types.h>
#include <sys/resource.h>
#include <limits.h>

#include "run.h"
#include "util.h"

void *base = 0;

// RETURN NULL IF NOT EXIST PROPER SPACE

p_meta find_meta(p_meta *last, size_t size) {
	p_meta index = base;
	p_meta result = NULL;

	switch(fit_flag){
		case FIRST_FIT:
			{
				while(index != NULL){
					if(index->free == 0 && index->size >= size){
						result = index;
						break;
					}

					index = (index->next);
				}
			}
			break;

		case BEST_FIT:
			{
				p_meta best = NULL;
				

				while(index != NULL){
					if((index->size) >=size){
						if(best == NULL)
							best = index;
						else if(best->size > index->size)
							best = index;
					}
					index = (index->next);
				}
			}
			break;

		case WORST_FIT:
			{
				p_meta worst = NULL;

				while(index != NULL){
					if((index->size) >= size){
						if(worst == NULL)
							worst = index;
						else if(best->size < index->size)
							worst = index;
					}
					index = (index->next);
				}
			}
			break;

	}
	return result;
}

void *m_malloc(size_t size) {
	p_meta find;

	find = find_meta(end, size);

	// find proper place

	if(list != NULL){
		// fill 0 
		memset(find, 0, size + META_SIZE);
		find->free = 1;

		// if exist rest 
		if(find->size != size + META_SIZE){
			p_meta list = find + size + META_SIZE;
			set(list, 0, find->next, find, (find->size)-size-META_SIZE);
			list->next = find->next;
		}
		
		find->size -= size + META_SIZE;
		
		//TODO : MERGING IF NEXT THING IS FREE
		
		return find->data;
	}
	else{ // add new place
		if(base == 0)
			base = end = sbrk(0);

		//if cannot allocate new things, return NULL

		if(sbrk(end+size+META_SIZE) == -1) 
			return NULL;

		end += size + META_SIZE; 
		
		// make new one
		p_meta list;

		set(list, 0, end, NULL, size + META_SIZE);

		find->next = list;

		return list->data;
	}
}

void m_free(void *ptr) {

}

void* m_realloc(void* ptr, size_t size)

}

void set(p_meta x, int free, p_meta next, p_meta prev, int size){
	(x->free) = free;
	(x->next) = next;
	(x->prev) = prev;
	(x->size) = size;
}

