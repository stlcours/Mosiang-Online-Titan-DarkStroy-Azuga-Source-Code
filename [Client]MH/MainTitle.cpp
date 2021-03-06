// MainTitle.cpp: implementation of the CMainTitle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MainTitle.h"
#include "MainGame.h"

#include "cWindowSystemFunc.h"
#include "WindowIDEnum.h"
#include "MHCamera.h"

#include "AppearanceManager.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cEditBox.h"
#include "./Interface/cResourceManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "ItemManager.h"
#include "KyungGongManager.h"
#include "ChatManager.h"
#include "MHTimeManager.h"

#include "FilteringTable.h"
#include "cMsgBox.h"
#include "SkillManager_client.h"

#include "mhMap.h"
#include "./Input/Mouse.h"
#include "./Input/Keyboard.h"
#include "MotionManager.h"
#include "AbilityManager.h"
#include "AbilityUpdater_Interface.h"
#include "AbilityUpdater_ApplyData.h"
#include "ServerListDialog.h"
#include "CharSelect.h"
//#include "cImageSelf.h"
#include "DissolutionManager.h"
#include "cMonsterSpeechManager.h"
#include "PetSpeechManager.h"
#include "TitanManager.h"	// magi82 - Titan(070518)
#include "OptionManager.h"

#ifdef _GMTOOL_
#include "GMToolManager.h"
#endif

#ifdef TAIWAN_LOCAL
#include "BillingManager.h"
#endif

#include "GMNotifyManager.h"
#include "ObjectManager.h"
#include "MoveManager.h"

#include "UserInfoManager.h"
#include "cScriptManager.h"
#include "./Input/UserInput.h"
#include "Intro.h"
#include "IntroReplayDlg.h"
#include "NumberPadDialog.h"	// 2008. 3. 17. CBH
#include "GameIn.h"

#include <time.h>
#include <shellapi.h>


#define _LOGOWINDOW_

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern char g_DistributeAddr[16];
extern WORD g_DistributePort;
extern char g_AgentAddr[16];
extern WORD g_AgentPort;
extern char g_szHeroIDName[];
extern int	g_nServerSetNum;

//extern char loginid[MAX_NAME_LEN];
//extern char loginpass[MAX_NAME_LEN];
//extern char g_CLIENTVERSION[];


#define ALPHA_PROCESS_TIME	1200
#define WAIT_CONNECT_TIME	60000	//에이젼트로 접속기다림 시간 90초




void SendOutMsg()
{
	MSGBASE msg;
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_REQUEST_DISTOUT;
#ifdef _HK_LOCAL_
	msg.dwObjectID = 5434;	//5434
#endif
	NETWORK->Send(&msg, sizeof(msg));
}

BOOL bFirst;
GLOBALTON(CMainTitle)
CMainTitle::CMainTitle()
{
#ifdef TAIWAN_LOCAL
	m_pAdvice		= NULL;
#endif
	m_pLogoWindow	= NULL;
	m_bInit			= FALSE;

	m_pServerListDlg = NULL;
	memset( m_DistributeAddr, 0, 16 );
	m_DistributePort = 0;
	m_bServerList = FALSE;

	m_bDisconntinToDist = FALSE;
	m_bDynamicDlg = FALSE;
#ifdef _HK_LOCAL_
	m_dwEnc = 0;
#endif

	m_pIntroReplayDlg = NULL;
}

CMainTitle::~CMainTitle()
{	
}

static BOOL g_bResourceLoaded = FALSE;

//cImageSelf image;

BOOL CMainTitle::Init(void* pInitParam)
{
	//image.LoadSprite("image/login.tif");

	//trustpak 2005/03/19
//	m_imageLoginBarTop.LoadSprite("image/login_bar00.TIF");
//	m_imageLoginBarBottom.LoadSprite("image/login_bar01.TIF");	
	m_imageLoginBarTop.LoadSprite("image/2D/login_bar00.TIF");
	m_imageLoginBarBottom.LoadSprite("image/2D/login_bar01.TIF");

	// magi82 - ServerListBackImage(070802)
	m_imageLoginBackGround.LoadSprite("image/2D/login.tga");

	//
	m_pLogoWindow	= NULL;
	m_bInit			= FALSE;
	m_pServerListDlg = NULL;
	memset( m_DistributeAddr, 0, 16 );
	m_DistributePort = 0;
	//
	
	m_bDisconntinToDist = FALSE;
	m_bNoDiconMsg		= FALSE;	//KES 유저가 접속을 끊었을때는 메세지를 안띄우기위한 변수
	m_bDynamicDlg = FALSE;

	m_pIntroReplayDlg = NULL;

	NETWORK->SetCurState(this);

	// 게임리소스 로드///////////////////////////////////////////////////////////////////////

	// magi82 - ServerListBackImage(070802)
	/*if(MAP->IsInited() == TRUE)
	{
		MAP->Release();
	}*/
	if(MAP->IsInited() == FALSE)
	{
		MAP->InitMap(94);//108//94
	}
	else
	{
		MAP->Release();
		MAP->InitMap(94);
	}

	if(g_bResourceLoaded == FALSE)
	{
		FILTERTABLE->Init();
		CHATMGR->Init();
		MON_SPEECHMGR->Init();		
		PET_SPEECHMGR->Init();
		//이걸 아래로 내렸더니 경로가 바뀌어서 파일을 로드할 수 없다.
		//아래중 경로를 바꾸어 놓는 것이 있으면 제자리로 돌려놓으시오~.
		MOTIONMGR->LoadMotionList();
		SKILLMGR->Init();
		ITEMMGR->LoadItemList();
		ITEMMGR->LoadItemToolTipList();
		ITEMMGR->LoadSetItemOption(); //2007. 6. 8. CBH - 세트아이탬 스크립트 로드 추가
		ITEMMGR->LoadRareItemInfo();		
		GAMERESRCMNGR->LoadMonsterList();
		GAMERESRCMNGR->LoadMonsterInfoInMapList(); //060608 - Add by wonju
		GAMERESRCMNGR->LoadMapChangeArea(); //060608 - Add by wonju
		GAMERESRCMNGR->LoadUserCommendList(); //060727 - Add by wonju
	//	GAMERESRCMNGR->LoadSMonsterList();
		GAMERESRCMNGR->LoadNpcChxList();
		GAMERESRCMNGR->LoadNpcList(GAMERESRCMNGR->m_NpcList);
		GAMERESRCMNGR->LoadItemChxList();
		GAMERESRCMNGR->LoadPetList();
		GAMERESRCMNGR->LoadPetRule();
		GAMERESRCMNGR->LoadPetBuffList();
		GAMERESRCMNGR->LoadTitanList();
		GAMERESRCMNGR->LoadExpPoint();
		GAMERESRCMNGR->LoadTacticStartInfoList();
		GAMERESRCMNGR->LoadPreDataTable();
		// JSD
		GAMERESRCMNGR->LoadServerList();
		
		//
		GAMERESRCMNGR->LoadShopItemDupList();

		//SW061205 깃발 NPC
		GAMERESRCMNGR->LoadFlagNpcInfo();

		KYUNGGONGMGR->Init();
		
		APPEARANCEMGR->Init();
		GAMERESRCMNGR->PreLoadData();
		ABILITYMGR->Init();
		ABILITYMGR->AddUpdater(CAbilityUpdater_Interface::GetInstance());
		ABILITYMGR->AddUpdater(CAbilityUpdater_ApplyData::GetInstance());
		
		DISSOLUTIONMGR->Init();	//KES

		// magi82 - Titan(070518)
		TITANMGR->LoadTPMList();
		TITANMGR->LoadTitanPartsKindList();
		TITANMGR->LoadTitanUpgradeInfoList();
		TITANMGR->LoadTitanBreakInfoList();
		GAMERESRCMNGR->LoadTitanRule();	// magi82 - Titan(070604)
		
		// magi82 - UniqueItem(070625)
		GAMERESRCMNGR->LoadUniqueItemOptionList();
		GAMERESRCMNGR->LoadUniqueItemMixList();	// magi82 - Unique(070709)

		GAMERESRCMNGR->LoadNomalClothesSkinList();	//2007. 12. 4. CBH - 일반 의복 스킨 아이탬 목록 로드
		GAMERESRCMNGR->LoadCostumeSkinList();	//2008. 1. 16. CBH - 코스튬 스킨 아이탬 목록 로드
		GAMERESRCMNGR->LoadMapKindInfo();	// magi82 임시
		g_bResourceLoaded = TRUE;
		GAMERESRCMNGR->LoadPlusItemEffect();	//2008. 6. 18. CBH - +10 이상 아이탬 이펙트 관련 데이터
	}
	/////////////////////////////////////////////////////////////////////////////////////////

	/*
	VECTOR3 vPivot;
	vPivot.x = 25408;
	vPivot.y = 6545;
	vPivot.z = 4004;

	CAMERA->InitCamera(0, 0.0f, -1.0f, 2000, &vPivot);// 거리 조정 
	CAMERA->SetCameraMode(eCM_LOGIN);
	CAMERA->SetCurCamera(0);
	*/

	// MHCamera 는 넣은 값을 뭔가의 계산을 통해 변형해서 쓰기 때문에 
	// 직접 카메라를 세팅하였다.
	//VECTOR3 v3Eye = { 19025.675781f, 156.450745f, 370.196259f };	
	
	// magi82 - ServerListBackImage(070802)
	
	//VECTOR3 v3Eye = { 19025.675781f, 10.0f, 370.196259f };	
	//VECTOR3 v3Rot = { 0.198968f, -0.010472f, 0.0f };
	/*VECTOR3 v3Eye = { 0.001095f, 134.809937f, 495.596436f };	
	VECTOR3 v3Rot = { 0.6f, 360.f, 0.0f };

	g_pExecutive->GetGeometry()->SetCameraPos(&v3Eye, 0);
	g_pExecutive->GetGeometry()->SetCameraAngleRad(&v3Rot, 0);

	CAMERA->SetCameraMode(eCM_LOGIN);
	CAMERA->SetCurCamera(0);*/
	VECTOR3 vPivot;
	vPivot.x = 0;
	vPivot.y = 0;
	vPivot.z = 0;

	CAMERA->InitCamera( 0, 0.6f, 360.f, -500, &vPivot);
	//x왠齡�豺株骸케龜쨌싹�,鑒令댕角蕨苟;y왠齡�豺株骸켰慘念싹�,鑒令댕角蕨璘;z角陶쐤
	//vPivot角�豺株骸켑뿟�,x깊刻璘塘,y깊刻�龜�,z깊刻품빈.

/* test
	MHCAMERAPOS MCPos;
	MCPos.m_AngleX = -2.0f;
	MCPos.m_AngleY = 17.0f;
	MCPos.m_Dist = 2000;
	MCPos.m_Pivot.x = 27740;
	MCPos.m_Pivot.y = 4960;
	MCPos.m_Pivot.z = 9104;	
	CAMERA->SetMovePos(eCP_CHARSEL, &MCPos);

	CAMERA->InitCamera(0, -2.0f, 17.0f, 2000, &MCPos.m_Pivot);// 거리 조정 
	CAMERA->SetCameraMode(eCM_LOGIN);
	CAMERA->SetCurCamera(0);	

	// 캐릭터 생성 
	vPivot.x = 24480;
	vPivot.y = 6045;
	vPivot.z = 9604;
	MCPos.m_AngleX = -5.0f;
	MCPos.m_AngleY = 0.0f;
	MCPos.m_Dist = 2000;
	MCPos.m_Pivot = vPivot;
	CAMERA->SetMovePos(eCP_CHARMAKE, &MCPos);
//*/
	
	WINDOWMGR->SetcbDropProcess(cbDragDropProcess);
//	WINDOWMGR->CreatMousePoint();
//

	CreateMainTitle_m();	
	m_pLogoWindow = WINDOWMGR->GetWindowForID( MT_LOGODLG );
	if( m_pLogoWindow )
		m_pLogoWindow->SetActive( TRUE );

#ifdef TAIWAN_LOCAL
	m_pAdvice = WINDOWMGR->GetWindowForID( CNA_CNADVICEDLG );
#endif

	m_pServerListDlg = (CServerListDialog*)WINDOWMGR->GetWindowForID( SL_SERVERLISTDLG );
	if( m_pServerListDlg )
	{
		m_pServerListDlg->Linking();
		m_pServerListDlg->SetActive( FALSE );
	}
//
	WINDOWMGR->AfterInit();

///	MHTIMEMGR->Process();
	m_bWaitConnectToAgent = FALSE;
	bFirst = TRUE;

	// magi82 - Intro(070802)
	m_pIntroReplayDlg = (CIntroReplayDlg*)WINDOWMGR->GetWindowForID( INTRO_DLG );
	if( m_pIntroReplayDlg )
	{
		m_pIntroReplayDlg->SetAbsXY(GET_MAINWIN_W - 41, GET_MAINWIN_H - 120);
		m_pIntroReplayDlg->SetActive( FALSE );
	}
		
#ifndef _LOGOWINDOW_
	m_pLogoWindow = NULL;	
	m_bServerList = TRUE;
#endif
	
#ifdef TAIWAN_LOCAL
	BILLINGMGR->Init();	
#endif

	//trustpak 2005/04/01
	
#ifndef TAIWAN_LOCAL
	InitMainTitleMonsters();
#endif
	///



	return TRUE;
}

void CMainTitle::Release(CGameState* pNextGameState)
{
	//image.Release();	

	m_imageLoginBarTop.Release();
	m_imageLoginBarBottom.Release();	
	m_imageLoginBackGround.Release();


#ifndef TAIWAN_LOCAL
	//trustpak 2005/04/01	

	RemoveMainTitleMonsters();

	///
#endif

	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(24);

	if(pNextGameState == NULL)
		MAP->Release();



}

void CameraRotate(CKeyboard& Keyboard,CMouse& Mouse);
void CameraWheelZoom(CKeyboard& Keyboard,CMouse& Mouse);
#include "ObjectGuagen.h"
void CMainTitle::Process()
{
#ifndef TAIWAN_LOCAL
	//trustpak 2005/04/01
	ProcessMainTitleMonsters();
	///
#endif

	//엔진이상 -_-
	if(bFirst == TRUE)	//이렇게 해도 깨지는데?
	{
		bFirst = FALSE;
		CAMERA->MouseRotate(1,0);
		m_dwStartTime = MHTIMEMGR->GetNewCalcCurTime();
	}

	if( m_pLogoWindow )
	{
		DWORD dwElapsed = MHTIMEMGR->GetNewCalcCurTime() - m_dwStartTime;
		
		if( dwElapsed > ALPHA_PROCESS_TIME )
		{
			m_pLogoWindow->SetAlpha( 0 );
			WINDOWMGR->AddListDestroyWindow( m_pLogoWindow );
			m_pLogoWindow = NULL;
			
#ifdef TAIWAN_LOCAL
			m_pAdvice->SetActive( TRUE );
#else
			m_bServerList = TRUE;
#endif
		}
		else
		{
			m_pLogoWindow->SetAlpha( (BYTE)(255 - ( dwElapsed * 255 / ALPHA_PROCESS_TIME )) );
		}
	}

	if( m_bServerList )
	{
		m_pServerListDlg->SetActive( TRUE );
		m_bServerList = FALSE;

		m_pIntroReplayDlg->SetActive( TRUE );	// magi82 - Intro(070802)

		//瞳侶쟁뇹잿�豺株骸켠틋�
		
	}

	// Dist Server에 접속되었다면...
	if( m_bInit )
	{
		if( m_bDynamicDlg == FALSE )
		{
			cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pDlg )
			if( !pDlg->IsActive() )
			{
				pDlg->SetDisable( FALSE );
				pDlg->SetActive( TRUE );
				cEditBox* pEdit = (cEditBox*)pDlg->GetWindowForID( MT_IDEDITBOX );
				pEdit->SetFocusEdit( TRUE );
#ifdef _HK_LOCAL_
				m_DistAuthKey = (m_DistAuthKey - 47)/3;
				m_dwEnc = (m_dwEnc-m_DistAuthKey-23)/7;
#endif

#ifdef _KOR_LOCAL_
				//보안번호는 키패드로만 입력만 가능하게 해야하기 때문에 직접 입력 못하게 막는다.				
				GAMEIN->GetNumberPadDlg()->InitProtectionStr();

				//2008. 3. 17. CBH - 보안번호 다이얼로그도 같이 띄운다. 단, 버디버디일때는 안띄운다
			
				cComboBox* pCombo = (cComboBox*)WINDOWMGR->GetWindowForIDEx(MT_LISTCOMBOBOX);
				int nIndex = pCombo->GetCurSelectedIdx();
				
				if( nIndex != 3)	//버디버디가 아니면 보안번호 입력 다이얼로그를 띄운다
				{
					CNumberPadDialog* pNumberPadDlg = GAMEIN->GetNumberPadDlg();
					pNumberPadDlg->SetActive(FALSE);//瞳longin썹충角뤠鞫刻갛홍鑒俚pad,侶쟁밑균
				}				
#endif



				//MSG_LOGIN_SYN msg;
				//msg.Category = MP_USERCONN;
				//msg.Protocol = MP_USERCONN_LOGIN_SYN;

				//SafeStrCpy(msg.id, loginid, MAX_NAME_LENGTH + 1 );
				//SafeStrCpy(msg.pw, loginpass, MAX_NAME_LENGTH + 1 );
				//SafeStrCpy(msg.Version,g_CLIENTVERSION, 16 );
				//msg.gate = 0;

				//msg.AuthKey = TITLE->GetDistAuthKey();
				//NETWORK->Send(&msg,sizeof(msg));

				//SafeStrCpy(g_szHeroIDName, loginid, MAX_NAME_LENGTH + 1 );

				//TITLE->StartWaitConnectToAgent( TRUE );

				//USERINFOMGR->SetUserID( loginid );



			}	
		}

		//접속기다리는 중이라면
		if( m_bWaitConnectToAgent == TRUE )
		{
			DWORD dwElapsed = gCurTime - m_dwWaitTime;
		
			if( dwElapsed > WAIT_CONNECT_TIME )
			{
				//취소버튼을 누른것 처럼
				cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
				if( pMsgBox )
					pMsgBox->ForceClose();

				m_bWaitConnectToAgent = FALSE;

				WINDOWMGR->MsgBox( MBI_TIMEOVER_TOCONNECT, MBT_OK, CHATMGR->GetChatMsg( 445 ) );
				//서버리스트를 감추자???
				m_pServerListDlg->SetActive( FALSE );
				m_bServerList = FALSE;

				m_pIntroReplayDlg->SetActive( FALSE );	// magi82 - Intro(070802)
			}
		}
/*
		if( m_bDisconntinToDist == TRUE ) // wait for disconnect
		{
			DWORD dwElapsed = gCurTime - m_dwDiconWaitTime;
			if( dwElapsed > 10 * 1000 ) //10 sec
			{
				if( NETWORK->IsConnected() )
				{
					NETWORK->Disconnect();
				}

				m_dwDiconWaitTime = gCurTime;
			}
		}
*/
	}	

}


void CMainTitle::BeforeRender()
{
	g_pExecutive->GetRenderer()->BeginRender(0,0,0);
	g_pExecutive->GetRenderer()->EndRender();
	MAP->Process( gCurTime );
}
void CMainTitle::AfterRender()
{
	/*
	VECTOR2 t;
	t.x = t.y = 0;
	image.RenderSprite(0,0,0,&t,0xffffffff);
	*/

	// magi82 - ServerListBackImage(070802)
	VECTOR2 v2Pos = { 0, 0 };
	VECTOR2 PosScale = { 1, 1 };
	//m_imageLoginBackGround.RenderSprite(&PosScale,0,0,&v2Pos,0xffffffff);


	VECTOR2 v2TopPos = { 0, 0 };
	VECTOR2 v2BottomPos = { 0, 640 	};
	
	//SW061111 저해상도옵션
	/*m_imageLoginBarTop.RenderSprite(0,0,0,&v2TopPos,0xffffffff);
	m_imageLoginBarBottom.RenderSprite(0,0,0,&v2BottomPos,0xffffffff);
	*/

	if( GAMERESRCMNGR->IsLowResolution() )
	{
		float scaleRate = (float)LOWRSLTN_W / MIDRSLTN_W;

		v2BottomPos.x *= scaleRate;
		v2BottomPos.y *= scaleRate;
		PosScale.x = PosScale.y = scaleRate;
	}
	
	m_imageLoginBarTop.RenderSprite(&PosScale,0,0,&v2TopPos,0xffffffff);
	m_imageLoginBarBottom.RenderSprite(&PosScale,0,0,&v2BottomPos,0xffffffff);

	WINDOWMGR->Render();

	/*
	// magi82 - ServerListBackImage(070802)
	if(m_pServerListDlg->IsActive() == FALSE)
		return;

	static int nAniPosition = 0;
	static int nAniTime = 0;

	POINT pxy;
	pxy.x = g_UserInput.GetMouse()->GetMouseX();
	pxy.y = g_UserInput.GetMouse()->GetMouseY();


	VECTOR2 v2DestPos = { GET_MAINWIN_W - 100, GET_MAINWIN_H - 100 };
	cImageRect rcSrc = {129+nAniPosition,772,129+41+nAniPosition,772+41};
	cImageRect rcDest = {v2DestPos.x, v2DestPos.y, v2DestPos.x+42, v2DestPos.y+41};

	if(PtInRect(&rcDest,pxy))
	{
		rcSrc.left = 160;
		rcSrc.right = rcSrc.left + 42;
		rcSrc.top = 729;
		rcSrc.bottom = rcSrc.top + 41;

		nAniPosition = 0;
	}
	else
	{
		nAniTime += 10;
		if(nAniTime >= 500)
		{
			nAniPosition += 42;
			if( nAniPosition >= 205 )
			{
				nAniPosition = 0;
			}
			nAniTime = 0;
		}
	}

	static BOOL bClickFlag = FALSE;
	if(PtInRect(&rcDest,pxy) && g_UserInput.GetMouse()->LButtonDown())
	{
        bClickFlag = TRUE;
	}

	if(bClickFlag == TRUE && g_UserInput.GetMouse()->LButtonUp())
	{
		if(PtInRect(&rcDest,pxy))
		{
			printf("bClickFlag : FLASE");
			MAINGAME->SetGameState(eGAMESTATE_INTRO);
			INTRO->SetReplayIntroFlag();

		}
		bClickFlag = FALSE;
	}

	if(bClickFlag)
	{
		rcSrc.left = 624;
		rcSrc.right = rcSrc.left + 42;
		rcSrc.top = 426;
		rcSrc.bottom = rcSrc.top + 41;
	}

	cImage Image;
	SCRIPTMGR->GetImage( 0, &Image, &rcSrc);
	Image.RenderSprite(NULL,NULL,0,&v2DestPos,0xffffffff );

	*/
}
void CMainTitle::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg)
{
	switch(Category)
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
#ifdef _GMTOOL_
			case MP_CHEAT_CHECKIP_ACK:
				{
					m_bInit = TRUE;
				}
				break;
			case MP_CHEAT_CHECKIP_NACK:
				{
					if( NETWORK->IsConnected() )
						NETWORK->Disconnect();
					WINDOWMGR->MsgBox( MBI_NOT_GM, MBT_NOBTN, CHATMGR->GetChatMsg( 162 ) );
				}
				break;
#endif
			case MP_USERCONN_USE_DYNAMIC_ACK:
				{
					StartWaitConnectToAgent( FALSE );
					cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
					if( pDlg )
					{
						pDlg->SetDisable( FALSE );
						pDlg->SetActive( FALSE );
					}
					cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
					if( pMsgBox )
					{
						pMsgBox->SetDisable( FALSE );
						pMsgBox->SetActive( FALSE );
					}

					MSG_USE_DYNAMIC_ACK* pmsg = (MSG_USE_DYNAMIC_ACK*)pMsg;
					m_bDynamicDlg = TRUE;
					pDlg = WINDOWMGR->GetWindowForID(MT_DYNAMICDLG);
					if( pDlg )
					{
						pDlg->SetDisable( FALSE );
						pDlg->SetActive( TRUE );

						cStatic* pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_DYNAMICID );
						pStc->SetStaticText( g_szHeroIDName );
						pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_COORD );
//						pStc->SetStaticText( "Dynamic Password" );
						pStc = (cStatic*)pDlg->GetWindowForID( MT_STC_COORD_MAT );
						pStc->SetStaticText( pmsg->mat );						
						((cEditBox*)pDlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetFocusEdit( TRUE );
						((cEditBox*)pDlg->GetWindowForID(MT_DYNAMICPWDEDITBOX))->SetEditText( "" );
					}					
				}
				break;
			case MP_USERCONN_USE_DYNAMIC_NACK:
				{
//					WINDOWMGR->MsgBox( MBI_CANTUSEDYNAMIC, MBT_OK, CHATMGR->GetChatMsg( 702 ) );
//					//msgbox
				}
				break;

			case MP_USERCONN_DIST_CONNECTSUCCESS:
				{
#ifndef _HK_LOCAL_
					MSGBASE* pmsg = (MSGBASE*)pMsg;
   					m_DistAuthKey = pmsg->dwObjectID;
   					SetBlock(FALSE);
   					m_bInit = TRUE;
#else
					MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
//					m_DistAuthKey = pmsg->dwObjectID;
					m_DistAuthKey = pmsg->dwData2;
					SetBlock(FALSE);
					m_bInit = TRUE;
					m_dwEnc = pmsg->dwData1;
					m_dwOld = m_dwEnc;
#endif
				}
				return;
			case MP_USERCONN_LOGIN_ACK:
				{
					MSG_LOGIN_ACK* pmsg = (MSG_LOGIN_ACK*)pMsg;

					strcpy(g_AgentAddr,pmsg->agentip);
					g_AgentPort = pmsg->agentport;
					m_UserIdx	= pmsg->userIdx;
					MAINGAME->SetUserLevel( (int)pmsg->cbUserLevel );
#ifdef _GMTOOL_
					if( pmsg->cbUserLevel <= eUSERLEVEL_GM )
						GMTOOLMGR->CanUse( TRUE );
#endif
					gUserID = m_UserIdx;
					USERINFOMGR->SetSaveFolderName( gUserID );

					m_bDisconntinToDist = TRUE;
					m_dwDiconWaitTime	= gCurTime;

					SendOutMsg();
				}
				break;
			case MP_USERCONN_SERVER_NOTREADY:
				{
					NoDisconMsg();
					OnLoginError( LOGIN_ERROR_NODISTRIBUTESERVER, 0 );
				}
				break;

			case MP_USERCONN_LOGIN_NACK:
				{
//					g_szHeroIDName[0] = 0;
					MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
					OnLoginError(pmsg->dwData1,pmsg->dwData2);
				}
				break;
			case MP_USERCONN_CHARACTERLIST_ACK:
				{
					SEND_CHARSELECT_INFO* pmsg = (SEND_CHARSELECT_INFO*)pMsg;
#ifdef _CRYPTCHECK_ 
#ifdef _HK_LOCAL_
					int nMixKey[4] = { 0, };
					for( int i = 0 ; i < pmsg->CharNum ; ++i )
					{
						nMixKey[1] += pmsg->BaseObjectInfo[i].dwObjectID % 98;
						nMixKey[0] += pmsg->ChrTotalInfo[i].Level;
						nMixKey[2] += pmsg->ChrTotalInfo[i].LoginMapNum>>1;
						nMixKey[3] += pmsg->ChrTotalInfo[i].Gender;
					}

//한번 꼬아 볼까나..
					pmsg->eninit.Keys.iRightMultiGab	-= nMixKey[0];
					pmsg->eninit.Keys.iLeftKey			-= nMixKey[1];
					pmsg->eninit.Keys.iMiddlePlusGab	+= m_UserIdx % 99;
					pmsg->deinit.Keys.iRightKey			-= nMixKey[2];
					pmsg->deinit.Keys.iMiddlePlusGab	-= nMixKey[3];
					pmsg->deinit.Keys.iMiddleKey		+= m_DistAuthKey % 100;
#endif
					NETWORK->SetKey( pmsg->eninit, pmsg->deinit );
#endif
					//
					if( NETWORK->IsConnected() ) //그사이 취소를 눌렀을 수도 있다.
						MAINGAME->SetGameState(eGAMESTATE_CHARSELECT, (void *)pmsg, sizeof(SEND_CHARSELECT_INFO));
				}
				return;
			case MP_USERCONN_CHARACTERLIST_NACK:
				{
					// 케릭정보를 받는데 실패했습니다.
					cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(5));
					cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
					if( pIDDlg && pMsgBox )
					{
						pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
					}
				}
				return;
				
#ifdef TAIWAN_LOCAL				
			case MP_USERCONN_REMAINTIME_NOTIFY:
				{
					MSG_INT* pmsg = (MSG_INT*)pMsg;
					BILLINGMGR->SetUseRemainTime(TRUE);
					BILLINGMGR->SetRemainTime( pmsg->nData );	//
				}
				break;
#endif
				
			case MP_USERCONN_AGENT_CONNECTSUCCESS:
				{
					MSGBASE* pmsg = (MSGBASE*)pMsg;
					
					MSG_DWORD2 msg;
					msg.Category = MP_USERCONN;
					msg.Protocol = MP_USERCONN_CHARACTERLIST_SYN;
					msg.dwData1 = m_UserIdx;
					msg.dwData2 = m_DistAuthKey;
					NETWORK->Send(&msg,sizeof(msg));
				}
				break;
			}
		}
		break;
	case MP_CHEAT:
		{
			switch(Protocol) 
			{
				case MP_CHEAT_EVENTNOTIFY_ON:
				{
					MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

					NOTIFYMGR->SetEventNotifyStr( pmsg->strTitle, pmsg->strContext );
					NOTIFYMGR->SetEventNotify( TRUE );

//					if( GAMEIN->GetEventNotifyDialog() )
//					{
//						GAMEIN->GetEventNotifyDialog()->SetTitle( pmsg->strTitle );
//						GAMEIN->GetEventNotifyDialog()->SetContext( pmsg->strContext );
//						GAMEIN->GetEventNotifyDialog()->SetActive( TRUE );
//						NOTIFYMGR->SetEventNotifyChanged( FALSE );
//						//사운드?
//					}
//					else
//					{
						NOTIFYMGR->SetEventNotifyChanged( TRUE );
//					}

					NOTIFYMGR->ResetEventApply();
					for(int i=0; i<eEvent_Max; ++i)
					{
						if( pmsg->EventList[i] )
							NOTIFYMGR->SetEventApply( i );
					}
				}
				break;
		
				case MP_CHEAT_EVENTNOTIFY_OFF:
				{
					MSGBASE* pmsg = (MSGBASE*)pMsg;
					NOTIFYMGR->SetEventNotify( FALSE );
					NOTIFYMGR->SetEventNotifyChanged( FALSE );
//					if( GAMEIN->GetEventNotifyDialog() )
//						GAMEIN->GetEventNotifyDialog()->SetActive( FALSE );
				}	
				break;
			}
		}
		break;		
	}
//	LOG(EC_UNKNOWN_PROTOCOL);
}



void CMainTitle::OnLoginError(DWORD errorcode,DWORD dwParam)
{
	//접속중입니다 메세지박스 지우기
	cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID( MBI_WAIT_LOGINCHECK );
	if( pMsgBox )
		WINDOWMGR->AddListDestroyWindow( pMsgBox );

	m_bWaitConnectToAgent = FALSE;
	
	ySWITCH(errorcode)
		yCASE(LOGIN_ERROR_INVALID_VERSION)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(9) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_OVERLAPPEDLOGIN)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_OVERLAPPEDLOGIN, MBT_YESNO, CHATMGR->GetChatMsg(8) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
		yCASE(LOGIN_ERROR_OVERLAPPEDLOGININOTHERSERVER)
			char serverName[128] = {0,};
			for(int n=0;n<30;++n)
			{
				if(GAMERESRCMNGR->m_ServerList[n].ServerNo == dwParam)
				{
#ifdef _TW_LOCAL_
					strncpy(serverName,GAMERESRCMNGR->m_ServerList[n].ServerName,4);
#else
					strcpy(serverName,GAMERESRCMNGR->m_ServerList[n].ServerName);
#endif

					break;
				}
			}
			//SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_OVERLAPPEDLOGININOTHERSERVER, MBT_OK, CHATMGR->GetChatMsg(1),
												serverName, serverName);
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			NoDisconMsg();

		yCASE(LOGIN_ERROR_NOAGENTSERVER)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(7) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
			SendOutMsg();
			NoDisconMsg();
			
		yCASE(LOGIN_ERROR_NODISTRIBUTESERVER)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(7) );
//			if( pMsgBox )
//				pMsgBox->SetAbsXY( 414, 398 );
		yCASE(LOGIN_ERROR_INVALIDUSERLEVEL)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(6));
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_DISTSERVERISBUSY)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(674));
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_WRONGIDPW)

			// 버디버디의 경우 아이디가 없다면 서비스사용정보 동의를 안한것이다.
			if( dwParam == 2 && OPTIONMGR->GetGameOption()->nLoginCombo == 3 )
			{
				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_BUDDY_INFOAGREE, MBT_OK, CHATMGR->GetChatMsg(1647) );
				cEditBox* pEditId = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_IDEDITBOX );
				cEditBox* pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_PWDEDITBOX );
				if( pEditId )
					pEditId->SetFocusEdit( FALSE );
				if( pEditPwd )
				{
					pEditPwd->SetEditText( "" );
					pEditPwd->SetFocusEdit( TRUE );
				}
				cDialog* pDlg1 = (cDialog*)WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				pDlg1->SetDisable( FALSE );

				cDialog* pDlg2 = (cDialog*)WINDOWMGR->GetWindowForID( MT_DYNAMICDLG );

				if( pDlg2 )
				if( pDlg2->IsActive() )				
				{
					pDlg1->SetActive( TRUE );
					pDlg2->SetDisable( FALSE );
					pDlg2->SetActive( FALSE	);
					pEditPwd = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_DYNAMICPWDEDITBOX );
					pEditPwd->SetEditText( "" );
					//				pEditPwd->SetFocusEdit( TRUE );
				}
				
				return;
			}

			cEditBox* pEdit = (cEditBox*)WINDOWMGR->GetWindowForIDEx( MT_PWDEDITBOX );
			pEdit->SetFocusEdit( FALSE );
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_PASSERROR, MBT_OK, CHATMGR->GetChatMsg(281) );
			//id/pw창 리드온리 혹은 디스에이블로
			//메시지 박스 클릭시에 id/pw비워주고 포커스 id에 주기
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

		yCASE(LOGIN_ERROR_BLOCKUSERLEVEL)
/*			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 447 ) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();
*/
			char buf[256] = {0,};
			if( dwParam > 0 )
			{
				int hour = dwParam / 60;
				int min = dwParam - hour * 60;

				wsprintf( buf, CHATMGR->GetChatMsg( 1703 ), hour, min ) ;
//				wsprintf( buf, "입장할 수 없는 계정입니다. %d시간 %d분 이후에 입장할 수 있습니다.", hour, min ) ;
			}
			else
			{
				wsprintf( buf, CHATMGR->GetChatMsg( 305 ) );
			}

			// cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 305 ) );
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, buf );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();

			
		yCASE(LOGIN_ERROR_INVALID_IP)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_PASSERROR, MBT_OK, CHATMGR->GetChatMsg( 446 ) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		yCASE(LOGIN_ERROR_DISTCONNET_ERROR)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_DISTCONNECT_ERROR, MBT_OK, CHATMGR->GetChatMsg(453) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

		yCASE(LOGIN_ERROR_MAXUSER)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(454) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
		yCASE(LOGIN_ERROR_MINOR_INADULTSERVER)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(443) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		yCASE(LOGIN_ERROR_SECEDEDACCOUNT)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(672) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
			
			SendOutMsg();
			NoDisconMsg();
			
		yCASE(LOGIN_ERROR_NOT_CLOSEBETAWINNER)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(438) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		
			SendOutMsg();
			NoDisconMsg();
		yCASE(LOGIN_ERROR_NOREMAINTIME)
//			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, "You have not remain time, please recharge ASAP" );
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, "퀭돨념令엥쟁綠쒔청唐假岱珂쇌，헝쐴우돕《카窮》바렘貢籃www.moxiang.com.cn념令。" );
		
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();
		yCASE(LOGIN_ERROR_NOIPREGEN)
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg(1237) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_BUDDY_WEBFAILED)

			char msg[256] = { 0, };
			sprintf( msg, "%s (001)", CHATMGR->GetChatMsg(1645) );
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, msg );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_BUDDY_IDAUTHFAILED)

			char msg[256] = { 0, };
			sprintf( msg, "%s (002)", CHATMGR->GetChatMsg(1645) );
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, msg );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

			SendOutMsg();
			NoDisconMsg();

		yCASE(LOGIN_ERROR_BUDDY_AUTHCODEFAILED)

			// 실명인증 안됨
			if( dwParam == 99 )
			{
				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_BUDDY_NAMECER, MBT_OK, CHATMGR->GetChatMsg(1646) );
			}
			else
			{
				char msg[256] = { 0, };
				sprintf( msg, "%s (003)", CHATMGR->GetChatMsg(1645) );
				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, msg );
				cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
				if( pIDDlg && pMsgBox )
				{
					pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
				}
			}

			cDialog* dlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( dlg )
			{
				dlg->SetDisable( FALSE );
				cEditBox* editboxId = (cEditBox *)dlg->GetWindowForID(MT_IDEDITBOX);
				if( editboxId )
				{
					editboxId->SetEditText("");
					editboxId->SetFocusEdit(TRUE);
				}
				cEditBox* editboxPwd = (cEditBox *)dlg->GetWindowForID(MT_PWDEDITBOX);
				if( editboxPwd )
				{
					editboxPwd->SetEditText("");
					editboxPwd->SetFocusEdit(FALSE);
				}
			}

			TITLE->NoDisconMsg();		//
			TITLE->ShowServerList();

		yCASE(LOGIN_ERROR_PROTECTIONNUMBER)	//보안번호 틀릴때 오류

			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 1690 ) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		
		yCASE(LOGIN_ERROR_PROTECTIONNUMBER_REGIST)	//보안번호 미등록 오류

				cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 1692 ) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}

		yCASE(LOGIN_ERROR_PROTECTIONNUMBER_INSERT)	//보안번호 입력이 안됐을때 오류

			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_LOGINERRORCLOSE, MBT_OK, CHATMGR->GetChatMsg( 1693 ) );
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		
	yENDSWITCH
}

void CMainTitle::OnDisconnect()
{
	if(m_bDisconntinToDist)		//에이젼트로 접속을 위한 끊김
	{
		if(NETWORK->ConnectToServer(g_AgentAddr, g_AgentPort) == FALSE)
		{
			cMsgBox* pMsgBox = WINDOWMGR->MsgBox( MBI_VERCHKERROR, MBT_OK, CHATMGR->GetChatMsg(7));
			cDialog* pIDDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
			if( pIDDlg && pMsgBox )
			{
				pMsgBox->SetAbsXY( pIDDlg->GetAbsX(), pIDDlg->GetAbsY() + pIDDlg->GetHeight() );
			}
		}
		
		m_bDisconntinToDist = FALSE;
	}
	else if( !m_bNoDiconMsg )	//유저의 강제 접속 끊음이 아니면
	{
		OnLoginError(LOGIN_ERROR_DISTCONNET_ERROR, 0);

		//로그인창 디스에이블 추가.
		cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
		if( pDlg )
		{
			pDlg->SetDisable( TRUE );
		}

		NETWORK->ReleaseKey();
	}

	m_bNoDiconMsg = FALSE;
}

void CMainTitle::ConnectToServer( int index )
{
	if( index > -1 )
	{
		SEVERLIST* pServerList = GAMERESRCMNGR->m_ServerList;
		if( !pServerList[index].bEnter )	return;

		m_pServerListDlg->SetDisable( TRUE );
		m_pIntroReplayDlg->SetDisable( TRUE );	// magi82 - Intro(070802)
			
		if( NETWORK->ConnectToServer( pServerList[index].DistributeIP, pServerList[index].DistributePort ) == FALSE )
		{
			OnLoginError( LOGIN_ERROR_NODISTRIBUTESERVER, 0 );
		}

		m_pServerListDlg->SetDisable( FALSE );
		m_pServerListDlg->SetActive( FALSE );

		// magi82 - Intro(070802)
		m_pIntroReplayDlg->SetDisable( FALSE );
		m_pIntroReplayDlg->SetActive( FALSE );
		
		g_nServerSetNum = pServerList[index].ServerNo;
	}
	m_ConnectionServerNo = index;



	//MSG_LOGIN_SYN msg;
	//msg.Category = MP_USERCONN;
	//msg.Protocol = MP_USERCONN_LOGIN_SYN;

	//SafeStrCpy(msg.id, loginid, MAX_NAME_LENGTH + 1 );
	//SafeStrCpy(msg.pw, loginpass, MAX_NAME_LENGTH + 1 );
	//SafeStrCpy(msg.Version,g_CLIENTVERSION, 16 );
	//msg.gate = 0;

	//msg.AuthKey = TITLE->GetDistAuthKey();
	//NETWORK->Send(&msg,sizeof(msg));

	//SafeStrCpy(g_szHeroIDName, loginid, MAX_NAME_LENGTH + 1 );

	//TITLE->StartWaitConnectToAgent( TRUE );

	//USERINFOMGR->SetUserID( loginid );
}

void CMainTitle::ShowServerList()
{
	m_bServerList = TRUE;

	cDialog* pDlg = WINDOWMGR->GetWindowForID( MT_LOGINDLG );
	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( FALSE );
	}
	pDlg = WINDOWMGR->GetWindowForID(MT_DYNAMICDLG);
	if( pDlg )
	{
		pDlg->SetDisable( FALSE );
		pDlg->SetActive( FALSE );
	}
	m_bDynamicDlg = FALSE;

	pDlg = WINDOWMGR->GetWindowForID(NUMBERPAD_DLG);
	if(pDlg)
	{		
		pDlg->SetActive( FALSE );
	}

	if( NETWORK->IsConnected() )
		NETWORK->Disconnect();
	m_bInit = FALSE;

}

void CMainTitle::StartWaitConnectToAgent( BOOL bStart )
{
	m_dwWaitTime			= gCurTime;
	m_bWaitConnectToAgent	= bStart;
}


//trustpak 2005/04/01

#ifndef TAIWAN_LOCAL

void CMainTitle::InitMainTitleMonsters(void)
{	
	srand( (unsigned)time( NULL ) );

	//SetRect(&m_rcMainTitleMonsterArea, 17800, 3000, 20300, 3500);

	SetRect(&m_rcMainTitleMonsterArea, 18000, 2400, 20000, 3600);

	memset(m_apMonster, 0, sizeof(m_apMonster));
	memset(m_av3Target, 0, sizeof(m_av3Target));	
	memset(m_adwLastActionTime, 0, sizeof(m_adwLastActionTime));
	memset(m_aiLastAction, 0, sizeof(m_aiLastAction));


	SEND_MONSTER_TOTALINFO		info;
	VECTOR3						pos;
	BASEMOVE_INFO				bminfo;
	CMonster*					pMonster = NULL;


	DWORD dwID = 10;
	DWORD dwIndex = 0;	

	
	memset(&info,0,sizeof(info));

	info.BaseObjectInfo.dwObjectID = dwID++;
	info.BaseObjectInfo.BattleID = gChannelNum+1;	

	sprintf(info.BaseObjectInfo.ObjectName,"MainTitleMonster01");

	pos.x = 18447.0f;
	pos.y = 0.0f;
	pos.z = 3200.0f;

	m_av3LastPosition[dwIndex] = pos;

	info.MoveInfo.CurPos.Compress(&pos);
	
	bminfo.SetFrom(&info.MoveInfo);	
	bminfo.SetCurTargetPosIdx(0);
	bminfo.SetMaxTargetPosIdx(0);
	bminfo.SetTargetPosition(0, pos);
	info.TotalInfo.MonsterKind = 143;										// 꼬꼬
	
	m_apMonster[dwIndex] = OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);	
	m_adwLastActionTime[dwIndex] = gCurTime;

	//trustpak 2005/04/19
	MOVEMGR->InitMove(m_apMonster[dwIndex], &bminfo);
	///

	++dwIndex;

	
	pos.x += 250;	
	pos.z -= 100;

	m_av3LastPosition[dwIndex] = pos;

	memset(&info,0,sizeof(info));

	info.BaseObjectInfo.dwObjectID = dwID++;
	info.BaseObjectInfo.BattleID = gChannelNum+1;

	sprintf(info.BaseObjectInfo.ObjectName,"MainTitleMonster02");	

	info.MoveInfo.CurPos.Compress(&pos);

	bminfo.SetFrom(&info.MoveInfo);
	bminfo.SetCurTargetPosIdx(0);
	bminfo.SetMaxTargetPosIdx(0);
	bminfo.SetTargetPosition(0, pos);
	info.TotalInfo.MonsterKind = 143;

	m_apMonster[dwIndex] = OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
	m_adwLastActionTime[dwIndex] = gCurTime;

	//trustpak 2005/04/19
	MOVEMGR->InitMove(m_apMonster[dwIndex], &bminfo);
	///

	++dwIndex;

	


	pos.x += 350;	

	m_av3LastPosition[dwIndex] = pos;

	memset(&info,0,sizeof(info));

	info.BaseObjectInfo.dwObjectID = dwID++;
	info.BaseObjectInfo.BattleID = gChannelNum+1;

	sprintf(info.BaseObjectInfo.ObjectName,"MainTitleMonster03");	

	info.MoveInfo.CurPos.Compress(&pos);

	bminfo.SetFrom(&info.MoveInfo);
	bminfo.SetCurTargetPosIdx(0);
	bminfo.SetMaxTargetPosIdx(0);
	bminfo.SetTargetPosition(0, pos);
	info.TotalInfo.MonsterKind = 143;											

	m_apMonster[dwIndex] = OBJECTMGR->AddMonster(&info.BaseObjectInfo,&bminfo,&info.TotalInfo);
	m_adwLastActionTime[dwIndex] = gCurTime;

	//trustpak 2005/04/19
	MOVEMGR->InitMove(m_apMonster[dwIndex], &bminfo);
	///
	++dwIndex;	

#ifdef _HK_LOCAL_
//=======여기에다가도..암호화를...=_=;
	BYTE enkey[8] = { 0xfe, 0xef, 0xcd, 0xf1, 0x0f, 0xa2, 0xda, 0x1f };
	memcpy( m_enkey, enkey, sizeof(enkey) );
#endif
}


void CMainTitle::RemoveMainTitleMonsters(void)
{
	int i = 0;
	for (i = 0; i < _MAX_MAINTITLE_MONSTER_NUM; i++)
	{
		OBJECTMGR->AddGarbageObject(m_apMonster[i]);
		m_apMonster[i] = NULL;
	}
#ifdef _HK_LOCAL_
//=======암호화 해제
	ZeroMemory( m_enkey, sizeof(m_enkey) );
#endif
}

BOOL IsNear(VECTOR3* pv3Pos1, VECTOR3* pv3Pos2, int iMoveMode)
{
	float fCheckRange = 0.0f;

	if (iMoveMode == eMoveMode_Walk)
	{
		fCheckRange = 5.0f;
	}
	else
	{
		fCheckRange = 15.0f;
	}

	if (fCheckRange > fabs(pv3Pos1->x - pv3Pos2->x) &&
		fCheckRange > fabs(pv3Pos1->z - pv3Pos2->z))
	{
		return TRUE;
	}

	return FALSE;
}

//trustpak 2005/04/19
void CMainTitle::ProcessMainTitleMonsters(void)
{	
	const int	c_aiReactionTime[_MAX_MAINTITLE_MONSTER_NUM] = { 900, 1300, 1500 };
	

	VECTOR3		pos = { 0.0f, 0.0f, 0.0f };	
	CMonster*	pMonster = NULL;
	BYTE		byState = 0;

	int i = 0;
	for (i = 0; i < _MAX_MAINTITLE_MONSTER_NUM; i++)	
	{
		pMonster = m_apMonster[i];	
		byState = pMonster->GetState();
		
		if (eObjectState_None == byState)
		{
			// 아직 선택을 다시 할 시간이 안됐으면 보류.
			
			if (m_adwLastActionTime[i] + c_aiReactionTime[i] >= gCurTime)
			{
				continue;
			}			

			// Action choice. 
			// 0 : 뛰기
			// 1, 2 : 걷기			
			// 나머지 : 멈춰있기

			const int c_iMoveModeRun = 0;			

			int iRand = rand();
			int iAction = iRand / ( RAND_MAX / 5 );	

			if (2 < iAction)
			{
				m_adwLastActionTime[i] = gCurTime;
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);//eMA_RUNAWAY//eMA_STAND
				continue;
			}

			// 전에 이동을 했으면 한번 멈췄다가 다시 이동하게 한다.

			if (0 != m_aiLastAction[i])
			{
				m_adwLastActionTime[i] = gCurTime;
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);
				m_aiLastAction[i] = !m_aiLastAction[i];
				continue;				
			}

			// 타겟을 정한다. 이동 범위는 350 , 350 이내로.

			int iMoveX = rand() / (RAND_MAX / 350);
			int iMoveZ = rand() / (RAND_MAX / 350);

			if (60 > iMoveX)
			{
				iMoveX = 60;
			}

			if (40 > iMoveZ)
			{
				iMoveZ = 40;
			}
			
			if (0== rand() / (RAND_MAX / 2) )
			{
				iMoveX = -iMoveX;
			}

			if (0 == rand() / (RAND_MAX /2) )
			{
				iMoveZ = -iMoveZ;
			}									
			
			pMonster->GetPosition(&pos);			


			// 만일 위치가 0, 0, 0 으로 튀어버렸으면 마지막으로 멈추었던 위치로 값을 조정해준다. 

			if (0.0f == pos.x && 0.0f == pos.z)
			{
				pMonster->SetPosition(&m_av3LastPosition[i]);
			}


			// 타겟이 될 곳이 움직일 수 있는 범위 바깥인지 검사.

			if ( (m_rcMainTitleMonsterArea.left	>= (int)pos.x + iMoveX)  )
			{
				iMoveX = 0;
			}

			if ( (m_rcMainTitleMonsterArea.right <= (int)pos.x + iMoveX) )
			{
				iMoveX = 0;
			}

			if ( (m_rcMainTitleMonsterArea.top >= (int)pos.z + iMoveZ) )
			{			

				iMoveZ = 0;
			}

			if ( (m_rcMainTitleMonsterArea.bottom	<= (int)pos.z + iMoveZ) )
			{
				iMoveZ = 0;
			}			


			if (0 == iMoveX && 0 == iMoveZ)
			{
				continue;
			}

			pos.x += (float)iMoveX;		
			pos.z += (float)iMoveZ;
			
			// 타겟 지점을 저장해 둠.
			m_av3Target[i] = pos;

			
			// 행동을 지정.

			//if (c_iMoveModeRun == iAction)
			//{
			//	MOVEMGR->SetMonsterMoveType(pMonster, eMA_RUNAWAY);
			//}
			//else // c_iMoveModeRun
			//{			
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_WALKAROUND);
			//}
						
			MOVEMGR->StartMoveEx(pMonster,gCurTime, &pos);
			
			m_adwLastActionTime[i] = gCurTime;
			m_aiLastAction[i] = !m_aiLastAction[i];
		}
		else		
		{
			VECTOR3 v3Pos;						
			pMonster->GetPosition(&v3Pos);

			if (TRUE == IsNear(&v3Pos, &m_av3Target[i], MOVEMGR->GetMoveMode(pMonster)))
			{
				MOVEMGR->MoveStop(pMonster, &v3Pos);
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);
				

				// 현재 위치를 타겟으로 갱신해줌. 그렇지 않으면 위치 0, 0, 0 으로 튀어버리는 경우가 있음. 
	
				BASEMOVE_INFO bmif;
				
				pMonster->GetBaseMoveInfo(&bmif);
				bmif.SetTargetPosition(&m_av3Target[i]);
				MOVEMGR->InitMove(pMonster, &bmif);				
				
				
				// 위치가 0, 0, 0 으로 튀어버릴 때 위치를 되돌려주기 위해 현재 위치를 저장해 놓는다. 
				m_av3LastPosition[i] = v3Pos;

					
			}
		}	
	}
	
}



//trustpak 2005/04/19
//original code

/*
void CMainTitle::ProcessMainTitleMonsters(void)
{	
	const int	c_aiReactionTime[_MAX_MAINTITLE_MONSTER_NUM] = { 900, 1300, 1500 };
	

	VECTOR3		pos = { 0.0f, 0.0f, 0.0f };	
	CMonster*	pMonster = NULL;
	BYTE		byState = 0;

	int i = 0;
	for (i = 0; i < _MAX_MAINTITLE_MONSTER_NUM; i++)	
	{
		pMonster = m_apMonster[i];	
		byState = pMonster->GetState();
		
		if (eObjectState_None == byState)
		{
			// 아직 선택을 다시 할 시간이 안됐으면 보류.

			if (m_adwLastActionTime[i] + c_aiReactionTime[i] >= gCurTime)
			{
				continue;
			}

			// Action choice. 
			// 0 : 뛰기
			// 1, 2 : 걷기			
			// 나머지 : 멈춰있기

			const int c_iMoveModeRun = 0;			

			int iRand = rand();
			int iAction = iRand / ( RAND_MAX / 5 );	

			if (2 < iAction)
			{
				m_adwLastActionTime[i] = gCurTime;
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);
				continue;
			}

			// 전에 이동을 했으면 한번 멈췄다가 다시 이동하게 한다.

			if (0 != m_aiLastAction[i])
			{
				m_adwLastActionTime[i] = gCurTime;
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);
				m_aiLastAction[i] = !m_aiLastAction[i];
				continue;				
			}

			// 타겟을 정한다. 이동 범위는 350 , 350 이내로.

			int iMoveX = rand() / (RAND_MAX / 350);
			int iMoveZ = rand() / (RAND_MAX / 350);

			if (60 > iMoveX)
			{
				iMoveX = 60;
			}

			if (40 > iMoveZ)
			{
				iMoveZ = 40;
			}
			
			if (0== rand() / (RAND_MAX / 2) )
			{
				iMoveX = -iMoveX;
			}

			if (0 == rand() / (RAND_MAX /2) )
			{
				iMoveZ = -iMoveZ;
			}			
						
			pMonster->GetPosition(&pos);

			// 타겟이 될 곳이 움직일 수 있는 범위 바깥인지 검사.

			if ( (m_rcMainTitleMonsterArea.left	>= (int)pos.x + iMoveX)  )
			{
				//iMoveX = m_rcMainTitleMonsterArea.left - (int)pos.x + iMoveX;

				iMoveX = 0;
			}

			if ( (m_rcMainTitleMonsterArea.right <= (int)pos.x + iMoveX) )
			{
				//iMoveX = (int)pos.x + iMoveX - m_rcMainTitleMonsterArea.right;

				iMoveX = 0;
			}

			if ( (m_rcMainTitleMonsterArea.top >= (int)pos.z + iMoveZ) )
			{
				//iMoveZ = m_rcMainTitleMonsterArea.top - (int)pos.z + iMoveZ;

				iMoveZ = 0;
			}

			if ( (m_rcMainTitleMonsterArea.bottom	<= (int)pos.z + iMoveZ) )
			{
				//iMoveZ = (int)pos.z + iMoveZ - m_rcMainTitleMonsterArea.bottom;

				iMoveZ = 0;
			}	

			// 너무 수치가 작으면 움직임이 어색함으로 보간.
			
			//if (30 > iMoveX)
			//{
			//	iMoveX = 0;
			//}

			//if (30 > iMoveZ)
			//{
			//	iMoveZ = 0;
			//}
			


			if (0 == iMoveX && 0 == iMoveZ)
			{
				continue;
			}

			pos.x += (float)iMoveX;		
			pos.z += (float)iMoveZ;
			
			// 타겟 지점을 저장해 둠.
			m_av3Target[i] = pos;

			
			// 행동을 지정.

			if (c_iMoveModeRun == iAction)
			{
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_RUNAWAY);
			}
			else // c_iMoveModeRun
			{			
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_WALKAROUND);
			}
						
			MOVEMGR->StartMoveEx(pMonster,gCurTime, &pos);		
			
			m_adwLastActionTime[i] = gCurTime;
			m_aiLastAction[i] = !m_aiLastAction[i];
		}
		else		
		{
			VECTOR3 v3Pos;
			pMonster->GetPosition(&v3Pos);

			if (TRUE == IsNear(&v3Pos, &m_av3Target[i], MOVEMGR->GetMoveMode(pMonster)))
			{	
				MOVEMGR->SetMonsterMoveType(pMonster, eMA_STAND);
				MOVEMGR->EndMove(pMonster);					
			}
		}	
	}
	
}
*/

#endif

///
