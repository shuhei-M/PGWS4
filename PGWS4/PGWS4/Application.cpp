#include "Application.h"
#include <tchar.h>
#include "Dx12Wrapper.h"
#include "PMDRenderer.h"
#include "PMDActor.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
// @param format �t�H�[�}�b�g�i%d �Ƃ� %f �Ƃ��́j
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���
static void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf_s(format, valist);
	va_end(valist);
#endif
}

static inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

// �ʓ|�����Ǐ����Ȃ���΂����Ȃ��֐�
static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0); // OS �ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // ����̏������s��
}


static HWND CreateGameWindow(WNDCLASSEX& windowClass, UINT width, UINT height)
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure; // �R�[���o�b�N�֐��̎w��
	windowClass.lpszClassName = _T("DX12Sample"); // �A�v���P�[�V�����N���X���i�K���ł悢�j
	windowClass.hInstance = GetModuleHandle(nullptr); // �n���h���̎擾

	RegisterClassEx(&windowClass); // �A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w��� OS �ɓ`����j

	RECT wrc = { 0, 0, (LONG)width, (LONG)height }; // �E�B���h�E�T�C�Y�����߂�

	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// �E�B���h�E�I�u�W�F�N�g�̐���
	return CreateWindow(windowClass.lpszClassName, // �N���X���w��
		_T("DX12 �e�X�g "), // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW, // �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT, // �\�� x ���W�� OS �ɂ��C��
		CW_USEDEFAULT, // �\�� y ���W�� OS �ɂ��C��
		wrc.right - wrc.left, // �E�B���h�E��
		wrc.bottom - wrc.top, // �E�B���h�E��
		nullptr, // �e�E�B���h�E�n���h��
		nullptr, // ���j���[�n���h��
		windowClass.hInstance, // �Ăяo���A�v���P�[�V�����n���h��
		nullptr); // �ǉ��p�����[�^�[
}


bool Application::Init()
{
	ThrowIfFailed(CoInitializeEx(0, COINIT_MULTITHREADED));// �Ă΂Ȃ��Ɠ��삵�Ȃ����\�b�h������

	_hwnd = CreateGameWindow(_windowClass, _window_width, _window_height);// �E�B���h�E����

	_dx12.reset(new Dx12Wrapper(_hwnd, _window_width, _window_height));
	_pmdRenderer.reset(new PMDRenderer(*_dx12));

	//	_pmdActor.reset(new PMDActor("Model/�������J.pmd", *_pmdRenderer));
	//	_pmdActor.reset(new PMDActor("Model/�����~�Nmetal.pmd", *_pmdRenderer));
	_pmdActor.reset(new PMDActor("Model/�����~�N.pmd", *_pmdRenderer));

	return true;
}

void Application::Run()
{
	//�E�B���h�E�\��
	ShowWindow(_hwnd, SW_SHOW);

	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �A�v���P�[�V�������I���Ƃ��� message �� WM_QUIT �ɂȂ�
			if (msg.message == WM_QUIT)
			{
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		_pmdActor->Update();

		_dx12->BeginDraw();

		auto _cmdList = _dx12->CommandList();
		_cmdList->SetPipelineState(_pmdRenderer->GetPipelineState());
		_cmdList->SetGraphicsRootSignature(_pmdRenderer->GetRootSignature());

		_dx12->ApplySceneDescHeap();

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_pmdActor->Draw();

		_dx12->EndDraw();
	}
}

void Application::Terminate()
{
	//�����N���X�g��񂩂�o�^����
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}

Application::Application()
{
}


Application::~Application()
{
}

Application& Application::Instance()
{
	static Application instance;
	return instance;
}
