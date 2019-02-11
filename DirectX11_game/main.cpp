///////////////////////////////////////////////////////////////////////
//	DirectX11 ゲーム
//	タイトル：main.cpp
//	作成者	：戸部俊太
//	作成日	：2019/02/10
///////////////////////////////////////////////////////////////////////
//	更新ログ
//	2019/02/10	・ウィンドウ作成,D3D初期化まで
//	2019/02/12	・基本的なシェーダ追加,三角ポリゴン表示まで
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//	インクルードファイル
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
			WINDOW_HEIGHT, (LPSTR)API_NAME)))//	文字セットをマルチバイトに設定してもconst char* から LPSTR　の互換がない　のエラーが発生するのでLPSTRにcastしている
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

///////////////////////////////////////////////////////////////////////
//	ウィンドプロシージャ
///////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_pMain->MsgProc(hWnd, uMsg, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////
//	コンストラクタ
///////////////////////////////////////////////////////////////////////
CMain::CMain()
{
	ZeroMemory(this, sizeof(CMain));
}

///////////////////////////////////////////////////////////////////////
//	デストラクタ
///////////////////////////////////////////////////////////////////////
CMain::~CMain()
{
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
//	メッセージプロシージャ
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
//	メインループ処理
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
//	アプリケーション処理
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

	//	hlslファイル読み込み	ブロブ作成
	ID3DBlob *pCompileShader = NULL;

	//	バーテックスシェーダ作成
	if (FAILED(MakeShader((LPSTR)SIMPLE_SHADER_FILE, (LPSTR)"VS", (LPSTR)"vs_5_0", (void**)&m_pVertexShader, &pCompileShader))) return E_FAIL;
	//	頂点インプットレイアウト定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//	頂点インプットレイアウト作成
	if (FAILED(m_pDevice->CreateInputLayout(layout, numElements, pCompileShader->GetBufferPointer(), pCompileShader->GetBufferSize(), &m_pVertexLayout)))
	{
		return FALSE;
	}
	SAFE_RELEASE(pCompileShader);

	//	ピクセルシェーダ作成
	if (FAILED(MakeShader((LPSTR)SIMPLE_SHADER_FILE, (LPSTR)"PS", (LPSTR) "ps_5_0", (void**)&m_pPixelShader, &pCompileShader))) return E_FAIL;
	SAFE_RELEASE(pCompileShader);

	//	コンスタントバッファ作成
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(D3DXMATRIX);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDevice->CreateBuffer(&cb, NULL, &m_pConstantBuffer)))
	{
		return E_FAIL;
	}

	//	ポリゴン、メッシュなどのジオメトリ関連作成
	if (FAILED(InitModel()))
	{
		return E_FAIL;
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////
//	hlslファイルからシェーダ作成
///////////////////////////////////////////////////////////////////////
HRESULT CMain::MakeShader(LPSTR szFileName, LPSTR szFuncName, LPSTR szProfileName, void** ppShader, ID3DBlob** ppBlob)
{
	ID3DBlob *pErrors = NULL;

	if (FAILED(D3DX11CompileFromFileA(szFileName, NULL, NULL, szFuncName, szProfileName, D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, NULL, ppBlob, &pErrors, NULL)))
	{
		char* p = (char*)pErrors->GetBufferPointer();
		MessageBox(0, p, 0, MB_OK);
		return E_FAIL;
	}
	SAFE_RELEASE(pErrors);

	char szProfile[3] = { 0 };
	memcpy(szProfile,szProfileName, 2);

	//	頂点シェーダ
	if (strcmp(szProfile, "vs") == 0)
	{
		if( FAILED(m_pDevice->CreateVertexShader((*ppBlob)->GetBufferPointer(),(*ppBlob)->GetBufferSize(),NULL, (ID3D11VertexShader**)ppShader)))	return E_FAIL;
	}
	//	ピクセルシェーダ
	if (strcmp(szProfile, "ps") == 0)
	{
		if (FAILED(m_pDevice->CreatePixelShader((*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, (ID3D11PixelShader**)ppShader)))	return E_FAIL;
	}
	//	ジオメトリシェーダ
	if (strcmp(szProfile, "gs") == 0)
	{
		if (FAILED(m_pDevice->CreateGeometryShader((*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, (ID3D11GeometryShader**)ppShader)))	return E_FAIL;
	}
	//	フルシェーダ
	if (strcmp(szProfile, "hs") == 0)
	{
		if (FAILED(m_pDevice->CreateHullShader((*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, (ID3D11HullShader**)ppShader)))	return E_FAIL;
	}
	//	ドメインシェーダ
	if (strcmp(szProfile, "ds") == 0)
	{
		if (FAILED(m_pDevice->CreateDomainShader((*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, (ID3D11DomainShader**)ppShader)))	return E_FAIL;
	}
	//ComputeShader
	if (strcmp(szProfile, "cs") == 0)
	{
		if (FAILED(m_pDevice->CreateComputeShader((*ppBlob)->GetBufferPointer(), (*ppBlob)->GetBufferSize(), NULL, (ID3D11ComputeShader**)ppShader)))	return E_FAIL;
	}
	return S_OK;
}


///////////////////////////////////////////////////////////////////////
//	ポリゴン、メッシュなどのジオメトリ関連を初期化
///////////////////////////////////////////////////////////////////////
HRESULT CMain::InitModel()
{
	//	バーテックスバッファ作成
	SimpleVertex vertices[]=
	{
		D3DXVECTOR3(0.0f, 0.5f, 0.0f),
		D3DXVECTOR3(0.5f, -0.5f, 0.0f),
		D3DXVECTOR3(-0.5f, -0.5f,0.0f),
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	if (FAILED(m_pDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer)))
	{
		return E_FAIL;
	}
	//	バーテックスバッファセット
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

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

	D3DXMATRIX World;
	D3DXMATRIX View;
	D3DXMATRIX Proj;
	//	ワールドトランスフォーム
	D3DXMatrixRotationY(&World, timeGetTime() / 100.0f);

	//	ビュートランスフォーム
	D3DXVECTOR3 vEyePt(0.0f, 1.0f, -2.0f);		//	視点
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);	//	注視点
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);		//	上方位置
	D3DXMatrixLookAtLH(&View, &vEyePt, &vLookatPt, &vUpVec);

	//	プロジェクショントランスフォーム
	D3DXMatrixPerspectiveFovLH(&Proj, D3DX_PI / 4, (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, 0.1f, 100.0f);

	//	シェーダセット
	m_pDeviceContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, NULL, 0);

	//	シェーダのコンスタントバッファに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	SIMPLESHADER_CONSTANT_BUFFER cb;
	if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//	ワールド、カメラ、射影行列を渡す
		D3DXMATRIX matrix = World * View * Proj;
		D3DXMatrixTranspose(&matrix, &matrix);
		cb.mWVP = matrix;

		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		m_pDeviceContext->Unmap(m_pConstantBuffer, 0);
	}

	//	このコンスタントバッファをどのシェーダで使用するか
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	//	頂点インプットレイアウトセット
	m_pDeviceContext->IASetInputLayout(m_pVertexLayout);
	//	プリミティブ・トポロジーをセット
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//	プリミティブをレンダリング
	m_pDeviceContext->Draw(3, 0);

	m_pSwapChain->Present(0, 0);	//フロント、バックバッファ切り替え
}

///////////////////////////////////////////////////////////////////////
//	Direct3Dオブジェクト解放
///////////////////////////////////////////////////////////////////////
void CMain::DestroyD3D()
{
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pBackBuffer_TexRTV);
	SAFE_RELEASE(m_pBuckBuffer_DSTexDSV);
	SAFE_RELEASE(m_pBuckBuffer_DSTex);
	SAFE_RELEASE(m_pDeviceContext);
	SAFE_RELEASE(m_pDevice);
}