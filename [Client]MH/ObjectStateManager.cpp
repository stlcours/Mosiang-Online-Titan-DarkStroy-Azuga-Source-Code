// ObjectStateManager.cpp: implementation of the CObjectStateManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "ObjectActionManager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CObjectStateManager);
CObjectStateManager::CObjectStateManager()
{

}

CObjectStateManager::~CObjectStateManager()
{

}

void CObjectStateManager::InitObjectState(CObject* pObject) 
{
	BYTE State = pObject->m_BaseObjectInfo.ObjectState;
	if(State == eObjectState_Die)
	{		
		if(pObject->GetObjectKind() & eObjectKind_Monster)
		{
			pObject->GetEngineObject()->ChangeMotion(eMonsterMotion_Died);
			pObject->GetEngineObject()->DisablePick();
		}
		// 죽어서도 클릭되도록
		else if(pObject->GetObjectKind() & eObjectKind_Player)
		{
			pObject->GetEngineObject()->ChangeMotion(eMotion_Died_Normal);
			pObject->GetEngineObject()->EnablePick();
		}
		
		return;
	}
	pObject->OnStartObjectState(State);
	pObject->SetState(State);
}

BOOL CObjectStateManager::StartObjectState(CObject* pObject,BYTE State) 
{
	if( !pObject )			return FALSE;

	switch(pObject->GetState())
	{		
	case eObjectState_Die:
		{
			// 횁횞째횇쨀짧 째횚?횙?쨩 쨀징쨀쨩쨈횂째횉 쩐챨횁짝쨀짧 째징쨈횋횉횕쨈횢.
			if( State != eObjectState_Exit )
			{
				char buf[256];
				sprintf(buf, "StartObjectState(NowState : %d, ChangeState : %d)", pObject->GetState(), State);
//				ASSERTMSG(0, buf);
			}
		}
		return FALSE;

	case eObjectState_Move:
		{
			EndObjectState(pObject,pObject->GetState());
		}
		break;
	case eObjectState_Tactic:
		if(State == eObjectState_Tactic)
			break;

	case eObjectState_Ungijosik:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
	case eObjectState_StreetStall_Guest:
	case eObjectState_Deal:
	case eObjectState_TiedUp:
		{ 
			// 횁횞째횇쨀짧 째횚?횙?쨩 쨀징쨀쨩쨈횂째횉 쩐챨횁짝쨀짧 째징쨈횋횉횕쨈횢.
			if( State != eObjectState_Die &&
				State != eObjectState_Exit &&
				pObject->GetID() == HERO->GetID())
			{
				char buf[256];
				sprintf(buf, "StartObjectState(NowState : %d, ChangeState : %d)", pObject->GetState(), State);
//				ASSERTMSG(0, buf);
				return FALSE;
			}
		}
		break;
	case eObjectState_TitanRecall:
	case eObjectState_Society:
		{
			//쩌짯짹창?횕쨋짠쨍쨍 쨉청쨌횓 횄쨀쨍짰횉횘 째횒?횑 횉횎쩔채횉횕쨈횢.
			if( pObject->EndSocietyAct() == FALSE )
				return FALSE;
			else
				EndObjectState( pObject, pObject->GetState() );
		}
		break;		
	default:
		break;
	}

	//?횙쩍횄
	if(pObject->GetState() == eObjectState_Ungijosik)
		pObject->OnEndObjectState(eObjectState_Ungijosik);

//	if( pObject->GetState() == eObjectState_Die )
//		return;

	pObject->OnStartObjectState(State);
	pObject->SetState(State);

	return TRUE;
}


void CObjectStateManager::EndObjectState(CObject* pObject,BYTE State,DWORD EndStateCount)
{
	if( !pObject )			return;

	if(pObject->GetState() != State) 
	{
		if( pObject->GetState() == eObjectState_Die )		//KES횄횩째징
		{
			return;	//횁횞쩐첬?쨩쨋짠 none?쨍쨌횓 쨔횢짼횢쨈횂째횒?쨘 쩔챘쨀쨀횉횕횁철 쨍첩횉횗쨈횢!
		}

		if((pObject->GetState() == eObjectState_None) && (State == eObjectState_Ungijosik))
		{
		}
		else
		{
			char str[256];
			sprintf(str,"EndState Assert Cur:%d EndState:%d",pObject->GetState(),State);
			
//			ASSERTMSG(0,str);
		}
	}

	if(EndStateCount == 0)	// 횁철짹횦 횁챦쩍횄 쨀징쨀쩍쨈횢
	{
		pObject->SetState(eObjectState_None);
		pObject->OnEndObjectState(State);		//setstate()횉횕짹창?체쩔징 횊짙횄창횉횠쩐횩횉횗쨈횢.
	}
	else
	{
		pObject->m_ObjectState.State_End_Time = gCurTime + EndStateCount;
		pObject->m_ObjectState.bEndState = TRUE;
	}
}

void CObjectStateManager::StateProcess(CObject* pObject)
{
	if(pObject->m_ObjectState.bEndState)
	{
		if(pObject->m_ObjectState.State_End_Time < gCurTime)
		{
			EndObjectState(pObject,pObject->m_BaseObjectInfo.ObjectState);
		}
		else
		{
			// 쨀징쨀쨩짹창 쨉쩔?횤?쨍쨌횓 ?체횊짱
			DWORD RemainTime = pObject->m_ObjectState.State_End_Time - gCurTime;
			if(RemainTime <= pObject->m_ObjectState.State_End_MotionTime)
			{
				pObject->m_ObjectState.State_End_MotionTime = 0;
				if(pObject->m_ObjectState.State_End_Motion != -1)
				{
					pObject->ChangeMotion(pObject->m_ObjectState.State_End_Motion,FALSE);

					// magi82(10) - Effect(071025) 경공시에는 경공종료 모션으로 바뀔때 2번째 경공 이펙트를 종료해야한다.
					if(pObject->m_ObjectState.State_End_Motion == eMotion_KyungGong1_End_NoWeapon)
					{
						pObject->RemoveObjectEffect(KYUNGGONG_EFFECTID);
					}
				}
			}
		}
	}
}

BYTE CObjectStateManager::GetObjectState(CObject* pObject)
{
	ASSERT(pObject);
	return pObject->GetState();
}

BOOL CObjectStateManager::IsEndStateSetted(CObject* pObject)
{
	return pObject->m_ObjectState.bEndState;
}
/*
BOOL CObjectStateManager::CheckObjectState(CPlayer* pPlayer, BYTE State)
{
	BYTE CurState = pPlayer->GetState();

	switch(State)
	{
	case eObjectState_Ungijosik:
	case eObjectState_Exchange:
	case eObjectState_StreetStall_Owner:
		{
			if( CurState != eObjectState_None ) 
				return FALSE;
		}
		break;
	case eObjectState_SkillStart:
	case eObjectState_SkillSyn:
	case eObjectState_SkillBinding:
	case eObjectState_SkillUsing:
	case eObjectState_SkillDelay:
	case eObjectState_Deal:
	case eObjectState_StreetStall_Guest:
	case eObjectState_Tactic:
		{
			if( CurState != eObjectState_None &&
				CurState != eObjectState_Move ) 
				return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
*/

#define NOT_AVAILABLESTATE(a)		{	if(pObject->GetState() == (a))	return FALSE;	}
#define AVAILABLESTATE(a)			{	if(pObject->GetState() == (a))	return TRUE;	}

BOOL CObjectStateManager::CheckAvailableState(CObject* pObject,eCHECKAVAILABLESTATE cas)
{
	if( pObject->GetState() == eObjectState_Die )
	{
		//횁횞?쨘 쨩처횇횂쩔징쩌짯쨈횂 ?책횂첩?쨩 쨘짱째챈횉횘 쩌철 쩐첩쩍?쨈횕쨈횢.
		return FALSE;
	}

	ySWITCH(cas)
		yCASE(eCAS_EQUIPITEM)
			NOT_AVAILABLESTATE(eObjectState_SkillStart)
			NOT_AVAILABLESTATE(eObjectState_SkillSyn)
			NOT_AVAILABLESTATE(eObjectState_SkillBinding)
			NOT_AVAILABLESTATE(eObjectState_SkillUsing)
			NOT_AVAILABLESTATE(eObjectState_SkillDelay)
			return TRUE;
	yENDSWITCH

	return TRUE;
}
