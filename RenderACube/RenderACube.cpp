#include "stdafx.h"
#include <iostream>
#include <math.h>
#include <fstream>
#include "CImg.h"
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PI float(3.1415926)
using namespace std;
using namespace cimg_library;

class vec2uint
{
public:
	unsigned int x;
	unsigned int y;
	vec2uint operator+(vec2uint v)
	{
		return {x+v.x,y+v.y};
	}
	vec2uint operator/(int i)
	{
		return {x/i,y/i};
	}
	bool operator<(int i)
	{
		return x<i||y<i;
	}
};
class vec3int
{
public:
	int x;
	int y;
	int z;
	vec3int operator-(vec3int v)
	{
		return {x-v.x,y-v.y,z-v.z};
	}
};
class vec2
{
public:
	float x;
	float y;
	vec2 operator+(vec2 v)
	{
		return {x+v.x,y+v.y};
	}
	vec2 operator*(float f)
	{
		return {x*f,y*f};
	}
	vec2 operator/(float f)
	{
		return {x/f,y/f};
	}
	bool operator<(float f)
	{
		return x<f||y<f;
	}
};
class vec3
{
public:
	float x;
	float y;
	float z;
	vec3 operator+(vec3 v)
	{
		return {x+v.x,y+v.y,z+v.z};
	}
	vec3 operator-()
	{
		return {-x,-y,-z};
	}
	vec3 operator*(float f)
	{
		return {x*f,y*f,z*f};
	}
	vec3 operator*(vec3 v)
	{
		return {x*v.x,y*v.y,z*v.z};
	}
	void rotate(vec3 trans)
	{
		vec3 ret;
		ret.x=x;
		ret.y=y*cos(trans.x)+z*sin(trans.x);
		ret.z=z*cos(trans.x)-y*sin(trans.x);
		x=ret.x*cos(trans.y)-ret.z*sin(trans.y);
		y=ret.y;
		z=ret.x*sin(trans.y)+ret.z*cos(trans.y);
		ret.x=x*cos(trans.z)+y*sin(trans.z);
		ret.y=y*cos(trans.z)-x*sin(trans.z);
		ret.z=z;
		x=ret.x;
		y=ret.y;
		z=ret.z;
	}
};
class Color 
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};
class Vertex		//用来临时存数据的结构
{
public:
	vec3 Pos;
	vec3 Nor;
	vec2 UV;
};
class Face
{
public:
	vec3int VertexIndex;
	vec3int NormalIndex;
	vec3int UVIndex;
};
class Mesh
{
public:
	vec3* LocalSpacePosBuffer;
	vec3* WorldSpacePosBuffer;
	vec3* CameraSpacePosBuffer;
	unsigned int PosCount=0;
	vec3* LocalSpaceNormalBuffer;
	vec3* WorldSpaceNormalBuffer;
	vec3* CameraSpaceNormalBuffer;
	unsigned int NormalCount=0;
	Face* FaceBuffer;
	unsigned int FaceCount=0;
	vec3 MeshPosition = {0,3,0};
	vec3 MeshRotation = {0,0,0};

	void Translate(vec3* from,vec3* to,int count,vec3 trans)
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<count;i++)
		{
			*(t+i)=*(f+i)+trans;
		}
	}
	void Rotate(vec3* from,vec3* to,int count,vec3 trans)
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<count;i++)
		{
			*(t+i)=*(f+i);
			(t+i)->rotate(trans);
		}
	}
	void AddLocation(vec3 trans)
	{
		MeshPosition = MeshPosition+trans;
	}
	void AddRotation(vec3 trans)
	{
		MeshRotation=MeshRotation+trans;
	}
	void SortIndexByPosY()
	{
		for (int i = 0; i < FaceCount; i++)
		{
			vec3int* index_pos=&((FaceBuffer + i)->VertexIndex);
			vec3int* index_nor=&((FaceBuffer + i)->NormalIndex);
			float bot = (CameraSpacePosBuffer + index_pos->z)->y;
			float mid = (CameraSpacePosBuffer + index_pos->y)->y;
			float top = (CameraSpacePosBuffer + index_pos->x)->y;
			int temp;
			if (bot> mid) 
			{
				temp = index_pos->z; index_pos->z = index_pos->y; index_pos->y = temp;
				temp = index_nor->z; index_nor->z = index_nor->y; index_nor->y = temp;
				float f=bot;bot=mid;mid=f;
			}
			if (mid> top) 
			{ 
				temp = index_pos->y; index_pos->y = index_pos->x; index_pos->x = temp; 
				temp = index_nor->y; index_nor->y = index_nor->x; index_nor->x = temp;
				float f=mid;mid=top;top=f;
			}
			if (bot> mid) 
			{
				temp = index_pos->z; index_pos->z = index_pos->y; index_pos->y = temp;
				temp = index_nor->z; index_nor->z = index_nor->y; index_nor->y = temp;
			}
		}
	}
	void Perspective()
	{
		vec3* pos;
		for(int i=0;i<PosCount;i++)
		{
			pos=CameraSpacePosBuffer+i;
			--pos->z;
			pos->x/=pos->z;
			pos->y/=pos->z;
		}
	}
};
class RenderingPipeLine
{
public:
	vec3 CameraPosition = {0,0,0};
	vec3 CameraRotation = {0,0,0};
	float CameraFOV = PI/3;
	float CameraDepth=-100;
	vec2 CameraSize = {0,0};
	vec2 CameraHalfSize = {0,0};
	vec2uint Resolution = {10,10};
	float PixelSize=0;
	CImg<unsigned char>* ViewPort;
	CImg<unsigned char>* ViewPortInRender;
	Color BackGroundColor = {0,0,0,1};
	double* ZBuffer;
	vec3* NormalBuffer;
	Mesh* mesh;

	//debug
	bool* DebugConsole=0;

	void SetCameraPosition(vec3 pos)
	{
		CameraPosition=pos;
	}
	void SetCameraRotation(vec3 rot)
	{
		CameraRotation=rot;
	}
	void SetCameraFOV(float fov)
	{
		if (fov >= PI) {return;}
		CameraFOV=fov;
		CameraHalfSize.x=1/tan((PI-fov)/2);
		CameraHalfSize.y=CameraHalfSize.x*Resolution.y/Resolution.x;
		CameraSize=CameraHalfSize*2;
		PixelSize=CameraSize.x/Resolution.x;
	}
	void SetCameraSize(vec2 cam)
	{
		if (cam < 0) {return;}
		CameraSize=cam;
		CameraHalfSize=cam/2;
		PixelSize=CameraSize.x/Resolution.x;
	}
	void SetResolution(vec2uint res)
	{
		if (res < 0) {return;}
		Resolution = res;
		PixelSize = CameraSize.x / Resolution.x;

		if (ViewPort) { delete ViewPort; }
		ViewPort =new CImg<unsigned char>(res.x,res.y,1,3);
		ViewPort->fill(BackGroundColor.r,BackGroundColor.g,BackGroundColor.b);//填充背景颜色
		if (ViewPortInRender) { delete ViewPortInRender; }
		ViewPortInRender =new CImg<unsigned char>(res.x,res.y,1,3);
		ViewPortInRender->fill(BackGroundColor.r,BackGroundColor.g,BackGroundColor.b);

		if (ZBuffer) { delete ZBuffer; }
		ZBuffer = new double[res.x*res.y];

		if (NormalBuffer) { delete NormalBuffer; }
		NormalBuffer = new vec3[res.x*res.y]{0,0,1};

		//debug
		if (DebugConsole) { delete DebugConsole; }
		DebugConsole = new bool[res.x*res.y]{0};
	}
	void SetMesh(Mesh* m)
	{
		mesh=m;
		mesh->WorldSpacePosBuffer=new vec3[mesh->PosCount];
		mesh->CameraSpacePosBuffer=new vec3[mesh->PosCount];
		mesh->WorldSpaceNormalBuffer=new vec3[mesh->NormalCount];
		mesh->CameraSpaceNormalBuffer=new vec3[mesh->NormalCount];
	}

	void PrepareRender()
	{
		ViewPortInRender->fill(BackGroundColor.r,BackGroundColor.g,BackGroundColor.b);//清除上一帧的画面
		for (int i = 0; i < Resolution.x*Resolution.y; i++) { *(ZBuffer + i) = 1; }
	}
	void LocalToWorld()
	{
		mesh->Rotate(mesh->LocalSpacePosBuffer,mesh->WorldSpacePosBuffer,mesh->PosCount,mesh->MeshRotation);
		mesh->Translate(mesh->WorldSpacePosBuffer,mesh->WorldSpacePosBuffer,mesh->PosCount,mesh->MeshPosition);

		mesh->Rotate(mesh->LocalSpaceNormalBuffer,mesh->WorldSpaceNormalBuffer,mesh->NormalCount,mesh->MeshRotation);
	}
	void WorldToCamera()
	{
		mesh->Rotate(mesh->WorldSpacePosBuffer,mesh->CameraSpacePosBuffer,mesh->PosCount,-CameraRotation);
		mesh->Translate(mesh->CameraSpacePosBuffer,mesh->CameraSpacePosBuffer,mesh->PosCount,-CameraPosition);

		mesh->Rotate(mesh->WorldSpaceNormalBuffer,mesh->CameraSpaceNormalBuffer,mesh->NormalCount,-CameraRotation);
	}
	void CameraToView()	
	{ 
		mesh->Perspective();
		mesh->SortIndexByPosY();
		vec3* pos=mesh->CameraSpacePosBuffer;
		vec3* nor=mesh->CameraSpaceNormalBuffer;
		Face* face=mesh->FaceBuffer;
		for (int i = 0; i < mesh->FaceCount; i++)
		{	
			//debug
			//if (i != 3) {continue;}
			vec3 top_pos = *(pos + (face + i)->VertexIndex.x);
			vec3 left_pos =*(pos + (face+ i)->VertexIndex.y);	
			vec3 bot_pos = *(pos + (face + i)->VertexIndex.z);
			float tan_mid= (top_pos.y - bot_pos.y) / (top_pos.x - bot_pos.x);
			float lerp=(top_pos.y-left_pos.y)/(top_pos.y-bot_pos.y);
			float rx=bot_pos.x + (left_pos.y - bot_pos.y) / tan_mid;
			float rz=bot_pos.z*lerp+top_pos.z*(1-lerp);//插值z
			vec3 right_pos={rx, left_pos.y,rz};
			vec3 top_nor=*(nor + (face + i)->NormalIndex.x);
			vec3 left_nor=*(nor + (face + i)->NormalIndex.y);
			vec3 bot_nor=*(nor + (face + i)->NormalIndex.z);
			vec3 right_nor=bot_nor*lerp+top_nor*(1-lerp);//插值法线

			/*vec3 top_uv=*(nor + (face + i)->UVIndex.x);
			vec3 left_uv=*(nor + (face + i)->UVIndex.y);
			vec3 bot_uv=*(nor + (face + i)->UVIndex.z);
			float lerp=(right_pos.y-bot_pos.y)/(top_pos.y-bot_pos.y);
			vec3 right_nor=top_nor*lerp+bot_nor*(1-lerp);*/

			Vertex top = {top_pos,top_nor};
			Vertex left = {left_pos,left_nor};
			Vertex bot = {bot_pos,bot_nor};
			Vertex right = {right_pos,right_nor};
			
			if (left_pos.x > right_pos.x) 
			{
				Vertex v=left;
				left=right;
				right=v; 

				Rasterization(left,right,top,tan_mid,(top.Pos.y-right.Pos.y)/(top.Pos.x-right.Pos.x));
				Rasterization(left,right,bot,tan_mid,(bot.Pos.y-right.Pos.y)/(bot.Pos.x-right.Pos.x));
			}
			else 
			{
				Rasterization(left,right,top,(top.Pos.y-left.Pos.y)/(top.Pos.x-left.Pos.x),tan_mid);
				Rasterization(left,right,bot,(bot.Pos.y-left.Pos.y)/(bot.Pos.x-left.Pos.x),tan_mid);
			}
		}
	}
	void Rasterization(Vertex left,Vertex right,Vertex mid,float tan_left,float tan_right)
	{
		const bool b=mid.Pos.y>left.Pos.y;
		float ys=min(max(left.Pos.y,-CameraHalfSize.y),CameraHalfSize.y);
		float ye=min(max(mid.Pos.y,-CameraHalfSize.y),CameraHalfSize.y);

		for (float y =ys ; b? y<ye : y>ye ; y += b? PixelSize : -PixelSize)
		{
			float xs=min(max(left.Pos.x+(y-left.Pos.y)/tan_left,-CameraHalfSize.x),CameraHalfSize.x);
			float xe=min(max(right.Pos.x+(y-left.Pos.y)/tan_right,-CameraHalfSize.x),CameraHalfSize.x);
			
			for (float x = xs; x < xe;x+=PixelSize)
			{
				float lerp2=(mid.Pos.y-y)/(mid.Pos.y-left.Pos.y);
				float lerp1=(mid.Pos.x-left.Pos.x-(mid.Pos.x-x)/lerp2)/(right.Pos.x-left.Pos.x);

				float z=(right.Pos.z*lerp1+left.Pos.z*(1-lerp1))*lerp2+mid.Pos.z*(1-lerp2);

				unsigned int w = (x / CameraSize.x + 0.5)*Resolution.x + 0.5;
				unsigned int h = (y / CameraSize.y + 0.5)*Resolution.y + 0.5;

				if (!DepthTest({w,h}, z)) { continue; }//判断是否被遮挡
				vec3 nor=(right.Nor*lerp1+left.Nor*(1-lerp1))*lerp2+mid.Nor*(1-lerp2);
				Color c = {(unsigned char)abs(nor.x*255),(unsigned char)abs(nor.y*255),(unsigned char)abs(nor.z*255)};
				DrawToBuffer({ w,h }, c);//单纯把法线当作颜色来显示
			}
		}
	}
	bool DepthTest(vec2uint pos,float z)
	{
		if (z<0 &&z >CameraDepth) 
		{
			z/=CameraDepth;
			double* f=ZBuffer+ pos.y*Resolution.x + pos.x;
			if ( z<*f )
			{
				*f=z;
				return true;
			}
			else {return false;}
		}
		else {return false;}
	}
	void DrawToBuffer(vec2uint pos,Color c)	
	{
		unsigned char ch[] = { c.r,c.g,c.b };
		ViewPortInRender->draw_point(pos.x,pos.y,ch,1);//CImg的画像素函数

		//debug
		*(DebugConsole+pos.y*Resolution.x+pos.x)=true;
	}

	void Render()
	{
		PrepareRender(); 
		LocalToWorld();
		WorldToCamera();
		CameraToView(); 
		CImg<unsigned char>* c=ViewPort;//交换要显示的视口
		ViewPort=ViewPortInRender;
		ViewPortInRender=c;
	}
};

RenderingPipeLine* CreatePipeLine()
{
	RenderingPipeLine* RPL=new RenderingPipeLine;
	return RPL;
}
Mesh* LoadMesh(string path)
{
	ifstream mf ;
	mf.open(path);
	string s;
	char c;

	Mesh* m=new Mesh;
	m->PosCount=0;
	m->NormalCount=0;
	m->FaceCount=0;
	
	while (!mf.eof())//遍历文件得到顶点，面，法线的数目
	{
		c=mf.get();
		if (c == 'v') {m->PosCount++;}	
		if (mf.get() == 'n') {m->PosCount--;m->NormalCount++;}
		if (c == 'f') {m->FaceCount++;}
		mf.ignore(unsigned(-1),'\n');
	}

	vec3* pos=new vec3[m->PosCount];
	vec3* nor=new vec3[m->NormalCount];
	Face* face=new Face[m->FaceCount];

	mf.clear();
	mf.seekg(0, ios::beg);
	for (int i=0;i<m->PosCount;i++)//读顶点
	{
		while (mf.get() != 'v') {mf.ignore(unsigned(-1),'\n');}
		mf >> (pos+i)->x >>(pos+i)->y >>(pos+i)->z;
		mf.ignore(unsigned(-1),'\n');
	}
	for (int i=0;i<m->NormalCount;i++)//读法线
	{
		while (mf.get() != 'v') {mf.ignore(unsigned(-1),'\n');}
		mf.get();
		mf >> (nor+i)->x >>(nor+i)->y >>(nor+i)->z;
		mf.ignore(unsigned(-1),'\n');
	}
	vec3int v = {1,1,1};
	for (int i = 0; i < m->FaceCount; i++)//读面
	{
		while (mf.get() != 'f') {mf.ignore(unsigned(-1),'\n');}
		mf>>(face+i)->VertexIndex.x>>c>>c>>(face+i)->NormalIndex.x;
		mf>>(face+i)->VertexIndex.y>>c>>c>>(face+i)->NormalIndex.y;
		mf>>(face+i)->VertexIndex.z>>c>>c>>(face+i)->NormalIndex.z;
		(face + i)->VertexIndex = (face + i)->VertexIndex -v;
		(face + i)->NormalIndex = (face + i)->NormalIndex -v;
		mf.ignore(unsigned(-1),'\n');
	}
	m->LocalSpacePosBuffer=pos;
	m->LocalSpaceNormalBuffer=nor;
	m->FaceBuffer=face;
	return m;
}
void InitilizePipeLine(RenderingPipeLine* RPL)
{
	RPL->SetCameraPosition({0,0,0});
	RPL->SetCameraRotation({(float)-PI/2,0,0});
	RPL->SetCameraFOV(PI/2);
	RPL->SetResolution({256,256});

}
void BindResource(RenderingPipeLine* RPL)
{
	Mesh* m=LoadMesh("cube.obj");
	RPL->SetMesh(m);
}
void Update(RenderingPipeLine* RPL)
{
	RPL->mesh->AddRotation({ 0,0,(float)0.03 });
}
void Render(RenderingPipeLine* RPL)
{
	RPL->Render();
}
void Present(RenderingPipeLine* RPL,CImgDisplay* window)
{
	window->display(*(RPL->ViewPort));
}
void DestroyPipeLine(RenderingPipeLine* RPL)
{
	delete RPL;
}
int main()
{
	CImgDisplay* window=new CImgDisplay;
	window->show();
	RenderingPipeLine* RPL=CreatePipeLine();
	InitilizePipeLine(RPL);
	BindResource(RPL);
	while(1)
	{
		cout<<1;
		Update(RPL);
		Render(RPL);
		Present(RPL, window); //while (1) {}
	}
	DestroyPipeLine(RPL);
	delete window;
}
