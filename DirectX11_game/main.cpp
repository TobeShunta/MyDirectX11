///////////////////////////////////////////////////////////////////////
//	DirectX11 ゲーム
//	作成者：戸部俊太
//	作成日：2019/2/10
///////////////////////////////////////////////////////////////////////
#include "main.h"

///////////////////////////////////////////////////////////////////////
// グローバル変数
///////////////////////////////////////////////////////////////////////
CMain* g_pMain = NULL;

///////////////////////////////////////////////////////////////////////
//	プロトタイプ宣言
///////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

///////////////////////////////////////////////////////////////////////
//	メインウィンドウ
///////////////////////////////////////////////////////////////////////
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{
	g_pMain = new CMain;
	if (g_pMain != NULL)
	{
		if (SUCCEEDED(g_pMain->InitWindow(hInstance,0,0,WINDOW_WIDTH,
			WINDOW_HEIGHT, (LPSTR)API_NAME)))
		{
			if (SUCCEEDED(g_pMain->InitD3D()))
			{
				g_pMain->Loop();
			}
		}
		g_pMain->DestroyD3D();
		delete g_pMain;
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pMain->MsgProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////
//	ウィンドウ作成
///////////////////////////////////////////////////////////////////////
HRESULT CMain::InitWindow(HINSTANCE hInstance, INT iX, INT iY, INT iWidth, INT iHeight, LPSTR WindowName)
{
	//	ウィンドウ定義
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);

	//	ウィンドウ作成
	m_hWnd = CreateWindow(WindowName, WindowName, WS_EX_OVERLAPPEDWINDOW, 0, 0, iWidth, iHeight, 0, 0, hInstance, 0);
	if (!m_hWnd)
	{
		return E_FAIL;
	}

	//	ウィンドウ表示
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////
//	メッセージ
///////////////////////////////////////////////////////////////////////
LRESULT CMain::MsgProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
		case WM_KEYDOWN:
			switch ((char)wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, iMsg, wParam,lParam);
}

///////////////////////////////////////////////////////////////////////
//	メインループ
///////////////////////////////////////////////////////////////////////
void CMain::Loop()
{
	//	メッセージループ
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			App();
		}
	}

	//	api終了
}

///////////////////////////////////////////////////////////////////////
//	アプリケーション
///////////////////////////////////////////////////////////////////////
void CMain::App()
{
	Render();
}

///////////////////////////////////////////////////////////////////////
//	Direct3D初期化
///////////////////////////////////////////////////////////////////////
HRESULT CMain::InitD3D()
{
	//	デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = WINDOW_WIDTH;
	sd.BufferDesc.Height = WINDOW_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = NULL;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		0, &pFeatureLevels, 1, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice,
		pFeatureLevel, &m_pDeviceContext)))
	{
		return FALSE;
	}

	//	テクスチャと、付帯する各種ビュー作成

	//	バックバッファテクスチャを取得
	ID3D11Texture2D *pBackBuffer_Tex;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer_Tex);
	//	レンダターゲットビュー作成
	m_pDevice->CreateRenderTargetView(pBackBuffer_Tex, NULL, &m_pBackBuffer_TexRTV);
	SAFE_RELEASE(pBackBuffer_Tex);

	//	デプスステンシルビューテクスチャ作成
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = WINDOW_WIDTH;
	descDepth.Height = WINDOW_HEIGHT;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pBuckBuffer_DSTex);
	//	テクスチャに対しデプスステンシルビューを作成
	m_pDevice->CreateDepthStencilView(m_pBuckBuffer_DSTex, NULL, &m_pBuckBuffer_DSTexDSV);

	//	レンダターゲットビューとステンシルビューをパイプラインにセット
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBuffer_TexRTV, m_pBuckBuffer_DSTexDSV);

	//	ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = WINDOW_WIDTH;
	vp.Height = WINDOW_HEIGHT;

	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pDeviceContext->RSSetViewports(1, &vp);

	//	ラスタライズ設定
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.CullMode = D3D11_CULL_NONE;
	rdc.FillMode = D3D11_FILL_SOLID;
	ID3D11RasterizerState* pIr = NULL;
	m_pDevice->CreateRasterizerState(&rdc, &pIr);
	m_pDeviceContext->RSSetState(pIr);

	return S_OK;
}

///////////////////////////////////////////////////////////////////////
//	描画処理
///////////////////////////////////////////////////////////////////////
void CMain::Render()
{
	//	画面クリア
	float ClearColor[4] = { 0,0,5,1 };																//	クリア色 RGBA
	m_pDeviceContext->ClearRenderTargetView(m_pBackBuffer_TexRTV, ClearColor);						//	カラーバッファクリア
	m_pDeviceContext->ClearDepthStencilView(m_pBuckBuffer_DSTexDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);	//	デプスステンシルバッファクリア

	m_pSwapChain->Present(0, 0);	//フロント、バックバッファ切り替え
}

///////////////////////////////////////////////////////////////////////
//	Direct3Dオブジェクト解放
///////////////////////////////////////////////////////////////////////
void CMain::DestroyD3D()
{
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pBuckBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBuckBuffer_DSTex);
	SAFE_RELEASE(m_pDevice);
}