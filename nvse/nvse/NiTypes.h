#pragma once
#include "Utilities.h"
#if RUNTIME


const UInt32 _NiTMap_Lookup = 0x00853130;

#endif
// 8
struct NiRTTI
{
	const char	* name;
	NiRTTI		* parent;
};


// C
struct NiVector3
{
	float	x, y, z;

	NiVector3() : x(0),y(0),z(0) {}
	__forceinline NiVector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	__forceinline NiVector3(const NiVector3& rhs) { *this = rhs; }
	//__forceinline explicit NiVector3(const NiMatrix33& rhs) { *this = rhs; }
	__forceinline explicit NiVector3(const __m128 rhs) { SetPS(rhs); }

	__forceinline __m128 PS() const { return _mm_loadu_ps(&x); }
	__forceinline __m128 PS2() const { return _mm_castsi128_ps(_mm_loadu_si64(this)); }
	__forceinline __m128 PS3() const { return PS() & GET_PS(4); }
	__forceinline NiVector3& SetPS(const __m128 rhs)
	{
		
		_mm_storeu_si64(this, _mm_castps_si128(rhs));
		_mm_store_ss(&z, _mm_unpackhi_ps(rhs, rhs));
		return *this;
	}

	inline float __vectorcall DotProduct(const NiVector3& rhs) const
	{
		__m128 k = _mm_setzero_ps();
		return _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(PS3() * rhs.PS(), k), k));
	}

	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() - packedPS; }
	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline NiVector3& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }
	__forceinline NiVector3& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }
	__forceinline NiVector3& operator*=(float s) { return SetPS(*this * s); }
	__forceinline NiVector3& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }

	inline operator float* () { return &x; }
	__forceinline void operator=(const NiVector3& rhs)
	{
		_mm_storeu_si64(this, _mm_loadu_si64(&rhs));
		z = rhs.z;
	}
	__forceinline void operator *=(const float& flt){
		x *= flt;
		y *= flt;
		z *= flt;
	}
	__forceinline void operator *=(const NiVector3& rhs){
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
	}

	// add by me for convenience
	__forceinline void MoveAlone(const NiVector3& _direction, float depth) {
		__m128 _depthVec = _mm_set_ps(depth, depth, depth, depth);
		__m128 _direVec = _direction.PS();
		__m128 _increasement = _mm_mul_ps(_direVec,_depthVec );
		*this += _increasement;
	}

	
	inline float __vectorcall Length() const { return Length_V4(PS3()); }
	__m128 __vectorcall CrossProduct(const NiVector3& vB) const;
	NiVector3& Normalize();
};

float __vectorcall Point2Distance(const NiVector3& pt1, const NiVector3& pt2);
float __vectorcall Point3Distance(const NiVector3& pt1, const NiVector3& pt2);

__forceinline NiVector3 GetVector(const NiVector3& begin, const NiVector3& end) {
	return NiVector3{ (end.x - begin.x),(end.y - begin.y),(end.z - begin.z) };
}

__forceinline NiVector3 ScaleVector(const NiVector3& vec, const NiVector3& scale) {
	return NiVector3{ (vec.x * scale.x),(vec.y * scale.y),(vec.z * scale.z) };
}
__m128 __vectorcall Normalize_V4(__m128 inPS);
__m128 __vectorcall GetAnglesBetweenPoints(__m128 pt1, __m128 pt2);
// 10 - always aligned?
struct NiVector4
{
	float	x, y, z, w;
	NiVector4() {}
	__forceinline NiVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	__forceinline NiVector4(const NiVector4& rhs) { *this = rhs; }
	__forceinline NiVector4(const NiVector3& rhs) { *this = rhs; }
	__forceinline explicit NiVector4(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiVector4&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
	}
	__forceinline void operator=(const NiVector4& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiVector3& rhs) { SetPS(rhs.PS3()); }

	__forceinline NiVector4& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&x, rhs);
		return *this;
	}

	__forceinline __m128 operator+(const NiVector3& rhs) const { return PS() + rhs.PS3(); }
	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }

	__forceinline __m128 operator-(const NiVector3& rhs) const { return PS() - rhs.PS3(); }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(const NiVector3& rhs) const { return PS() * rhs.PS3(); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline NiVector4& operator+=(const NiVector3& rhs) { return SetPS(*this + rhs); }
	__forceinline NiVector4& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }

	__forceinline NiVector4& operator-=(const NiVector3& rhs) { return SetPS(*this - rhs); }
	__forceinline NiVector4& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }

	__forceinline NiVector4& operator*=(float s) { return SetPS(*this * s); }
	__forceinline NiVector4& operator*=(const NiVector3& rhs) { return SetPS(*this * rhs); }
	__forceinline NiVector4& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }

	inline operator float* () { return &x; }
	inline operator NiVector3& () const { return *(NiVector3*)this; }

	__forceinline __m128 PS() const { return _mm_loadu_ps(&x); }

	bool RayCastCoords(const NiVector3& posVector, float* rotMatRow, float maxRange, SInt32 layerType);
};

// 10 - always aligned?


// 24
struct NiMatrix33
{
	float	cr[3][3] ;

	NiMatrix33() {}
	__forceinline NiMatrix33(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22)
	{
		cr[0][0] = m00; cr[0][1] = m10; cr[0][2] = m20;
		cr[0][3] = m01; cr[0][4] = m11; cr[0][5] = m21;
		cr[0][6] = m02; cr[0][7] = m12; cr[0][8] = m22;
	}
	__forceinline explicit NiMatrix33(const NiMatrix33& from) { *this = from; }
	__forceinline void operator=(NiMatrix33&& rhs)
	{
		cr[0][0] = rhs.cr[0][0]; cr[0][1] = rhs.cr[0][1]; cr[0][2] = rhs.cr[0][2];
		cr[0][3] = rhs.cr[0][3]; cr[0][4] = rhs.cr[0][4]; cr[0][5] = rhs.cr[0][5];
		cr[0][6] = rhs.cr[0][6]; cr[0][7] = rhs.cr[0][7]; cr[0][8] = rhs.cr[0][8];
	}
	__forceinline void operator=(const NiMatrix33& rhs)
	{
		_mm_storeu_ps(&cr[0][0], _mm_loadu_ps(&rhs.cr[0][0]));
		_mm_storeu_ps(&cr[0][4], _mm_loadu_ps(&rhs.cr[0][4]));
		cr[0][8] = rhs.cr[0][8];
	}

	inline operator float* () const { return (float*)this; }
	float __vectorcall From2Points(__m128 pt1, __m128 pt2);
	NiMatrix33& __vectorcall FromEulerPRY(__m128 pry);
	NiMatrix33& __vectorcall FromEulerPRYInv(__m128 pry);
	__m128 __vectorcall ToEulerPRY() const;
	__m128 __vectorcall ToEulerPRYAlt() const;
	__m128 __vectorcall ToEulerPRYInv() const;
	float __vectorcall ExtractPitch() const;
	float __vectorcall ExtractRoll() const;
	float __vectorcall ExtractYaw() const;

	NiMatrix33& __fastcall MultiplyMatrices(const NiMatrix33& matB);
	NiMatrix33& __fastcall MultiplyMatricesInv(const NiMatrix33& matB);
	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	NiMatrix33& __vectorcall Rotate(__m128 rot);

	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(__m128 pry) { FromEulerPRY(pry); }
	__forceinline NiMatrix33& operator*=(const NiMatrix33& rhs) { return MultiplyMatrices(rhs); }
	__forceinline __m128 operator*(__m128 vec) const { return MultiplyVector(vec); }
};

// 34
struct NiTransform
{
	NiMatrix33	rotate;		// 00
	NiVector3	translate;	// 24
	float		scale;		// 30
};

// 10
struct NiSphere
{
	float	x, y, z, radius;
};

// 10
struct NiViewport
{
	float	l;
	float	r;
	float	t;
	float	b;

	//NiViewport() {}
	__forceinline NiViewport(float _l, float _r, float _t, float _b) : l(_l), r(_r), t(_t), b(_b) {}
	__forceinline explicit NiViewport(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiViewport&& rhs) noexcept
	{
		l = rhs.l;
		r = rhs.r;
		t = rhs.t;
		b = rhs.b;
	}

	__forceinline NiViewport& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&l, rhs);
		return *this;
	}

	__forceinline __m128 PS() const { return _mm_loadu_ps(&l); }

	//void __vectorcall SetFOV(float fov);
};



// 1C
struct NiFrustum
{
	NiViewport	viewPort;	// 00
	float		dNear;		// 10
	float		dFar;		// 14
	bool		ortho;		// 18
	UInt8		pad19[3];	// 19
};

// C
struct NiColor
{
	float	r;
	float	g;
	float	b;
};

// 10
struct NiColorAlpha
{
	float	r;
	float	g;
	float	b;
	float	a;
};

// 10
struct NiPlane
{
	NiVector3	nrm;
	float		offset;
};


struct NiBound
{
	NiVector3	center;
	float		radius;

	NiBound() {}
	__forceinline NiBound(float cX, float cY, float cZ, float rad) : center(cX, cY, cZ), radius(rad) {} 
	__forceinline NiBound(const NiBound& rhs) { *this = rhs; }
	__forceinline explicit NiBound(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiBound&& rhs)
	{
		center.x = rhs.center.x;
		center.y = rhs.center.y;
		center.z = rhs.center.z;
		radius = rhs.radius;
	}
	__forceinline void operator=(const NiBound& rhs) { SetPS(rhs.PS()); }

	__forceinline NiBound& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&center.x, rhs);
		return *this;
	}

	inline operator float* () { return &center.x; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&center.x); }

	UInt8 __fastcall CalculateSide(const NiPlane& plane) const;
	//void Merge(const NiBound* other) { ThisCall(0xA7F3F0, this, other); }
};


// 10
// NiTArrays are slightly weird: they can be sparse
// this implies that they can only be used with types that can be NULL?
// not sure on the above, but some code only works if this is true
// this can obviously lead to fragmentation, but the accessors don't seem to care
// weird stuff
template <typename T>
struct NiTArray
{
	//void	** _vtbl;		 00
	virtual void* Destroy(UINT32 doFree);
	T		* data;			// 04
	UInt16	capacity;		// 08 - init'd to size of preallocation
	UInt16	firstFreeEntry;	// 0A - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt16	numObjs;		// 0C - init'd to 0
	UInt16	growSize;		// 0E - init'd to size of preallocation

	T operator[](UInt32 idx) {
		if (idx < firstFreeEntry)
			return data[idx];
		return NULL;
	}

	T Get(UInt32 idx) { return data[idx]; }
	UInt16 Size() const { return firstFreeEntry; }
	bool Empty() const { return !firstFreeEntry; }
	T* Data() const { return const_cast<T*>(data); }


	UInt16 Length(void) { return firstFreeEntry; }
	__forceinline int Append(T* item)
	{
		return ThisStdCall<int>(0xA5EB20, this, item);
	}
	__forceinline void AddAtIndex(UInt32 index, T* item)
	{
		ThisStdCall(0x4B0310, this, index, item);
	}
	__forceinline void SetCapacity(UInt16 newCapacity)
	{
		ThisStdCall(0x8696E0, this, newCapacity);
	}

	class Iterator
	{
		friend NiTArray; 

		T* pData;
		UInt32		count;

	public:
		explicit operator bool() const { return count != 0; }
		bool End() const { return !count; }
		void operator++()
		{
			pData++;
			count--;
		}

		T& operator*() const { return *pData; }
		T& operator->() const { return *pData; }
		T& Get() const { return *pData; }

		Iterator(NiTArray& source) : pData(source.data), count(source.firstFreeEntry) {} 
	};

	Iterator Begin() { return Iterator(*this); } 
};

#if RUNTIME

//template <typename T>
//void NiTArray<T>::AddAtIndex(UInt32 index, T* item)
//{
//	ThisStdCall<void>(0x00869640, this, index, item);
//}
//
//template <typename T>
//void NiTArray<T>::SetCapacity(UInt16 newCapacity)
//{
//	ThisStdCall<void>(0x008696E0, this, newCapacity);
//}

#endif

// 18
// an NiTArray that can go above 0xFFFF, probably with all the same weirdness
// this implies that they make fragmentable arrays with 0x10000 elements, wtf
template <typename T>
class NiTLargeArray
{
public:
	NiTLargeArray();
	~NiTLargeArray();

	void	** _vtbl;		// 00
	T		* data;			// 04
	UInt32	capacity;		// 08 - init'd to size of preallocation
	UInt32	firstFreeEntry;	// 0C - index of the first free entry in the block of free entries at the end of the array (or numObjs if full)
	UInt32	numObjs;		// 10 - init'd to 0
	UInt32	growSize;		// 14 - init'd to size of preallocation

	T operator[](UInt32 idx) {
		if (idx < firstFreeEntry)
			return data[idx];
		return NULL;
	}

	T Get(UInt32 idx) { return (*this)[idx]; }

	UInt32 Length(void) { return firstFreeEntry; }
};

// 8
template <typename T>
struct NiTSet
{
	T		* data;		// 00
	UInt16	capacity;	// 04
	UInt16	length;		// 06
};

// 10
// this is a NiTPointerMap <UInt32, T_Data>
// todo: generalize key
template <typename T_Data>
class NiTPointerMap
{
public:
	NiTPointerMap();
	virtual ~NiTPointerMap();

	
	virtual UInt32	CalculateBucket(UInt32 key);
	virtual bool	CompareKey(UInt32 lhs, UInt32 rhs);
	virtual void	Fn_03(UInt32 arg0, UInt32 arg1, UInt32 arg2);	// assign to entry
	virtual void	Fn_04(UInt32 arg);
	virtual void	Fn_05(void);	// locked operations
	virtual void	Fn_06(void);	// locked operations

	struct Entry
	{
		Entry	* next;
		UInt32	key;
		T_Data	* data;
	};

	// note: traverses in non-numerical order
	class Iterator
	{
		friend NiTPointerMap;

	public:
		Iterator(NiTPointerMap * table, Entry * entry = NULL, UInt32 bucket = 0)
			:m_table(table), m_entry(entry), m_bucket(bucket) { FindValid(); }
		~Iterator() { }

		T_Data *	Get(void);
		UInt32		GetKey(void);
		bool		Next(void);
		bool		Done(void);

	private:
		void		FindValid(void);

		NiTPointerMap	* m_table;
		Entry		* m_entry;
		UInt32		m_bucket;
	};


	T_Data *	Lookup(UInt32 key);
	bool		Insert(Entry* nuEntry);

//	void	** _vtbl;		// 0
	UInt32	m_numBuckets;	// 4
	Entry	** m_buckets;	// 8
	UInt32	m_numItems;		// C
};

template <typename T_Data>
T_Data * NiTPointerMap <T_Data>::Lookup(UInt32 key)
{
	for(Entry * traverse = m_buckets[key % m_numBuckets]; traverse; traverse = traverse->next)
		if(traverse->key == key)
			return traverse->data;
	
	return NULL;
}

template <typename T_Data>
bool NiTPointerMap<T_Data>::Insert(Entry* nuEntry)
{
	// game code does not appear to care about ordering of entries in buckets
	UInt32 bucket = nuEntry->key % m_numBuckets;
	Entry* prev = NULL;
	for (Entry* cur = m_buckets[bucket]; cur; cur = cur->next) {
		if (cur->key == nuEntry->key) {
			return false;
		}
		else if (!cur->next) {
			prev = cur;
			break;
		}
	}

	if (prev) {
		prev->next = nuEntry;
	}
	else {
		m_buckets[bucket] = nuEntry;
	}

	m_numBuckets++;
	return true;
}

template <typename T_Data>
T_Data * NiTPointerMap <T_Data>::Iterator::Get(void)
{
	if(m_entry)
		return m_entry->data;

	return NULL;
}

template <typename T_Data>
UInt32 NiTPointerMap <T_Data>::Iterator::GetKey(void)
{
	if(m_entry)
		return m_entry->key;

	return 0;
}

template <typename T_Data>
bool NiTPointerMap <T_Data>::Iterator::Next(void)
{
	if(m_entry)
		m_entry = m_entry->next;

	while(!m_entry && (m_bucket < (m_table->m_numBuckets - 1)))
	{
		m_bucket++;

		m_entry = m_table->m_buckets[m_bucket];
	}

	return m_entry != NULL;
}

template <typename T_Data>
bool NiTPointerMap <T_Data>::Iterator::Done(void)
{
	return m_entry == NULL;
}

template <typename T_Data>
void NiTPointerMap <T_Data>::Iterator::FindValid(void)
{
	// validate bucket
	if(m_bucket >= m_table->m_numBuckets) return;

	// get bucket
	m_entry = m_table->m_buckets[m_bucket];

	// find non-empty bucket
	while(!m_entry && (m_bucket < (m_table->m_numBuckets - 1)))
	{
		m_bucket++;

		m_entry = m_table->m_buckets[m_bucket];
	}
}

// 10
// todo: NiTPointerMap should derive from this
// cleaning that up now could cause problems, so it will wait
template <typename T_Key, typename T_Data>
class NiTMapBase
{
public:
	NiTMapBase();
	~NiTMapBase();

	struct Entry
	{
		Entry	* next;	// 000
		T_Key	key;	// 004
		T_Data	data;	// 008
	};

	virtual NiTMapBase<T_Key, T_Data>*	Destructor(bool doFree);						// 000
	virtual UInt32						Hash(T_Key key);								// 001
	virtual void						Equal(T_Key key1, T_Key key2);					// 002
	virtual void						FillEntry(Entry entry, T_Key key, T_Data data);	// 003
	virtual	void						Unk_004(void * arg0);							// 004
	virtual	void						Unk_005(void);									// 005
	virtual	void						Unk_006();										// 006

	//void	** _vtbl;	// 0
	UInt32	numBuckets;	// 4
	Entry	** buckets;	// 8
	UInt32	numItems;	// C
#if RUNTIME
	DEFINE_MEMBER_FN_LONG(NiTMapBase, Lookup, bool, _NiTMap_Lookup, T_Key key, T_Data * dataOut);
#endif
};

// 14
template <typename T_Data>
class NiTStringPointerMap : public NiTPointerMap <T_Data>
{
public:
	NiTStringPointerMap();
	~NiTStringPointerMap();

	UInt32	unk010;
};

// not sure how much of this is in NiTListBase and how much is in NiTPointerListBase
// 10
template <typename T>
class NiTListBase
{
public:
	NiTListBase();
	~NiTListBase();

	struct Node
	{
		Node	* next;
		Node	* prev;
		T		* data;
	};

	virtual void	Destructor(void);
	virtual Node *	AllocateNode(void);
	virtual void	FreeNode(Node * node);

//	void	** _vtbl;	// 000
	Node	* start;	// 004
	Node	* end;		// 008
	UInt32	numItems;	// 00C
};

// 10
template <typename T>
class NiTPointerListBase : public NiTListBase <T>
{
public:
	NiTPointerListBase();
	~NiTPointerListBase();
};

// 10
template <typename T>
class NiTPointerList : public NiTPointerListBase <T>
{
public:
	NiTPointerList();
	~NiTPointerList();
};

// 4
template <typename T>
class NiPointer
{
public:
	NiPointer(T *init) : data(init)		{	}

	T	* data;

	const T&	operator *() const { return *data; }
	T&			operator *() { return *data; }

	operator const T*() const { return data; }
	operator T*() { return data; }
};

// 14
template <typename T>
class BSTPersistentList
{
public:
	BSTPersistentList();
	~BSTPersistentList();

	virtual void	Destroy(bool destroy);

//	void	** _vtbl;	// 00
	UInt32	unk04;		// 04
	UInt32	unk08;		// 08
	UInt32	unk0C;		// 0C
	UInt32	unk10;		// 10
};

struct alignas(16) AlignedVector4
{
	float	x, y, z, w;

	AlignedVector4() {}
	__forceinline AlignedVector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
	__forceinline AlignedVector4(const AlignedVector4& rhs) { *this = rhs; }
	__forceinline AlignedVector4(const NiVector4& rhs) { *this = rhs; }
	__forceinline explicit AlignedVector4(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(AlignedVector4&& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = rhs.w;
	}
	__forceinline void operator=(const AlignedVector4& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiVector4& rhs) { SetPS(rhs.PS()); }

	__forceinline AlignedVector4& SetPS(const __m128 rhs)
	{
		_mm_store_ps(&x, rhs);
		return *this;
	}

	__forceinline __m128 operator+(__m128 packedPS) const { return PS() + packedPS; }
	__forceinline __m128 operator-(__m128 packedPS) const { return PS() - packedPS; }
	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(__m128 packedPS) const { return PS() * packedPS; }

	__forceinline AlignedVector4& operator+=(__m128 packedPS) { return SetPS(*this + packedPS); }
	__forceinline AlignedVector4& operator-=(__m128 packedPS) { return SetPS(*this - packedPS); }
	__forceinline AlignedVector4& operator*=(float s) { return SetPS(*this * s); }
	__forceinline AlignedVector4& operator*=(__m128 packedPS) { return SetPS(*this * packedPS); }


	inline operator float* () { return &x; }
	inline operator NiVector3& () const { return *(NiVector3*)this; }

	__forceinline __m128 PS() const { return _mm_load_ps(&x); }
};

typedef AlignedVector4 hkVector4;

struct hkCdBody;

struct RayCastData
{
	hkVector4	pos0;		// 00	
	hkVector4	pos1;		// 10
	UInt8		byte20;		// 20
	UInt8		pad21[3];	// 21
	UInt8		layerType;	// 24
	UInt8		filterFlags;// 25
	UInt16		group;		// 26
	UInt32		unk28[6];	// 28
	float		flt40;		// 40
	UInt32		unk44[15];	// 44
	hkCdBody* cdBody;	// 80
	UInt32		unk84[3];	// 84
	hkVector4	vector90;	// 90
	UInt32		unkA0[3];	// A0
	UInt8		byteAC;		// AC
	UInt8		padAD[3];	// AD
};
static_assert(sizeof(RayCastData) == 0xB0);

float __vectorcall Point3Distance(const NiVector3& pt1, const NiVector3& pt2);
class hkQuaternion;
class AxisAngle;


struct NiQuaternion
{
	float	x, y, z, w;

	NiQuaternion() {}
	__forceinline NiQuaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}
	__forceinline NiQuaternion(const NiQuaternion& from) { *this = from; }
	__forceinline explicit NiQuaternion(const NiMatrix33& rotMat) { *this = rotMat; }
	__forceinline explicit NiQuaternion(const NiVector3& pry) { *this = pry; }
	__forceinline explicit NiQuaternion(const AxisAngle& axisAngle) { *this = axisAngle; }
	__forceinline explicit NiQuaternion(const hkQuaternion& hkQt) { *this = hkQt; }
	__forceinline explicit NiQuaternion(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(NiQuaternion&& rhs)
	{
		w = rhs.w;
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
	}
	__forceinline void operator=(const NiQuaternion& rhs) { SetPS(rhs.PS()); }
	__forceinline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(const AxisAngle& axisAngle) { FromAxisAngle(axisAngle); }
	void operator=(const hkQuaternion& hkQt);

	__forceinline NiQuaternion& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&w, rhs);
		return *this;
	}

	__forceinline __m128 operator+(const NiQuaternion& rhs) const { return PS() + rhs.PS(); }

	__forceinline __m128 operator-(const NiQuaternion& rhs) const { return PS() - rhs.PS(); }

	__forceinline __m128 operator*(float s) const { return PS() * _mm_set_ps1(s); }
	__forceinline __m128 operator*(const NiQuaternion& rhs) const { return MultiplyQuaternion(rhs); }
	__forceinline __m128 operator*(__m128 vec) const { return MultiplyVector(vec); }

	__forceinline NiQuaternion& operator+=(const NiQuaternion& rhs) { return SetPS(*this + rhs); }
	__forceinline NiQuaternion& operator-=(const NiQuaternion& rhs) { return SetPS(*this - rhs); }
	__forceinline NiQuaternion& operator*=(float s) { return SetPS(*this * s); }
	__forceinline NiQuaternion& operator*=(const NiQuaternion& rhs) { return SetPS(*this * rhs); }

	__forceinline bool operator==(const NiQuaternion& rhs) const { return Equal_V4(PS(), rhs.PS()); }
	__forceinline bool operator!=(const NiQuaternion& rhs) const { return !(*this == rhs); }

	inline operator float* () { return &w; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&w); }

	NiQuaternion& __vectorcall FromEulerPRY(__m128 pry);
	NiQuaternion& __vectorcall FromEulerYPR(__m128 pry);
	NiQuaternion& __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	float __vectorcall From2Points(__m128 pt1, __m128 pt2);
	NiQuaternion& __fastcall FromAxisAngle(const AxisAngle& axisAngle);

	__m128 __vectorcall MultiplyVector(__m128 vec) const;
	__m128 __vectorcall MultiplyQuaternion(const NiQuaternion& rhs) const;

	__forceinline __m128 Invert() const { return PS() ^ _mm_load_ps((const float*)0x10C8780); }

	__forceinline __m128 Negate() const { return PS() ^ GET_PS(2); }

	__forceinline float __vectorcall DotProduct(const NiQuaternion& rhs) const
	{
		__m128 k = _mm_setzero_ps();
		return _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(PS() * rhs.PS(), k), k));
	}

	NiQuaternion& Normalize() { return SetPS(Normalize_V4(PS())); }

	__m128 __vectorcall ToEulerPRY() const;
	__m128 __vectorcall ToEulerYPR() const;

	NiQuaternion& Rotate(const NiQuaternion& rot) { return *this *= rot; }
	NiQuaternion& Rotate(const AxisAngle& rot) { return *this *= NiQuaternion(rot); }
	NiQuaternion& __vectorcall Rotate(__m128 pry);

	__m128 __vectorcall Lerp(const NiQuaternion& qb, float t) const;
	__m128 __vectorcall Slerp(const NiQuaternion& qb, float t) const;

	void Dump() const;
};


struct AxisAngle
{
	NiVector3	axis;
	float		theta;

	AxisAngle() {}
	__forceinline AxisAngle(float x, float y, float z, float t) : axis(x, y, z), theta(t) {}
	__forceinline explicit AxisAngle(const NiVector3& _axis, float t) : axis(_axis), theta(t) {}
	__forceinline AxisAngle(const AxisAngle& from) { *this = from; }
	__forceinline explicit AxisAngle(const NiVector3& pry) { *this = pry; }
	__forceinline explicit AxisAngle(const NiMatrix33& rotMat) { *this = rotMat; }
	__forceinline explicit AxisAngle(const NiQuaternion& qt) { *this = qt; }
	__forceinline explicit AxisAngle(const __m128 rhs) { SetPS(rhs); }

	__forceinline void operator=(AxisAngle&& from)
	{
		axis.x = from.axis.x;
		axis.y = from.axis.y;
		axis.z = from.axis.z;
		theta = from.theta;
	}
	__forceinline void operator=(const AxisAngle& from) { SetPS(from.PS()); }
	__forceinline void operator=(const NiVector3& pry) { FromEulerPRY(pry.PS()); }
	__forceinline void operator=(const NiMatrix33& rotMat) { FromRotationMatrix(rotMat); }
	__forceinline void operator=(const NiQuaternion& qt) { FromQuaternion(qt); }

	__forceinline AxisAngle& SetPS(const __m128 rhs)
	{
		_mm_storeu_ps(&axis.x, rhs);
		return *this;
	}

	inline operator float* () { return &axis.x; }
	__forceinline __m128 PS() const { return _mm_loadu_ps(&axis.x); }

	__forceinline bool operator==(const AxisAngle& rhs) const { return Equal_V4(PS(), rhs.PS()); }
	__forceinline bool operator!=(const AxisAngle& rhs) const { return !(*this == rhs); }

	AxisAngle& __vectorcall FromEulerPRY(__m128 pry);
	AxisAngle& __fastcall FromRotationMatrix(const NiMatrix33& rotMat);
	AxisAngle& __fastcall FromQuaternion(const NiQuaternion& qt);

	__m128 __vectorcall ToEulerPRY() const;

	void Dump() const;
};