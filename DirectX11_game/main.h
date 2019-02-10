#pragma once
//	�C���N���[�h�t�@�C��
#include <windows.h>
#include <d3d11.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <d3dcompiler.h>

//	���C�u����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dCompiler.lib")

//	�x����\��
#pragma warning(disable : 4305)

//	�萔��`
#define WINDOW_WIDTH (640)			//	�E�B���h�E��
#define WINDOW_HEIGHT (480)			//	�E�B���h�E����
#define API_NAME "DirectX11 �e�X�g"	//	�A�v���P�[�V������

//	�}�N����`
#define SAFE_RELEASE(p) if(p){p->Release(); p=NULL;}

//	���C�����[�`���N���X
class CMain
{
public:
	HRESULT InitWindow(HINSTANCE, INT, INT, INT, INT, LPSTR);
	HRESULT InitD3D();
	LRESULT MsgProc(HWND, UINT, WPARAM, LPARAM);
	void Loop();
	void App();
	void Render();
	void DestroyD3D();

	HWND m_hWnd;	//	�E�B���h�E���
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pBuckBuffer_DSTex;
	ID3D11RenderTargetView* m_pBackBuffer_TexRTV;
	ID3D11DepthStencilView* m_pBuckBuffer_DSTexDSV;
};