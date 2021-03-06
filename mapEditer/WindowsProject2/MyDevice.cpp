#include "stdafx.h"
#include "MyDevice.h"

namespace MyDevice
{
	//디바이스 생성함수
	HRESULT CreateDevice(int sizeX, int sizeY)
	{
		LPDIRECT3D9 d3d9;
		d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		//IDirect3D9 객체얻기가 실패하면 NULL 이 참조됨
		if (d3d9 == NULL)
		{
			MessageBox(hWnd, T("Direct3DCreate9() - FAILED"), T("FAILED"), MB_OK);
			return E_FAIL;
		}

		//2. 하드웨어 정보를 가지고 와서 자신의 정점 프로세스 타입을 정하자
		D3DCAPS9	caps;
		//Direct3D9 객체를 통해 비디오 카드의 하드웨어 정보를 가지고 온다.
		d3d9->GetDeviceCaps(
			D3DADAPTER_DEFAULT,		//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,			//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
			&caps					//디바이스 정보를 받아올 D3DCAPS9 포인터
		);

		//정점계산 처리방식을 지정할 플레그 값
		DWORD vertexProcessing = 0;

		//정점 처리와 정점 광원 처리를 하드웨어에서 지원하는가?
		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
			vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else
			vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		//3. D3DPRESENT_PARAMETERS 구조체 정보를 생성
		D3DPRESENT_PARAMETERS d3dpp = GetD3DPP(sizeX, sizeY);

		//4. Device 생성

		//Direct3D9 객체를 통해서 Device 를 생성한다.

		HRESULT result = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,			//주 디스플레이 그래픽 카드 그냥 D3DADAPTER_DEFAULT
			D3DDEVTYPE_HAL,				//디바이스타입 설정 그냥 D3DDEVTYPE_HAL
			hWnd,						//디바이스를 사용할 윈도우 핸들
			vertexProcessing | D3DCREATE_MULTITHREADED,			//정점 처리 방식에 대한 플레그 | 멀티스레딩 허용
			&d3dpp,						//앞에서 정의한 D3DPRESENT_PARAMETERS 구조체 포인터
			&Device);					//얻어올 디바이스 더블포인터

		if (FAILED(result))
		{
			//디바이스 생성실패
			//d3d9 객체 날려야한다
			d3d9->Release();
			d3d9 = NULL;


			MessageBox(hWnd, T("CreateDevice() - FAILED"), T("FAILED"), MB_OK);
			return E_FAIL;
		}


		//디바이스 생성성공
		d3d9->Release();
		d3d9 = NULL;

		return S_OK;
	}

	//디바이스 해제함수
	void ReleaseDevice()
	{
		if (Device)
			Device->Release();
		Device = nullptr;
	}

	D3DPRESENT_PARAMETERS GetD3DPP(int sizeX, int sizeY)
	{
		D3DPRESENT_PARAMETERS d3dpp;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;				//뎁스 버퍼와 스테실 버퍼 크기 뎁스 24bit 스텐실 버퍼 8 비트
		d3dpp.BackBufferCount = 1;						//백버퍼는 한개
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;			//백버퍼 색상 포맷 알파8비트,레드8비트,그린8비트,블루8비트
		d3dpp.BackBufferWidth = sizeX;
		d3dpp.BackBufferHeight = sizeY;
		d3dpp.EnableAutoDepthStencil = true;						//자동 깊이버퍼 사용 여부 ( 그냥 true )
		d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;											//일단 0
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	//화면 주사율 ( 그냥 D3DPRESENT_RATE_DEFAULT 모니터 주사율과 동일시 )
		d3dpp.hDeviceWindow = hWnd;								//Device 가 출력될 윈도우 핸들
		d3dpp.MultiSampleQuality = 0;						//멀티 샘플링 질 
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;		//멀티 샘플링 타입 ( D3DMULTISAMPLE_NONE 안티알리아싱 사용안함 ) 
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;	//화면 전송 간격 ( 그냥 D3DPRESENT_INTERVAL_ONE 모니터 주사율과 동일시
		//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	//화면 전송 간격 ( 그냥 D3DPRESENT_INTERVAL_ONE 모니터 주사율과 동일시 
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//화면 스왑 체인 방식 ( D3DSWAPEFFECT_DISCARD )
		d3dpp.Windowed = true;						//창모드 / 전체화면

		return d3dpp;
	}
	vector<POINT> GetDeviceSize()
	{
		vector<POINT> vecSize;

		LPDIRECT3D9 d3d9;
		d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

		if (d3d9 == NULL)
		{
			MessageBox(hWnd, T("Direct3DCreate9() - FAILED"), T("FAILED"), MB_OK);
			return vecSize;
		}

		int size = d3d9->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
		for (int i = 0; i < size; i++)
		{
			D3DDISPLAYMODE mode;
			POINT dissize;
			d3d9->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &mode);
			dissize.x = mode.Width;
			dissize.y = mode.Height;

			vecSize.push_back(dissize);
		}

		return vecSize;
	}
}