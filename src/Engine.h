//***************************************************************************************
// Engine.h by ChamHerz (C) 2020
//***************************************************************************************

#pragma once

// Activa los features para adiministrar la memoria en Debug
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <windows.h>
#include <xstring>

//DirectX12
#include <d3d12.h>				// para el Device
#include <dxgi1_6.h>			// para el IDXGIFactory4

// Punteros Inteligentes
#include <wrl.h>
using namespace Microsoft::WRL;

static const UINT8 chainSize = 2;

class Engine
{
public:

protected:
	// constructor protegido SINGLETON
	Engine(HINSTANCE hInstance);
	Engine(const Engine& rhs) = delete;
	Engine& operator=(const Engine& rhs) = delete;
	virtual ~Engine();

	static Engine* engine;
	HINSTANCE engineInstance = nullptr;			// instancia de la aplicacion
	HWND      engineHwnd = nullptr;				// handle de la ventana principal

	bool initMainWindow();
	bool initDirectX();
	int clientWidth = 800;
	int clientHeight = 600;
	std::wstring engineCaption = L"Shader Game";

	// ==== DIRECTX12 ==== //

	// Dispositivos
	ComPtr<IDXGIFactory4> factory;				// Constructor de Interfaces
	ComPtr<ID3D12Device> device;				// Placa de video

	void createDevice();

	// Colas
	ComPtr<ID3D12CommandQueue> CommandQueue;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;

	// Fences
	ComPtr<ID3D12Fence> Fence;
	HANDLE FenceEvent;
	UINT64 FenceValue;

	// Swapchain
	ComPtr<IDXGISwapChain3> Swapchain;
	ComPtr<ID3D12DescriptorHeap> RenderTargetViewHeap;
	ComPtr<ID3D12Resource> RenderTargets[chainSize];


public:
	static Engine* getEngine();

	virtual bool initialize();

	int run();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};