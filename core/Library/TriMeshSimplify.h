#pragma once
#include "IncludeHeader.h"

class TriMeshSimplify
{
public:
	TriMeshSimplify(MyMesh&a, MyMesh::FaceHandle b);

	~TriMeshSimplify();

	void UpdateTriQ();

	int TriCollapse();
	int TriCollapse3();

	bool is_TriMesh();//�жϸ��������Ƿ���������
	int is_TriMesh(MyMesh::VertexHandle vh);
	bool is_Three_Tri(MyMesh::FaceHandle cc, MyMesh::HalfedgeHandle &hh); //�жϸ��������Ƿ�������Ķ�����Χֻ��������������Σ�

	//debug;
	bool DebugInfo2();
	void DebugInfo();

	bool UpdateHalfEdge(MyMesh::HalfedgeHandle cc);

	double ReturnMx() { return m_x; }
	MESHSETEDGE ReturnTriSet() { return m_QX_Set; }
	INT_INT_MAP ReturnTriMap() { return m_Face_Arround;}

private:
	MyMesh &mesh;

	MyMesh::FaceHandle m_Mesh_Face;
	MyMesh::HalfedgeHandle m_Collapse_Half;//ָ����Ҫcollapse�İ�߽ṹ��
	//MyMesh::HalfedgeHandle m_Collapse_Opposit_Half;//ָ����Ҫcollapse�İ�߽ṹ��

	Matrix4d m_Face_Q, m_Q_Equation;

	MyMesh::Point m_v_mesh;

	double m_x;//���

	MESHSETEDGE m_QX_Set;

	INT_INT_MAP m_Face_Arround;//������¼��Χ�������Σ������Ƚϱ���ǰ��ȱ�ٵ�������

	void ComputeSurfaceQ(MyMesh::Point *vp); //���������ε�Qֵ

	void ComputeMaxPoint(); //�������ŵ����ĵ�

};
typedef map<int, TriMeshSimplify*>	INT_MAP_TRIMESH;

class MyTriOpenMesh
{
public:
	MyTriOpenMesh();
	MyTriOpenMesh(MyMesh a):mesh(a){}
	~MyTriOpenMesh();

	MyMesh mesh;

	int Readfile(const char * argg);

	void Writefile(const char *argg, int i);
	void Writefile(const char *argg);

	int MeshSimplification(float dest);

	void Release() { //ɾ���Ѿ���Ч��collapse��
		cout << "Release RAM" << endl;
		mesh.garbage_collection();
		INT_MAP_TRIMESH::iterator it_edge(m_TriMesh_Map.begin());
		for (; it_edge != m_TriMesh_Map.end(); it_edge++) {
			delete it_edge->second;//�ͷ�new �������ڴ�
		}
		cout << "Release RAM OVER!" << endl;
	}

	int CollapseIterator();
	int CollapseIterator(int imf);

private:

	OpenMesh::IO::Options opt;

	INT_MAP_TRIMESH m_TriMesh_Map;

	void FillTriMeshMap();

	STRUCT_SET_EDGEPAIR m_TriMesh_Set;
};
