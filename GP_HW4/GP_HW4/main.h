#pragma once
#pragma comment(lib, "D2D1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "winmm.lib")

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <queue>

// DX Header Files
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <dwmapi.h>

#include "Animation.h"
#include "SoundManager.h"
#include "Character.h"

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_DELETE(p) { if(p) {delete (p); (p)=NULL; } }

// 현재 모듈의 시작주소 얻기.
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif
#endif
class Character;

class DemoApp
{
public:
	DemoApp();
	~DemoApp();
	HRESULT Initialize(HINSTANCE hInstance);
	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();
	HRESULT OnRender();
	void OnResize(UINT width, UINT height);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void DrawMenu();
	void DrawSeaweed(float translateX);
	void DrawBackground();
	void DrawCreateModeAction(int randomFish);
	void DrawFish(Character target, float opacity);
	void FishAction();
	void SharkAction();
	void DrawHealthBar(Character target);
	void DrawFeedingModeAction();
	void DrawHuntingModeAction();
	void DrawBubble(HRESULT hr, float length, D2D1_SIZE_F bubbleLocation);
	HRESULT LoadResourceBitmap(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap);
	void DrawFood();
	void ShotAction();

private:
	HWND m_hwnd;

	ID2D1Factory* m_pD2DFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pBaseBrush;
	ID2D1SolidColorBrush* m_pMenuBoxBrush;
	ID2D1SolidColorBrush* m_pTextBrush;

	ID2D1PathGeometry* m_pBubblePathGeometry;
	ID2D1PathGeometry* m_pSeaweedGeometry[3];
	ID2D1PathGeometry* m_pLeftSeaweedGeometry;
	ID2D1PathGeometry* m_pCenterSeaweedGeometry;
	ID2D1PathGeometry* m_pRightSeaweedGeometry;
	ID2D1PathGeometry* m_pHealthBarGeometry;

	//DWrite
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;

	// WIC and Bitmaps
	IWICImagingFactory* m_pWICFactory;
	ID2D1Bitmap* m_pBackgroundBitmap;
	ID2D1Bitmap* m_pFish1Bitmap;
	ID2D1Bitmap* m_pFish2Bitmap;
	ID2D1Bitmap* m_pFish3Bitmap;
	ID2D1Bitmap* m_pSharkBitmap;
	ID2D1Bitmap* m_pBubbleBitmap;
	ID2D1Bitmap* m_pFish1MaskBitmap;
	ID2D1Bitmap* m_pFish2MaskBitmap;
	ID2D1Bitmap* m_pFish3MaskBitmap;
	ID2D1Bitmap* m_pSharkMaskBitmap;
	ID2D1Bitmap* m_pBubbleMaskBitmap;
	
	// Bitmap Brushes
	ID2D1BitmapBrush* m_pFish1BitmapBrush;
	ID2D1BitmapBrush* m_pFish2BitmapBrush;
	ID2D1BitmapBrush* m_pFish3BitmapBrush;
	ID2D1BitmapBrush* m_pSharkBitmapBrush;
	ID2D1BitmapBrush* m_pBubbleBitmapBrush;

	LinearAnimation<float> m_BubbleAnimation;
	DWM_TIMING_INFO m_BubbleDwmTimingInfo;
};