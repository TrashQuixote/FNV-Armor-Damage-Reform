#pragma once
#include "Gathering_Code.h"
#include "Gathering_Havok.h"

//__declspec(naked) NiAVObject* __fastcall GetNifBlock(TESObjectREFR* thisObj, UInt32 pcNode, char* blockName)
//{
//	__asm
//	{
//		test	dl, dl
//		jz		notPlayer
//		cmp		dword ptr[ecx + 0xC], 0x14
//		jnz		notPlayer
//		test	dl, 1
//		jz		get1stP
//		mov		eax, [ecx + 0x64]
//		test	eax, eax
//		jz		done
//		mov		eax, [eax + 0x14]
//		jmp		gotRoot
//		get1stP :
//		mov		eax, [ecx + 0x694]
//		jmp		gotRoot
//	notPlayer :
//		call	TESObjectREFR::GetNiNode
//	gotRoot :
//		test	eax, eax
//			jz		done
//			mov		edx, [esp + 4]
//			cmp[edx], 0
//			jz		done
//			mov		ecx, eax
//			call	NiNode::GetBlock
//			done :
//		retn	4
//	}
//}


void __forceinline ResetNodeCollision(NiNode* _node) {
	_node->m_collisionObject->flags |= bhkNiCollisionObject::Flags::kFlag_Reset; 
}

void GetCollisionNodes(NiNode* _node,OUT std::vector<NiNode*>& _res)
{
	if (_node->m_collisionObject && _node->m_collisionObject->worldObj)
		_res.emplace_back(_node);
	for (auto iter = _node->m_children.Begin(); iter; ++iter) 
		if (*iter && IS_NODE(*iter))
			GetCollisionNodes((NiNode*)*iter, OUT _res);
}

void GetCollisionNodes(TESObjectREFR* _this, OUT std::vector<NiNode*>& _res) {
	if (NiNode* rootNode = _this->GetRefNiNode())
	{
		GetCollisionNodes(rootNode, _res);
	}
}

__forceinline hkpWorldObject* GetNodeWorldObject(NiNode* _node) {
	if (_node && _node->m_collisionObject) { 
		if (bhkWorldObject* worldObj = _node->m_collisionObject->worldObj) {
			gLog.Message("Find Node World Obj");
			return static_cast<hkpWorldObject*>(worldObj->refObject);
		}
		else {
			gLog.Message("Cant Find Node World Obj");
		}
	}
	else {
		gLog.Message("Node is none or Node collisionObj is none");
	}
	return nullptr;
}

__forceinline CollisionLayerTypes GetCollisionLayerType(NiNode* _node) {
	if (_node && _node->m_collisionObject)
		if (bhkWorldObject* worldObj = _node->m_collisionObject->worldObj)
			return (CollisionLayerTypes)(((hkpWorldObject*)worldObj->refObject)->layerType);
	return LAYER_MAX;
}


ProjectileListener* GetProjectileListener(Projectile* _proj) {
	if (auto* pj_process = _proj->baseProcess; pj_process  && pj_process->processLevel == BaseProcess::kProcessLevel_High) {
		HighProcess* pj_hProc = static_cast<HighProcess*>(pj_process);
		if (auto* pj_charCtrl = pj_hProc->charCtrl) {
			if (IS_TYPE(pj_charCtrl, ProjectileListener)) {
				gLog.Message("Find ProjectileListener");
				return static_cast<ProjectileListener*>(pj_charCtrl);
			}
			gLog.Message("Find bhkCharacterController but not ProjectileListener");
			return nullptr;
		}
		gLog.Message("Cant Find bhkCharacterController");
		return nullptr;
	}
	return nullptr;
}

bool __forceinline ResetProjectileCharCtrlFlag(Projectile* _proj) {
	if (auto * pjListener = GetProjectileListener(_proj)) {
		pjListener->chrListener.flags = 0x80000;
		return true;
	}
	return false;
}

void __forceinline PrintProjListenerInfo(Projectile* _proj) {
	auto* pjListener = GetProjectileListener(_proj);
	if (pjListener) {
		gLog.FormattedMessage("Print PJListener : unk650 %x",pjListener->unk650);
		gLog.FormattedMessage("Print charCtrl : chrListener.flags %x",pjListener->chrListener.flags);
		gLog.FormattedMessage("Print charCtrl : chrContext.hkState %x,chrContext.sizeAndFlag %x",pjListener->chrContext.hkState, pjListener->chrContext.sizeAndFlags);
	}
}

void __forceinline PrintWorldObjInfo(NiNode* _node) {
	auto* w_obj = GetNodeWorldObject(_node);
	if (w_obj) {
		gLog.FormattedMessage("Print world obj : collisionType %x", w_obj->collisionType);
		gLog.FormattedMessage("Print world obj : layerType %x, filterFlags %x, group %x", w_obj->layerType, w_obj->filterFlags, w_obj->group);
	}
	else {
		gLog.Message("world obj is none");
	}
}





