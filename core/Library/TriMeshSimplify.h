#pragma once
#include "IncludeHeader.h"

class TriMeshSimplify
{
public:
	TriMeshSimplify(MyMesh&a, MyMesh::FaceHandle b);

	~TriMeshSimplify();

	void UpdateTriQ();

	int TriCollapse(vector<int>&a,vector<int>&b);

	bool is_TriMesh();//�жϸ��������Ƿ���������
	int is_TriMesh(MyMesh::VertexHandle vh);
	bool is_Three_Tri(MyMesh::FaceHandle cc, MyMesh::HalfedgeHandle &hh); //�жϸ��������Ƿ�������Ķ�����Χֻ��������������Σ�

	//debug;
	bool DebugInfo2();
	void DebugInfo();

	bool UpdateHalfEdge(MyMesh::HalfedgeHandle cc);

	//double ReturnMx() { return m_x; }
	MESHSETEDGE ReturnTriSet() { return m_QX_Set; }
	//INT_INT_MAP ReturnTriMap() { return m_Face_Arround;}

private:
	MyMesh &mesh;

	MyMesh::FaceHandle m_Mesh_Face;
	MyMesh::HalfedgeHandle m_Collapse_Half;//ָ����Ҫcollapse�İ�߽ṹ��
	//MyMesh::HalfedgeHandle m_Collapse_Opposit_Half;//ָ����Ҫcollapse�İ�߽ṹ��

	//Matrix4d m_Face_Q, m_Q_Equation;

	MyMesh::Point m_v_mesh;

	//double m_x;//���

	MESHSETEDGE m_QX_Set;

	//INT_INT_MAP m_Face_Arround;//������¼��Χ�������Σ������Ƚϱ���ǰ��ȱ�ٵ�������

	Matrix4d ComputeSurfaceQ(MyMesh::Point *vp); //���������ε�Qֵ

	double ComputeMaxPoint(Matrix4d &x); //�������ŵ����ĵ�

};
typedef map<int, TriMeshSimplify*>	INT_MAP_TRIMESH;
typedef vector<TriMeshSimplify*>	VECTOR_TRIMESH;

class MyTriOpenMesh
{
public:
	MyTriOpenMesh();
	MyTriOpenMesh(MyMesh a,float b):mesh(a), m_dest(b){}
	~MyTriOpenMesh();

	MyMesh mesh; //Public;

	/*
		�ļ��Ķ�д
	*/
	int Readfile(const char * argg);

	void Writefile(const char *argg, int i); //����int i��1 ��ʾ�����ƣ�0 ��ʾASCII
	void Writefile(const char *argg);		 //ʹ��Ĭ�Ϲ�������ļ�

	/*
		��������ں�����
		����Ϊ����:0<dest<1��
	*/
	int MeshSimplification(float dest);

	/*
		ɾ���Ѿ���Ч��collapse�㣬ͬʱ�ͷ��ڴ�
	*/
	void Release() {
		cout << "Release RAM" << endl;
		mesh.garbage_collection();
		INT_MAP_TRIMESH::iterator it_edge(m_TriMesh_Map.begin());
		for (; it_edge != m_TriMesh_Map.end(); it_edge++) {
			delete it_edge->second;//�ͷ�new �������ڴ�
		}
		cout << "Release RAM OVER!" << endl;
	}

	/*
		����ָ���Ľӿں�����
	*/
	void InterFacePort(string in,string out,float dest); //������ø������������ļ��е������
	/*
		������
			MyTriOpenMesh ims;
			ims.InterFacePort("123.stl","456.stl",0.6);
	*/

	void InterFacePort();//�������ʹ��mesh�������ͣ���ָ�������ļ������
	/*
		������
			MyMesh mesh;
			MyTriOpenMesh ims(mesh,0.6);
			ims.InterFacePort();
			mesh=ims.mesh;
	*/

private:
	float m_dest=0;

	OpenMesh::IO::Options opt;

	INT_MAP_TRIMESH m_TriMesh_Map;

	//VECTOR_TRIMESH m_TriMesh_Vector;


	void FillTriMeshMap();

	STRUCT_SET_EDGEPAIR m_TriMesh_Set;

	/*
		�������򻯹��̣�
	*/
	int CollapseIterator();
	//int CollapseIterator(int imf,int saw,vector<int>&a,vector<int> &b);
};
