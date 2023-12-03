#ifndef SIGNATURE_LINUX_H
#define SIGNATURE_LINUX_H

#define tickint_idx	0x09 // rodata
#define rate_idx	0x2C
#define info_off	0x01
#define info_idx	0x89
#define sv_off		0x06 // bss
#define edict_idx	0x85
#define slots_idx	0x5F
#define cookie_idx	0x5B
#define maxcl_idx	0x41
#define steam3_idx	0x41
#define steam3_off	0x0D
#define authreq_idx	0x1A // rodata (?)
#define authrsp_idx	0x24 // bss
#define lobbyreq_idx	0x3E // rodata

uchar max_player_new[] = {0x06, 0x00, 0xB8, '*', 0x00, 0x00, 0x00, 0xC3};
uchar lobby_req_new[] = {0x04, 0x00, '*', '*', '*', '*'};

#endif // SIGNATURE_LINUX_H
