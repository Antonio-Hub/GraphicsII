#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include <atomic>
#include "..\Common\DirectXHelper.h"

using namespace DX_Graphics;
using namespace Windows::UI::Core;
using namespace DirectX;
using namespace Windows::Foundation;

extern CoreWindow^ gwindow;
extern bool mouse_move;
extern bool left_click;
extern float diffx;
extern float diffy;
extern char buttons[256];

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	asteroid_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	////Load asteroid model
	obj.loadOBJ("asteroid.obj", asteroid_vertices, asteroid_vertexIndices);
	asteroid_indexCount = (unsigned int)asteroid_vertexIndices.size();
	////Load planet model
	obj.loadOBJ("sphere.obj", planet_vertices, planet_vertexIndices);
	planet_indexCount = (unsigned int)planet_vertexIndices.size();
	////Load skybox cube model
	obj.loadOBJ("cube.obj", skybox_vertices, skybox_vertexIndices);
	skybox_indexCount = (unsigned int)skybox_vertexIndices.size();
	////Load ship model
	obj.loadOBJ("talon.obj", ship_vertices, ship_vertexIndices);
	ship_indexCount = (unsigned int)ship_vertexIndices.size();

	//number of geomitry to create
	geometry_indexCount = 1;


	D3D11_RASTERIZER_DESC back_raster_desc;
	back_raster_desc.FillMode = D3D11_FILL_SOLID;
	back_raster_desc.CullMode = D3D11_CULL_FRONT;
	back_raster_desc.FrontCounterClockwise = false;
	back_raster_desc.DepthBias = 0;
	back_raster_desc.SlopeScaledDepthBias = 0.0f;
	back_raster_desc.DepthBiasClamp = 0.0f;
	back_raster_desc.DepthClipEnable = true;
	back_raster_desc.ScissorEnable = false;
	back_raster_desc.MultisampleEnable = false;
	back_raster_desc.AntialiasedLineEnable = false;

	D3D11_RASTERIZER_DESC front_raster_desc;
	front_raster_desc.FillMode = D3D11_FILL_SOLID;
	front_raster_desc.CullMode = D3D11_CULL_FRONT;
	front_raster_desc.FrontCounterClockwise = true;
	front_raster_desc.DepthBias = 0;
	front_raster_desc.SlopeScaledDepthBias = 0.0f;
	front_raster_desc.DepthBiasClamp = 0.0f;
	front_raster_desc.DepthClipEnable = true;
	front_raster_desc.ScissorEnable = false;
	front_raster_desc.MultisampleEnable = false;
	front_raster_desc.AntialiasedLineEnable = false;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&back_raster_desc, &back_raster_state);
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&front_raster_desc, &front_raster_state);


	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
	//camera data
	static const XMVECTORF32 eye = { 0.0f, 0.0f, -5.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera,XMMatrixInverse(NULL,  XMMatrixLookAtLH(eye, at, up)));

	//model data
	XMStoreFloat4x4(&m_camera.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	//light data
	XMStoreFloat4(&m_constantlightbufferdata.light_pos, XMLoadFloat4(&XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));
	XMStoreFloat4(&m_constantlightbufferdata.light_dir, XMLoadFloat4(&XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f)));
	XMStoreFloat4(&m_constantlightbufferdata.light_ambient, XMLoadFloat4(&XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
	XMStoreFloat4(&m_constantlightbufferdata.spot_light_pos, XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	XMStoreFloat4(&m_constantlightbufferdata.spot_light_dir, XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)));
	
	//world transforms
	XMStoreFloat4x4(&w_asteroid, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_sun, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_mercury, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_venus, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_earth, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_moon, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_mars, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_jupiter, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_saturn, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_uranus, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_neptune, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_pluto, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_skybox, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_ship, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_geomitry, XMMatrixTranspose(XMMatrixIdentity()));
	XMMATRIX newpos = XMMatrixIdentity();
	newpos.r[3] = XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, -5.5f, 1.0f));
	XMStoreFloat4x4(&w_ship, XMMatrixTranspose(newpos));

	for (size_t i = 0; i < 5; i++)
		XMStoreFloat4x4(&(w_instancedmodel[i]), XMMatrixTranspose(XMMatrixIdentity()));
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	auto device = m_deviceResources->GetD3DDevice();
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	//set Viewport[0]
	viewports[0].Width = outputSize.Width / 2;
	viewports[0].Height = outputSize.Height / 2;
	viewports[0].MinDepth = 0.0f;
	viewports[0].MaxDepth = 1.0f;
	viewports[0].TopLeftX = 0.0f;
	viewports[0].TopLeftY = 0.0f;
	//set Viewport[1]
	viewports[1].Width = outputSize.Width / 2;
	viewports[1].Height = outputSize.Height / 2;
	viewports[1].MinDepth = 0.0f;
	viewports[1].MaxDepth = 1.0f;
	viewports[1].TopLeftX = outputSize.Width / 2 - 5;
	viewports[1].TopLeftY = outputSize.Height / 2 - 5;

	textureDesc.Width = viewports[1].Width;
	textureDesc.Height = viewports[1].Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	device->CreateTexture2D(&textureDesc, nullptr, m_renderTargetTexture.GetAddressOf());

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(m_renderTargetTexture.Get(), &renderTargetViewDesc, m_renderTargetView.GetAddressOf());

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_renderTargetTexture.Get(), &shaderResourceViewDesc, m_shaderResourceView.GetAddressOf());

	CD3D11_TEXTURE2D_DESC1 depthStencilDesc(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		lround(textureDesc.Width),
		lround(textureDesc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);

	DX::ThrowIfFailed(
		device->CreateTexture2D1(
			&depthStencilDesc,
			nullptr,
			m_depthStencil.GetAddressOf()
		)
	);

	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(
		device->CreateDepthStencilView(
			m_depthStencil.Get(),
			&depthStencilViewDesc,
			m_depthStencilView.GetAddressOf()
		)
	);

	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(
		fovAngleY,
		aspectRatio,
		0.01f,
		1000000.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
	XMStoreFloat4x4(
		&m_camera.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
		Rotate(radians);
	}
	XMMATRIX newcamera = XMLoadFloat4x4(&camera);
	XMMATRIX newship = XMLoadFloat4x4(&w_ship);

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * +timer.GetElapsedSeconds() * 5.0;
		newship.r[3] = newship.r[3] + newship.r[2] * +timer.GetElapsedSeconds() * 5.0;
	}

	if (buttons['A'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
		newship.r[3] = newship.r[3] + newship.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (buttons['S'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
		newship.r[3] = newship.r[3] + newship.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (buttons['D'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
		newship.r[3] = newship.r[3] + newship.r[0] * timer.GetElapsedSeconds() * 5.0;
	}
	if (buttons['J'])
	{
		m_constantlightbufferdata.light_dir.x-=0.1f;
	}
	if (buttons['L'])
	{
		m_constantlightbufferdata.light_dir.x+= 0.1f;
	}
	if (buttons['I'])
	{
		m_constantlightbufferdata.light_dir.z-= 0.1f;
	}
	if (buttons['K'])
	{
		m_constantlightbufferdata.light_dir.z+= 0.1f;
	}

	if (buttons['R'])
	{
		XMStoreFloat4(&m_constantlightbufferdata.light_pos, XMLoadFloat4(&XMFLOAT4(0.0f, m_constantlightbufferdata.light_pos.y += 0.1f, 0.0f, 1.0f)));
	}
	if (buttons['F'])
	{
		XMStoreFloat4(&m_constantlightbufferdata.light_pos, XMLoadFloat4(&XMFLOAT4(0.0f, m_constantlightbufferdata.light_pos.y -= 0.1f, 0.0f, 1.0f)));

	}
	if (mouse_move)
	{
		if (left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			newcamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newcamera = XMMatrixRotationX(diffy*0.01f) * newcamera * XMMatrixRotationY(diffx*0.01f);
			newcamera.r[3] = pos;

			XMVECTOR pos2 = newship.r[3];
			newship.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newship = XMMatrixRotationX(diffy*0.01f) * newship * XMMatrixRotationY(diffx*0.01f);
			newship.r[3] = pos2;
		}
	}
	//store camera matrix data
	XMStoreFloat4x4(&camera, newcamera);
	XMStoreFloat4x4(&m_camera.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));

	//store ship matrix data
	XMStoreFloat4x4(&w_ship, XMMatrixTranspose(newcamera));

	//move spotloght with camera
	XMStoreFloat4(&m_constantlightbufferdata.spot_light_pos, XMLoadFloat4(&XMFLOAT4(newcamera.r[3].m128_f32[0], newcamera.r[3].m128_f32[1], newcamera.r[3].m128_f32[2], newcamera.r[3].m128_f32[3])));
	XMStoreFloat4(&m_constantlightbufferdata.spot_light_dir, XMLoadFloat4(&XMFLOAT4(newcamera.r[2].m128_f32[0], newcamera.r[2].m128_f32[1], newcamera.r[2].m128_f32[2], newcamera.r[2].m128_f32[3])));

	mouse_move = false;
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	XMMATRIX orbit = XMMatrixIdentity();
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(2.0f, 0.0f, 0.0f, 1.0f));
	//orbit = XMMatrixRotationY(radians) * orbit;
	//orbit = orbit * XMMatrixRotationY(radians);

	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&w_asteroid, XMMatrixTranspose(orbit));
	//sun
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_sun, XMMatrixTranspose(orbit));
	//mercury

	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(4.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_mercury, XMMatrixTranspose(orbit));
	//venus
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(8.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_venus, XMMatrixTranspose(orbit));
	//earth
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(12.0f, 0.0f, 0.0f, 1.0f));
	//orbit = XMMatrixRotationY(radians) * orbit;
	orbit = orbit * XMMatrixRotationY(radians);
	XMStoreFloat4x4(&w_earth, XMMatrixTranspose(orbit));
	//moon
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(16.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_moon, XMMatrixTranspose(orbit));
	//mars
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(20.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_mars, XMMatrixTranspose(orbit));
	//jupiter
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(24.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_jupiter, XMMatrixTranspose(orbit));
	//saturn
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(28.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_saturn, XMMatrixTranspose(orbit));
	//uranus
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(32.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_uranus, XMMatrixTranspose(orbit));
	//neptune
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(36.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_neptune, XMMatrixTranspose(orbit));
	//pluto
	orbit.r[3] = XMLoadFloat4(&XMFLOAT4(40.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4x4(&w_pluto, XMMatrixTranspose(orbit));

	//instanced matrix data
	for (size_t i = 0; i < 5; i++)
	{
		orbit = XMMatrixIdentity();
		orbit.r[3] = XMLoadFloat4(&XMFLOAT4((float)i + 15.0f, 0.0f, 0.0f, 1.0f));
		orbit = XMMatrixRotationY(radians) * orbit;
		orbit = orbit * XMMatrixRotationY(radians);
		XMStoreFloat4x4(&(w_instancedmodel[i]), XMMatrixTranspose(orbit));
	}
}

// Renders one frame using the vertex and pixel shaders.//comment for instancing within//
void Sample3DSceneRenderer::Render()
{
	RenderToViewPort(0);
	//RenderToViewPort(1);
}

void Sample3DSceneRenderer::setRenderTarget()
{
	m_deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	m_deviceResources->GetD3DDeviceContext()->ClearRenderTargetView(m_renderTargetView.Get(), DirectX::Colors::CornflowerBlue);
	m_deviceResources->GetD3DDeviceContext()->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	Render();

}

void Sample3DSceneRenderer::RenderToViewPort(int vp)
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	//context->RSSetViewports(1, &viewports[vp]);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
#pragma region skybox
	/*
	m_deviceResources->GetD3DDeviceContext()->RSSetState(back_raster_state.Get());
	//set viewport
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_cameraConstBuffer.Get(),
		0,
		NULL,
		&m_camera,
		0,
		0,
		0
	); 
	model.position = w_skybox;
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);

	//asteroid model
	context->IASetVertexBuffers(
		0,
		1,
		m_skyboxVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetIndexBuffer(
		m_skyboxIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_skyboxpixelShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->VSSetConstantBuffers1(
		1,
		1,
		m_modelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//send constant buffer to gpu
	context->PSSetConstantBuffers1(
		0,
		1,
		m_lightConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//texture set to PS
	context->PSSetShaderResources(0, 1, skyboxTex.GetAddressOf());
	// Draw the asteroid.
	context->DrawIndexed(
		skybox_indexCount,
		0,
		0
	);
	m_deviceResources->GetD3DDeviceContext()->RSSetState(front_raster_state.Get());
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	*/
#pragma endregion

#pragma region ship
// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_cameraConstBuffer.Get(),
		0,
		NULL,
		&m_camera,
		0,
		0,
		0
	);


	//w_ship = camera;
	model.position = w_ship;
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_lightConstantBuffer.Get(),
		0,
		NULL,
		&m_constantlightbufferdata,
		0,
		0,
		0
	);

	//asteroid model
	context->IASetVertexBuffers(
		0,
		1,
		m_shipVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetIndexBuffer(
		m_shipIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->VSSetConstantBuffers1(
		1,
		1,
		m_modelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//send constant buffer to gpu
	context->PSSetConstantBuffers1(
		0,
		1,
		m_lightConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//texture set to PS
	context->PSSetShaderResources(1, 1, shipTex.GetAddressOf());
	// Draw the asteroid.
	context->DrawIndexed(
		ship_indexCount,
		0,
		0
	);
#pragma endregion

	/*context->UpdateSubresource1(
		m_cameraConstBuffer.Get(),
		0,
		NULL,
		&m_camera,
		0,
		0,
		0
	);
	model.position = w_asteroid;
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_lightConstantBuffer.Get(),
		0,
		NULL,
		&m_constantlightbufferdata,
		0,
		0,
		0
	);

	//asteroid model
	context->IASetVertexBuffers(
		0,
		1,
		m_asteroidVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetIndexBuffer(
		m_asteroidIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->VSSetConstantBuffers1(
		1,
		1,
		m_modelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//send constant buffer to gpu
	context->PSSetConstantBuffers1(
		0,
		1,
		m_lightConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//texture set to PS
	context->PSSetShaderResources(1, 1, asteroidTex.GetAddressOf());
	// Draw
	context->DrawIndexed(
		asteroid_indexCount,
		0,
		0
	);
	*/

#pragma region planets
	model.position = w_sun;
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);

	//planet model
	context->IASetVertexBuffers(
		0,
		1,
		m_planetVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_planetIndexBuffer.Get(),
		DXGI_FORMAT_R32_UINT,
		0
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->VSSetConstantBuffers1(
		1,
		1,
		m_modelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//send constant buffer to gpu
	context->PSSetConstantBuffers1(
		0,
		1,
		m_lightConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetShaderResources(1, 1, sunTex.GetAddressOf());

	// Draw the objects.
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_mercury;
	context->PSSetShaderResources(1, 1, mercuryTex.GetAddressOf());
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_venus;
	context->PSSetShaderResources(1, 1, venusTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_earth;
	context->PSSetShaderResources(1, 1, earthTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_moon;
	context->PSSetShaderResources(1, 1, moonTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_mars;
	context->PSSetShaderResources(1, 1, marsTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_jupiter;
	context->PSSetShaderResources(1, 1, jupiterTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_saturn;
	context->PSSetShaderResources(1, 1, saturnTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_uranus;
	context->PSSetShaderResources(1, 1, uranusTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_neptune;
	context->PSSetShaderResources(1, 1, neptuneTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);
	model.position = w_pluto;
	context->PSSetShaderResources(1, 1, plutoTex.GetAddressOf());

	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->DrawIndexed(
		planet_indexCount,
		0,
		0
	);

#pragma endregion

#pragma region indexed render
/*
	for (size_t i = 0; i < 5; i++)
		instancemodels.position[i] = w_instancedmodel[i];

	context->UpdateSubresource1(
		m_instancingModelConstBuffer.Get(),
		0,
		NULL,
		&instancemodels,
		0,
		0,
		0
	);
	// Attach our vertex shader.
	context->VSSetShader(
		m_instanceingVertexShader.Get(),
		nullptr,
		0
	);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->VSSetConstantBuffers1(
		1,
		1,
		m_instancingModelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->DrawIndexedInstanced(planet_indexCount, 5, 0, 0, 0);
*/
#pragma endregion

#pragma region geomitry render
	/*
	model.position = w_geomitry;
	context->UpdateSubresource1(
		m_geomitryConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);
	context->IASetVertexBuffers(
		0,
		1,
		m_geomitryVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertextoGeometryShader.Get(),
		nullptr,
		0
	);
	//attach geometry shader
	context->GSSetShader(
		m_geometryShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->GSSetConstantBuffers1(
		0,
		1,
		m_cameraConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->GSSetConstantBuffers1(
		1,
		1,
		m_modelConstBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//send constant buffer to gpu
	context->PSSetConstantBuffers1(
		0,
		1,
		m_lightConstantBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	//texture set to PS
	context->PSSetShaderResources(1, 1, m_shaderResourceView.GetAddressOf());
	// Draw the asteroid.
	context->Draw(
		geometry_indexCount,
		0
	);
	context->GSSetShader(
		nullptr,
		nullptr,
		0
	);
	*/
#pragma endregion
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{


	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadInstanceVSTask = DX::ReadDataAsync(L"InstancingVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	auto loadskyboxPSTask = DX::ReadDataAsync(L"skyboxPixelShader.cso");
	auto loadVtoGSTask = DX::ReadDataAsync(L"VertextoGeometryShader.cso");
	auto loadGSTask = DX::ReadDataAsync(L"GeometryShader.cso");
	
	HRESULT dbug = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"rock.dds", NULL, &asteroidTex);
	HRESULT dbug1 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"sunmap.dds", NULL, &sunTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"mercurymap.dds", NULL, &mercuryTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"venusmap.dds", NULL, &venusTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"earthmap.dds", NULL, &earthTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"moonmap.dds", NULL, &moonTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"marsmap.dds", NULL, &marsTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"jupitermap.dds", NULL, &jupiterTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"saturnmap.dds", NULL, &saturnTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"uranusmap.dds", NULL, &uranusTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"neptunemap.dds", NULL, &neptuneTex);
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"plutomap.dds", NULL, &plutoTex);

	HRESULT dbug2 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"OutputCube.dds", NULL, &skyboxTex);
	HRESULT dbug3 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"talon.dds", NULL, &shipTex);


	auto createskyboxPSTask = loadskyboxPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_skyboxpixelShader
			)
		);
	});

	auto createVStoGSTask = loadVtoGSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertextoGeometryShader
			)
		);
	});
	
	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_geometryShader
			)
		);
	});

	auto createInstancingVSTask = loadInstanceVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_instanceingVertexShader
			)
		);
	});

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {



		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_cameraConstBuffer
			)
		);
		CD3D11_BUFFER_DESC light_constantBufferDesc(sizeof(LightData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&light_constantBufferDesc,
				nullptr,
				&m_lightConstantBuffer
			)
		);
		CD3D11_BUFFER_DESC constantBufferDesc1(sizeof(Models), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc1,
				nullptr,
				&m_modelConstBuffer
			)
		);
		CD3D11_BUFFER_DESC constantBufferDesc2(sizeof(InstanceModels), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc2,
				nullptr,
				&m_instancingModelConstBuffer
			)
		);
		CD3D11_BUFFER_DESC constantBufferDesc3(sizeof(VertexPositionColor), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc3,
				nullptr,
				&m_geomitryConstBuffer
			)
		);
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createInstancingVSTask && createVStoGSTask && createInstancingVSTask && createskyboxPSTask).then([this]() {

		//set buffer for the ship verts and index
		D3D11_SUBRESOURCE_DATA vertexBufferData_ship = { 0 };
		vertexBufferData_ship.pSysMem = ship_vertices.data();
		vertexBufferData_ship.SysMemPitch = 0;
		vertexBufferData_ship.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_ship(sizeof(VertexPositionColor)*ship_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_ship,
				&vertexBufferData_ship,
				&m_shipVertexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA indexBufferData_ship = { 0 };
		indexBufferData_ship.pSysMem = ship_vertexIndices.data();;
		indexBufferData_ship.SysMemPitch = 0;
		indexBufferData_ship.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc_ship(sizeof(unsigned int) * ship_indexCount, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc_ship,
				&indexBufferData_ship,
				&m_shipIndexBuffer
			)
		);

		//set buffer for the asteroid verts and index
		D3D11_SUBRESOURCE_DATA vertexBufferData_asteroid = { 0 };
		vertexBufferData_asteroid.pSysMem = asteroid_vertices.data();
		vertexBufferData_asteroid.SysMemPitch = 0;
		vertexBufferData_asteroid.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_asteroid(sizeof(VertexPositionColor)*asteroid_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_asteroid,
				&vertexBufferData_asteroid,
				&m_asteroidVertexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA indexBufferData_asteroid = { 0 };
		indexBufferData_asteroid.pSysMem = asteroid_vertexIndices.data();;
		indexBufferData_asteroid.SysMemPitch = 0;
		indexBufferData_asteroid.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc_asteroid(sizeof(unsigned int) * asteroid_indexCount, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc_asteroid,
				&indexBufferData_asteroid,
				&m_asteroidIndexBuffer
			)
		);
		//set buffer for the planet verts and index
		D3D11_SUBRESOURCE_DATA vertexBufferData_planet = { 0 };
		vertexBufferData_planet.pSysMem = planet_vertices.data();
		vertexBufferData_planet.SysMemPitch = 0;
		vertexBufferData_planet.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_planet(sizeof(VertexPositionColor)*planet_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_planet,
				&vertexBufferData_planet,
				&m_planetVertexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA indexBufferData_planet = { 0 };
		indexBufferData_planet.pSysMem = planet_vertexIndices.data();
		indexBufferData_planet.SysMemPitch = 0;
		indexBufferData_planet.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc_planet(sizeof(unsigned int) * planet_indexCount, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc_planet,
				&indexBufferData_planet,
				&m_planetIndexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA vertexBufferData_skybox = { 0 };
		vertexBufferData_skybox.pSysMem = skybox_vertices.data();
		vertexBufferData_skybox.SysMemPitch = 0;
		vertexBufferData_skybox.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_skybox(sizeof(VertexPositionColor)*skybox_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_skybox,
				&vertexBufferData_skybox,
				&m_skyboxVertexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA indexBufferData_skybox= { 0 };
		indexBufferData_skybox.pSysMem = skybox_vertexIndices.data();
		indexBufferData_skybox.SysMemPitch = 0;
		indexBufferData_skybox.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc_skybox(sizeof(unsigned int) * skybox_indexCount, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc_skybox,
				&indexBufferData_skybox,
				&m_skyboxIndexBuffer
			)
		);
		//geometry shader vert
		VertexPositionColor g;
		g.pos = XMFLOAT3(0.0f, -1.0f, 0.0f);
		geometry_vertices.push_back(g);
		geometry_indexCount = 1;
		//set buffer for the geometry verts
		D3D11_SUBRESOURCE_DATA vertexBufferData_geomitry = { 0 };
		vertexBufferData_geomitry.pSysMem = geometry_vertices.data();
		vertexBufferData_geomitry.SysMemPitch = 0;
		vertexBufferData_geomitry.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_geometry(sizeof(VertexPositionColor)*geometry_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_geometry,
				&vertexBufferData_geomitry,
				&m_geomitryVertexBuffer
			)
		);
	});
	createCubeTask.then([this]() {
		m_loadingComplete = true;
	});

}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_cameraConstBuffer.Reset();
	m_asteroidVertexBuffer.Reset();
	m_asteroidIndexBuffer.Reset();
	m_planetVertexBuffer.Reset();
	m_planetIndexBuffer.Reset();
	back_raster_state.Reset();
	front_raster_state.Reset();
}