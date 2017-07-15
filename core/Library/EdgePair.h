#pragma once
#include "IncludeHeader.h"
//#include "MyStruct.h"

class VertexPoint {
public:
	VertexPoint(MyMesh::VertexHandle a,MyMesh &c) :
		m_vh(a),
		cmesh(c)
	{
		m_Q << 0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0;
	}

	int QMSurfaceCoe();

	int UpdateVertexQ();//���µ��Q����
	
	Matrix4d ReturnMatrix() {
		return m_Q;
	}
private:
	MyMesh&cmesh;
	MyMesh::VertexHandle m_vh;

	Matrix4d m_Q;

	//vector<MyMesh::EdgeHandle> m_arr_Edg; //�õ���Χ�ı�

	void ComputeSurfaceQ(MyMesh::Point *vp);
};

typedef map<int, VertexPoint*>	INT_MAP_VERTEXP;

class EdgePair
{
public:
	EdgePair(MyMesh &c);
	EdgePair(MyMesh::EdgeHandle e, MyMesh &c);

	~EdgePair();

	//int AddVertexSurfaceCoe();//˼·��ÿ�������������㣬Ȼ���������ٸ�����Χ������������϶����ظ����㣻

	int UpdatePair(INT_MAP_VERTEXP&a);//���±ߵ�����ϵ��x,ͬʱ�������ŵ�Vmax
	//int ComputeQArroundSurface(MyMesh::VertexHandle a);
	//void ComputeSurfaceQ(MyMesh::Point *vp);

	double ReturnQX() {//���ظú�����QȨֵ
		return m_x;
	}

	//�����ñߵ�������
	MyMesh::VertexHandle ReturnMa() {
		return m_a;
	}
	MyMesh::VertexHandle ReturnMb() { 
		return m_b;
	}

	//��ǰ�߸���
	MyMesh::EdgeHandle ReturnEdge() {
		return m_edge_h;
	}

	MESHSETEDGE ReturnEdgeSet() {
		return m_QX_Idx;
	}

	MyMesh::HalfedgeHandle ReturnHalfEdge() {
		return m_halfedge_h;
	}
	//ɾ��b���ƶ�a
	int PairCollapse();//�������������񾫼�collapse
	int PairCollapse2();
	int PairCollapseInverse();

private:
	MyMesh&cmesh;

	MyMesh::VertexHandle m_a, m_b; //���ն���collapse��a��

	MyMesh::EdgeHandle m_edge_h;//һ���߽�

	MyMesh::HalfedgeHandle m_halfedge_h;

	//vector<MyMesh::VertexHandle> m_a_arr, m_b_arr;

	//vector<Vector4d> m_equ_surf_a, m_equ_surf_b;//�洢ƽ�淽�̲���

	//Matrix4d m_Q_a, m_Q_b;//����Q����

	//Vector4d m_v;//��������� (x,y,z,1)
	MyMesh::Point m_v_mesh;

	Matrix4d m_Q, m_Q_Equation;

	MESHSETEDGE m_QX_Idx;//��pair��id�󶨵�Qֵ�Ľṹ��

	double m_x = 0; //�ñߵĺϲ�ϵ��Ȩֵ

	void UpdateQEquation(Matrix4d x);
	void ComputeMaxPoint();// (MyMesh::VertexHandle a, MyMesh::VertexHandle b);//���ļ������������Լ�xֵ�ĺ�����
};

typedef map<int, EdgePair*>		INT_MAP_EDGEPAIR; //value - edge->idx : value - EdgePair* 

