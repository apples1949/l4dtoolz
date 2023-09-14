#include "interface.h"
#include "eiface.h"
#include "tier1/tier1.h"

#include "signature.h"

#define BINMSIZE	0x1400000 // 20M
#define CHKPTR(P, V)	(P && !((uint)(P)&V))
#define CMPPTR(P, V, C)	(CHKPTR(P, V) && abs((int)P-(int)C)<BINMSIZE)
#define READCALL(P)	((P+5-1)+*(int *)(P))
#define CHKVAL \
	int new_value = ((ConVar *)var)->GetInt(); \
	int old_value = atoi(pOldValue); \
	if(new_value==old_value) return;

#pragma pack(push, 1)
struct ValidateAuthTicketResponse_t{
	uint64 id;
	int code;
	uint64 owner;
};
#pragma pack(pop)

struct netadr_s{
	int type;
	unsigned char ip[4];
	unsigned short port;
};

class l4dtoolz:public IServerPluginCallbacks{
public:
	virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void Unload();
	virtual void Pause(){ }
	virtual void UnPause(){ }
	virtual const char *GetPluginDescription(){ return "L4DToolZ v2.2.5, https://github.com/lakwsh/l4dtoolz"; }
	virtual void LevelInit(char const *pMapName){ }
	virtual void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void GameFrame(bool simulating){ }
	virtual void LevelShutdown(){ }
	virtual void ClientActive(edict_t *pEntity){ }
	virtual void ClientDisconnect(edict_t *pEntity){ }
	virtual void ClientPutInServer(edict_t *pEntity, char const *playername){ }
	virtual void SetCommandClient(int index){ }
	virtual void ClientSettingsChanged(edict_t *pEdict){ }
	virtual PLUGIN_RESULT ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual PLUGIN_RESULT ClientCommand(edict_t *pEntity, const CCommand &args){ return PLUGIN_CONTINUE; }
	virtual PLUGIN_RESULT NetworkIDValidated(const char *pszUserName, const char *pszNetworkID){ return PLUGIN_CONTINUE; }
	virtual void OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue){ }

	static void Cookie_f(const CCommand &args);
	static void OnChangeMax(IConVar *var, const char *pOldValue, float flOldValue);
	static void OnSetMaxCl(IConVar *var, const char *pOldValue, float flOldValue);
	static void OnBypassAuth(IConVar *var, const char *pOldValue, float flOldValue);
	static void OnAntiSharing(IConVar *var, const char *pOldValue, float flOldValue);
	static void OnForceUnreserved(IConVar *var, const char *pOldValue, float flOldValue);

#ifdef WIN32
	static void PostAuth(ValidateAuthTicketResponse_t *);
#else
	static void PostAuth(void *, ValidateAuthTicketResponse_t *);
#endif
	static void ConnectionStart(uint ***);
private:
	static uint *slots_ptr;
	static uint64 *cookie_ptr;
	static uint *maxcl_ptr;
	static uint *gamerules_ptr;
	static void *rules_max_ptr;
	static void *rules_max_org;
	static void *dsp_max_ptr;
	static void *dsp_max_org;
	static void *lobby_req_ptr;
	static void *lobby_req_org;
	static uint *steam3_ptr;
	static void *authreq_ptr;
	static void *authreq_org;
	static uint *authrsp_ptr;
	static uint authrsp_org;
	static uint *tickint_ptr;
	static void *tickint_org;
	static void *set_rate_ptr;
	static void *set_rate_org;
};
extern l4dtoolz g_l4dtoolz;

class Handler{
public:
	virtual	~Handler(){ }
	virtual void ConnectionStart(uint ***chan){ l4dtoolz::ConnectionStart(chan); }
	virtual void ConnectionClosing(const char *){ }
	virtual void ConnectionCrashed(const char *){ }
};
