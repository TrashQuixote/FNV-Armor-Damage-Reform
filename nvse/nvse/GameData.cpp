#include "GameData.h"

#if RUNTIME

DataHandler* DataHandler::Get()
{
	DataHandler** g_dataHandler = (DataHandler**)0x011C3F2C;
	return *g_dataHandler;
}
#else

DataHandler* DataHandler::Get()
{
	DataHandler** g_dataHandler = (DataHandler**)0xED3B0C;
	return *g_dataHandler;
}

#endif

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(ModInfo* modInfo)
	{
		return !StrCompare(modInfo->name, m_stringToFind);
	}
};

const ModInfo * DataHandler::LookupModByName(const char * modName)
{
	return modList.modInfoList.Find(LoadedModFinder(modName));
}

const ModInfo ** DataHandler::GetActiveModList()
{
	static const ModInfo* activeModList[0x100] = { 0 };

	if (!(*activeModList))
	{
		UInt16 index = 0;
		for (index = 0  ; index < DataHandler::Get()->modList.modInfoList.Count() ; index++)
		{
			ModInfo* entry = DataHandler::Get()->modList.modInfoList.GetNthItem(index);
			if (entry->IsLoaded())
				activeModList[index] = entry;
		}
	}

	return activeModList;
}

UInt8 DataHandler::GetModIndex(const char* modName)
{
	return modList.modInfoList.GetIndexOf(LoadedModFinder(modName));
}

const char* DataHandler::GetNthModName(UInt32 modIndex)
{
	const ModInfo** activeModList = GetActiveModList();
	if (modIndex < GetActiveModCount() && activeModList[modIndex])
		return activeModList[modIndex]->name;
	else
		return "";
}

void DataHandler::DisableAssignFormIDs(bool shouldAsssign)
{
	ThisStdCall(0x464D30, this, shouldAsssign);
}

struct IsModLoaded
{
	bool Accept(ModInfo* pModInfo) const {
		return pModInfo->IsLoaded();
	}
};

UInt8 DataHandler::GetActiveModCount() const
{
	UInt32 count = modList.modInfoList.CountIf(IsModLoaded());
	return count;
}

ModInfo::ModInfo() {
	//
};

ModInfo::~ModInfo() {
	//
};

TES* g_TES = TES::GetSingleton();
GridCellArray* g_gridCellArray = g_TES->gridCellArray;
__declspec(naked) TESObjectCELL* __vectorcall GridCellArray::GetCellAtCoord(__m128i cellXY) const
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		movd	eax, xmm0
		movsx	edx, ax
		sar		eax, 0x10
		mov		ecx, [ebx + 0xC]
		shr		ecx, 1
		sub		eax, [ebx + 4]
		add		eax, ecx
		cmp		eax, [ebx + 0xC]
		jnb		retnNull
		sub		edx, [ebx + 8]
		add		edx, ecx
		cmp		edx, [ebx + 0xC]
		jnb		retnNull
		add		edx, eax
		shl		eax, cl
		add		edx, eax
		mov		ecx, [ebx + 0x10]
		mov		eax, [ecx + edx * 4]
		pop		ebx
		retn
	retnNull :
		xor eax, eax
			pop		ebx
			retn
	}
}
