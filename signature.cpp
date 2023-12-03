#include <string.h>
#include <stdlib.h>
#include "signature.h"

#ifdef WIN32
#include <windows.h>
//#include <TlHelp32.h>
#else
#include <sys/mman.h>
#include <unistd.h>
//#include <link.h>
#endif

#define HEADER_LEN	2
#define LEN_BYTE	0
#define OFF_BYTE	1

/*
uintptr_t get_func(void *addr, const char *func)
{
#ifdef WIN32
	return (uintptr_t)GetProcAddress((HMODULE)addr, func);
#else
	uintptr_t ptr = 0;
	Dl_info info;
	if(dladdr(addr, &info)){
		void *handle = dlopen(info.dli_fname, RTLD_NOW);
		if(handle){
			ptr = (uintptr_t)dlsym(handle, func);
			dlclose(handle);
		}
	}
	return ptr;
#endif
}

#ifndef WIN32
static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
	mem_info *d = (mem_info *)data;
	if(!info->dlpi_name || !strstr(info->dlpi_name, d->name)) return 0; // path
	d->addr = (void *)info->dlpi_addr;
	d->len = info->dlpi_phdr[0].p_filesz; // p_type=1 p_offset=0
	return 1;
}
#endif

bool find_base(mem_info *data)
{
	data->addr = NULL;
	data->len = 0;
#ifdef WIN32
	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
	if(hModuleSnap==INVALID_HANDLE_VALUE) return false;
	bool succ = false;
	MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
	while(Module32Next(hModuleSnap, &me32)){ // srcds
		if(!strcmp(me32.szModule, data->name)){
			data->addr = (void *)me32.modBaseAddr;
			data->len = (size_t)me32.modBaseSize;
			succ = true;
		}
	}
	CloseHandle(hModuleSnap);
	return succ;
#else
	return dl_iterate_phdr(callback, (void *)data)==1;
#endif
}

uintptr_t find_signature(uchar *mask, mem_info *base, bool pure)
{
	if(!base->addr) return 0;
	uchar *p = (uchar *)base->addr;
	size_t len = mask[LEN_BYTE];
	uchar *end = p+(base->len-len);
#ifndef WIN32
	mlock(base->addr, base->len);
#endif
	uintptr_t ret = 0;
	while(p<end){
		uint i = LEN_BYTE+1;
		for(uchar *tmp = p; i<=len; ++i, ++tmp){
			if(!pure && mask[i]==0xC3U) continue;
			if(mask[i]!=*tmp) break;
		}
		if(--i==len){
			ret = (uintptr_t)p;
			break;
		}
		p++;
	}
#ifndef WIN32
	munlock(base->addr, base->len);
#endif
	return ret;
}
*/

void read_signature(uintptr_t addr, uchar *new_sign, uchar *&org_sign)
{
	if(!addr) return;
	size_t len = new_sign[LEN_BYTE];
	org_sign = (uchar *)malloc(len+HEADER_LEN);
	if(!org_sign) return;
	memcpy((void *)org_sign, (void *)new_sign, HEADER_LEN);
	void *src = (void *)(addr+((char *)new_sign)[OFF_BYTE]);
	void *dst = (void *)(org_sign+HEADER_LEN);
	memcpy(dst, src, len);
}

void write_signature(uintptr_t addr, uchar *sign)
{
	if(!addr || !sign) return;
	size_t len = sign[LEN_BYTE];
	void *src = (void *)(sign+HEADER_LEN);
	void *dst = (void *)(addr+((char *)sign)[OFF_BYTE]);
#ifdef WIN32
	DWORD old;
	VirtualProtect(dst, len, PAGE_EXECUTE_READWRITE, &old);
	memcpy(dst, src, len);
	VirtualProtect(dst, len, old, &old);
#else
	void *pa_addr = (void *)((uintptr_t)dst&~(sysconf(_SC_PAGESIZE)-1));
	size_t size = (uintptr_t)dst-(uintptr_t)pa_addr+len;
	mlock(pa_addr, size);
	mprotect(pa_addr, size, PROT_READ|PROT_WRITE|PROT_EXEC);
	memcpy(dst, src, len);
	//mprotect(pa_addr, size, PROT_READ|PROT_EXEC); // restore
	munlock(pa_addr, size);
#endif
}

void free_signature(uintptr_t addr, uchar *&sign)
{
	write_signature(addr, sign);
	free((void *)sign);
	sign = NULL;
}
