#include "NiTypes.h"


// Copied from JIP LN NVSE
__declspec(naked) float __vectorcall Point3Distance(const NiVector3& pt1, const NiVector3& pt2)
{
	__asm
	{
		movaps	xmm2, PS_XYZ0Mask
		movups	xmm0, [ecx]
		andps	xmm0, xmm2
		movups	xmm1, [edx]
		andps	xmm1, xmm2
		subps	xmm0, xmm1
		mulps	xmm0, xmm0
		xorps	xmm1, xmm1
		haddps	xmm0, xmm1
		haddps	xmm0, xmm1
		comiss	xmm0, xmm1
		jz		done
		movaps	xmm1, xmm0
		rsqrtss	xmm2, xmm0
		mulss	xmm1, xmm2
		mulss	xmm1, xmm2
		movss	xmm3, SS_3
		subss	xmm3, xmm1
		mulss	xmm3, xmm2
		mulss	xmm3, PS_V3_Half
		mulss	xmm0, xmm3
	done :
		retn
	}
}
__declspec(naked) float __vectorcall Point2Distance(const NiVector3& pt1, const NiVector3& pt2)
{
	__asm
	{
		movq	xmm0, qword ptr[ecx]
		movq	xmm1, qword ptr[edx]
		subps	xmm0, xmm1
		mulps	xmm0, xmm0
		xorps	xmm1, xmm1
		haddps	xmm0, xmm1
		comiss	xmm0, xmm1
		jz		done
		movq	xmm1, xmm0
		rsqrtss	xmm2, xmm0
		mulss	xmm1, xmm2
		mulss	xmm1, xmm2
		movss	xmm3, SS_3
		subss	xmm3, xmm1
		mulss	xmm3, xmm2
		mulss	xmm3, PS_V3_Half
		mulss	xmm0, xmm3
		done :
		retn
	}
}
__declspec(naked) __m128 __vectorcall Normalize_V4(__m128 inPS)
{
	__asm
    {
		movaps	xmm1, xmm0
		movaps	xmm2, xmm1
		mulps	xmm2, xmm2
		xorps	xmm0, xmm0
		haddps	xmm2, xmm0
		haddps	xmm2, xmm0
		comiss	xmm2, PS_Epsilon
		jb		zeroLen
		rsqrtss	xmm3, xmm2
		movss	xmm0, SS_3
		mulss	xmm2, xmm3
		mulss	xmm2, xmm3
		subss	xmm0, xmm2
		mulss	xmm0, xmm3
		mulss	xmm0, PS_V3_Half
		shufps	xmm0, xmm0, 0
		mulps	xmm0, xmm1
	zeroLen:
        retn
    }
}

__declspec(naked) __m128 __vectorcall GetAnglesBetweenPoints(__m128 pt1, __m128 pt2)
{
	__asm
	{
		subps	xmm0, xmm1
		andps	xmm0, PS_XYZ0Mask
		pshufd	xmm1, xmm0, 0xFD
		xorps	xmm1, PS_FlipSignMask0
		movaps	xmm2, xmm0
		xorps	xmm6, xmm6
		unpcklpd	xmm2, xmm6
		mulps	xmm2, xmm2
		haddps	xmm2, xmm6
		sqrtss	xmm6, xmm2
		pshufd	xmm7, xmm0, 0xFE
		call	ATan2
		movq	xmm1, xmm6
		movq	xmm6, xmm0
		movq	xmm0, xmm7
		call	ATan2
		xorps	xmm0, PS_FlipSignMask0
		unpcklpd	xmm0, xmm6
		retn
	}
}

__declspec(naked) __m128 __vectorcall NiVector3::CrossProduct(const NiVector3& vB) const
{
	__asm
	{
		movups	xmm1, [edx]
		andps	xmm1, PS_XYZ0Mask
		movups	xmm2, [ecx]
		pshufd	xmm0, xmm1, 0xC9
		pshufd	xmm3, xmm2, 0x12
		mulps	xmm0, xmm3
		pshufd	xmm3, xmm1, 0xD2
		pshufd	xmm4, xmm2, 9
		mulps	xmm3, xmm4
		subps	xmm0, xmm3
		jmp		Normalize_V4
	}
}

__declspec(naked) NiVector3& NiVector3::Normalize()
{
	__asm
	{
		mov		eax, ecx
		movups	xmm1, [eax]
		andps	xmm1, PS_XYZ0Mask
		movaps	xmm2, xmm1
		mulps	xmm2, xmm2
		xorps	xmm0, xmm0
		haddps	xmm2, xmm0
		haddps	xmm2, xmm0
		comiss	xmm2, PS_Epsilon
	jb	zeroLen
		rsqrtss	xmm3, xmm2
		movss	xmm0, SS_3
		mulss	xmm2, xmm3
		mulss	xmm2, xmm3
		subss	xmm0, xmm2
		mulss	xmm0, xmm3
		mulss	xmm0, PS_V3_Half
		shufps	xmm0, xmm0, 0xC0
		mulps	xmm0, xmm1
	zeroLen :
		pshufd	xmm2, xmm0, 2
		movlps[eax], xmm0
		movss[eax + 8], xmm2
		retn
	}
}

__declspec(naked) float __vectorcall NiMatrix33::From2Points(__m128 pt1, __m128 pt2)
{
	__asm
	{
		subps	xmm0, xmm1
		andps	xmm0, PS_XYZ0Mask
		xorps	xmm6, xmm6
		movq	xmm7, xmm6
		movaps	xmm1, xmm0
		mulps	xmm1, xmm1
		movaps	xmm2, xmm1
		haddps	xmm2, xmm7
		haddps	xmm2, xmm7
		comiss	xmm2, xmm7
		jz		allZero
		pshufd	xmm3, xmm0, 0xFE
		xorps	xmm3, PS_FlipSignMask0
		unpcklpd	xmm1, xmm7
		haddps	xmm1, xmm7
		comiss	xmm1, xmm7
		jz		zZero
		unpcklpd	xmm2, xmm1
		movaps	xmm4, xmm2
		rsqrtps	xmm6, xmm2
		mulps	xmm2, xmm6
		mulps	xmm2, xmm6
		movss	xmm5, SS_3
		unpcklpd	xmm5, xmm5
		subps	xmm5, xmm2
		mulps	xmm5, xmm6
		mulps	xmm5, PS_V3_Half
		movaps	xmm2, xmm5
		mulps	xmm4, xmm2
		movaps	xmm6, xmm4
		movsldup	xmm2, xmm2
		unpckhpd	xmm4, xmm4
		unpcklps	xmm3, xmm4
		unpcklpd	xmm3, xmm0
		cmpeqps	xmm0, xmm7
		movmskps	eax, xmm0
		and al, 3
		cmp		al, 2
		jz		getSC
		pshufd	xmm0, PS_FlipSignMask0, 0x15
		xorps	xmm3, xmm0
	getSC :
		mulps	xmm3, xmm2
		jmp		makeMat
	zZero :
		movaps	xmm6, xmm3
		pshufd	xmm0, PS_V3_One, 0x3C
		andps	xmm3, PS_FlipSignMask0
		xorps	xmm6, xmm3
		xorps	xmm3, xmm0
		jmp		makeMat
	allZero :
		pshufd	xmm3, PS_V3_One, 0x33
	makeMat :
			pshufd	xmm0, xmm3, 0xA
			pshufd	xmm1, xmm3, 0xF
			movss[ecx], xmm1
			movss[ecx + 0xC], xmm0
			movss[ecx + 0x18], xmm7
			movlps[ecx + 0x1C], xmm3
			shufps	xmm3, xmm7, 1
			mulps	xmm0, xmm3
			movlps[ecx + 4], xmm0
			xor byte ptr[ecx + 7], 0x80
			mulps	xmm3, xmm1
			movlps[ecx + 0x10], xmm3
			xor byte ptr[ecx + 0x17], 0x80
			movq	xmm0, xmm6
			retn
	}
}

__declspec(naked) NiMatrix33& __vectorcall NiMatrix33::FromEulerPRY(__m128 pry)
{
	__asm
	{
		andps	xmm0, PS_XYZ0Mask
		call	GetSinCos_V3
		pshufd	xmm2, xmm0, 0x55
		pshufd	xmm3, xmm1, 0x55
		pshufd	xmm4, xmm0, 0xA
		shufps	xmm4, xmm1, 0xA0
		shufps	xmm4, xmm4, 0x22
		shufps	xmm0, xmm1, 0
		shufps	xmm1, xmm0, 0
		pshufd	xmm5, xmm0, 8
		unpcklpd	xmm5, xmm4
		mulps	xmm3, xmm5
		mulps	xmm0, xmm2
		mulps	xmm0, xmm4
		shufps	xmm4, xmm4, 0x11
		mulps	xmm1, xmm4
		pshufd	xmm4, PS_FlipSignMask0, 0x14
		xorps	xmm1, xmm4
		addps	xmm0, xmm1
		xorps	xmm2, xmm4
		movhpd[ecx], xmm3
		movss[ecx + 8], xmm2
		movlpd[ecx + 0xC], xmm0
		shufps	xmm0, xmm3, 0x4E
		shufps	xmm0, xmm0, 0xD2
		movups[ecx + 0x14], xmm0
		mov		eax, ecx
		retn
	}
}

__declspec(naked) __m128 __vectorcall NiMatrix33::ToEulerPRY() const
{
	__asm
	{
		movq	xmm1, qword ptr[ecx]
		pshufd	xmm0, xmm1, 0xA9
		call	ATan2
		movq	xmm6, xmm0
		movss	xmm0, [ecx + 8]
		xorps	xmm0, PS_FlipSignMask0
		call	ASin
		movq	xmm7, xmm0
		movups	xmm0, [ecx + 0x14]
		pshufd	xmm1, xmm0, 3
		call	ATan2
		unpcklps	xmm0, xmm7
		unpcklpd	xmm0, xmm6
		retn
	}
}

__m128 __vectorcall NiMatrix33::ToEulerPRYAlt() const
{
	__m128 ret;
	__asm
	{
		movq	xmm1, qword ptr[ecx]
		pshufd	xmm0, xmm1, 0xA9
		call	ATan2
		movq	xmm6, xmm0
		movss	xmm0, [ecx + 8]
		xorps	xmm0, PS_FlipSignMask0
		call	ASin
		movq	xmm7, xmm0
		movups	xmm0, [ecx + 0x14]
		pshufd	xmm1, xmm0, 3
		call	ATan2
		unpcklps	xmm0, xmm7
		unpcklpd	xmm0, xmm6
		movaps ret, xmm0
	}
	return __m128{ret};
}

__declspec(naked) __m128 __vectorcall NiMatrix33::ToEulerPRYInv() const
{
	__asm
	{
		movups	xmm1, [ecx]
		pshufd	xmm0, xmm1, 3
		call	ATan2
		movq	xmm6, xmm0
		movss	xmm0, [ecx + 0x18]
		xorps	xmm0, PS_FlipSignMask0
		call	ASin
		movq	xmm7, xmm0
		movq	xmm0, qword ptr[ecx + 0x1C]
		pshufd	xmm1, xmm0, 0xA9
		call	ATan2
		unpcklps	xmm0, xmm7
		unpcklpd	xmm0, xmm6
		retn
	}
}

__declspec(naked) float __vectorcall NiMatrix33::ExtractPitch() const
{
	__asm
	{
		movss	xmm0, [ecx + 0x1C]
		xorps	xmm0, PS_FlipSignMask0
		jmp		ASin
	}
}

__declspec(naked) float __vectorcall NiMatrix33::ExtractRoll() const
{
	__asm
	{
		movups	xmm0, [ecx + 0x18]
		pshufd	xmm1, xmm0, 2
		jmp		ATan2
	}
}

__declspec(naked) float __vectorcall NiMatrix33::ExtractYaw() const
{
	__asm
	{
		movups	xmm0, [ecx + 4]
		pshufd	xmm1, xmm0, 3
		jmp		ATan2
	}
}

__declspec(naked) NiMatrix33& __vectorcall NiMatrix33::FromEulerPRYInv(__m128 pry)
{
	__asm
	{
		andps	xmm0, PS_XYZ0Mask
		call	GetSinCos_V3
		pshufd	xmm2, xmm0, 0x55
		pshufd	xmm3, xmm1, 0x55
		pshufd	xmm4, xmm1, 0xA
		shufps	xmm4, xmm0, 0xA0
		unpcklps	xmm0, xmm1
		unpcklpd	xmm0, xmm0
		pshufd	xmm1, xmm0, 0x11
		pshufd	xmm5, xmm4, 8
		unpcklpd	xmm5, xmm0
		mulps	xmm3, xmm5
		mulps	xmm0, xmm2
		mulps	xmm0, xmm4
		shufps	xmm4, xmm4, 0xA
		mulps	xmm1, xmm4
		pshufd	xmm4, PS_FlipSignMask0, 0x14
		xorps	xmm1, xmm4
		addps	xmm0, xmm1
		xorps	xmm2, xmm4
		movhpd	[ecx + 0x10], xmm0
		movss	[ecx + 0x18], xmm2
		movhpd	[ecx + 0x1C], xmm3
		unpcklpd	xmm0, xmm3
		shufps	xmm0, xmm0, 0xD2
		movups	[ecx], xmm0
		mov		eax, ecx
		retn
	}
}

__declspec(naked) NiMatrix33& __fastcall NiMatrix33::MultiplyMatrices(const NiMatrix33& matB)
{
	__asm
	{
		movups	xmm0, [edx]
		movups	xmm1, [edx + 0xC]
		movups	xmm2, [edx + 0x18]
		movups	xmm3, [ecx]
		movups	xmm4, [ecx + 0x10]
		pshufd	xmm5, xmm3, 0
		mulps	xmm5, xmm0
		pshufd	xmm6, xmm3, 0x15
		mulps	xmm6, xmm1
		addps	xmm5, xmm6
		pshufd	xmm6, xmm3, 0x2A
		mulps	xmm6, xmm2
		addps	xmm5, xmm6
		movups	[ecx], xmm5
		pshufd	xmm5, xmm3, 0x3F
		mulps	xmm5, xmm0
		pshufd	xmm6, xmm4, 0
		mulps	xmm6, xmm1
		addps	xmm5, xmm6
		pshufd	xmm6, xmm4, 0x15
		mulps	xmm6, xmm2
		addps	xmm5, xmm6
		movups	[ecx + 0xC], xmm5
		pshufd	xmm5, xmm4, 0x2A
		mulps	xmm5, xmm0
		pshufd	xmm6, xmm4, 0x3F
		mulps	xmm6, xmm1
		addps	xmm5, xmm6
		movss	xmm3, [ecx + 0x20]
		shufps	xmm3, xmm3, 0x40
		mulps	xmm3, xmm2
		addps	xmm5, xmm3
		movlps	[ecx + 0x18], xmm5
		unpckhpd	xmm5, xmm5
		movss	[ecx + 0x20], xmm5
		mov		eax, ecx
		retn
	}
}

__declspec(naked) NiMatrix33& __fastcall NiMatrix33::MultiplyMatricesInv(const NiMatrix33& matB)
{
	__asm
	{
		movups	xmm0, [edx]
		movups	xmm1, [ecx]
		pshufd	xmm2, xmm0, 0
		mulps	xmm2, xmm1
		pshufd	xmm3, xmm0, 0x55
		mulps	xmm3, xmm1
		pshufd	xmm4, xmm0, 0xAA
		mulps	xmm4, xmm1
		movups	xmm0, [edx + 0xC]
		movups	xmm1, [ecx + 0xC]
		pshufd	xmm5, xmm0, 0
		mulps	xmm5, xmm1
		pshufd	xmm6, xmm0, 0x55
		mulps	xmm6, xmm1
		pshufd	xmm7, xmm0, 0xAA
		mulps	xmm7, xmm1
		addps	xmm2, xmm5
		addps	xmm3, xmm6
		addps	xmm4, xmm7
		movups	xmm0, [edx + 0x18]
		movups	xmm1, [ecx + 0x18]
		pshufd	xmm5, xmm0, 0
		mulps	xmm5, xmm1
		pshufd	xmm6, xmm0, 0x55
		mulps	xmm6, xmm1
		pshufd	xmm7, xmm0, 0xAA
		mulps	xmm7, xmm1
		addps	xmm2, xmm5
		addps	xmm3, xmm6
		addps	xmm4, xmm7
		movups	[ecx], xmm2
		movups	[ecx + 0xC], xmm3
		movlps	[ecx + 0x18], xmm4
		unpckhpd	xmm4, xmm4
		movss	[ecx + 0x20], xmm4
		mov		eax, ecx
		retn
	}
}

__declspec(naked) __m128 __vectorcall NiMatrix33::MultiplyVector(__m128 vec) const
{
	__asm
	{
		movaps	xmm1, xmm0
		andps	xmm1, PS_XYZ0Mask
		movups	xmm0, [ecx]
		mulps	xmm0, xmm1
		xorps	xmm3, xmm3
		haddps	xmm0, xmm3
		haddps	xmm0, xmm3
		movups	xmm2, [ecx + 0xC]
		mulps	xmm2, xmm1
		haddps	xmm2, xmm3
		haddps	xmm2, xmm3
		unpcklps	xmm0, xmm2
		movups	xmm2, [ecx + 0x18]
		mulps	xmm2, xmm1
		haddps	xmm2, xmm3
		haddps	xmm2, xmm3
		unpcklpd	xmm0, xmm2
		retn
	}
}

NiMatrix33& __vectorcall NiMatrix33::Rotate(__m128 rot)
{
	__asm
	{
		push	ecx
		lea		ecx, [esp - 0x28]
		call	NiMatrix33::FromEulerPRYInv
		mov		edx, eax
		pop		ecx
		jmp		NiMatrix33::MultiplyMatrices
	}
}


