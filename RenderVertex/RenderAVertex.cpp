#include"stdafx.h"
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
	vec3 rotate(vec3 trans)
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
		return ret;
	}
};
class Mesh
{
public:
	vec3* LocalSpaceVertexBuffer;//原始的顶点数据
	vec3* CameraSpaceVertexBuffer;//被变换过的顶点数据
	int VertexCount;//顶点数
	void Translate(vec3* from,vec3* to,vec3 trans)//顶点平移
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<VertexCount;i++)
		{
			*(t+i)=*(f+i)+trans;
		}
	}
	void Rotate(vec3* from,vec3* to,vec3 trans)//顶点旋转
	{
		vec3* f=from;
		vec3* t=to;
		for(int i=0;i<VertexCount;i++)
		{
			*(t+i)=(f+i)->rotate(trans);
		}
	}
};
class RenderingPipeLine//渲染管线对象
{
public:
	vec3 CameraPosition;//摄像机位置
	vec3 CameraRotation;//摄像机旋转（朝向）
	vec2 CameraSize;//摄像机大小，目前只用一个长方形表示摄像机，长方形越大，能看到的顶点就越多
	vec2int Resolution;//分辨率
	bool* ViewPort=0;//视口可以是一个二维数组，用来存像素，但是数组长度不能动态根据随分辨率改变而改变，所以这样方便些
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
	}
	void SetResolution(vec2int res)
	{
		Resolution=res;
		if (ViewPort) { delete ViewPort; }
		ViewPort = new bool[res.x*res.y]{0};//分辨率改变时，创建新的等大小视口
	}
	void SetMesh(vec3* res1,vec3* res2,int vertexcount)//暂时用来设置顶点数据，将来将从模型文件加载
	{
		mesh->LocalSpaceVertexBuffer=res1;
		mesh->CameraSpaceVertexBuffer=res2;
		mesh->VertexCount=vertexcount;
	}

	void WorldToCamera()//摄像机向前一步走，就把世界向后拉一步，
	{
		mesh->Rotate(mesh->LocalSpaceVertexBuffer,mesh->CameraSpaceVertexBuffer,-CameraRotation);
		mesh->Translate(mesh->CameraSpaceVertexBuffer,mesh->CameraSpaceVertexBuffer,-CameraPosition);
	}
	void DrawPrimitive()//变换到摄像机空间的顶点如果在摄像机大小范围内，就在视口输出1
	{
		vec2 camh=CameraSize/2;
		vec3* ver=mesh->CameraSpaceVertexBuffer;	
		for(int i=0;i<mesh->VertexCount;i++)
		{
			if (abs((ver + i)->x)>camh.x || abs((ver + i)->y)>camh.y) {continue;}
			int w=((ver+i)->x/CameraSize.x+0.5)*Resolution.x+0.5;
			int h=((ver+i)->y/CameraSize.y+0.5)*Resolution.y+0.5;
			*(ViewPort+h*Resolution.x+w)=true;
		}	
	}
	void Render()//现在的渲染管线只有两大步
	{
		WorldToCamera();
		DrawPrimitive();
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
	RPL->SetCameraSize({50,25});
	RPL->SetResolution({20,10});
	vec3* res1 = new vec3{2,2,4};//设置一个位于2,2,4的顶点
	vec3* res2 = new vec3{2,2,4};
	RPL->SetMesh(res1,res2,1);
}
void Render(RenderingPipeLine* RPL)
{
	RPL->Render();
}
void Present(RenderingPipeLine* RPL)
{
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
	Render(RPL);
	Present(RPL);
	DestroyPipeLine(RPL);
	while (1) {}
}