#pragma once
///////////////////////////////////////////////////////////////////////
//	DirectX11 ゲーム
//	作成者：戸部俊太
//	作成日：2019/2/10
///////////////////////////////////////////////////////////////////////
//	更新ログ
//	2019/02/10	・ウィンドウ作成,D3D初期化まで
//	2019/02/12	・基本的なシェーダ追加, 三角ポリゴン表示まで
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//	インクルードファイル
///////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <d3d11.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <d3dcompiler.h>

///////////////////////////////////////////////////////////////////////
//	ライブラリ
///////////////////////////////////////////////////////////////////////
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dCompiler.lib")

///////////////////////////////////////////////////////////////////////
//	警告非表示
///////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4305)

///////////////////////////////////////////////////////////////////////
//	マクロ定義
///////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(p) if(p){p->Release(); p=NULL;}

///////////////////////////////////////////////////////////////////////
//	定数定義
///////////////////////////////////////////////////////////////////////
#define WINDOW_WIDTH (640)			//	ウィンドウ幅
#define WINDOW_HEIGHT (480)			//	ウィンドウ高さ
#define API_NAME "DirectX11 テスト"	//	アプリケーション名
#define SIMPLE_SHADER_FILE	"shader/SimpleShader.hlsl"

//	頂点構造体
struct SimpleVertex
{
	D3DXVECTOR3 Pos;	//	位置情報
};

//	シェーダ用のコンスタントバッファのアプリ側構造体
struct SIMPLESHADER_CONSTANT_BUFFER
{
	D3DXMATRIX mWVP;
};

///////////////////////////////////////////////////////////////////////
//	CMain
//	メインウィンドウクラス
///////////////////////////////////////////////////////////////////////
class CMain
{
public:
	CMain();
	~CMain();
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPSTR);
	HRESULT InitD3D();
	HRESULT InitModel();
	HRESULT MakeShader(LPSTR, LPSTR, LPSTR, void**, ID3DBlob**);
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
	void Loop();
	void App();
	void Render();
	void DestroyD3D();

	//	メンバ変数
	HWND					m_hWnd;						//	ウィンドウ情報
	ID3D11Device*			m_pDevice;					//	D3Dデバイス
	ID3D11DeviceContext*	m_pDeviceContext;			//	デバイスコンテキスト
	IDXGISwapChain*			m_pSwapChain;				//	スワップチェイン
	ID3D11Texture2D*		m_pBuckBuffer_DSTex;		//	2Dテクスチャ
	ID3D11RenderTargetView*	m_pBackBuffer_TexRTV;		//	レンダターゲット
	ID3D11DepthStencilView*	m_pBuckBuffer_DSTexDSV;		//	ステンシルビュー

	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*		m_pVertexShader;			//	頂点シェーダー
	ID3D11PixelShader*		m_pPixelShader;				//	ピクセルシェーダー
	ID3D11Buffer*			m_pConstantBuffer;
	ID3D11Buffer*			m_pVertexBuffer;			//	頂点バッファ
};