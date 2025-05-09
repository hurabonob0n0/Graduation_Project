#pragma once
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <assimp/material.h>
//#include <DirectXTK\WICTextureLoader.h>
//#include <DirectXTK\ScreenGrab.h>
//#include <DirectXTK\SpriteBatch.h> 
//#include <DirectXTK\SpriteFont.h>

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"


#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <Windows.h>
#include <wrl.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dUtil.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "UploadBuffer.hpp"
#include "stb_image.h"
#include "DDSTextureLoader.h"

#define MaxLights 16

// 필요한 d3d12 라이브러리 링크
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

BEGIN(Engine)
	enum MOUSEKEYSTATE { LBTN, RBTN, MBTN, MKS_END };
	enum MOUSEMOVESTATE { X, Y, Z };
END



//static const int gNumFrameResources = 3;

using namespace std;
using namespace Engine;
using namespace DirectX;
using namespace Microsoft::WRL;