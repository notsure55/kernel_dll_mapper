#pragma once

#include <dxgi.h>
#include <d3d11.h>

namespace Menu {
	void display(ID3D11DeviceContext* context, ID3D11RenderTargetView* mainRenderTargetView);
}
