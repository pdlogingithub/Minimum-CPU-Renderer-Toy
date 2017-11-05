#include "stdafx.h"
#include <iostream>
#include <math.h>
#define PI 3.141592653
using namespace std;

class vec2int
{
public:
	int x;
	int y;
	vec2int operator+(vec2int v)
	{
		return {x+v.x,y+v.y};
	}
	vec2int operator/(int i)
	{
		return {x/i,y/i};
	}
};
class vec3int
{
public:
	int x;
	int y;
	int z;
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
	vec2 operator/(float f)
	{
		return {x/f,y/f};
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
class Face	
{
public:
	vec3int VertexIndex;
};
class Mesh
{
public:
	vec3* LocalSpaceVertexBuffer;
	vec3* WorldSpaceVertexBuffer;	
	vec3* CameraSpaceVertexBuffer;
	int VertexCount;
	Face* FaceBuffer;	
	int FaceCount;
	vec3 MeshPosition;
	vec3 MeshRotation;

	void Translate(vec3* from,vec3* to,vec3 trans)
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<VertexCount;i++)
		{
			*(t+i)=*(f+i)+trans;
		}
	}
	void Rotate(vec3* from,vec3* to,vec3 trans)
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<VertexCount;i++)
		{
			*(t+i)=*(f+i);
			(t+i)->rotate(trans);
		}
	}
	void AddRotation(vec3 trans)
	{
		MeshRotation=MeshRotation+trans;
	}
	void SortIndexByY()//根据变换到摄像机空间的顶点的y值将所有面的索引数据排序，顶点y值越大，它在面数据里三个索引的位置越靠前
	{
		for (int i = 0; i < FaceCount; i++)
		{
			vec3int* index=&((FaceBuffer + i)->VertexIndex);
			float bot = (CameraSpaceVertexBuffer + index->z)->y;
			float mid = (CameraSpaceVertexBuffer + index->y)->y;
			float top = (CameraSpaceVertexBuffer + index->x)->y;
			if (bot> mid) { int i = index->z; index->z = index->y; index->y = i; float f=bot;bot=mid;mid=f;}
			if (mid> top) { int i = index->y; index->y = index->x; index->x = i; float f=mid;mid=top;top=f;}
			if (bot> mid) { int i = index->z; index->z = index->y; index->y = i; float f=bot;bot=mid;mid=f;}
		}
	}
};
class RenderingPipeLine
{
public:
	vec3 CameraPosition;
	vec3 CameraRotation;
	vec2 CameraSize;
	vec2 CameraHalfSize;
	vec2int Resolution;
	float PixelSize;	
	bool* ViewPort=0;
	Mesh m;
	Mesh* mesh=&m;

	void SetCameraPosition(vec3 pos)
	{
		CameraPosition=pos;
	}
	void SetCameraRotation(vec3 rot)
	{
		CameraRotation=rot;
	}
	void SetCameraSize(vec2 cam)
	{
		CameraSize=cam;
		CameraHalfSize=cam/2;
		PixelSize=CameraSize.x/Resolution.x;
	}
	void SetResolution(vec2int res)
	{
		Resolution=res;
		PixelSize=CameraSize.x/Resolution.x;
		if (ViewPort) { delete ViewPort; }
		ViewPort = new bool[res.x*res.y]{0};
	}
	void SetMesh(vec3* res1,vec3* res2,vec3* res3,int vertex_count,Face* res4,int face_count)	
	{
		mesh->LocalSpaceVertexBuffer=res1;
		mesh->CameraSpaceVertexBuffer=res2;
		mesh->WorldSpaceVertexBuffer=res3;
		mesh->VertexCount=vertex_count;
		mesh->FaceBuffer=res4;
		mesh->FaceCount=face_count;
	}

	void LocalToWorld()
	{
		mesh->Rotate(mesh->LocalSpaceVertexBuffer,mesh->WorldSpaceVertexBuffer,mesh->MeshRotation);
		mesh->Translate(mesh->WorldSpaceVertexBuffer,mesh->WorldSpaceVertexBuffer,mesh->MeshPosition);
	}
	void WorldToCamera()
	{
		mesh->Rotate(mesh->WorldSpaceVertexBuffer,mesh->CameraSpaceVertexBuffer,-CameraRotation);
		mesh->Translate(mesh->CameraSpaceVertexBuffer,mesh->CameraSpaceVertexBuffer,-CameraPosition);
	}
	void Draw()	
	{  
		mesh->SortIndexByY();
		vec3* ver=mesh->CameraSpaceVertexBuffer;
		Face* face=mesh->FaceBuffer;
		for (int i = 0; i < mesh->FaceCount; i++)
		{
			vec3 top = *(ver + (face + i)->VertexIndex.x);	
			vec3 left =*(ver + (face+ i)->VertexIndex.y);	
			vec3 bot = *(ver + (face + i)->VertexIndex.z);
			float tan_mid= (top.y - bot.y) / (top.x - bot.x);
			vec3 right={bot.x + (left.y - bot.y) / tan_mid, left.y,0};
			
			DrawLine({ top.x,top.y }, { left.x,left.y }); 
			DrawLine({ top.x,top.y }, { bot.x,bot.y });
			DrawLine({ left.x,left.y }, {bot.x,bot.y });
			return;

			if (left.x > right.x) 
			{
				vec3 v=left;
				left=right;
				right=v;
				Rasterization(left,right,top,tan_mid,(top.y-right.y)/(top.x-right.x));
				Rasterization(left,right,bot,tan_mid,(bot.y-right.y)/(bot.x-right.x));
			}
			else 
			{
				Rasterization(left,right,top,(top.y-left.y)/(top.x-left.x),tan_mid);
				Rasterization(left,right,bot,(bot.y-left.y)/(bot.x-left.x),tan_mid);
			}
		}
	}
	void DrawLine(vec2 top,vec2 bot)
	{
		float tan = (top.y - bot.y) / (top.x - bot.x);
		float step = PixelSize* 1/sqrt(1/tan/tan + 1);
		for (float y = bot.y; y <top.y; y += step)
		{
			float x = bot.x + (y - bot.y) / tan;
			DrawVertex({x,y,0});
			
		}
	}
	void Rasterization(vec3 left,vec3 right,vec3 mid,float tan_left,float tan_right)	//光栅化
	{
		const bool b=mid.y>left.y;
		for (float y = left.y; b?y < mid.y:y>mid.y; y += b?PixelSize:-PixelSize)
		{
			for (float x = left.x+(y-left.y)/tan_left; x < right.x+(y-left.y)/tan_right;x+=PixelSize)
			{
				DrawVertex({x,y,0});
			}
		}
	}
	void DrawVertex(vec3 ver)
	{
		if (abs(ver.x)>CameraHalfSize.x || abs(ver.y)>CameraHalfSize.y) {return;}
		int w=(ver.x/CameraSize.x+0.5)*Resolution.x+0.5;
		int h=(ver.y/CameraSize.y+0.5)*Resolution.y+0.5;
		*(ViewPort+h*Resolution.x+w)=true;
	}
	void Render()
	{
		memset(ViewPort, 0, sizeof(bool)*Resolution.x*Resolution.y);
		LocalToWorld();
		WorldToCamera();
		Draw();
	}
};

RenderingPipeLine* CreatePipeLine()
{
	RenderingPipeLine* RPL=new RenderingPipeLine;
	return RPL;
}
void InitilizePipeLine(RenderingPipeLine* RPL)
{
	RPL->SetCameraPosition({0,0,0});
	RPL->SetCameraRotation({(float)-PI/2,0,0});
	RPL->SetCameraSize({5,5});
	RPL->SetResolution({20,20});

	vec3* res1 = new vec3[3];
	*res1={2,1,-2};
	*(res1+1)={2,1,2};
	*(res1+2)={-2,1,1};
	vec3* res2 = new vec3[3];
	*res2={2,1,-4};
	*(res2+1)={2,1,4};
	*(res2+2)={-1,1,1};
	vec3* res3 = new vec3[3];
	*res3={2,1,-4};
	*(res3+1)={2,1,4};
	*(res3+2)={-1,1,1};
	Face* res4 = new Face;
	res4->VertexIndex = {0,1,2};
	RPL->SetMesh(res1,res2,res3,3,res4,1);
}
void Update(RenderingPipeLine* RPL)
{
	RPL->mesh->AddRotation({ 0,0,(float)0.1 });
}
void Render(RenderingPipeLine* RPL)
{
	RPL->Render();
}
void Present(RenderingPipeLine* RPL)
{
	//system("cls");
	for(int h=RPL->Resolution.y-1;h>=0;h--)
	{
		for(int w=0;w<RPL->Resolution.x;w++)
		{
			bool* pixel=RPL->ViewPort+h*RPL->Resolution.x+w;
			cout<<*pixel;
		}
		cout<<endl;
	}
}
void DestroyPipeLine(RenderingPipeLine* RPL)
{
	delete RPL;
}
int main()
{
	RenderingPipeLine* RPL=CreatePipeLine();
	InitilizePipeLine(RPL);	
	while(1)
	{
		//Update(RPL);
		Render(RPL);
		Present(RPL); while (1) {}
	}
	DestroyPipeLine(RPL);
}
