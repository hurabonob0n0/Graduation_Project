#pragma once
#include "Engine_Macro.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include "MathHelper.h"
#include <assimp/material.h>


#define MaxLights 16

using namespace DirectX;

static const int gNumFrameResources = 3;

namespace Engine
{
	struct ENGINE_DLL ObjectConstants
	{
		XMFLOAT4X4 World = MathHelper::Identity4x4();
		XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT     MaterialIndex;
		UINT     ObjPad0;
		UINT     ObjPad1;
		UINT     ObjPad2;
	};

	struct ENGINE_DLL Light
	{
		DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
		float FalloffStart = 1.0f;                          // point/spot light only
		DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
		float FalloffEnd = 10.0f;                           // point/spot light only
		DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
		float SpotPower = 64.0f;                            // spot light only
	};

	struct ENGINE_DLL PassConstants
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float cbPerObjectPad1 = 0.0f;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;

		DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

		// Indices [0, NUM_DIR_LIGHTS) are directional lights;
		// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
		// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
		// are spot lights for a maximum of MaxLights per object.
		Light Lights[MaxLights];
	};


	struct ENGINE_DLL MaterialConstants
	{
		// �� ���͸��� �ش�Ǵ� ��� ������ �ε����Դϴ�.
		int MatCBIndex;

		// ��ǻ�� �ؽ��Ŀ� �ش��ϴ� SRV ���� �ε����Դϴ�.
		int DiffuseSrvHeapIndex = -1;

		// �븻 �ؽ��Ŀ� �ش��ϴ� SRV ���� �ε����Դϴ�.
		int NormalSrvHeapIndex = -1;

		// ���͸����� ����Ǿ��ٴ� ���� ��Ÿ���� ��Ƽ �÷����Դϴ�.
		// �׸��� ������� ��� ��� ���۸� ������Ʈ �ؾ��մϴ�.
		// ���͸��� ��� ���۴� �� �����Ӹ��� �����ϱ� ������ ��� ������ ���ҽ���
		// ������Ʈ �ؾ��մϴ�. �׷��Ƿ� ���͸����� ������� �� NumFramesDirty = gNumFrameResources��
		// �����ؼ� ��� ������ ���ҽ��� ������Ʈ �ǵ��� �ؾ��մϴ�.
		int NumFramesDirty = gNumFrameResources;

		// ���̵��� ���Ǵ� ���͸��� ��� ���� �������Դϴ�.
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
	};

	struct ENGINE_DLL MaterialData
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.5f;

		// Used in texture mapping.
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

		UINT DiffuseMapIndex = 0;
		UINT NormalMapIndex = 0;
		UINT MaterialPad1;
		UINT MaterialPad2;
	};
}

BEGIN(Engine)
typedef struct ENGINE_DLL tagModelMaterial
	{
		class CTexture* pMaterials[AI_TEXTURE_TYPE_MAX];
	}MODEL_MATERIAL;
END

namespace Engine
{
	typedef struct ENGINE_DLL tagVertex_Position
	{
		XMFLOAT3		vPosition;

		static const unsigned int iNumElements = 1;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXPOS;

	typedef struct ENGINE_DLL tagVertex_Position_Normal
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;

		static const unsigned int iNumElements = 2;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXPOSNOR;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = 2;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int iNumElements = 3;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Cube_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexcoord;

		static const unsigned int iNumElements = 2;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int iNumElements = 4;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXMESH;

	typedef struct ENGINE_DLL tagVertex_Anim_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int iNumElements = 6;
		static const D3D12_INPUT_ELEMENT_DESC	VertexElements[iNumElements];

	}VTXANIMMESH;

}