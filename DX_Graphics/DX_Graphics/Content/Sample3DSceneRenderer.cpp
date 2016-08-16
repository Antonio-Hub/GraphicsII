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
	asteroid_indexCount = asteroid_vertexIndices.size();
	////Load sun model
	obj.loadOBJ("sphere.obj", sun_vertices, sun_vertexIndices);
	sun_indexCount = sun_vertexIndices.size();

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

}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
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
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);
	XMStoreFloat4x4(
		&m_camera.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	//static const XMVECTORF32 eye = { 0.0f, 0.7f, 2.5f, 0.0f };
	//static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	//static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	//XMStoreFloat4x4(&camera, XMMatrixLookAtLH(eye, at, up));
	//XMStoreFloat4x4(&m_camera.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));

	XMStoreFloat4x4(&w_asteroid, XMMatrixTranspose(XMMatrixIdentity()));
	XMStoreFloat4x4(&w_sun, XMMatrixTranspose(XMMatrixIdentity()));

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

	if (buttons['W'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * +timer.GetElapsedSeconds() * 5.0;
	}

	if (buttons['A'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}

	if (buttons['S'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}

	if (buttons['D'])
	{
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * timer.GetElapsedSeconds() * 5.0;
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
		}
	}

	XMStoreFloat4x4(&camera, newcamera);
	XMStoreFloat4x4(&m_camera.view, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));
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
	orbit = XMMatrixRotationY(radians) * orbit;
	orbit = orbit * XMMatrixRotationY(radians);
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&w_asteroid, XMMatrixTranspose(orbit));

}


// Renders one frame using the vertex and pixel shaders.//comment for instancing within//
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;

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
	context->PSSetShaderResources(0, 1, asteroidTex.GetAddressOf());
	//context->PSSetShaderResources(0, 1, skyBox.GetAddressOf());

	// Draw the asteroid.
	context->DrawIndexed(
		asteroid_indexCount,
		0,
		0
	);
//////////////////////////////////////////////////////////////////////////////////////////	
	//change buffer matrix to be the suns location
	//XMStoreFloat4x4(&model.position, XMMatrixIdentity());
	model.position = w_sun;
	//put the location in vram of gpu 
	context->UpdateSubresource1(
		m_modelConstBuffer.Get(),
		0,
		NULL,
		&model,
		0,
		0,
		0
	);

	//sun model
	context->IASetVertexBuffers(
		0,
		1,
		m_sunVertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetIndexBuffer(
		m_sunIndexBuffer.Get(),
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
	context->PSSetShaderResources(0, 1, sunTex.GetAddressOf());

	// Draw the objects.
	context->DrawIndexed(
		sun_indexCount,
		0,
		0
	);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{


	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");
	HRESULT dbug = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"rock.dds", NULL, &asteroidTex);
	HRESULT dbug1 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"sunmap.dds", NULL, &sunTex);

	

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
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

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
		//set buffer for the sun verts and index
		D3D11_SUBRESOURCE_DATA vertexBufferData_sun = { 0 };
		vertexBufferData_sun.pSysMem = sun_vertices.data();
		vertexBufferData_sun.SysMemPitch = 0;
		vertexBufferData_sun.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc_sun(sizeof(VertexPositionColor)*sun_indexCount, D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc_sun,
				&vertexBufferData_sun,
				&m_sunVertexBuffer
			)
		);
		D3D11_SUBRESOURCE_DATA indexBufferData_sun = { 0 };
		indexBufferData_sun.pSysMem = sun_vertexIndices.data();
		indexBufferData_sun.SysMemPitch = 0;
		indexBufferData_sun.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc_sun(sizeof(unsigned int) * sun_indexCount, D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc_sun,
				&indexBufferData_sun,
				&m_sunIndexBuffer
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
	m_sunVertexBuffer.Reset();
	m_sunIndexBuffer.Reset();
}