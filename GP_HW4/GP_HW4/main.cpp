#include "main.h"

std::queue <FOOD*> foodQueue;
std::queue <Character*> fishQueue;

D2D1_SIZE_F rtSize;			// 화면의 전체 크기를 저장
D2D_POINT_2F currentPoint;	// 현재 마우스 포인터의 위치

// 모드 변수들
BOOL createMode  = false;	// 물고기 생성모드
BOOL feedingMode = false;	// 먹이주기 모드
BOOL huntingMode = false;	// 사냥 모드
BOOL newFish	 = false;	// 새로운 물고기 생성 가능한 상태
BOOL shot		 = false;	// 발사 중
BOOL playHuntSound = false;	// 사냥모드 진입 시 사운드
BOOL sharkOn	 = false;	// 상어가 나타날 시 on

int randomFish = -1;	// 0~2 사이의 무작위 물고기 생성을 위한 변수
int foodID = DEFAULT_ID_FOOD;
int fishID = DEFAULT_ID_FISH;
int seaweedNumber = 0;	// 수초는 0~2의 형태를 가짐
int UPorDOWN = 1;		// 수초의 변화 방향을 정하기 위해 사용하는 변수
clock_t seaweed1, seaweed2;	// 수초의 움직임 간격을 정의할 변수

D2D1_SIZE_F bubbleLocation = D2D1::SizeF(0.0f, 0.0f);	// 생성될 물방울의 위치
CSoundManager* soundManager = NULL;

Character shark = Character(Character::TYPE_SHARK, Character::STATE_STAND, false, 0, 0, true);
clock_t sharkTime1, sharkTime2;	// 상어 등장 간격을 정의할 변수

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize(hInstance)))
			{	
				app.RunMessageLoop();
			}

			else
			{
				SAFE_DELETE(soundManager);
			}
		}
		CoUninitialize();
	}


	return 0;
}

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRenderTarget(NULL),
	m_pBaseBrush(NULL),
	m_pMenuBoxBrush(NULL),
	m_pTextBrush(NULL),
	m_pBubblePathGeometry(NULL),
	m_pLeftSeaweedGeometry(NULL),
	m_pCenterSeaweedGeometry(NULL),
	m_pRightSeaweedGeometry(NULL),
	m_pHealthBarGeometry(NULL),
	m_pDWriteFactory(NULL),
	m_pTextFormat(NULL),
	m_pWICFactory(NULL),
	m_pBackgroundBitmap(NULL),
	m_pFish1Bitmap(NULL),
	m_pFish2Bitmap(NULL),
	m_pFish3Bitmap(NULL),
	m_pSharkBitmap(NULL),
	m_pBubbleBitmap(NULL),
	m_pFish1MaskBitmap(NULL),
	m_pFish2MaskBitmap(NULL),
	m_pFish3MaskBitmap(NULL),
	m_pSharkMaskBitmap(NULL),
	m_pBubbleMaskBitmap(NULL),
	m_pFish1BitmapBrush(NULL),
	m_pFish2BitmapBrush(NULL),
	m_pFish3BitmapBrush(NULL),
	m_pSharkBitmapBrush(NULL),
	m_pBubbleBitmapBrush(NULL),
	m_BubbleAnimation()
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBaseBrush);
	SAFE_RELEASE(m_pMenuBoxBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pBubblePathGeometry);
	SAFE_RELEASE(m_pLeftSeaweedGeometry);
	SAFE_RELEASE(m_pCenterSeaweedGeometry);
	SAFE_RELEASE(m_pRightSeaweedGeometry);
	SAFE_RELEASE(m_pHealthBarGeometry);
	SAFE_RELEASE(m_pDWriteFactory);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pBackgroundBitmap);
	SAFE_RELEASE(m_pFish1Bitmap);
	SAFE_RELEASE(m_pFish2Bitmap);
	SAFE_RELEASE(m_pFish3Bitmap);
	SAFE_RELEASE(m_pSharkBitmap);
	SAFE_RELEASE(m_pBubbleBitmap);
	SAFE_RELEASE(m_pFish1MaskBitmap);
	SAFE_RELEASE(m_pFish2MaskBitmap);
	SAFE_RELEASE(m_pFish3MaskBitmap);
	SAFE_RELEASE(m_pSharkMaskBitmap);
	SAFE_RELEASE(m_pBubbleMaskBitmap);
	SAFE_RELEASE(m_pFish1BitmapBrush);
	SAFE_RELEASE(m_pFish2BitmapBrush);
	SAFE_RELEASE(m_pFish3BitmapBrush);
	SAFE_RELEASE(m_pSharkBitmapBrush);
	SAFE_RELEASE(m_pBubbleBitmapBrush);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;
	seaweed1 = clock();
	sharkTime1 = clock();

	// Register the window class.
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DemoApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"HW4_201101584";
	RegisterClassEx(&wcex);

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		// Create the application window.
		m_hwnd = CreateWindow(
			L"HW4_201101584", L"GP_HW4, 201101584 민동휘",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			800, 600, NULL, NULL, hInstance, this);
		hr = m_hwnd ? S_OK : E_FAIL;

		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pBubblePathGeometry->ComputeLength(
				NULL, //no transform
				&length
				);
			
			if (SUCCEEDED(hr))
			{
				m_BubbleAnimation.SetStart(0);        //start at beginning of path
				m_BubbleAnimation.SetEnd(length);     //length at end of path
				m_BubbleAnimation.SetDuration(2.5f);  //seconds

				ZeroMemory(&m_BubbleDwmTimingInfo, sizeof(m_BubbleDwmTimingInfo));
				m_BubbleDwmTimingInfo.cbSize = sizeof(m_BubbleDwmTimingInfo);

				// Get the composition refresh rate. If the DWM isn't running, get the refresh rate from GDI -- probably going to be 60Hz
				if (FAILED(DwmGetCompositionTimingInfo(NULL, &m_BubbleDwmTimingInfo)))
				{
					HDC hdc = GetDC(m_hwnd);
					m_BubbleDwmTimingInfo.rateCompose.uiDenominator = 1;
					m_BubbleDwmTimingInfo.rateCompose.uiNumerator = GetDeviceCaps(hdc, VREFRESH);
					ReleaseDC(m_hwnd, hdc);
				}
				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}
	}

	HWND hWnd = m_hwnd;
	
	soundManager = new CSoundManager;
	if (!soundManager->init(hWnd))
		return FALSE;

	// 사운드 파일을 추가함.
	int id;
	if (!soundManager->add(L"gun.wav", &id)) //id=0부터 시작함.
		return FALSE;
	if (!soundManager->add(L"huntmode.wav", &id))
		return FALSE;
	if (!soundManager->add(L"BGM.wav", &id))
		return FALSE;

	soundManager->play(2, true);

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	// Create a Direct2D factory.
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	ID2D1GeometrySink *pSink = NULL;

	if (SUCCEEDED(hr)) // 물방울의 움직임 정의
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pBubblePathGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pBubblePathGeometry->Open(&pSink);

			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddLine(D2D1::Point2F(0, -100));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();
		}
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC factory.
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	if (SUCCEEDED(hr))
	{
		// Create a shared DirectWrite factory
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",     // The font family name.
			NULL,           // The font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			20.0f,
			L"en-us",
			&m_pTextFormat
			);
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); // 가운데정렬
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	}

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pLeftSeaweedGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pLeftSeaweedGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(-15, -10),
						D2D1::Point2F(-15, -20),
						D2D1::Point2F(0, -30)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(15, -40),
						D2D1::Point2F(15, -50),
						D2D1::Point2F(0, -60)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(-15, -70),
						D2D1::Point2F(-15, -80),
						D2D1::Point2F(0, -90)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(15, -100),
						D2D1::Point2F(33, -100),
						D2D1::Point2F(20, -90)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(5, -80),
						D2D1::Point2F(5, -70),
						D2D1::Point2F(20, -60)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(30, -50),
						D2D1::Point2F(30, -40),
						D2D1::Point2F(20, -30)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(5, -20),
						D2D1::Point2F(5, -10),
						D2D1::Point2F(20, 0)
						));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pCenterSeaweedGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pCenterSeaweedGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);

				pSink->AddLine(D2D1::Point2F(0, -100));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(20, -100), D2D1::SizeF(10, 7), 0,
					D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->AddLine(D2D1::Point2F(20, 0));				
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pRightSeaweedGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pRightSeaweedGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(15, -10),
						D2D1::Point2F(15, -20),
						D2D1::Point2F(0, -30)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(-15, -40),
						D2D1::Point2F(-15, -50),
						D2D1::Point2F(0, -60)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(15, -70),
						D2D1::Point2F(15, -80),
						D2D1::Point2F(0, -90)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(-15, -100),
						D2D1::Point2F(7, -100),
						D2D1::Point2F(20, -90)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(30, -80),
						D2D1::Point2F(30, -70),
						D2D1::Point2F(20, -60)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(5, -50),
						D2D1::Point2F(5, -40),
						D2D1::Point2F(20, -30)
						));
				pSink->AddBezier(
					D2D1::BezierSegment(
						D2D1::Point2F(30, -20),
						D2D1::Point2F(30, -10),
						D2D1::Point2F(20, 0)
						));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pHealthBarGeometry);

		if (SUCCEEDED(hr))
		{
			hr = m_pHealthBarGeometry->Open(&pSink);
			if (SUCCEEDED(hr))
			{
				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(
					D2D1::Point2F(0, 0),
					D2D1_FIGURE_BEGIN_FILLED
					);

				pSink->AddLine(D2D1::Point2F(150, 0));
				pSink->AddLine(D2D1::Point2F(150, 20));
				pSink->AddLine(D2D1::Point2F(0, 20));
				pSink->AddLine(D2D1::Point2F(0, 0));
				pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			}
			hr = pSink->Close();
			SAFE_RELEASE(pSink);
		}
	}

	m_pSeaweedGeometry[0] = m_pLeftSeaweedGeometry;
	m_pSeaweedGeometry[1] = m_pCenterSeaweedGeometry;
	m_pSeaweedGeometry[2] = m_pRightSeaweedGeometry;

	return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// Create a Direct2D render target.
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
			);


		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &m_pBaseBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &m_pMenuBoxBrush);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"BackgroundImage", L"Image", &m_pBackgroundBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish1", L"Image", &m_pFish1Bitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish2", L"Image", &m_pFish2Bitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish3", L"Image", &m_pFish3Bitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Shark", L"Image", &m_pSharkBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Bubble", L"Image", &m_pBubbleBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish1Mask", L"Image", &m_pFish1MaskBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish2Mask", L"Image", &m_pFish2MaskBitmap);
		}
		
		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"Fish3Mask", L"Image", &m_pFish3MaskBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"SharkMask", L"Image", &m_pSharkMaskBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadResourceBitmap(m_pRenderTarget, m_pWICFactory, L"BubbleMask", L"Image", &m_pBubbleMaskBitmap);
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(m_pFish1MaskBitmap, propertiesXClampYClamp, &m_pFish1BitmapBrush);
			}
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(m_pFish2MaskBitmap, propertiesXClampYClamp, &m_pFish2BitmapBrush);
			}
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(m_pFish3MaskBitmap, propertiesXClampYClamp, &m_pFish3BitmapBrush);
			}
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(m_pSharkMaskBitmap, propertiesXClampYClamp, &m_pFish3BitmapBrush);
			}
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			if (SUCCEEDED(hr))
			{
				hr = m_pRenderTarget->CreateBitmapBrush(m_pBubbleMaskBitmap, propertiesXClampYClamp, &m_pBubbleBitmapBrush);
			}
		}

		if (SUCCEEDED(hr))
		{
			// Create a solid color brush for writing text.
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pTextBrush);
		}
	}

	rtSize = m_pRenderTarget->GetSize();

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pBaseBrush);
	SAFE_RELEASE(m_pMenuBoxBrush);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pBackgroundBitmap);
	SAFE_RELEASE(m_pFish1BitmapBrush);
	SAFE_RELEASE(m_pFish2BitmapBrush);
	SAFE_RELEASE(m_pFish3BitmapBrush);
	SAFE_RELEASE(m_pSharkBitmapBrush);
	SAFE_RELEASE(m_pBubbleBitmapBrush);
}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		srand(GetTickCount());
		static float bubble_time = 0.0f;
		float bubbleLength = m_BubbleAnimation.GetValue(bubble_time);

		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		DrawBackground();
		DrawFood();
		FishAction();
		SharkAction();

		if(bubble_time == 0.0f)
		{	// 물방울이 끝까지 올라가면 새로운 위치를 선정함.
			int randX = rand() % 745 + 25;
			int randY = rand() % 380 + 180;
			bubbleLocation.width = (float)randX;
			bubbleLocation.height = (float)randY;
		}
		
		DrawBubble(hr, bubbleLength, bubbleLocation);
		DrawMenu();

		if (createMode)
		{
			if (newFish)
			{
				randomFish = rand() % 3;	// 물고기가 생성되면 새 물고기의 번호를 다시 지정
				newFish = false;
			}
			DrawCreateModeAction(randomFish);
		}
		else if (feedingMode)
		{
			DrawFeedingModeAction();
		}
		else if (huntingMode)
		{
			DrawHuntingModeAction();
			if (shot)
			{
				ShotAction();
			}
		}

		hr = m_pRenderTarget->EndDraw();

		// When we reach the end of the animation, loop back to the beginning.
		if (bubble_time >= m_BubbleAnimation.GetDuration())
		{
			bubble_time = 0.0f;
		}
		else
		{
			float elapsedTime = static_cast<float>(m_BubbleDwmTimingInfo.rateCompose.uiDenominator) /
				static_cast<float>(m_BubbleDwmTimingInfo.rateCompose.uiNumerator);
			bubble_time += elapsedTime;
		}
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_MOUSEMOVE:
				currentPoint.x = LOWORD(lParam);
				currentPoint.y = HIWORD(lParam);
				break;

			case WM_LBUTTONDOWN:
				if (currentPoint.x >= 1 && currentPoint.x <= 149 &&
					currentPoint.y >= 0 && currentPoint.y <= 79)
				{	// 물고기 생성 버튼 눌렀을 때
					createMode = !createMode;	// 물고기 생성 상태 <-> 일반상태
					huntingMode = false;		// 모드 중복 방지
					feedingMode = false;
					newFish = true;				// 클릭하면 새로운 랜덤 물고기 생성
				}

				if (currentPoint.x >= 151 && currentPoint.x <= 299 &&
					currentPoint.y >= 0 && currentPoint.y <= 79)
				{	// 먹이주기 버튼 눌렀을 때
					feedingMode = !feedingMode;	// 먹이주기상태 <-> 일반상태
					createMode  = false;
					huntingMode = false;
				}

				if (currentPoint.x >= 301 && currentPoint.x <= 449 &&
					currentPoint.y >= 0 && currentPoint.y <= 79)
				{	// 사냥하기 버튼 눌렀을 때
					huntingMode = !huntingMode;	// 사냥하기상태 <-> 일반상태
					playHuntSound = true;
					createMode  = false;
					feedingMode = false;
				}

				if (currentPoint.x >= 451 && currentPoint.x <= 599 &&
					currentPoint.y >= 0 && currentPoint.y <= 79)
				{	// 종료 버튼 눌렀을 때
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}

				if (createMode && currentPoint.y >= 80)
				{	
					// 물고기 생성 모드가 활성화되었을 때 화면을 클릭하면 그 자리에 물고기가 생성됨
					Character *tmp = new Character(Character::TYPE_FISH, Character::STATE_STAND,
						currentPoint, true, randomFish, DEFAULT_VELOCITY_FISH, true, fishID++);

					fishQueue.push(tmp);
					newFish = true;
				}
				
				if (feedingMode && currentPoint.y >= 80)
				{	// 먹이주기 모드가 활성화되었을 때 박스 하단에서 클릭하면 먹이를 큐에 저장
					FOOD *tmp = new FOOD;
					tmp->pos = currentPoint;
					tmp->exist = true;
					tmp->velocity = 20.0f;	// 초기속도
					tmp->ID = foodID++;
					foodQueue.push(tmp);
				}

				if (huntingMode && currentPoint.y >= 80 )
				{
					shot = true;
				}
				break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_PAINT:
			{
				pDemoApp->OnRender();
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			wasHandled = true;
			result = 1;
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}

void DemoApp::DrawMenu()
{
	WCHAR create[12], feed[10], hunt[10], exit[10], scoreBoard[30];
	swprintf_s(create, L"물고기 생성");
	swprintf_s(feed, L"먹이주기");
	swprintf_s(hunt, L"사냥하기");
	swprintf_s(exit, L"종료");
	swprintf_s(scoreBoard, L"물고기 : %d마리 ", fishQueue.size());

	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pRenderTarget->FillRectangle(
		D2D1::Rect(0.0f, 0.0f, 150.0f, 80.0f),
		m_pMenuBoxBrush);
	m_pRenderTarget->DrawRectangle(
		D2D1::Rect(0.0f, 0.0f, 150.0f, 80.0f),
		m_pBaseBrush);
	m_pRenderTarget->DrawText(
		create,
		wcslen(create),
		m_pTextFormat,
		D2D1::Rect(0.0f, 0.0f, 150.0f, 80.0f),
		m_pTextBrush
		);

	m_pRenderTarget->FillRectangle(
		D2D1::Rect(150.0f, 0.0f, 300.0f, 80.0f),
		m_pMenuBoxBrush);
	m_pRenderTarget->DrawRectangle(
		D2D1::Rect(150.0f, 0.0f, 300.0f, 80.0f),
		m_pBaseBrush);
	m_pRenderTarget->DrawText(
		feed,
		wcslen(feed),
		m_pTextFormat,
		D2D1::Rect(150.0f, 0.0f, 300.0f, 80.0f),
		m_pTextBrush
		);

	m_pRenderTarget->FillRectangle(
		D2D1::Rect(300.0f, 0.0f, 450.0f, 80.0f),
		m_pMenuBoxBrush);
	m_pRenderTarget->DrawRectangle(
		D2D1::Rect(300.0f, 0.0f, 450.0f, 80.0f),
		m_pBaseBrush);
	m_pRenderTarget->DrawText(
		hunt,
		wcslen(hunt),
		m_pTextFormat,
		D2D1::Rect(300.0f, 0.0f, 450.0f, 80.0f),
		m_pTextBrush
		);

	m_pRenderTarget->FillRectangle(
		D2D1::Rect(450.0f, 0.0f, 600.0f, 80.0f),
		m_pMenuBoxBrush);
	m_pRenderTarget->DrawRectangle(
		D2D1::Rect(450.0f, 0.0f, 600.0f, 80.0f),
		m_pBaseBrush);
	m_pRenderTarget->DrawText(
		exit,
		wcslen(exit),
		m_pTextFormat,
		D2D1::Rect(450.0f, 0.0f, 600.0f, 80.0f),
		m_pTextBrush
		);

	m_pRenderTarget->FillRectangle(
		D2D1::Rect(600.0f, 0.0f, 800.0f, 80.0f),
		m_pMenuBoxBrush);
	m_pRenderTarget->DrawRectangle(
		D2D1::Rect(600.0f, 0.0f, 800.0f, 80.0f),
		m_pBaseBrush);
	m_pRenderTarget->DrawText(
		scoreBoard,
		wcslen(scoreBoard),
		m_pTextFormat,
		D2D1::Rect(600.0f, 0.0f, 800.0f, 80.0f),
		m_pTextBrush
		);

	m_pRenderTarget->DrawLine(
		D2D1::Point2F(0.0f, 80.0f),
		D2D1::Point2F(m_pRenderTarget->GetSize().width, 80.0f),
		m_pBaseBrush);
}

void DemoApp::DrawSeaweed(float translateX)
{
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

	// Apply the translation transform to the render target.
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(translateX, rtSize.height));

	// 수초 그리기
	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LimeGreen, 1.0f));
	m_pRenderTarget->FillGeometry(m_pSeaweedGeometry[seaweedNumber], m_pBaseBrush);

	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pRenderTarget->DrawGeometry(m_pSeaweedGeometry[seaweedNumber], m_pBaseBrush, 1.0f);

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	
	seaweed2 = clock();	// 초기 start값은 initial 함수에서 정의함.

	if (seaweed2 - seaweed1 > 250)
	{	// 약 0.25초에 한 번 수초의 모양이 변화함. 수초의 인덱스는 0, 1, 2
		if (seaweedNumber == 0)		 UPorDOWN = 1;	// 숫자가 0이면 상승
		else if (seaweedNumber == 2) UPorDOWN = -1;	// 숫자가 2이면 하강

		seaweedNumber += 1 * UPorDOWN;
		seaweed1 = clock();
	}
}

void DemoApp::DrawBackground()
{
	m_pRenderTarget->DrawBitmap(m_pBackgroundBitmap, D2D1::RectF(0, 0, rtSize.width, rtSize.height), 0.5f);
	// 배경을 그림

	float trans = 20.0f;

	while (trans <= rtSize.width)
	{	// 수초 다발들을 그림
		DrawSeaweed(trans);
		DrawSeaweed(trans + 35.0f);
		trans += 115.0f;
	}
}

void DemoApp::DrawCreateModeAction(int randomFish)
{
	// 마우스를 따라다니는 물고기 모형 생성
	Character tmp = Character(Character::TYPE_FISH, currentPoint, randomFish);
	DrawFish(tmp, 0.7f);
}

void DemoApp::DrawFish(Character _target, float opacity = 1.0f)
{
	// 번호에 맞는 물고기 또는 상어를 해당 좌표에 그림.
	// 투명도는 기본이 1.0f, 마우스를 따라다닐 때 0.7f
	D2D1_RECT_F target;
	D2D1_POINT_2F pos = _target.pos;

	if (_target.type == Character::TYPE_FISH)
	{	// 물고기인 경우 물고기 번호에 맞춰 그림.
		switch (_target.fishNumber)
		{
		case 0:
			target = D2D1::RectF(
				pos.x - 55, pos.y - 55,
				pos.x + 55, pos.y + 55);
			m_pRenderTarget->DrawBitmap(m_pFish1MaskBitmap, target, opacity);
			break;

		case 1:
			target = D2D1::RectF(
				pos.x - 50, pos.y - 50,
				pos.x + 50, pos.y + 50);
			m_pRenderTarget->DrawBitmap(m_pFish2MaskBitmap, target, opacity);
			break;

		case 2:
			target = D2D1::RectF(
				pos.x - 50, pos.y - 70,
				pos.x + 50, pos.y + 70);
			m_pRenderTarget->DrawBitmap(m_pFish3MaskBitmap, target, opacity);
			break;

		default:
			break;
		}
	}

	else if (_target.type == Character::TYPE_SHARK)
	{
		target = D2D1::RectF(
			pos.x - 100, pos.y - 60,
			pos.x + 100, pos.y + 60);
		m_pRenderTarget->DrawBitmap(m_pSharkMaskBitmap, target, opacity);
	}
}

void DemoApp::FishAction()
{
	std::queue <Character*> tmp;

	while (!fishQueue.empty())
	{
		if (!fishQueue.front()->exist)
		{
			fishQueue.pop();
		}
		else
		{
			tmp.push(fishQueue.front());
			fishQueue.pop();
		}
	}

	while (!tmp.empty())
	{
		Character *target = tmp.front();

		if (!target->isLeft)
		{	// 방향이 오른쪽이면 좌우 반전
			D2D1_MATRIX_3X2_F reverseMatrix = D2D1::Matrix3x2F(
				-1.0f, 0.0f,
				0.0f, 1.0f,
				0.0f, 0.0f);

			D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
				2 * target->pos.x, 0.0f);

			m_pRenderTarget->SetTransform(reverseMatrix * translation);
		}

		DrawFish(*target);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		target->CharacterAction(rtSize, foodQueue, shark);

		fishQueue.push(target);
		tmp.pop();
	}
}

void DemoApp::SharkAction()
{
	sharkTime2 = clock();

	if (!sharkOn && sharkTime2 - sharkTime1 > 15000)
	{	// 상어가 15초정도 존재하지 않으면 새 상어가 나타남.
		sharkOn = true;
		shark.NewRandomPos(rtSize);
		shark.exist = true;
		shark.health = 10;
		shark.velocity = DEFAULT_VELOCITY_SHARK;
	}

	if (sharkOn)
	{
		sharkTime1 = clock();	// 상어가 나타나면 시간을 재지 않음

		if (shark.health <= 0)
		{
			sharkOn = false;
			shark.exist = false;
		}

		else
		{

			if (!shark.isLeft)
			{	// 방향이 오른쪽이면 좌우 반전
				D2D1_MATRIX_3X2_F reverseMatrix = D2D1::Matrix3x2F(
					-1.0f, 0.0f,
					0.0f, 1.0f,
					0.0f, 0.0f);

				D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
					2 * shark.pos.x, 0.0f);

				m_pRenderTarget->SetTransform(reverseMatrix * translation);
			}

			DrawFish(shark);
			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			DrawHealthBar(shark);

			shark.CharacterAction(rtSize, fishQueue, currentPoint);
		}
	}
}

void DemoApp::DrawHealthBar(Character target)
{
	float redBarSize = 0.096f * target.health;

	D2D1::Matrix3x2F grayBarTrans = D2D1::Matrix3x2F::Translation(
		target.pos.x - 70.0f, target.pos.y + 70.0f);

	D2D1::Matrix3x2F redBarTrans = D2D1::Matrix3x2F::Translation(
		target.pos.x - 67.0f, target.pos.y + 73.0f);

	D2D1::Matrix3x2F redBarScale = D2D1::Matrix3x2F::Scale(
		D2D1::Size(redBarSize, 0.7f), D2D1::Point2F(0.0f, 0.0f));

	m_pRenderTarget->SetTransform(grayBarTrans);
	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::DarkGray, 1.0f));
	m_pRenderTarget->FillGeometry(m_pHealthBarGeometry, m_pBaseBrush);

	m_pRenderTarget->SetTransform(redBarScale * redBarTrans);
	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::IndianRed, 1.0f));
	m_pRenderTarget->FillGeometry(m_pHealthBarGeometry, m_pBaseBrush);

	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

void DemoApp::DrawFeedingModeAction()
{
	D2D1_ELLIPSE foodMouse = D2D1::Ellipse(
		D2D1::Point2F(currentPoint.x, currentPoint.y), 8, 8); // 마우스를 따라다니는 먹이모양 아이콘

	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.4f));
	m_pRenderTarget->DrawEllipse(foodMouse, m_pBaseBrush);
	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::SandyBrown, 0.7f));
	m_pRenderTarget->FillEllipse(foodMouse, m_pBaseBrush);
	// 마우스를 따라다니는 먹이 모형 생성
}

void DemoApp::DrawHuntingModeAction()
{
	if (playHuntSound)
	{
		soundManager->play(1, FALSE);
		playHuntSound = false;
	}
	
	m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));

	m_pRenderTarget->DrawLine(
		D2D1::Point2F(currentPoint.x - 50.0f, currentPoint.y),
		D2D1::Point2F(currentPoint.x + 50.0f, currentPoint.y),
		m_pBaseBrush);	// 가로 선 그리기

	m_pRenderTarget->DrawLine(
		D2D1::Point2F(currentPoint.x, currentPoint.y - 50.0f),
		D2D1::Point2F(currentPoint.x, currentPoint.y + 50.0f),
		m_pBaseBrush);	// 세로 선 그리기

	m_pRenderTarget->DrawEllipse(
		D2D1::Ellipse(D2D1::Point2F(currentPoint.x, currentPoint.y), 35, 35),
		m_pBaseBrush);	// 원형 그리기
}

void DemoApp::DrawBubble(HRESULT hr, float length, D2D1_SIZE_F bubbleLocation)
{
	D2D1_POINT_2F point;
	D2D1_POINT_2F tangent;
	D2D1_MATRIX_3X2_F bubbleMatrix;

	hr = m_pBubblePathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

	Assert(SUCCEEDED(hr));

	D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(
		bubbleLocation.width, bubbleLocation.height);

	bubbleMatrix = D2D1::Matrix3x2F(
		tangent.x, tangent.y,
		tangent.y, tangent.x,
		point.x, point.y);

	m_pRenderTarget->SetTransform(bubbleMatrix * translation);
	m_pRenderTarget->DrawBitmap(m_pBubbleMaskBitmap, D2D1::RectF(0.0f, 0.0f, 70.0f, 50.0f), 0.75f);
	m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}

HRESULT DemoApp::LoadResourceBitmap(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);

	return hr;
}

void DemoApp::DrawFood()
{
	std::queue <FOOD*> tmp;

	while (!foodQueue.empty())
	{
		if (foodQueue.front()->pos.y > 570)
		{
			foodQueue.front()->exist = false;
		}

		else if (foodQueue.front()->velocity < 0.01f)
		{
			foodQueue.front()->exist = false;
		}

		if (!foodQueue.front()->exist)
		{
			foodQueue.pop();
		}
		else
		{
			tmp.push(foodQueue.front());
			foodQueue.pop();
		}
	}

	while (!tmp.empty())
	{
		float dt = 0.1f;	// 먹이객체의 하강 시간 간격(timestep)
		D2D1_ELLIPSE target = D2D1::Ellipse(tmp.front()->pos, 8, 8);

		float acc = tmp.front()->pos.y / 75 * dt;

		m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
		m_pRenderTarget->DrawEllipse(target, m_pBaseBrush);
		m_pBaseBrush->SetColor(D2D1::ColorF(D2D1::ColorF::SandyBrown, 1.0f));
		m_pRenderTarget->FillEllipse(target, m_pBaseBrush);

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		tmp.front()->pos.y += tmp.front()->velocity * dt;
		tmp.front()->velocity -= acc * dt;
		foodQueue.push(tmp.front());
		tmp.pop();
	}
}

void DemoApp::ShotAction()
{
	soundManager->play(0, FALSE);
	shot = false;

	float distance = sqrt(pow((currentPoint.x - shark.pos.x), 2) +
						  pow((currentPoint.x - shark.pos.y), 2));

	if (distance < 180.0f)
	{
		--shark.health;
		shark.state = Character::STATE_RUNAWAY;
		shark.timeCheck1 = clock();
	}
}