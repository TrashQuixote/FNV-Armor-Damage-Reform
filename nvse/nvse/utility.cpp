#include "nvse/utility.h"

memcpy_t _memcpy = memcpy, _memmove = memmove;

__declspec(naked) PrimitiveCS *PrimitiveCS::Enter()
{
	__asm
	{
		push	ebx
		mov		ebx, ecx
		call	GetCurrentThreadId
		cmp		[ebx], eax
		jnz		doSpin
	done:
		mov		eax, ebx
		pop		ebx
		retn
		NOP_0xA
	doSpin:
		mov		ecx, eax
		xor		eax, eax
		lock cmpxchg [ebx], ecx
		test	eax, eax
		jz		done
		push	esi
		push	edi
		mov		esi, ecx
		mov		edi, 0x2710
	spinHead:
		dec		edi
		mov		edx, edi
		shr		edx, 0x1F
		push	edx
		call	Sleep
		xor		eax, eax
		lock cmpxchg [ebx], esi
		test	eax, eax
		jnz		spinHead
		pop		edi
		pop		esi
		mov		eax, ebx
		pop		ebx
		retn
	}
}

__declspec(naked) TESForm* __stdcall LookupFormByRefID(UInt32 refID)
{
	__asm
	{
		mov		ecx, ds:[0x11C54C0]
		mov		eax, [esp+4]
		xor		edx, edx
		div		dword ptr [ecx+4]
		mov		eax, [ecx+8]
		mov		eax, [eax+edx*4]
		test	eax, eax
		jz		done
		mov		edx, [esp+4]
		ALIGN 16
	iterHead:
		cmp		[eax+4], edx
		jz		found
		mov		eax, [eax]
		test	eax, eax
		jnz		iterHead
		retn	4
	found:
		mov		eax, [eax+8]
	done:
		retn	4
	}
}

__declspec(naked) int __vectorcall ifloor(float value)
{
	__asm
	{
		movd	eax, xmm0
		test	eax, eax
		jns		isPos
		push	0x3FA0
		ldmxcsr	[esp]
		cvtss2si	eax, xmm0
		mov		dword ptr [esp], 0x1FA0
		ldmxcsr	[esp]
		pop		ecx
		retn
	isPos:
		cvttss2si	eax, xmm0
		retn
	}
}

__declspec(naked) int __vectorcall iceil(float value)
{
	__asm
	{
		movd	eax, xmm0
		test	eax, eax
		js		isNeg
		push	0x5FA0
		ldmxcsr	[esp]
		cvtss2si	eax, xmm0
		mov		dword ptr [esp], 0x1FA0
		ldmxcsr	[esp]
		pop		ecx
		retn
	isNeg:
		cvttss2si	eax, xmm0
		retn
	}
}

__declspec(naked) UInt32 __fastcall StrLen(const char *str)
{
	__asm
	{
		test	ecx, ecx
		jnz		proceed
		xor		eax, eax
		retn
	proceed:
		push	ecx
		test	ecx, 3
		jz		iter4
	iter1:
		mov		al, [ecx]
		inc		ecx
		test	al, al
		jz		done1
		test	ecx, 3
		jnz		iter1
		ALIGN 16
	iter4:
		mov		eax, [ecx]
		mov		edx, 0x7EFEFEFF
		add		edx, eax
		not		eax
		xor		eax, edx
		add		ecx, 4
		test	eax, 0x81010100
		jz		iter4
		mov		eax, [ecx-4]
		test	al, al
		jz		done4
		test	ah, ah
		jz		done3
		test	eax, 0xFF0000
		jz		done2
		test	eax, 0xFF000000
		jnz		iter4
	done1:
		lea		eax, [ecx-1]
		pop		ecx
		sub		eax, ecx
		retn
	done2:
		lea		eax, [ecx-2]
		pop		ecx
		sub		eax, ecx
		retn
	done3:
		lea		eax, [ecx-3]
		pop		ecx
		sub		eax, ecx
		retn
	done4:
		lea		eax, [ecx-4]
		pop		ecx
		sub		eax, ecx
		retn
	}
}

__declspec(naked) void __fastcall MemZero(void *dest, UInt32 bsize)
{
	__asm
	{
		push	edi
		test	ecx, ecx
		jz		done
		mov		edi, ecx
		xor		eax, eax
		mov		ecx, edx
		shr		ecx, 2
		jz		write1
		rep stosd
	write1:
		and		edx, 3
		jz		done
		mov		ecx, edx
		rep stosb
	done:
		pop		edi
		retn
	}
}

__declspec(naked) char* __fastcall StrCopy(char *dest, const char *src)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
		test	edx, edx
		jnz		proceed
		mov		[eax], 0
	done:
		retn
	proceed:
		push	ecx
		mov		ecx, edx
		call	StrLen
		pop		edx
		push	eax
		inc		eax
		push	eax
		push	ecx
		push	edx
		call	_memmove
		add		esp, 0xC
		pop		ecx
		add		eax, ecx
		retn
	}
}

__declspec(naked) char* __fastcall StrNCopy(char *dest, const char *src, UInt32 length)
{
	__asm
	{
		mov		eax, ecx
		test	ecx, ecx
		jz		done
		test	edx, edx
		jz		nullTerm
		cmp		dword ptr [esp+4], 0
		jz		nullTerm
		push	esi
		mov		esi, ecx
		mov		ecx, edx
		call	StrLen
		mov		edx, [esp+8]
		cmp		edx, eax
		cmova	edx, eax
		push	edx
		push	ecx
		push	esi
		add		esi, edx
		call	_memmove
		add		esp, 0xC
		mov		eax, esi
		pop		esi
	nullTerm:
		mov		[eax], 0
	done:
		retn	4
	}
}

__declspec(naked) char* __fastcall StrCat(char *dest, const char *src)
{
	__asm
	{
		test	ecx, ecx
		jnz		proceed
		mov		eax, ecx
		retn
	proceed:
		push	edx
		call	StrLen
		pop		edx
		add		ecx, eax
		jmp		StrCopy
	}
}

alignas(16) const char
kLwrCaseConverter[] =
{
	'\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E', '\x0F',
	'\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
	'\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28', '\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F',
	'\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B', '\x3C', '\x3D', '\x3E', '\x3F',
	'\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x5B', '\x5C', '\x5D', '\x5E', '\x5F',
	'\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6A', '\x6B', '\x6C', '\x6D', '\x6E', '\x6F',
	'\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7A', '\x7B', '\x7C', '\x7D', '\x7E', '\x7F',
	'\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87', '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
	'\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97', '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
	'\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7', '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
	'\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7', '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
	'\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7', '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
	'\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7', '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
	'\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7', '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
	'\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7', '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF'
};

// Returns 0 if both strings are equal.
char __fastcall StrCompare(const char *lstr, const char *rstr)
{
	if (!lstr) return rstr ? -1 : 0;
	if (!rstr) return 1;
	UInt8 lchr, rchr;
	while (*lstr)
	{
		lchr = game_toupper(*(UInt8*)lstr);
		rchr = game_toupper(*(UInt8*)rstr);
		if (lchr == rchr)
		{
			lstr++;
			rstr++;
			continue;
		}
		return (lchr < rchr) ? -1 : 1;
	}
	return *rstr ? -1 : 0;
}

bool __fastcall StrEqual(const char* lstr, const char* rstr)
{
	return StrCompare(lstr, rstr) == 0;
}

void __fastcall StrToLower(char *str)
{
	if (!str) return;
	UInt8 curr;
	while (curr = *str)
	{
		*str = game_tolower(curr);
		str++;
	}
}

char* __fastcall SubStrCI(const char *srcStr, const char *subStr)
{
	int srcLen = StrLen(srcStr);
	if (!srcLen) return NULL;
	int subLen = StrLen(subStr);
	if (!subLen) return NULL;
	srcLen -= subLen;
	if (srcLen < 0) return NULL;
	int index;
	do
	{
		index = 0;
		while (true)
		{
			if (game_tolower(*(UInt8*)(srcStr + index)) != game_tolower(*(UInt8*)(subStr + index)))
				break;
			if (++index == subLen)
				return const_cast<char*>(srcStr);
		}
		srcStr++;
	}
	while (--srcLen >= 0);
	return NULL;
}

char* __fastcall SlashPos(const char *str)
{
	if (!str) return NULL;
	char curr;
	while (curr = *str)
	{
		if ((curr == '/') || (curr == '\\'))
			return const_cast<char*>(str);
		str++;
	}
	return NULL;
}

__declspec(naked) char* __fastcall CopyString(const char *key)
{
	__asm
	{
		call	StrLen
		inc		eax
		push	eax
		push	ecx
		push	eax
#if !_DEBUG
		call    _malloc_base
#else
		call	malloc
#endif
		pop		ecx
		push	eax
		call	_memcpy
		add		esp, 0xC
		retn
	}
}

__declspec(naked) char* __fastcall CopyString(const char* key, UInt32 length)
{
	__asm
	{
		mov		eax, edx // length
		inc		eax // length++ to account for null terminator
		push	eax
		push	ecx
		push	eax
#if !_DEBUG
		call    _malloc_base
#else
		call	malloc
#endif
		pop		ecx // equivalent to "add esp, 4" here
		// stack is now:
		// [esp+4]: length + 1
		// [esp]: "key" arg passed to CopyString

		// length -= 1, to avoid copying null terminator or past-the-end part of string (if it had no null terminator)
		dec		[esp+4]

		push	eax // dest = malloc's new ptr
		call	_memcpy
		// eax is now memcpy's new string
		add		esp, 8
		pop		edx // get pushed eax back (length)
		// add null terminator to copied string in case string arg didn't have one
		add		edx, eax  // advance to last char of string (where null terminator should be)
		mov		byte ptr[edx], 0
		retn
	}
}

__declspec(naked) char* __fastcall IntToStr(char *str, int num)
{
	__asm
	{
		push	esi
		push	edi
		test	edx, edx
		jns		skipNeg
		neg		edx
		mov		[ecx], '-'
		inc		ecx
	skipNeg:
		mov		esi, ecx
		mov		edi, ecx
		mov		eax, edx
		mov		ecx, 0xA
	workIter:
		xor		edx, edx
		div		ecx
		add		dl, '0'
		mov		[esi], dl
		inc		esi
		test	eax, eax
		jnz		workIter
		mov		[esi], 0
		mov		eax, esi
	swapIter:
		dec		esi
		cmp		esi, edi
		jbe		done
		mov		dl, [esi]
		mov		cl, [edi]
		mov		[esi], cl
		mov		[edi], dl
		inc		edi
		jmp		swapIter
	done:
		pop		edi
		pop		esi
		retn
	}
}

// By JazzIsParis
__declspec(naked) UInt32 __fastcall StrHashCS(const char *inKey)
{
	__asm
	{
		mov		eax, 0x6B49D20B
		test	ecx, ecx
		jz		done
		ALIGN 16
	iterHead:
		movzx	edx, byte ptr[ecx]
		test	dl, dl
		jz		done
		shl		edx, 4
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		movzx	edx, byte ptr[ecx + 1]
		test	dl, dl
		jz		done
		shl		edx, 0xC
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		movzx	edx, byte ptr[ecx + 2]
		test	dl, dl
		jz		done
		shl		edx, 0x14
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		movzx	edx, byte ptr[ecx + 3]
		test	dl, dl
		jz		done
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		add		ecx, 4
		jmp		iterHead
	done :
		ret
	}
}

// By JazzIsParis
// "only < 0.008% collisions"
__declspec(naked) UInt32 __fastcall StrHashCI(const char* inKey)
{
	__asm
	{
		push	esi
		mov		eax, 0x6B49D20B
		test	ecx, ecx
		jz		done
		mov		esi, ecx
		xor ecx, ecx
		ALIGN 16
	iterHead:
		mov		cl, [esi]
		test	cl, cl
		jz		done
		movzx	edx, kLwrCaseConverter[ecx]
		shl		edx, 4
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		mov		cl, [esi + 1]
		test	cl, cl
		jz		done
		movzx	edx, kLwrCaseConverter[ecx]
		shl		edx, 0xC
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		mov		cl, [esi + 2]
		test	cl, cl
		jz		done
		movzx	edx, kLwrCaseConverter[ecx]
		shl		edx, 0x14
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		mov		cl, [esi + 3]
		test	cl, cl
		jz		done
		movzx	edx, kLwrCaseConverter[ecx]
		sub		eax, edx
		mov		edx, eax
		shl		eax, 5
		sub		eax, edx
		add		esi, 4
		jmp		iterHead
	done :
		pop		esi
		retn
	}
}

void SpinLock::Enter()
{
	UInt32 threadID = GetCurrentThreadId();
	if (owningThread == threadID)
	{
		enterCount++;
		return;
	}
	while (InterlockedCompareExchange(&owningThread, threadID, 0));
	enterCount = 1;
}

#define FAST_SLEEP_COUNT 10000UL

void SpinLock::EnterSleep()
{
	UInt32 threadID = GetCurrentThreadId();
	if (owningThread == threadID)
	{
		enterCount++;
		return;
	}
	UInt32 fastIdx = FAST_SLEEP_COUNT;
	while (InterlockedCompareExchange(&owningThread, threadID, 0))
	{
		if (fastIdx)
		{
			fastIdx--;
			Sleep(0);
		}
		else Sleep(1);
	}
	enterCount = 1;
}

void SpinLock::Leave()
{
	if (owningThread && !--enterCount)
		owningThread = 0;
}

// From JIP
alignas(16) const UInt32 kPackedValues[] =
{
	PS_DUP_4(0x7FFFFFFF),
	PS_DUP_1(0x7FFFFFFF),
	PS_DUP_4(0x80000000),
	PS_DUP_1(0x80000000),
	PS_DUP_3(0xFFFFFFFF),
	0xFFFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FFFFFFF,
	0, 0x80000000, 0, 0x80000000,
	PS_DUP_4(HEX(FLT_EPSILON)),
	PS_DUP_3(HEX(FltPId180)),
	PS_DUP_3(HEX(Flt180dPI)),
	PS_DUP_3(HEX(FltPId2)),
	PS_DUP_3(HEX(FltPI)),
	PS_DUP_3(HEX(FltPIx2)),
	PS_DUP_3(HEX(0.5F)),
	PS_DUP_3(HEX(1.0F)),
	PS_DUP_3(0x40DFF8D6),
	HEX(0.001F), HEX(0.01F), HEX(0.1F), HEX(0.25F),
	HEX(3.0F), HEX(10.0F), HEX(100.0F), 0
};

alignas(16) const UInt32 kASinConsts[] =
{
	0xBC996E30, 0x3D981627, 0x3E593484, 0x3FC90FDB, 0x34000000, 0x3F800000, 0x40490FDB, 0x03800000,
	0x37202A00, 0x37E24000, 0x38333600, 0x386E4C00, 0x38913200, 0x38A84000, 0x38BC5200, 0x38CDAC00,
	0x38DC5000, 0x38E8C000, 0x38F2C800, 0x38FAA800, 0x39005600, 0x39029000, 0x3903C800, 0x39046400,
	0x39042C00, 0x39036800, 0x39022800, 0x39003400, 0x38FB7000, 0x38F60800, 0x38EFD800, 0x38E8B800,
	0x38E15800, 0x38D96000, 0x38D0C000, 0x38C83000, 0x38BF2000, 0x38B59800, 0x38AC5800, 0x38A2E000,
	0x38998000, 0x388FD000, 0x38866000, 0x387AA000, 0x38678000, 0x3855E000, 0x3844A000, 0x38338000,
	0x38234000, 0x3812E000, 0x38042000, 0x37EB4000, 0x37CF4000, 0x37B64000, 0x379D4000, 0x37874000,
	0x37658000, 0x373F8000, 0x371A8000, 0x36F90000, 0x36C30000, 0x36900000, 0x364E0000, 0x360E0000,
	0x35B40000, 0x35500000, 0x34D00000, 0x33C00000, 0x34000000, 0x34800000, 0x35400000, 0x35A80000,
	0x36040000, 0x36400000, 0x367C0000, 0x36A60000, 0x36D60000, 0x37040000, 0x371D0000, 0x37390000,
	0x37560000, 0x37750000, 0x378B0000, 0x379C0000, 0x37AC8000, 0x37BE8000, 0x37D08000, 0x37E30000,
	0x37F58000, 0x3803C000, 0x380D0000, 0x3815C000, 0x381EC000, 0x38274000, 0x382F8000, 0x38380000,
	0x383F8000, 0x38470000, 0x384E8000, 0x3854C000, 0x385B0000, 0x38604000, 0x38658000, 0x3869C000,
	0x386D4000, 0x38708000, 0x3872C000, 0x38744000, 0x38750000, 0x38750000, 0x38740000, 0x38724000,
	0x38700000, 0x386C0000, 0x3867C000, 0x3862C000, 0x385C8000, 0x38558000, 0x384D8000, 0x38450000,
	0x383B0000, 0x38318000, 0x38260000, 0x381A0000, 0x380D8000, 0x38010000, 0x37E70000, 0x37CB0000,
	0x37AF0000, 0x37930000, 0x376C0000, 0x37340000, 0x37020000, 0x36A00000, 0x36180000, 0x35000000
};

__declspec(naked) __m128 __vectorcall Cos_V3(__m128 angles)
{
	__asm
	{
		andps	xmm0, PS_AbsMask
		movaps	xmm1, xmm0
		movaps	xmm2, PS_V3_PIx2
		mulps	xmm1, kPS2dPI
		cvttps2dq	xmm1, xmm1
		movaps	xmm3, xmm1
		pcmpgtd	xmm3, kQuadTest
		movmskps	edx, xmm3
		test	dl, dl
		jz		perdOK
		movaps	xmm3, xmm1
		andps	xmm1, kQuadTest
		psrld	xmm3, 2
		cvtdq2ps	xmm3, xmm3
		mulps	xmm3, xmm2
		subps	xmm0, xmm3
		perdOK :
		movaps	xmm3, xmm1
		pcmpgtd	xmm1, kQuadTest + 0x10
		andps	xmm2, xmm1
		andnps	xmm1, xmm3
		xorps	xmm3, xmm3
		pcmpgtd	xmm1, xmm3
		movaps	xmm3, xmm1
		pslld	xmm3, 0x1F
		andps	xmm1, PS_V3_PI
		orps	xmm1, xmm2
		subps	xmm0, xmm1
		mulps	xmm0, xmm0
		movaps	xmm1, kCosConsts
		mulps	xmm1, xmm0
		subps	xmm1, kCosConsts + 0x10
		mulps	xmm1, xmm0
		addps	xmm1, kCosConsts + 0x20
		mulps	xmm1, xmm0
		subps	xmm1, kCosConsts + 0x30
		mulps	xmm0, xmm1
		addps	xmm0, kCosConsts + 0x40
		xorps	xmm0, xmm3
		retn
		ALIGN 16
	kPS2dPI:
		EMIT_PS_3(0x3F22F983)
	kQuadTest :
		EMIT_PS_3(0x00000003)
		EMIT_PS_3(0x00000002)
	kCosConsts :
		EMIT_PS_3(0x37C23AB1)
		EMIT_PS_3(0x3AB59551)
		EMIT_PS_3(0x3D2AA76F)
		EMIT_PS_3(0x3EFFFFE0)
		EMIT_PS_3(0x3F7FFFFF)
	}
}


__declspec(naked) __m128 __vectorcall GetSinCos_V3(__m128 angles)
{
	__asm
	{
		movaps	xmm5, xmm0
		call	Cos_V3
		movaps	xmm4, xmm0
		movaps	xmm0, PS_V3_PId2
		subps	xmm0, xmm5
		call	Cos_V3
		movaps	xmm1, xmm4
		retn
	}
}

__declspec(naked) float __vectorcall ASin(float x)
{
	__asm
	{
		xorps	xmm4, xmm4
		comiss	xmm0, xmm4
		jz		done
		movq	xmm4, xmm0
		andps	xmm4, PS_FlipSignMask0
		xorps	xmm0, xmm4
		movss	xmm3, kASinConsts + 0x14
		comiss	xmm0, xmm3
		jnb		ooRange
		movss	xmm1, kASinConsts + 0x1C
		paddd	xmm1, xmm0
		movaps	xmm2, kASinConsts
		cvttss2si	eax, xmm1
		movq	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		addss	xmm1, xmm2
		mulss	xmm1, xmm0
		psrldq	xmm2, 4
		subss	xmm1, xmm2
		mulss	xmm1, xmm0
		subss	xmm3, xmm0
		movshdup	xmm0, xmm2
		addss	xmm1, xmm0
		sqrtss	xmm2, xmm3
		mulss	xmm2, xmm1
		subss	xmm0, xmm2
		addss	xmm0, kASinConsts[eax * 4 + 0x20]
		xorps	xmm0, xmm4
		done :
		retn
			ooRange :
		movss	xmm0, kASinConsts + 0xC
			xorps	xmm0, xmm4
			retn
	}
}

__declspec(naked) float __vectorcall ATan2(float y, float x)
{
	__asm
	{
		xorps	xmm2, xmm2
		comiss	xmm0, xmm2
		jz		zeroY
		comiss	xmm1, xmm2
		jz		zeroX
		movq	xmm2, xmm0
		unpcklpd	xmm2, xmm1
		andps	xmm2, PS_AbsMask
		movaps	xmm3, xmm2
		pshufd	xmm4, xmm2, 0xFE
		maxss	xmm3, xmm4
		minss	xmm4, xmm2
		divss	xmm4, xmm3
		movq	xmm3, xmm4
		mulss	xmm3, xmm4
		movq	xmm5, xmm3
		mulss	xmm5, kATanConsts
		addss	xmm5, kATanConsts + 4
		mulss	xmm5, xmm3
		subss	xmm5, kATanConsts + 8
		mulss	xmm5, xmm3
		addss	xmm5, kATanConsts + 0xC
		mulss	xmm5, xmm3
		subss	xmm5, kATanConsts + 0x10
		mulss	xmm5, xmm3
		addss	xmm5, kATanConsts + 0x14
		mulss	xmm4, xmm5
		movss	xmm3, PS_FlipSignMask0
		pshufd	xmm5, xmm2, 0xFE
		comiss	xmm2, xmm5
		jbe		doneCmp1
		xorps	xmm4, xmm3
		addss	xmm4, PS_V3_PId2
	doneCmp1 :
		xorps	xmm2, xmm2
			comiss	xmm1, xmm2
			jnb		doneCmp2
			xorps	xmm4, xmm3
			addss	xmm4, PS_V3_PI
	doneCmp2 :
		andps	xmm0, xmm3
			xorps	xmm0, xmm4
			retn
	zeroY :
		comiss	xmm1, xmm2
			jnb		done
			movss	xmm0, PS_V3_PI
			retn
	zeroX :
		movss	xmm1, PS_V3_PId2
			andps	xmm0, PS_FlipSignMask0
			orps	xmm0, xmm1
	done :
			retn
			ALIGN 16
	kATanConsts :
			EMIT_DW(0xBC5CDD30)
			EMIT_DW(0x3D6B6D55)
			EMIT_DW(0x3DF84C31)
			EMIT_DW(0x3E4854C9)
			EMIT_DW(0x3EAA7E45)
			EMIT_DW(0x3F7FFFB7)
	}
}