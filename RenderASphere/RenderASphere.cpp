#include "stdafx.h"
#include "RenderASphere.h"
#include <math.h>
#include <fstream>
#include <vector>

using namespace std;
#define uint unsigned int
#define uchar unsigned char
const float PI = float(3.1415926);

HANDLE hConsol;
void  cout(float f)
{
	char c[4];
	_ltoa_s(f, c, 4);
	WriteFile(hConsol, c, 4, nullptr, 0);
}
namespace library
{
	template<typename T>
	inline static T clamp(T t, T t1, T t2)
	{
		if (t<t1) { return t1; }
		if (t>t2) { return t2; }
		return t;
	}

	template<typename T>
	inline static void exchange(T &t1, T &t2)
	{
		T t = *t1;
		*t1 = t2;
		t2 = t;
	}
}

class vec2uint
{
public:
	uint x, y;

	vec2uint()
	{
		x = 0;
		y = 0;
	}
	vec2uint(uint u1, uint u2)
	{
		x = u1;
		y = u2;
	}
	inline vec2uint operator+(vec2uint v)
	{
		return{x+v.x , y+v.y};
	}
};
class vec3uint
{
public:
	uint x, y, z;

	vec3uint()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	vec3uint(uint u)
	{
		x = u;
		y = u;
		z = u;
	}
	vec3uint(uint u1, uint u2, uint u3)
	{
		x = u1;
		y = u2;
		z = u3;
	}
	inline uint operator[](uint u)
	{
		switch (u)
		{
			case 0:return x;
			case 1:return y;
			case 2:return z;
		}
	}
	inline vec3uint operator-(vec3uint v)
	{
		return{x-v.x, y-v.y, z-v.z};
	}
};
class vec2
{
public:
	float x, y;

	vec2()
	{
		x = 0;
		y = 0;
	}
	vec2(float f)
	{
		x = f;
		y = f;
	}
	vec2(float f1, float f2)
	{
		x = f1;
		y = f2;
	}
	inline vec2 operator+(vec2 v)
	{
		return{x+v.x, y+v.y};
	}
	inline vec2 operator-(vec2 v)
	{
		return{x-v.x, y-v.y};
	}
	inline vec2 operator*(vec2 v)
	{
		return{x*v.x, y*v.y};
	}
	inline vec2 operator/(vec2 v)
	{
		return{x/v.x, y/v.y};
	}
};

class matrix4	
{
public:
	float table[4][4];

	enum eTransType
	{
		LocalToObject, LocalToWorld, LocalToRenderTarget,
		ObjectToWorld, ObjectToRenderTarget,
		WorldToRenderTarget,
		WorldToTangent
	};
	matrix4()
	{
		memset(table, 0, 64);
	}
	matrix4(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12, float f13, float f14, float f15)
	{
		table[0][0] = f0; table[0][1] = f1; table[0][2] = f2; table[0][3] = f3;
		table[1][0] = f4; table[1][1] = f5; table[1][2] = f6; table[1][3] = f7;
		table[2][0] = f8; table[2][1] = f9; table[2][2] = f10; table[2][3] = f11;
		table[3][0] = f12; table[3][1] = f13; table[3][2] = f14; table[3][3] = f15;
	}
	inline matrix4 operator*(matrix4 m)
	{
		matrix4 ret;
		for (uchar row = 0; row<4; row++)
		{
			for (uchar col = 0; col<4; col++)
			{
				for (uchar u = 0; u<4; u++)
				{
					ret.table[row][col] += table[row][u]*m.table[u][col];
				}
			}
		}
		return ret;
	}
	inline static matrix4 unit()
	{
		return matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	inline static matrix4 scalor(float sx, float sy, float sz)
	{
		return matrix4(
			sx, 0, 0, 0,
			0, sy, 0, 0,
			0, 0, sz, 0,
			0, 0, 0, 1);
	}
	inline static matrix4 rotator(float rx, float ry, float rz)
	{
		return matrix4(
			1, 0, 0, 0,
			0, cos(rx), -sin(rx), 0,
			0, sin(rx), cos(rx), 0,
			0, 0, 0, 1)
			*matrix4(
			cos(ry), 0, sin(ry), 0,
			1, 0, 0, 0,
			-sin(ry), 0, cos(ry), 0,
			0, 0, 0, 1)
			*matrix4(
			cos(ry), -sin(ry), 0, 0,
			sin(ry), cos(ry), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	inline static matrix4 translator(float tx, float ty, float tz)
	{
		return matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			tx, ty, tz, 1);
	}
	inline static matrix4 transfomer(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz)
	{
		return matrix4::scalor(sx, sy, sz)*matrix4::rotator(rx, ry, rz)*matrix4::translator(tx, ty, tz);
	}
};
class vec3
{
public:
	float x, y, z;

	vec3()
	{

		x = 0;
		y = 0;
		z = 0;
	}
	vec3(float f)
	{
		x = f;
		y = f;
		z = f;
	}
	vec3(float f1, float f2, float f3)
	{
		x = f1;
		y = f2;
		z = f3;
	}
	inline vec3 operator+(vec3 v)
	{
		return{x+v.x, y+v.y, +v.z};
	}
	inline vec3 operator-()
	{
		return{-x, -y, -z};
	}
	inline vec3 operator*(vec3 v)
	{
		return{x*v.x, y*v.y, z*v.z};
	}
	inline float dot(vec3 v)
	{
		return x*v.x+y*v.y+z*v.z;
	}
	inline float dot(float f0, float f1, float f2, float f3)
	{
		return x*f0+y*f1+z*f2+f3;
	}
	inline vec3 transform(matrix4 trans_matrix)
	{
		return{
			dot(trans_matrix.table[0][0], trans_matrix.table[1][0], trans_matrix.table[2][0], trans_matrix.table[3][0]),
			dot(trans_matrix.table[0][1], trans_matrix.table[1][1], trans_matrix.table[2][1], trans_matrix.table[3][1]),
			dot(trans_matrix.table[0][2], trans_matrix.table[1][2], trans_matrix.table[2][2], trans_matrix.table[3][2])};
	}
};

class Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	inline static Color White()
	{
		return{255,255,255,255};
	}
};
struct Fragment
{
public:
	vec3 Pos;
	vec2 UV;
	vec3 Nor;
	float tan_up;
	float tan_down;
	uint LightID = 0;

	enum ePos { top, right, bot, left };
	enum eDir { down, up };
	void SortFragment(Fragment frag[4])
	{
		if (frag[bot].Pos.y>frag[right].Pos.y)
		{
			exchange(frag[bot], frag[right]);
		}
		if (frag[right].Pos.y>frag[top].Pos.y)
		{
			exchange(frag[right], frag[top]);
		}
		if (frag[bot].Pos.y>frag[right].Pos.y)
		{
			exchange(frag[bot], frag[right]);
		}
		frag[right].tan_up = (frag[top].Pos.y-frag[right].Pos.y)/(frag[top].Pos.x-frag[right].Pos.x);
		frag[right].tan_down = (frag[right].Pos.y-frag[bot].Pos.y)/(frag[right].Pos.x-frag[bot].Pos.x);
		frag[left].tan_up =
			frag[left].tan_down = (frag[top].Pos.y-frag[bot].Pos.y)/(frag[top].Pos.x-frag[bot].Pos.x);
		float lerp = (frag[top].Pos.y-frag[right].Pos.y)/(frag[top].Pos.y-frag[bot].Pos.y);
		frag[left].Pos.x = frag[bot].Pos.x+(frag[right].Pos.y-frag[bot].Pos.y)/frag[left].tan_down;
		frag[left].Pos.y = frag[right].Pos.y;
		frag[left].Pos.z = frag[bot].Pos.z*lerp+frag[top].Pos.z*(right-lerp);
		frag[left].UV = frag[bot].UV*lerp+frag[top].UV*(1-lerp);
		frag[left].Nor = frag[bot].Nor*lerp+frag[top].Nor*(1-lerp);
		if (frag[left].Pos.x>frag[right].Pos.x)
		{
			exchange(frag[left], frag[right]);
		}
	}
};
struct Face
{
public:
	vec3uint PosIndex;
	vec3uint NorIndex;
	vec3uint UVIndex;
};
class Element
{
public:
	uint FaceCount = 0;
	uchar ShaderID = 0;
	bool bSmoothShading = 0;
};

class RenderTarget
{
public:
	enum eType { Camera, Light };
	eType Type = Camera;
	vec3 Position;
	vec3 Rotation;
	vec2 Size = {1.154f,1.154f};
	vec2 HalfSize = {0.577f,0.577f};
	float PixelSize=0.002255f;
	float Depth=-100;
	vec2uint Resolution = {512,512};
	float FOV = PI/3;

	bool bPerspective = 1;
	bool bHasZBuffer=1;
	bool bHasPOSBuffer = 0;
	bool bHasUVBuffer = 1;
	bool bHasNormalBuffer = 1;
	bool bHasShaderIDBuffer = 1;
	bool bHasLightBitIDBuffer = 0;
	bool bHasPresentBuffer = 1;

	float* ZBuffer =nullptr;
	vec3* POSBuffer = nullptr;
	vec2* UVBuffer = nullptr;
	vec3* NormalBuffer = nullptr;
	uchar* ShaderIDBuffer = nullptr;
	uint* LightBitIDBuffer = nullptr;
	Color* PresentBuffer = nullptr;

	RenderTarget()
	{
		SetResolution({512,512});
	}
	void SetFOV(float fov)
	{
		if (!bPerspective) { return; }
		if (fov>=PI) { return; }
		FOV = fov;
		HalfSize.x = 1/tan((PI-fov)/2);
		HalfSize.y = HalfSize.x*Resolution.y/Resolution.x;
		Size = HalfSize*2;
		PixelSize = Size.x/Resolution.x;
	}
	void SetSize(vec2 size)
	{
		if (bPerspective) { return; }
		Size = size;
		HalfSize = size/2;
		PixelSize = Size.x/Resolution.x;
	}
	void SetResolution(vec2uint res)
	{
		Resolution = res;
		if (bPerspective)
		{
			HalfSize.x = 1/tan((PI-FOV)/2);
			HalfSize.y = HalfSize.x*Resolution.y/Resolution.x;
			Size = HalfSize*2;
		}
		PixelSize = Size.x/Resolution.x;

		if (bHasZBuffer) 
		{
			if (ZBuffer){delete ZBuffer;}
			ZBuffer = new float[Resolution.x*Resolution.y];
			InitBuffer(ZBuffer, Depth);
		}
		if (bHasUVBuffer)
		{
			if (UVBuffer) { delete UVBuffer; }
			UVBuffer = new vec2[Resolution.x*Resolution.y];
			InitBuffer(UVBuffer, {0,0});
		}
		if (bHasNormalBuffer)
		{
			if (NormalBuffer) { delete NormalBuffer; }
			NormalBuffer = new vec3[Resolution.x*Resolution.y];
			InitBuffer(NormalBuffer, {0,0,1});
		}
		if (bHasShaderIDBuffer)
		{
			if (ShaderIDBuffer) { delete ShaderIDBuffer; }
			ShaderIDBuffer = new uchar[Resolution.x*Resolution.y];
			InitBuffer(ShaderIDBuffer, uchar(0));
		}
		if (bHasLightBitIDBuffer)
		{
			if (LightBitIDBuffer) { delete LightBitIDBuffer; }
			LightBitIDBuffer = new uint[Resolution.x*Resolution.y];
			InitBuffer(LightBitIDBuffer, uint(0));
		}
		if (bHasPresentBuffer)
		{
			if (PresentBuffer) { delete PresentBuffer; }
			PresentBuffer = new Color[Resolution.x*Resolution.y];
			InitBuffer(PresentBuffer, {0,255,0});
		}
	}
	void ClearTarget()
	{
		if (bHasZBuffer) { InitBuffer(ZBuffer, Depth); }
		if (bHasUVBuffer) { InitBuffer(UVBuffer, {0,0}); }
		if (bHasNormalBuffer) { InitBuffer(NormalBuffer, {0,0,1}); }
		if (bHasShaderIDBuffer) { InitBuffer(ShaderIDBuffer, uchar(0)); }
		if (bHasLightBitIDBuffer) { InitBuffer(LightBitIDBuffer, uint(0)); }
		if (PresentBuffer) { InitBuffer(PresentBuffer, {0,0,255}); }
	}
	inline bool DepthTest(uint pos_offset, float z)
	{
		if (z<0&&z >Depth)
		{
			float* f = ZBuffer+pos_offset;
			if (z<*f)
			{
				*f = z;
				return true;
			}
			else { return false; }
		}
		else { return false; }
	}

	template<typename t>
	void InitBuffer(t* buffer, t val)
	{
		if (buffer)
		{
			for (uint i = 0; i<Resolution.x*Resolution.y; i++)
			{
				*(buffer+i) = val;
			}
		}
	}
	template<typename t>
	void WriteToBuffer(t* buffer, uint pos_offset, t val)
	{
		*(buffer+pos_offset) = val;
	}
	void Release()
	{
		delete ZBuffer,
			UVBuffer,
			NormalBuffer,
			ShaderIDBuffer,
			LightBitIDBuffer,
			PresentBuffer,
			PresentBuffer;
		ZBuffer = nullptr;
		UVBuffer = nullptr;
		NormalBuffer = nullptr;
		ShaderIDBuffer = nullptr;
		LightBitIDBuffer = nullptr;
		PresentBuffer = nullptr;
		PresentBuffer = nullptr;
	}
};
class Texture
{
public:
	uchar ID;
	Color* ImageBuffer;//Cimg store rgb differently
	vec2uint Resolution;
	Color TextureLookup(vec2 uv) {}
};
class ShaderParameter
{
public:
	RenderTarget* current_target;
	vector<RenderTarget*> all_target;
	vec3 pos;
	vec2 uv;
	vec3 nor;
	matrix4* trans_matrix;
};
class Shader
{
public:
	vector<Texture> Tetures;

	virtual vec3 VertexShader(ShaderParameter para)
	{
		return para.pos.transform(para.trans_matrix[matrix4::eTransType::LocalToRenderTarget]);
	}
	virtual Color PixelShader(ShaderParameter para)
	{
		return{(uchar)(para.nor.x*255),(uchar)(para.nor.y*255),(uchar)(para.nor.z*255),255};
	}
};
class Shader_Default :public Shader
{
public:
	virtual vec3 VertexShader(ShaderParameter para) override
	{
		return Shader::VertexShader(para);
	}
	virtual Color PixelShader(ShaderParameter para) override
	{
		return Shader::PixelShader(para);
	}
};

class Mesh
{
public:
	vec3* PosBuffer = nullptr;
	uint PosCount = 0;
	vec3* NormalBuffer = nullptr;
	uint NormalCount = 0;
	vec2* UVBuffer = nullptr;
	uint UVCount = 0;
	vec2uint* EdgeBuffer = nullptr;
	uint EdgeCount = 0;
	Face* FaceBuffer = nullptr;
	uint FaceCount = 0;
	Element* ElementBuffer = nullptr;
	uint ElementCount;

	bool bPosCached = 0;
	vec3* CachedPosBuffer = nullptr;
	void Release()
	{
		delete PosBuffer;
		delete 	NormalBuffer;
		delete 	UVBuffer;
		delete 	EdgeBuffer;
		delete 	FaceBuffer;
		delete 	ElementBuffer;
		delete CachedPosBuffer;
		PosBuffer = nullptr;
		NormalBuffer = nullptr;
		UVBuffer = nullptr;
		EdgeBuffer = nullptr;
		FaceBuffer = nullptr;
		ElementBuffer = nullptr;
		CachedPosBuffer = nullptr;
	}
};
class MeshComponent
{
public:
	Mesh mesh;

	uchar ShaderID;
	bool bVisibility = 1;
	vec3 Position = {0,0,5};
	vec3 Rotation = {0,0,0};
	vec3 Scale = {1,1,1};

	void LoadMesh(string path)
	{
		ifstream mf;
		mf.open(path);
		char c1;
		char c2;
		while (!mf.eof())
		{
			c1 = mf.get();
			c2 = mf.get();
			if (c1=='v'&&c2==' ')
			{
				mesh.PosCount++;
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='v'&&c2=='t')
			{
				mesh.UVCount++;
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='v'&&c2=='n')
			{
				mesh.NormalCount++;
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='f')
			{
				mesh.FaceCount++;
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='u')
			{
				mesh.ElementCount++;
				mf.ignore(unsigned(-1), '\n');
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='s')
			{
				mesh.ElementCount++;
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
		}

		vec3* pos = new vec3[mesh.PosCount];
		vec2* uv = new vec2[mesh.UVCount];
		vec3* nor = new vec3[mesh.NormalCount];
		Face* face = new Face[mesh.FaceCount];
		Element* element = new Element[mesh.ElementCount];
		vec3* cpos = new vec3[mesh.PosCount];

		mf.clear(ios::eofbit);
		mf.seekg(0, ios::beg);
		for (uint i = 0; i<mesh.PosCount; i++)
		{
			while (mf.get()!='v') { mf.ignore(unsigned(-1), '\n'); }
			mf>>(pos+i)->x>>(pos+i)->y>>(pos+i)->z;
			mf.ignore(unsigned(-1), '\n');
		}
		for (uint i = 0; i<mesh.UVCount; i++)
		{
			while (mf.get()!='v') { mf.ignore(unsigned(-1), '\n'); }
			mf.get();
			mf>>(uv+i)->x>>(uv+i)->y;
			mf.ignore(unsigned(-1), '\n');
		}
		for (uint i = 0; i<mesh.NormalCount; i++)
		{
			while (mf.get()!='v') { mf.ignore(unsigned(-1), '\n'); }
			mf.get();
			mf>>(nor+i)->x>>(nor+i)->y>>(nor+i)->z;
			mf.ignore(unsigned(-1), '\n');
		}
		bool first_element = true;
		for (uint i = 0; i<mesh.FaceCount; i++)
		{
			c1 = mf.get();
			if (c1=='u')
			{
				i--;
				element = first_element ? element : element++;
				first_element = false;
				mf.ignore(unsigned(-1), ' ');
				int a;
				mf>>a;
				element->ShaderID=(uchar)a;
				mf.ignore(unsigned(-1), ' ');
				if (mf.get()=='o') { element->bSmoothShading = false; }
				else { element->bSmoothShading = true; }
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='s')
			{
				i--;
				uchar u = element->ShaderID;
				element++;
				element->ShaderID = u;
				if (mf.get()=='o') { element->bSmoothShading = false; }
				else { element->bSmoothShading = true; }
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='f')
			{
				element->FaceCount++;
				mf>>(face+i)->PosIndex.x>>c1>>(face+i)->UVIndex.x>>c1>>(face+i)->NorIndex.x;
				mf>>(face+i)->PosIndex.y>>c1>>(face+i)->UVIndex.y>>c1>>(face+i)->NorIndex.y;
				mf>>(face+i)->PosIndex.z>>c1>>(face+i)->UVIndex.z>>c1>>(face+i)->NorIndex.z;
				(face+i)->PosIndex = (face+i)->PosIndex-vec3uint(1);
				(face+i)->UVIndex = (face+i)->UVIndex-vec3uint(1);
				(face+i)->NorIndex = (face+i)->NorIndex-vec3uint(1);
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			mf.ignore(unsigned(-1), '\n');
		}
		mesh.PosBuffer = pos;
		mesh.UVBuffer = uv;
		mesh.NormalBuffer = nor;
		mesh.FaceBuffer = face;
		mesh.ElementBuffer = element;
		mesh.CachedPosBuffer = cpos;
	}
	void SetMaterial(uchar id)
	{
		ShaderID = id;
	}
	inline void Release()
	{
		mesh.Release();
	}
};
class Object
{
public:
	bool bVisibility = 1;
	vec3 Position = {0,0,0};
	vec3 Rotation = {0,0,0};
	vec3 Scale = {1,1,1};
	vector<MeshComponent> Components;

	void AddComponentMesh(string path)
	{
		MeshComponent Component;
		Component.LoadMesh(path);
		Components.push_back(Component);
	}
	void AddWorldPosition(vec3 pos)
	{
		Position = Position+pos;
	}
	void AddLocalRotation(vec3 rot)
	{
		Rotation = Rotation+rot;
	}
	inline void Release()
	{
		for (vector<MeshComponent>::iterator component = Components.begin(); component!=Components.end(); component++)
		{
			component->Release();
		}
	}
};
class Light
{
public:
	enum eType { DirectionalLight, PointLight, SpotLight };
	eType Type = DirectionalLight;
	uint BitID = 0;
	bool bVisibility = 1;
	bool bCastShadow = 1;
	Color Color = {255,255,255};
	float Intensity = 1;
	float Radius = 10;
	float FallOff = 0.5;
	vec2 ShadowResolution;

	RenderTarget* RenderProxy;

	inline void SetType(Light::eType light_type)
	{
		Type = light_type;
	}
	inline void Release()
	{
		RenderProxy->Release();
		delete RenderProxy;
		RenderProxy = nullptr;
	}
};
class Camera
{
public:
	RenderTarget* RenderProxy;

	inline void SetResolution(vec2uint res)
	{
		RenderProxy->SetResolution(res);
	}
	inline void Release()
	{
		RenderProxy->Release();
		delete RenderProxy;
		RenderProxy = nullptr;
	}

};
class Scene
{
public:
	vector<Object> Objects;
	vector<Light> Lights;
	vector<Camera> Cameras;

	inline void AddObject(Object object)
	{
		Objects.push_back(object);
	}
	inline void AddCamera(vec3 pos,vec3 rot,vec2uint res)//快捷函数，快速加一个摄像机到场景里
	{
		Camera camera;
		camera.RenderProxy = new RenderTarget;
		camera.RenderProxy->Position = pos;
		camera.RenderProxy->Rotation = rot;
		camera.RenderProxy->SetResolution(res);
		Cameras.push_back(camera);
	}
	inline void Release()
	{
		for (vector<Object>::iterator object = Objects.begin(); object!=Objects.end(); object++)
		{
			object->Release();
		}
		for (vector<Light>::iterator light = Lights.begin(); light!=Lights.end(); light++)
		{
			light->Release();
		}
		for (vector<Camera>::iterator camera = Cameras.begin(); camera!=Cameras.end(); camera++)
		{
			camera->Release();
		}
	}
};

class Renderer
{
public:
	Scene* pScene;
	vector<RenderTarget*> RenderTargets;
	RenderTarget* ActiveTarget;
	vector<Shader*> Shaders;

	void BindShaders(vector<Shader*> shaders)
	{
		Shaders = shaders;
	}
	void AddScene(Scene* scene)
	{
		pScene = scene;
		for (vector<Light>::iterator light = pScene->Lights.begin(); light!=pScene->Lights.end(); light++)
		{
			RenderTargets.push_back(light->RenderProxy);
		}
		for (vector<Camera>::iterator camera = pScene->Cameras.begin(); camera!=pScene->Cameras.end(); camera++)
		{
			RenderTargets.push_back(camera->RenderProxy);
			ActiveTarget = camera->RenderProxy;
		}
	}
	inline void Release()
	{
		pScene->Release();
		delete pScene;
		pScene = nullptr;
		for (vector<Shader*>::iterator shader = Shaders.begin(); shader!=Shaders.end(); shader++)
		{
			delete *shader;
			*shader = nullptr;
		}
	}

	void InitRenderTarget()
	{
		for (vector<RenderTarget*>::iterator rt = RenderTargets.begin(); rt!=RenderTargets.end(); rt++)
		{
			(*rt)->ClearTarget(); 
		}
	}
	void VertexShading()
	{//误清内存
		matrix4 trans_matrix[6];
		for (vector<RenderTarget*>::iterator rt = RenderTargets.begin(); rt!=RenderTargets.end(); rt++)
		{
			trans_matrix[matrix4::WorldToRenderTarget] = matrix4::transfomer(1, 1, 1, -(*rt)->Rotation.x, -(*rt)->Rotation.y, -(*rt)->Rotation.z, -(*rt)->Position.x, -(*rt)->Position.y, -(*rt)->Position.z);
			for (vector<Object>::iterator obj = pScene->Objects.begin(); obj!=pScene->Objects.end(); obj++)
			{
				trans_matrix[matrix4::ObjectToWorld] = matrix4::transfomer(obj->Scale.x, obj->Scale.y, obj->Scale.z, obj->Rotation.x, obj->Rotation.y, obj->Rotation.z, obj->Position.x, obj->Position.y, obj->Position.z);
				trans_matrix[matrix4::ObjectToRenderTarget] = trans_matrix[matrix4::ObjectToWorld]*trans_matrix[matrix4::WorldToRenderTarget];
				for (vector<MeshComponent>::iterator com = obj->Components.begin(); com!=obj->Components.end(); com++)
				{
					trans_matrix[matrix4::LocalToObject] = matrix4::transfomer(com->Scale.x, com->Scale.y, com->Scale.z, com->Rotation.x, com->Rotation.y, com->Rotation.z, com->Position.x, com->Position.y, com->Position.z);
					trans_matrix[matrix4::LocalToWorld] = trans_matrix[matrix4::LocalToObject]*trans_matrix[matrix4::ObjectToWorld];
					trans_matrix[matrix4::LocalToRenderTarget] = trans_matrix[matrix4::LocalToWorld]*trans_matrix[matrix4::WorldToRenderTarget];
					vec3* pos = com->mesh.PosBuffer;
					vec2* uv = com->mesh.UVBuffer;
					vec3* nor = com->mesh.NormalBuffer;
					Face* face = com->mesh.FaceBuffer;
					Element* element = com->mesh.ElementBuffer;
					for (uint ele=0; ele<com->mesh.ElementCount;  ele++,element++)
					{
						for (uint fa=0; fa<element->FaceCount; fa++,face++)
						{
							Fragment frag[4];
							for (uint i = 0; i<3; i++)
							{
								frag[i].Pos = *(pos+face->PosIndex[i]);
								frag[i].UV = *(uv+face->UVIndex[i]);
								frag[i].Nor = *(nor+face->NorIndex[i]); 
								frag[i].Pos = Shaders[uint(element->ShaderID)]->VertexShader({*rt,RenderTargets,frag[i].Pos,frag[i].UV,frag[i].Nor,trans_matrix});
							}
							frag->SortFragment(frag);
							Rasterization(*rt, element->ShaderID, frag[Fragment::ePos::left], frag[Fragment::ePos::right], frag[Fragment::ePos::top], Fragment::eDir::up);
							Rasterization(*rt, element->ShaderID, frag[Fragment::ePos::left], frag[Fragment::ePos::right], frag[Fragment::ePos::bot], Fragment::eDir::down);
						}
					}
				}
			}
		}
	}
	void Rasterization(RenderTarget* rt,uchar shader_id, Fragment left, Fragment right, Fragment mid, const bool is_upward)
	{
		float y_start = library::clamp(left.Pos.y, -rt->HalfSize.y, rt->HalfSize.y);
		float y_end = library::clamp(mid.Pos.y, -rt->HalfSize.y, rt->HalfSize.y);
		for (float y = y_start; (is_upward ? y<y_end : y>y_end); y += (is_upward ? rt->PixelSize : -rt->PixelSize))
		{
			float x_start = left.Pos.x+(y-left.Pos.y)/(is_upward ? left.tan_up : left.tan_down);
			float x_end = right.Pos.x+(y-left.Pos.y)/(is_upward ? right.tan_up : right.tan_down);
			x_start = library::clamp(x_start, -rt->HalfSize.x, rt->HalfSize.x);
			x_end = library::clamp(x_end, -rt->HalfSize.x, rt->HalfSize.x);
			for (float x = x_start; x < x_end; x += rt->PixelSize)
			{
				uint w = (x/rt->Size.x+0.5f)*rt->Resolution.x+0.5f;
				uint h = (y/rt->Size.y+0.5f)*rt->Resolution.y+0.5f;
				uint offset = rt->Resolution.x*h+w;

				float lerp2 = (mid.Pos.y-y)/(mid.Pos.y-left.Pos.y);
				float lerp1 = (mid.Pos.x-left.Pos.x-(mid.Pos.x-x)/lerp2)/(right.Pos.x-left.Pos.x);

				if (rt->bHasZBuffer)
				{
					float z = (right.Pos.z*lerp1+left.Pos.z*(1-lerp1))*lerp2+mid.Pos.z*(1-lerp2);
					if (!rt->DepthTest(offset, z)) { continue; }
				}
				if (rt->bHasUVBuffer)
				{
					vec2 uv = right.UV*lerp1;
					rt->WriteToBuffer(rt->UVBuffer, offset, uv);
				}
				if (rt->bHasNormalBuffer)
				{
					vec3 nor = (right.Nor*lerp1+left.Nor*(1-lerp1))*lerp2+mid.Nor*(1-lerp2);
					rt->WriteToBuffer(rt->NormalBuffer, offset, nor);
				}
				if (rt->bHasShaderIDBuffer)
				{
					rt->WriteToBuffer(rt->ShaderIDBuffer, offset, shader_id);
				}
			}
		}
	}
	void PixelShading()
	{
		for (vector<RenderTarget*>::iterator rt = RenderTargets.begin(); rt!=RenderTargets.end(); rt++)
		{
			if ((*rt)->PresentBuffer)
			{
				uint offset = 0;
				for (uint h = 0; h<(*rt)->Resolution.y; h++)
				{
					for (uint w = 0; w<(*rt)->Resolution.x; w++, offset++)
					{
						*((*rt)->PresentBuffer+offset)=Shaders[*((*rt)->ShaderIDBuffer+offset)]->PixelShader({});
					}
				}
			}
		}
	}
	void Render()
	{
		InitRenderTarget();
		VertexShading();
		PixelShading();
	}
};

enum ShaderName {Default};
enum ComponentName{cube};
enum ObjectName { Cube };
Renderer* pRenderer;
Renderer* CreateRenderer()
{
	Renderer* renderer = new Renderer;
	return renderer;
}
vector<Shader*> CompileShaders()
{
	vector<Shader*> Shaders;
	Shader_Default* Default=new Shader_Default;
	Shaders.push_back(Default);
	return Shaders;
}
Scene* CreateScene()
{
	Scene* pScene = new Scene;
	Object Cube;
	Cube.AddComponentMesh("cube.obj");
	Cube.AddWorldPosition({0, 5, 0});
	Cube.Components[ComponentName::cube].SetMaterial(ShaderName::Default);
	pScene->AddObject(Cube);
	pScene->AddCamera({0,0,0}, {-PI/2,0,0}, {256,256});
	return pScene;
}
LRESULT CALLBACK MsgHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			switch (wmId)
			{
				default:return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			
		}
		break;
		default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}
HWND CreateTheWindow(HINSTANCE hInst)
{
	WNDCLASSEXW wcex = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW|CS_VREDRAW,
		MsgHandler,
		0,0,hInst,NULL,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		L"MyWindow",
		NULL};
	RegisterClassExW(&wcex);
	HWND hWnd = CreateWindowW(
		L"MyWindow",
		L"MyRenderer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT,
		0, nullptr, nullptr, hInst, nullptr);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return hWnd;
}

bool UpdateTheWindow(HWND hWnd)
{
	MSG msg;
	PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
	if (msg.message==WM_DESTROY) { return 0; }
	DispatchMessage(&msg);
	return 1;
}
void UpdateScene(Scene* scene) 
{
	scene->Objects[ObjectName::Cube].AddLocalRotation({0,0,0.1f});
}
void Render(Renderer* renderer)
{
	renderer->Render();
}
void Present(HWND hWnd,Renderer* pRenderer)
{
	cout(1);
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);

	HBITMAP map = CreateBitmap(256, 256, 1, 32, pRenderer->ActiveTarget->PresentBuffer);
	HDC src = CreateCompatibleDC(hdc);
	SelectObject(src, map);
	BitBlt(hdc, 0, 0, 256, 256, src, 0, 0, SRCCOPY);
	DeleteDC(src);

	EndPaint(hWnd, &ps);
}
void Release( Renderer* renderer)
{
	renderer->Release();
	delete renderer;
	renderer = nullptr;
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPreInst, LPWSTR pCmdL, int pCmdS)
{
	AllocConsole();
	hConsol = GetStdHandle(STD_OUTPUT_HANDLE);
	pRenderer = CreateRenderer();
	pRenderer->BindShaders(CompileShaders());
	pRenderer->AddScene(CreateScene());
	HWND hWindow = CreateTheWindow(hInst); 
	
	while (UpdateTheWindow(hWindow))
	{
		UpdateScene(pRenderer->pScene);
		Render(pRenderer);
		Present(hWindow,pRenderer);
	}
	Release(pRenderer);
	FreeConsole();
}
