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
		void setRenderTarget();
	private:
		void Rotate(float radians);
		void RenderToViewPort(int vp);
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		//raster state
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> back_raster_state;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> front_raster_state;

		//shaders
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>m_geometryShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertextoGeometryShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_instanceingVertexShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_skyboxpixelShader;


		//constant buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cameraConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_modelConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_instancingModelConstBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_geomitryConstBuffer;


		// System resources to send to GPU
		ModelViewProjectionConstantBuffer	m_camera;
		LightData m_constantlightbufferdata;
		Models model;
		InstanceModels instancemodels;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		XMFLOAT4X4 camera, proj, w_asteroid, w_instancedmodel[5], w_skybox, w_ship, w_geomitry;
		XMFLOAT4X4 w_sun, w_mercury, w_venus, w_earth, w_moon, w_mars, w_jupiter, w_saturn, w_uranus, w_neptune, w_pluto;
		ObjLoader obj;

		//ship Vertices, Indices, and buffers 
		vector<VertexPositionColor> ship_vertices;
		vector<unsigned int> ship_vertexIndices;
		uint32	ship_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_shipVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_shipIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		shipTex;

		//asteroid Vertices, Indices, and buffers 
		vector<VertexPositionColor> asteroid_vertices;
		vector<unsigned int> asteroid_vertexIndices;
		uint32	asteroid_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_asteroidVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_asteroidIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		asteroidTex;

		//planet Vertices, Indices, and buffers 
		vector<VertexPositionColor> planet_vertices;
		vector<unsigned int> planet_vertexIndices;
		uint32	planet_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_planetVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_planetIndexBuffer;
		//planet textures
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		sunTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		mercuryTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		venusTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		earthTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		moonTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		marsTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		jupiterTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		saturnTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		uranusTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		neptuneTex;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		plutoTex;

		//skybox verts, indices, and buffers
		vector<VertexPositionColor> skybox_vertices;
		vector<unsigned int> skybox_vertexIndices;
		uint32	skybox_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_skyboxVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_skyboxIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		skyboxTex;

		//geometry vert data
		vector<VertexPositionColor> geometry_vertices;
		uint32 geometry_indexCount;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_geomitryVertexBuffer;

		//viewports
		D3D11_VIEWPORT viewports[2];

		//render to texture variables
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTargetTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D1> m_depthStencil;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	};
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 