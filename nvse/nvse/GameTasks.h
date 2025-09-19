#pragma once

#include "GameTypes.h"
#include "NiTypes.h"
//#include "NiNodes.h"

/*******************************************************
*
* BSTask
*	IOTask
*		QueuedFile
*			QueuedFileEntry
*				QueuedModel
*					QueuedDistantLOD
*					QueuedTreeModel
*				QueuedTexture
*					QueuedTreeBillboard
*				QueuedKF
*					QueuedAnimIdle
*				DistantLODLoaderTask
*				TerrainLODQuadLoadTask
*				SkyTask
*				LipTask
*				GrassLoadTask
*			QueuedReference
*				QueuedTree
*				QueuedActor
*					QueuedCharacter
*						QueuedPlayer
*					QueuedCreature
*			QueuedHead
*			QueuedHelmet
*			QueuedMagicItem
*		AttachDistant3DTask
*		ExteriorCellLoaderTask
*
* NiTArray< NiPointer<QueuedFile> >
*	QueuedChildren
*
*********************************************************/

class TESObjectREFR;
class TESModel;
class QueuedCharacter;
class TESNPC;
class BSFaceGenNiNode;
class BackgroundCloneThread;
class TESAnimGroup;
class BSFaceGenModel;
class QueuedChildren;
class QueuedReference;
class Character;
class AttachDistant3DTask;
class BSTaskManagerThread;

class ModelLoader;

class NiNode;
class NiControllerSequence;
class NiRefObject;
class RefNiObject;

class BSAnimGroupSequence;
struct BSAData;

class RefNiRefObject
{
public:
	NiRefObject*	niRefObject;
	
	//	RefNiRefObject SetNiRefObject(NiRefObject* niRefObject);
};

class Model  // NiObject
{
public:
	const char* path;		// 00
	UInt32		counter;	// 04
	NiNode* niNode;	// 08

};
// 18
class BSTask
{
public:
	virtual void Destroy(bool doFree);
	virtual void Run(void) = 0;
	virtual void Unk_02(void) = 0;
	virtual void Unk_03(UInt32 arg0, UInt32 arg1);						// doesNothing
	virtual bool GetDebugDescription(char* outDesc, UInt32 * arg1) = 0;	// return 0

	// void		** vtbl
	
	BSTask	* unk004;	// uninitialized OBSE, not confirmed, NiRefObject
	UInt32	refCounter; // Counter: NiRefObject RefCounter
	UInt32	unk00C;		// Semaphore/Status
	UInt32	unk010;		// Paired : 10 and 14 as a 64 bit integer, it could be very large flag bits or an integer
	UInt32	unk014;

	static UInt32* GetCounterSingleton() { return (UInt32*)0x11C3B38; }
};

// 18
class IOTask : public BSTask
{
public:
	virtual void Unk_05(void);			// doesNothing
	virtual void Unk_06(void);				
	virtual void Unk_07(UInt32 arg0);	// most (all?) implementations appear to call IOManager::00C3DF40(this, arg0) eventually. It updates the bits 23/32 of the giant bit flag possibly.

	IOTask();
	~IOTask();

};

class QueuedFile;

// 014
class QueuedChildren : public BSSimpleArray<NiPointer<QueuedFile>>
{
	UInt32	counter;
};

// 028
class QueuedFile : public IOTask
{
public:
	QueuedFile();
	~QueuedFile();

	//Unk_01:	doesNothing
	//Unk_02:	virtual void Call_Unk_0A(void);
	//Unk_03:	implemented
	//Unk_07:	recursivly calls Unk_07(arg_0) on all non null children before calling its parent.
	virtual void DoRequestHandles();
	virtual void DoOnChildrenFinished(UInt32 arg0);
	virtual void DoOnTaskFinished();

	// size?
	struct FileEntry {
		UInt32		unk00;
		UInt32		unk04;
		UInt32		size;
		UInt32		offset;
	};

	BSTask			* unk018;			// 018 init to tlsData:2B4 not confirmed OBSE for QueuedModel, seen QueuedReference (ref to which model will be attached)
	QueuedReference	* queuedRef;		// 01C could be last QueuedRef
	QueuedChildren	* queuedChildren;	// 020
	UInt32			* unk024;			// 024	struct, 004 is a base, 008 is a length
};

// 40
class QueuedReference : public QueuedFile
{
public:
	QueuedReference();
	~QueuedReference();

	virtual void QueueModels();			// Initialize bipedAnims (and cretae the 3D model?)
	virtual void UseDistant3D();
	virtual void AttachDistant3D(NiNode* arg0);
	virtual bool BackgroundClone();
	virtual void DoAttach();
	virtual void FinishAttach();

	TESObjectREFR	* refr;				// 028 
	BSTask* task2C;			// OBSE QueuedChildren	* queuedChildren;	// 02C
	Model* model;			// 030 
	NiNode* resultObj;			// 034 
	BSTask* task38;			// 038 
	UInt32			unk3C;				// 03C uninitialized
};

// 40
class QueuedActor : public QueuedReference
{
public:
	QueuedActor();
	~QueuedActor();
};

// 40
class QueuedCreature : public QueuedActor
{
public:
	QueuedCreature();
	~QueuedCreature();
};

// 48
class QueuedCharacter : public QueuedActor
{
public:
	QueuedCharacter();
	~QueuedCharacter();

	typedef RefNiRefObject RefQueuedHead;

	RefQueuedHead	* refQueuedHead;	// 040
	RefNiRefObject	* unk044;	// 044

};

// 48
class QueuedPlayer : public QueuedCharacter
{
public:
	QueuedPlayer();
	~QueuedPlayer();
};

// 030
class QueuedFileEntry : public QueuedFile
{
public:
	QueuedFileEntry();
	~QueuedFileEntry();

	virtual bool Unk_0B(void) = 0;

	char	* name;		// 028
	BSAData	* bsaData;	// 02C
};


// 44
class QueuedModel : public QueuedFileEntry
{
public:
	QueuedModel();
	~QueuedModel();

	virtual void Unk_0C(UInt32 arg0);

	Model		* model;		// 030
	TESModel	* tesModel;		// 034
	UInt32		baseFormClass;	// 038	table at offset : 0x045C708. Pickable, invisible, unpickable ? 6 is VisibleWhenDistant or internal
	UInt8		flags;			// 03C	bit 0 and bit 1 init'd by parms, bit 2 set after textureSwap, bit 3 is model set, bit 4 is file found.
	UInt8		pad03D[3];		// 03D
	float		flt040;			// 040

	// There are at least 3 Create/Initiator
};

// 30
class QueuedTexture : public QueuedFileEntry
{
public:
	QueuedTexture();
	~QueuedTexture();

	void	* niTexture;	// 030
};

// 014
class KFModel
{
	const char			* path;					// 000
	BSAnimGroupSequence	* controllerSequence;	// 004
	TESAnimGroup		* animGroup;			// 008
	UInt32				unk0C;					// 00C
	UInt32				unk10;					// 010

	__forceinline KFModel* Init(const char* kfPath, void* stream)
	{
		return ThisStdCall<KFModel*>(0x43B640, this, kfPath, stream);
	}
};

// 30
class QueuedKF : public QueuedFileEntry
{
public:
	QueuedKF();
	~QueuedKF();

	KFModel		* kf;		// 030
	UInt8		unk034;		// 034
	UInt8		pad035[3];	// 035
};

// 040
class QueuedAnimIdle : public QueuedKF
{
public:
	QueuedAnimIdle();
	~QueuedAnimIdle();

	ModelLoader	* modelLoader;	// 038	Init"d by arg2
	RefNiObject	* unk03C;		// 03C	Init"d by arg1
};

// 38
class QueuedHead : public QueuedFile
{
public:
	QueuedHead();
	~QueuedHead();

	TESNPC			* npc;				// 028
	BSFaceGenNiNode * faceNiNodes[2];	// 02C OBSE presumably male and female
	UInt32			unk034;				// 034
};

/*
// 38
class QueuedHelmet : public QueuedFile
{
public:
	QueuedHelmet();
	~QueuedHelmet();

	QueuedCharacter		* queuedCharacter;		// 18
	QueuedChildren		* queuedChildren;		// 1C
	void				* unk20;				// 20
	QueuedModel			* queuedModel;			// 24
	BSFaceGenModel		* faceGenModel;			// 28
	NiNode				* niNode;				// 2C
	Character			* character;			// 30
	UInt32				unk34;					// 34
};

// 30
class BSTaskManager : public LockFreeMap< NiPointer< BSTask > >
{
public:
	virtual void Unk_0F(UInt32 arg0) = 0;
	virtual void Unk_10(UInt32 arg0) = 0;
	virtual void Unk_11(UInt32 arg0) = 0;
	virtual void Unk_12(void) = 0;
	virtual void Unk_13(UInt32 arg0) = 0;

	UInt32				unk1C;			// 1C
	UInt32				unk20;			// 20
	UInt32				numThreads;		// 24
	BSTaskManagerThread	** threads;		// 28 array of size numThreads
	UInt32				unk2C;			// 2C
};

// 3C
class IOManager : public BSTaskManager
{
public:
	virtual void Unk_14(UInt32 arg0) = 0;

	static IOManager* GetSingleton();

	UInt32									unk30;			// 30
	LockFreeQueue< NiPointer< IOTask > >	* taskQueue;	// 34
	UInt32									unk38;			// 38

	bool IsInQueue(TESObjectREFR *refr);
	void QueueForDeletion(TESObjectREFR* refr);
	void DumpQueuedTasks();
};

extern IOManager** g_ioManager;
*/

// O4 assumed
class InterfacedClass {
	virtual void Destroy(bool doFree);
	virtual void AllocateTLSValue(void) = 0;		// not implemented
};

// 40
template<typename T_Key, class T_Data>
class LockFreeMap: public InterfacedClass
{
	// 0C
	struct Data004
	{
		UInt32	unk000;		// 00
		UInt32	unk004;		// 04
		UInt32	*unk008;	// 08
	};

	// 24
	struct TLSValue
	{
		LockFreeMap	*map;				// 00
		UInt32		mapData004Unk000;	// 04
		UInt32		mapData004Unk008;	// 08
		UInt32		*mapData004Unk00C;	// 0C	stores first DWord of bucket during lookup, next pointer is data, next flags bit 0 is status ok/found
		UInt32		unk010;				// 10	stores bucketOffset during lookup
		UInt32		*unk014;			// 14	stores pointer to bucket during lookup
		UInt32		*unk018;			// 18
		UInt32		unk01C;				// 1C
		UInt32		unk020;				// 20
	};

	// 10
	struct Data014
	{
		// 08
		struct Data008
		{
			UInt32		threadID;	// 00 threadID
			TLSValue	*tlsValue;	// 04 lpTlsValue obtained from AllocateTLSValue of LockFreeMap
		};

		UInt32	alloc;			// Init'd to arg0, count of array at 008
		UInt32	tlsDataIndex;	// Init'd by TlsAlloc
		Data008	**dat008;		// array of pair of DWord
		UInt32	count;		// Init'd to 0
	};	// most likely an array or a map

	struct Entry
	{
		T_Key		key;
		T_Data		data;
		Entry* next;
	};

	struct Bucket
	{
		Entry* entries;
	};

	Data004	**dat004;		// 04 array of arg0 12 bytes elements (uninitialized)
	UInt32	numBuckets;	// 08 Init'd to arg1, count of DWord to allocate in array at 000C
	Bucket *buckets;		// 0C array of arg1 DWord elements
	UInt32	unk010;			// 10 Init'd to arg2
	Data014	*dat014;		// 14 Init'd to a 16 bytes structure
	UInt32	numItems;			// 18
	UInt32	unk01C;			// 1C
	LightCS	semaphore;		// 20 
	UInt32	unk028[6];		// 28 
	
	Bucket* GetBuckets() const { return buckets; }
	Bucket* End() const { return buckets + numBuckets; }

public:
	/*08*/virtual bool		Lookup(T_Key key, T_Data* result);
	/*0C*/virtual bool		Unk_03(UInt32 arg1, UInt32 arg2, UInt32 arg3, UInt8 arg4);
	/*10*/virtual bool		Insert(T_Key key, T_Data* dataPtr, UInt8 arg3);
	/*14*/virtual bool		EraseKey(T_Key key);
	/*18*/virtual bool		Unk_06(UInt32 arg1, UInt32 arg2);
	/*1C*/virtual bool		Unk_07(UInt32 arg);
	/*20*/virtual bool		Unk_08(UInt32 arg1, UInt32 arg2);
	/*24*/virtual UInt32	CalcBucketIndex(T_Key key);
	/*28*/virtual void		FreeKey(T_Key key);
	/*2C*/virtual T_Key		GenerateKey(T_Key src);
	/*30*/virtual void		CopyKeyTo(T_Key src, T_Key* destPtr);
	/*34*/virtual bool		LKeyGreaterOrEqual(T_Key lkey, T_Key rkey);
	/*38*/virtual bool		KeysEqual(T_Key lkey, T_Key rkey);
	/*3C*/virtual UInt32	IncNumItems();
	/*40*/virtual UInt32	DecNumItems();
	/*44*/virtual UInt32	GetNumItems();

	UInt32 Size() const { return numItems; }
	bool Empty() const { return !numItems; }
	UInt32 BucketCount() const { return numBuckets; }

	class Iterator
	{
		LockFreeMap* table;
		Bucket* bucket;
		Entry* entry;

		void FindNonEmpty()
		{
			for (Bucket* end = table->End(); bucket != end; bucket++)
				if (entry = bucket->entries) return;
		}

	public:
		Iterator(LockFreeMap& _table) : table(&_table), bucket(table->buckets), entry(nullptr) { FindNonEmpty(); }

		explicit operator bool() const { return entry != nullptr; }
		void operator++()
		{
			entry = entry->next;
			if (!entry)
			{
				bucket++;
				FindNonEmpty();
			}
		}
		T_Data Get() const { return entry->data; }
		T_Key Key() const { return entry->key; }
	};

	Iterator Begin() { return Iterator(*this); }
};
static_assert(sizeof(LockFreeMap<int, int>) == 0x40);

template<class _C>
class LockFreeStringMap: LockFreeMap<char const *, _C> {};

template<class _C>
class LockFreeCaseInsensitiveStringMap: LockFreeStringMap<_C> {};
class QueuedReplacementKFList;
class QueuedHelmet;
class BSFileEntry;
class LoadedFile;
class Animation;
// 1C
class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	// #TODO: generalize key for LockFreeMap, document LockFreeStringMap

	LockFreeMap<const char*, Model *>				* modelMap;				// 00
	LockFreeMap<const char*, KFModel*>				* kfMap;				// 04
	LockFreeMap<TESObjectREFR*,QueuedReference*>	* refMap;				// 08 key is TESObjectREFR*
	LockFreeMap<TESObjectREFR*, QueuedReference*>	* refMap2;
	LockFreeMap<void*, QueuedAnimIdle*>				* idleMap;				// 10 key is AnimIdle*
	LockFreeMap<Animation*, QueuedReplacementKFList*>* animMap;				// 14 
	LockFreeMap<TESObjectREFR*, QueuedHelmet*>		* helmetMap;			// 18
	void*											attachQueue;			// 1C LockFreeQueue<AttachDistant3DTask*>*
	LockFreeMap<BSFileEntry*, QueuedTexture*>		* textureMap;			// 20
	LockFreeMap<const char*, LoadedFile*>			* fileMap;				// 24
	BackgroundCloneThread							* bgCloneThread;		// 28
	UInt8											byte2C;				// 2C
	UInt8											pad2D[3];			// 2D
	
	//LockFreeMap< NiPointer<QueuedAnimIdle *> >		* idleMap;					// 0C key is AnimIdle* (strange same constructor as for 08)
	//LockFreeMap< NiPointer<QueuedHelmet *> >			* helmetMap;				// 10 key is TESObjectREFR*
	//LockFreeQueue< NiPointer<AttachDistant3DTask *> >	* distant3DMap;				// 14
	//BackgroundCloneThread								* bgCloneThread;			// 18


	__forceinline static ModelLoader* GetSingleton() { return *(ModelLoader**)0x11C3B3C; }

	void QueueReference(TESObjectREFR* refr, UInt32 arg1, bool ifInMainThread);
};

