#ifndef SIGNATURE_H
#define SIGNATURE_H

#ifdef WIN32
typedef unsigned int uint;
typedef unsigned long long uint64;
#endif
typedef unsigned int uintptr_t;
typedef unsigned char uchar;

/*
typedef struct {
	const char *name;
	void *addr;
	size_t len;
} mem_info;

uintptr_t get_func(void *addr, const char *func);
bool find_base(mem_info *data);
uintptr_t find_signature(uchar *mask, mem_info *base, bool pure);
*/
void read_signature(uintptr_t addr, uchar *new_sign, uchar *&org_sign);
void write_signature(uintptr_t addr, uchar *sign);
void free_signature(uintptr_t addr, uchar *&sign);

#endif // SIGNATURE_H
