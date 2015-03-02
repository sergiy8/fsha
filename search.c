#if 0
static void *linux_bsearch(const void *key, const void *base, size_t num, size_t size,
int (*cmp)(const void *key, const void *elt))
{
	size_t start = 0, end = num;
	int result;

	while (start < end) {
		size_t mid = start + (end - start) / 2;
		result = cmp(key, base + mid * size);
		if (result < 0)
			end = mid;
		else if (result > 0)
			start = mid + 1;
		else
		return (void *)base + mid * size;
}
return NULL;
}
#endif
PROCTYPE unsigned search(uint32_t x, const uint32_t * array, unsigned end){
	unsigned start = 0;
	while(start < end ) {
		unsigned mid = start + (end-start)/2;
		if(array[mid] == x) return mid;
		if(array[mid] < x)
				start = mid + 1;
			else
				end = mid;
	}
	return -1;
}
