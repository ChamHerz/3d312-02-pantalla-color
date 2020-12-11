#include <cassert>
#include "Engine.h"

Engine* Engine::engine = nullptr;
Engine* Engine::getEngine()
{
	return engine;
}

Engine::Engine(HINSTANCE hInstance) : engineInstance(hInstance)
{
	assert(engine == nullptr);
	engine = this;
}

Engine::~Engine()
{
	// @TODO
	/*if (device != nullptr)
		flushCommandQueue();*/
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Reenviamos el hwnd, xq despues del CreateWindows el hwnd es valido
	return Engine::getEngine()->MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT Engine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_DESTROY se manda cuando comienza a destruirse
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Engine::initMainWindow()
{
	// inciar la ventana principal
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = engineInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"El registro de la clase fallo", 0, 0);
		return false;
	}

	// Creamos un rectangulo en base a las dimensiones del cliente
	RECT R = { 0, 0, clientWidth, clientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	engineHwnd = CreateWindow(L"MainWnd", engineCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, engineInstance, 0);
	if (!engineHwnd)
	{
		MessageBox(0, L"Error al crear la ventana principal.", 0, 0);
		return false;
	}

	ShowWindow(engineHwnd, SW_SHOW);
	UpdateWindow(engineHwnd);

	return true;
}

bool Engine::initDirectX()
{
	// iniciamos el DirectX 12
	return true;
}

bool Engine::initialize()
{
	if (!initMainWindow())
		return false;

	if (!initDirectX())
		return false;

	return true;
}

int Engine::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

void Engine::createDevice()
{
	// Creamos el Factory
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));

	// es un puntero inteligente, se destruye al salir de la funcion
	ComPtr<IDXGIAdapter1> adapter;

	// buscamos un adaptador e intentamos crear el device
	bool adapterFound = false;
	for (UINT adapterIndex = 0;
		!adapterFound && factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND;
		++adapterIndex)
	{
		// Extraemos los datos del adapter actual
		DXGI_ADAPTER_DESC1 AdapterDesc;
		adapter->GetDesc1(&AdapterDesc);

		// Una placa de video tiene este flag
		if (AdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		// al usar el nullptr, solo revisa si el adaptador se banca el DirectX
		const HRESULT result = D3D12CreateDevice(
			adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
		if (SUCCEEDED(result))
		{
			adapterFound = true;
		}
	}

	// Ahora si creamos el Device y lo guardamos en el puntero inteligente
	D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
}