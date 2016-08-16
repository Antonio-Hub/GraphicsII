#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <vector>
#include <DirectXMath.h>
#include <ObjLoader.h>
#include "Content\ShaderStructures.h"
#include "DDSTextureLoader.h"
using namespace DirectX;
using namespace std;
namespace DX_Graphics
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
	private:
		void Rotate(float radians);
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		
		//shaders
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;

		//constant buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cameraConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_modelConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightConstantBuffer;

		// System resources to send to GPU
		ModelViewProjectionConstantBuffer	m_camera;
		LightData m_constantlightbufferdata;
		Models model;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		XMFLOAT4X4 world, camera, proj, w_asteroid, w_sun;

		ObjLoader obj;

		//asteroid Vertices, Indices, and buffers 
		vector<VertexPositionColor> asteroid_vertices;
		vector<unsigned int> asteroid_vertexIndices;
		uint32	asteroid_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_asteroidVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_asteroidIndexBuffer;

		vector<VertexPositionColor> sun_vertices;
		vector<unsigned int> sun_vertexIndices;
		uint32	sun_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_sunVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_sunIndexBuffer;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		asteroidTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		skyBox;

		
	};
}

