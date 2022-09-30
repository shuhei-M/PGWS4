#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <vector>
#include <d3dcompiler.h>
#include <string>
#include "MyHeader.h"
#ifdef _DEBUG
#include <iostream>
#endif

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif // _DEBUG
}

// �ʓ|�����Ǐ����Ȃ���΂Ȃ�Ȃ��֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparm, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);   // OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparm, lparam);
}

#ifdef _DEBUG
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	if (!SUCCEEDED(result)) return;

	debugLayer->EnableDebugLayer();
	debugLayer->Release();
}
#endif // _DEBUG


#ifdef _DEBUG
int main()
{

#else
    int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG

    /*DebugOutputFormatString("Show window test.");
    getchar();
	return 0;*/

	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	// �C���^�[�t�F�C�X�̕ϐ��̐錾
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	ID3D12CommandQueue* _cmdQueue = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	// �E�B���h�E�N���X�̐��� & �o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;   // �R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DX12Sample");   // �A�v���P�[�V�����̃N���X��
	w.hInstance = GetModuleHandle(nullptr);   // �n���h���̎擾

	RegisterClassEx(&w);   // �A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j

	RECT wrc = { 0, 0, window_width, window_height };

	// �֐����g���ăE�B���h�E�T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,// �N���X���w��
		_T("DX12 �e�X�g "), // �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW, // �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT, // �\�� x ���W�� OS �ɂ��C��
		CW_USEDEFAULT, // �\�� y ���W�� OS �ɂ��C��
		wrc.right - wrc.left, // �E�B���h�E��
		wrc.bottom - wrc.top, // �E�B���h�E��
		nullptr, // �e�E�B���h�E�n���h��
		nullptr, // ���j���[�n���h��
		w.hInstance, // �Ăяo���A�v���P�[�V�����n���h��
		nullptr); // �ǉ��p�����[�^�[

	// D3D12Device�̐���
	HRESULT D3D12CreateDevice(
		IUnknown * pAdapter,
		D3D_FEATURE_LEVEL MinimumFeatureLevel,
		REFIID riid,
		void** ppDevice);

#ifdef _DEBUG
	// �f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif // _DEBUG

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG,IID_PPV_ARGS(&_dxgiFactory));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG

	// �A�_�v�^�[�̗񋓗p
	std::vector <IDXGIAdapter*> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);   // �A�_�v�^�[�̐����I�u�W�F�N�g�擾
		std::wstring strDesc = adesc.Description;
		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDA"))
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// Direct3D�f�o�C�X�̏�����
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;   // �����\�ȃo�[�W���������������烋�[�v��ł��؂�
		}
	}

	// �R�}���h���X�g�̐����i�f�o�C�X������j
	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator, nullptr,
		IID_PPV_ARGS(&_cmdList));

	// �R�}���h�L���[�̐����i�f�o�C�X������j
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// �^�C���A�E�g�Ȃ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^�[��1�����g��Ȃ����� 0 �ł悢
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// �L���[����
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	// �X���b�v�`�F�[������
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	// �o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	// �t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// ���Ɏw��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// �E�B���h�E�̃t���X�N���[���؂�ւ��\
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd,
		&swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&_swapchain);

	// �f�B�X�N���v�^�q�[�v�̐���
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (int idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		D3D12_CPU_DESCRIPTOR_HANDLE handle
			= rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += idx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);
	}

	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	// 3�̒��_�̒�`
	XMFLOAT3 vertices[] = {
		{-1.0f,-1.0f,0.0f} ,//����
		{-1.0f,+1.0f,0.0f} ,//����
		{+1.0f,-1.0f,0.0f} ,//�E��
	};

	// ���_�o�b�t�@�̐���
	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices);   // ���_�����邾���̃T�C�Y
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* vertBuff = nullptr;

	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	// ���_�f�[�^���R�s�[
	XMFLOAT3* vertMap = nullptr;   // ���C���������ɒ��_�o�b�t�@�Ɠ������������m��
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);   // �m�ۂ������C���������ɒ��_�f�[�^���R�s�[
	std::copy(std::begin(vertices), std::end(vertices), vertMap);   // CPU�Őݒ�͊��������̂Ńr�f���������ɓ]��
	vertBuff->Unmap(0, nullptr);

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();   // 
	vbView.SizeInBytes = sizeof(vertices);   // 
	vbView.StrideInBytes = sizeof(vertices[0]);   // 

	ID3DBlob* _vsBlob = nullptr;
	ID3DBlob* _psBlob = nullptr;

	ID3DBlob* errorBlob = nullptr;
	// �o�[�e�b�N�X�V�F�[�_�̃R���p�C��
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl",   // �V�F�[�_�[��
		nullptr, // define �͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,// �C���N���[�h�̓f�t�H���g
		"BasicVS", "vs_5_0",// �֐���BasicVS�A�ΏۃV�F�[�_�[��vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,// �f�o�b�O�p����эœK���Ȃ�
		0,
		&_vsBlob, &errorBlob);// �G���[����errorBlob �Ƀ��b�Z�[�W������
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		exit(1);   // �s�V�������ȁc
	}
	// �s�N�Z���V�F�[�_�̃R���p�C��
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl",   // �V�F�[�_�[��
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&_psBlob, &errorBlob);
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		exit(1);   // �s�V�������ȁc
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// ��ʐF�̏����l
	float r = 1.0f;
	float g = 0.0f;
	float b = 0.0f;

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

		//DirectX����
		//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

		D3D12_RESOURCE_BARRIER BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;// �J��
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// ���Ɏw��Ȃ�
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];// �o�b�N�o�b�t�@�[���\�[�X
		BarrierDesc.Transition.Subresource = 0;
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;// ���O��PRESENT ���
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;// ������RT���
		_cmdList->ResourceBarrier(1, &BarrierDesc);// �o���A�w����s

		//�����_�[�^�[�Q�b�g���w��
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		// ��ʐF���O���f�[�V����������
		Gradation(&r, &g, &b);

		// ��ʃN���A
		float clearColor[] = { r, g, b, 1.0f }; // ���F
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// �O�ゾ������ւ���
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		_cmdList->ResourceBarrier(1, &BarrierDesc);

		// ���߂̃N���[�Y
		_cmdList->Close();

		// �R�}���h���X�g�̎��s
		ID3D12CommandList* cmdlists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		////�҂�
		_cmdQueue->Signal(_fence, ++_fenceVal);

		if (_fence->GetCompletedValue() != _fenceVal) 
		{
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);// �C�x���g�n���h���̎擾
			WaitForSingleObject(event, INFINITE);// �C�x���g����������܂Ŗ����ɑ҂�
			CloseHandle(event);// �C�x���g�n���h�������
		}

		_cmdAllocator->Reset(); // �L���[���N���A
		_cmdList->Reset(_cmdAllocator, nullptr); // �ĂуR�}���h���X�g�����߂鏀��

		// �t���b�v
		_swapchain->Present(1, 0);
    }

	// �����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);

    return 0;
}
