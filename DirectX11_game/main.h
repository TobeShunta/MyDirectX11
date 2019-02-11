#pragma once
///////////////////////////////////////////////////////////////////////
//	DirectX11 �Q�[��
//	�쐬�ҁF�˕��r��
//	�쐬���F2019/2/10
///////////////////////////////////////////////////////////////////////
//	�X�V���O
//	2019/02/10	�E�E�B���h�E�쐬,D3D�������܂�
//	2019/02/12	�E��{�I�ȃV�F�[�_�ǉ�, �O�p�|���S���\���܂�
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
///////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <d3d11.h>
#include <D3DX10.h>
#include <D3DX11.h>
#include <d3dcompiler.h>

///////////////////////////////////////////////////////////////////////
//	���C�u����
///////////////////////////////////////////////////////////////////////
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dCompiler.lib")

///////////////////////////////////////////////////////////////////////
//	�x����\��
///////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4305)

///////////////////////////////////////////////////////////////////////
//	�}�N����`
///////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(p) if(p){p->Release(); p=NULL;}

///////////////////////////////////////////////////////////////////////
//	�萔��`
///////////////////////////////////////////////////////////////////////
#define WINDOW_WIDTH (640)			//	�E�B���h�E��
#define WINDOW_HEIGHT (480)			//	�E�B���h�E����
#define API_NAME "DirectX11 �e�X�g"	//	�A�v���P�[�V������
#define SIMPLE_SHADER_FILE	"shader/SimpleShader.hlsl"

//	���_�\����
struct SimpleVertex
{
	D3DXVECTOR3 Pos;	//	�ʒu���
};

//	�V�F�[�_�p�̃R���X�^���g�o�b�t�@�̃A�v�����\����
struct SIMPLESHADER_CONSTANT_BUFFER
{
	D3DXMATRIX mWVP;
};

///////////////////////////////////////////////////////////////////////
//	CMain
//	���C���E�B���h�E�N���X
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

	//	�����o�ϐ�
	HWND					m_hWnd;						//	�E�B���h�E���
	ID3D11Device*			m_pDevice;					//	D3D�f�o�C�X
	ID3D11DeviceContext*	m_pDeviceContext;			//	�f�o�C�X�R���e�L�X�g
	IDXGISwapChain*			m_pSwapChain;				//	�X���b�v�`�F�C��
	ID3D11Texture2D*		m_pBuckBuffer_DSTex;		//	2D�e�N�X�`��
	ID3D11RenderTargetView*	m_pBackBuffer_TexRTV;		//	�����_�^�[�Q�b�g
	ID3D11DepthStencilView*	m_pBuckBuffer_DSTexDSV;		//	�X�e���V���r���[

	ID3D11InputLayout*		m_pVertexLayout;
	ID3D11VertexShader*		m_pVertexShader;			//	���_�V�F�[�_�[
	ID3D11PixelShader*		m_pPixelShader;				//	�s�N�Z���V�F�[�_�[
	ID3D11Buffer*			m_pConstantBuffer;
	ID3D11Buffer*			m_pVertexBuffer;			//	���_�o�b�t�@
};