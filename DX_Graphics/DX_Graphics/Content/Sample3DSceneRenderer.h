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
		
		//vetex and index buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;

		//shaders
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;


		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cameraConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_modelConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightConstantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_camera;
		uint32	m_indexCount;
		//light resources
		LightData m_constantlightbufferdata;
		//model matrex
		Models model;
		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		XMFLOAT4X4 world, camera, proj;

		//model verts, uvs, and normals
		ObjLoader obj;
		vector<VertexPositionColor> obj_vertices;
		vector<unsigned int> obj_vertexIndices;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		asteroidTex;

	};
}

