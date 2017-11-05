#include "stdafx.h"
#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>
#include "CImg.h"
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

using namespace std;
using namespace cimg_library;
#define uint unsigned int
#define uchar unsigned char
const float PI = float(3.1415926);

enum ShaderName { NO, Default };
enum TextureName { Metal };
enum ComponentName { cube };
enum ObjectName { Cube };

namespace library
{
	template<typename T>
	inline T clamp(T t, T t1, T t2)
	{
		if (t<t1) { return t1; }
		if (t>t2) { return t2; }
		return t;
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
struct vec3m	
{
public:
	float x, y, z;

	inline vec3m operator-()
	{
		return{-x, -y, -z};
	}
};
class matrix4
{
public:
	float table[4][4];

	enum eTransType { ObjToWorld, ObjToTarget, WorldToTarget, WorldToTangent };

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
	inline static matrix4 scalor(vec3m sca)
	{
		return matrix4(
			sca.x, 0, 0, 0,
			0, sca.y, 0, 0,
			0, 0, sca.z, 0,
			0, 0, 0, 1);
	}
	inline static matrix4 rotator(vec3m rot)
	{
		return matrix4(
			1, 0, 0, 0,
			0, cos(rot.x), -sin(rot.x), 0,
			0, sin(rot.x), cos(rot.x), 0,
			0, 0, 0, 1)
			*matrix4(
			cos(rot.y), 0, sin(rot.y), 0,
			0, 1, 0, 0,			
			-sin(rot.y), 0, cos(rot.y), 0,
			0, 0, 0, 1)
			*matrix4(
			cos(rot.z), -sin(rot.z), 0, 0,
			sin(rot.z), cos(rot.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	inline static matrix4 translator(vec3m tra)
	{
		return matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			tra.x, tra.y, tra.z, 1);
	}
	inline static matrix4 transfomer(vec3m sca, vec3m rot, vec3m tra)
	{
		return matrix4::scalor(sca)*matrix4::rotator(rot)*matrix4::translator(tra);
	}
};
class vec3
{
public:
	float x, y, z;

	vec3m para()
	{
		return{x,y,z};
	}
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
		return{x+v.x, y+v.y, z+v.z};
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
	inline vec3 transform(matrix4 transform)
	{
		return{
			dot(transform.table[0][0], transform.table[1][0], transform.table[2][0], transform.table[3][0]),
			dot(transform.table[0][1], transform.table[1][1], transform.table[2][1], transform.table[3][1]),
			dot(transform.table[0][2], transform.table[1][2], transform.table[2][2], transform.table[3][2])};
	}
};

class Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	inline static Color mix(Color c1, Color c2)
	{
		return{(uchar)(c1.r/2+c2.r/2),(uchar)(c1.g/2+c2.g/2),(uchar)(c1.b/2+c2.b/2),};
	}
};
struct Fragment	//	渲染时临时存放的结构
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
			Fragment f = frag[bot];
			frag[bot] = frag[right];
			frag[right] = f;
		}
		if (frag[right].Pos.y>frag[top].Pos.y)
		{
			Fragment f = frag[right];
			frag[right] = frag[top];
			frag[top] = f;
		}
		if (frag[bot].Pos.y>frag[right].Pos.y)
		{
			Fragment f = frag[bot];
			frag[bot] = frag[right];
			frag[right] = f;
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
			Fragment f = frag[left];
			frag[left] = frag[right];
			frag[right] = f;
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
	float PixelSize = 0.002255f;
	float Depth = -100;
	vec2uint Resolution = {512,512};
	float FOV = PI/2;

	bool bPerspective = 1;
	bool bHasZBuffer = 1;
	bool bHasPOSBuffer = 0;
	bool bHasUVBuffer = 1;
	bool bHasNormalBuffer = 1;
	bool bHasShaderIDBuffer = 1;
	bool bHasLightBitIDBuffer = 0;

	float* ZBuffer = nullptr;
	vec3* PosBuffer = nullptr;
	vec2* UVBuffer = nullptr;
	vec3* NormalBuffer = nullptr;
	uchar* ShaderIDBuffer = nullptr;	
	uint* LightBitIDBuffer = nullptr;		

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
			if (ZBuffer) { delete ZBuffer; }
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
	}
	void InitTarget()
	{
		InitBuffer(ZBuffer, Depth);
		InitBuffer(UVBuffer, {0,0});
		InitBuffer(NormalBuffer, {0,0,1});
		InitBuffer(ShaderIDBuffer, uchar(0));
		InitBuffer(LightBitIDBuffer, uint(0));
	}
	inline bool DepthTest(uint pos_offset, float z)
	{
		if (z<0&&z >Depth)
		{
			float* f = ZBuffer+pos_offset;
			if (z>*f)
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
			PosBuffer,
			UVBuffer,
			NormalBuffer,
			ShaderIDBuffer,
			LightBitIDBuffer,
		ZBuffer = nullptr;
		UVBuffer = nullptr;
		NormalBuffer = nullptr;
		ShaderIDBuffer = nullptr;
		LightBitIDBuffer = nullptr;
	}
};
class Texture
{
public:
	Color* ImageBuffer = nullptr;
	bool bHasAlpha=0;
	uint Size;
	Color TextureLookup(vec2 uv) 
	{
		
		uv= uv*vec2(Size);
		return *(ImageBuffer+(uint)uv.y*Size+(uint)uv.x);
	}

	void Release()
	{
		delete ImageBuffer;
		ImageBuffer = nullptr;
	}
};
class ShaderParameter
{
public:
	RenderTarget* current_target;
	vector<RenderTarget*> all_target;
	vec3 pos;
	vec2 uv;
	vec3 nor;
	matrix4* transform;
};
class Shader
{
public:
	vector<Texture> Textures;

	void LoadTexture(char* path)
	{
		CImg<uchar> img("metal.bmp");
		Texture tex;
		tex.Size = img.width();
		tex.ImageBuffer = new Color[tex.Size*tex.Size];
		for (uint h = 0; h<tex.Size;h++)
		{ 
			for (uint w = 0; w<tex.Size; w++)
			{
				Color* c = tex.ImageBuffer+h*tex.Size+w;
				c->r = img(w, h, 0);
				c->g = img(w, h, 1);
				c->b = img(w, h, 2);
			}
		}
		Textures.push_back(tex);
	}
	virtual vec3 VertexShader(ShaderParameter para)
	{
		vec3 ret = para.pos.transform(*(para.transform+matrix4::ObjToTarget));
		--ret.z;
		ret.x /= ret.z;
		ret.y /= ret.z;
		return ret;
	}
	virtual Color PixelShader(ShaderParameter para)
	{
		Color c= {(uchar)abs(para.nor.x*255),(uchar)abs(para.nor.y*255),(uchar)abs(para.nor.z*255),255};
		return Color::mix(Textures[TextureName::Metal].TextureLookup(para.uv),c);
		//return;
	}

	inline void Release()
	{
		for (vector<Texture>::iterator t = Textures.begin(); t!=Textures.end(); t++)
		{
			t->Release();
		}
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
	uint ElementCount = 0;

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
class Object
{
public:
	Mesh mesh;

	bool bVisibility = 1;
	vec3 Position = {0,0,0};
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
			mf.ignore(unsigned(-1), '\n');
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
				--i;
				element = first_element ? element : element++;
				first_element = false;
				element->FaceCount = 0;
				mf.ignore(unsigned(-1), ' ');
				int a;
				mf>>a;
				element->ShaderID = (uchar)a;
				mf.ignore(unsigned(-1), ' ');
				if (mf.get()=='o') { element->bSmoothShading = false; }
				else { element->bSmoothShading = true; }
				mf.ignore(unsigned(-1), '\n');
				continue;
			}
			if (c1=='s')
			{
				--i;
				uchar u = element->ShaderID;
				++element;
				element->ShaderID = u;
				element->FaceCount = 0;
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
			--i;
			mf.ignore(unsigned(-1), '\n');
		}
		mesh.PosBuffer = pos;
		mesh.UVBuffer = uv;
		mesh.NormalBuffer = nor;
		mesh.FaceBuffer = face;
		mesh.ElementBuffer = element-mesh.ElementCount+1;
		mesh.CachedPosBuffer = cpos;
	}
	inline void AddWorldPosition(vec3 pos)
	{
		Position = Position+pos;
	}
	inline void AddLocalRotation(vec3 rot)
	{
		Rotation = Rotation+rot;
	}
	inline void Release()
	{
		mesh.Release();
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
	inline void AddCamera(vec3 pos, vec3 rot)/
	{
		Camera camera;
		camera.RenderProxy = new RenderTarget;
		camera.RenderProxy->Position = pos;
		camera.RenderProxy->Rotation = rot;
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
	Scene* pScene = nullptr;
	vector<Shader*> Shaders;
	vector<RenderTarget*> RenderTargets;
	CImg<uchar>* ViewPortReady;
	CImg<uchar>* ViewPortInRender;
	vec2uint Resolution;

	void SetResulotion(vec2uint res)
	{
		Resolution = res;
		if (ViewPortReady)
		{
			delete ViewPortReady;
		}
		ViewPortReady = new CImg<uchar>(res.x, res.y, 1, 3, 0);
		if (ViewPortInRender)
		{
			delete ViewPortInRender;
		}
		ViewPortInRender = new CImg<uchar>(res.x, res.y, 1, 3, 0);
	}
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
			camera->RenderProxy->SetResolution(Resolution);
			RenderTargets.push_back(camera->RenderProxy);
		}
	}
	inline void Release()
	{
		pScene->Release();
		delete pScene;
		pScene = nullptr;
		delete ViewPortReady;
		ViewPortReady = nullptr;
		delete ViewPortInRender;
		ViewPortInRender = nullptr;
		for (vector<Shader*>::iterator shader = Shaders.begin(); shader!=Shaders.end(); shader++)
		{
			(*shader)->Release();
			delete *shader;
			*shader = nullptr;
		}
	}

	void InitRenderTarget()
	{
		for (vector<RenderTarget*>::iterator rt = RenderTargets.begin(); rt!=RenderTargets.end(); rt++)
		{
			(*rt)->InitTarget();
		}
		ViewPortInRender->fill((uchar)0, (uchar)0, (uchar)0);
	}
	void VertexShading()
	{
		matrix4 transform[4];
		vector<RenderTarget*> rt = RenderTargets;
		for (uint rtnum = 0; rtnum<RenderTargets.size(); rtnum++)
		{
			transform[matrix4::WorldToTarget] = matrix4::transfomer({1,1,1}, -rt[rtnum]->Rotation.para(), -rt[rtnum]->Position.para());
			vector<Object> obj = pScene->Objects;
			for (uint objnum = 0; objnum<obj.size(); objnum++)
			{
				transform[matrix4::ObjToWorld] = matrix4::transfomer(obj[objnum].Scale.para(), obj[objnum].Rotation.para(), obj[objnum].Position.para());
				transform[matrix4::ObjToTarget] = transform[matrix4::ObjToWorld]*transform[matrix4::WorldToTarget];
				Mesh mesh = obj[objnum].mesh;
				Face* face = mesh.FaceBuffer;
				Element* element = mesh.ElementBuffer;
				for (uint ele = 0; ele<mesh.ElementCount; ele++, element++)
				{
					for (uint fa = 0; fa<element->FaceCount; fa++, face++)
					{

						Fragment frag[4];
						for (uint ver = 0; ver<3; ver++)
						{

							frag[ver].Pos = *(mesh.PosBuffer+face->PosIndex[ver]);
							frag[ver].UV = *(mesh.UVBuffer+face->UVIndex[ver]);
							frag[ver].Nor = *(mesh.NormalBuffer+face->NorIndex[ver]);
							frag[ver].Pos = Shaders[uint(element->ShaderID)]->VertexShader({rt[rtnum],RenderTargets,frag[ver].Pos,frag[ver].UV,frag[ver].Nor,transform});
						}
						frag->SortFragment(frag);
						Rasterization(rt[rtnum], element->ShaderID, element->bSmoothShading, frag[Fragment::left], frag[Fragment::right], frag[Fragment::top], Fragment::up);
						Rasterization(rt[rtnum], element->ShaderID, element->bSmoothShading, frag[Fragment::left], frag[Fragment::right], frag[Fragment::bot], Fragment::down);
					}
				}
			}
		}
	}
	void Rasterization(RenderTarget* rt, uchar shader_id, bool smooth, Fragment left, Fragment right, Fragment mid, const bool upward)
	{
		float y_start = library::clamp(left.Pos.y, -rt->HalfSize.y, rt->HalfSize.y);
		float y_end = library::clamp(mid.Pos.y+0.002f, -rt->HalfSize.y, rt->HalfSize.y);
		for (float y = y_start; (upward ? y<y_end : y>y_end); y += (upward ? rt->PixelSize : -rt->PixelSize))
		{
			float x_start = left.Pos.x-0.001f+(y-left.Pos.y)/(upward ? left.tan_up : left.tan_down);
			float x_end = right.Pos.x+0.002f+(y-left.Pos.y)/(upward ? right.tan_up : right.tan_down);
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
					
					vec2 uv = (right.UV*lerp1+left.UV*(1-lerp1))*lerp2+mid.UV*(1-lerp2); 
					
					rt->WriteToBuffer(rt->UVBuffer, offset, {abs(uv.x),abs(uv.y)});
				}
				if (rt->bHasNormalBuffer)
				{
					vec3 nor = smooth ? (right.Nor*lerp1+left.Nor*(1-lerp1))*lerp2+mid.Nor*(1-lerp2) : right.Nor;
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
		for (uint rtnum = 0; rtnum<RenderTargets.size(); rtnum++)
		{
			uint offset = 0;
			for (uint h = 0; h<Resolution.y; h++)
			{
				for (uint w = 0; w<Resolution.x; w++, offset++)
				{
					uchar id = *(RenderTargets[rtnum]->ShaderIDBuffer+offset);
					if (id==0) { continue; }
					ShaderParameter para;
					para.uv= *(RenderTargets[rtnum]->UVBuffer+offset);
					//cout<<para.uv.x<<endl<<para.uv.y<<endl;
					para.nor = *(RenderTargets[rtnum]->NormalBuffer+offset);
					Color c = Shaders[(uint)id]->PixelShader(para);
					uchar ch[3] = {c.r,c.g,c.b};
					ViewPortInRender->draw_point(w, h, ch);
				}
			}
		}
	}
	void Swap()
	{
		CImg<uchar>* C = ViewPortReady;
		ViewPortReady = ViewPortInRender;
		ViewPortInRender = C;
	}
	void Render()
	{
		InitRenderTarget();
		VertexShading();
		PixelShading();
		Swap();
	}
};

Renderer* CreateRenderer()
{
	return new Renderer;
}
vector<Shader*> CompileShaders()
{
	vector<Shader*> Shaders;
	Shader* NO = new Shader;
	Shaders.push_back(NO);
	Shader* Default = new Shader;
	Default->LoadTexture("metal.bmp");
	Shaders.push_back(Default);
	return Shaders;
}
Scene* CreateScene()
{
	Scene* pScene = new Scene;
	Object Cube;
	Cube.LoadMesh("susan.obj");
	Cube.AddWorldPosition({0, 1, 0});
	pScene->AddObject(Cube);
	pScene->AddCamera({0,0,0}, {-PI/2,0,0});
	return pScene;
}
CImgDisplay* CreateDisplayWindow()
{
	return new CImgDisplay;
}

void UpdateScene(Scene* scene)
{
	scene->Objects[ObjectName::Cube].AddLocalRotation({0,0,0.1f});
}
void Render(Renderer* renderer)
{
	renderer->Render();
}
void Present(CImgDisplay* pw, Renderer* pr)
{
	pw->display(*pr->ViewPortReady);
}
void Release(Renderer* renderer)
{
	renderer->Release();
	delete renderer;
	renderer = nullptr;
}

int main()
{
	Renderer* pRenderer = CreateRenderer();
	pRenderer->SetResulotion({256,256});
	pRenderer->BindShaders(CompileShaders());
	Scene* pScene = CreateScene();
	pRenderer->AddScene(pScene);
	CImgDisplay* pWnd = CreateDisplayWindow();


	while (1)
	{
		UpdateScene(pScene);
		Render(pRenderer);
		Present(pWnd, pRenderer); //while (1){}
	}
	Release(pRenderer);
}

