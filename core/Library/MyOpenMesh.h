#pragma once
//#include "IncludeHeader.h"
#include "EdgePair.h"
//#include "MyStruct.h"

//#define OUTDEBUG
//#define OUTDEBUG2
class MyOpenMesh
{
public:
	MyOpenMesh();
	~MyOpenMesh();

	//===========================================================================
	  /** 
	   @name Public mesh file (key word: "mesh")
	   **/
	//===========================================================================
	/*
	 * variable value for save mesh file input;
	 */
	MyMesh mesh;
	

	//===========================================================================
	  /**
	   @name file manage : Read and Out
	   **/
	//===========================================================================
	/*
	 * Read STL/OM/OFF/PLY file and Do some initialized work;
	 *
	 * @param argg "filename".stl "filename".stla "filename".stlb  
	 * @return void
	 */
	void Readfile(const char * argg);

	//===========================================================================
	/**
	@name Main Loop: Main Work
	**/
	//===========================================================================
	/*
	*  
	*
	* @param argg 50% of your destnation from your begin total numebr   0<argg<1; 
	* @return int 
	*/
	int MyOpenMesh::MainLoop(float dest);

	/*
	 * Output STL file and 
	 *
	 * @param argg "filename".stl
	 * @param i 1:Binary or 0:ASCII
	 * @return void
	 */
	void Writefile(const char *argg, int i);

	void PointEdgeRelease() { //ɾ���Ѿ���Ч��collapse��
		mesh.garbage_collection();
	}
private :
	bool m_mesh_init = false;
	/*
	 * open option for normal useage
	 */
	OpenMesh::IO::Options opt;

	//set<s_pair> m_Mesh_Pair_Set;

	INT_MAP_EDGEPAIR	m_EdgePair_Map;

	STRUCT_SET_EDGEPAIR     m_EdgePair_Set;

	//vector<class MeshPair*> m_Mesh_Pair_Vector;

	//map<int, Vector4d> m_Surf_Coe;

	INT_MAP_VERTEXP			m_VertexPoint_Map;
	
	//void FillMeshPair();		//��ʼ�����ж����Qֵ

	void InitFillEdgePair();	//��ʼ�����б߽��

	void FillVertexPointMap();	//��ʼ�����е��Qֵ

	MyMesh::EdgeHandle FindMiniestEdge(); //��map���Ҹ���������collapse��

	//MyMesh::EdgeHandle FindMiniestEdgeSet(); //ʹ��set����ʵ��һ���ѣ�������ֵ�ĸ����Լ����޳�


	int MeshCollapseIterate();
	int MeshCollapseIterate2();
	int MeshCollapseIterate3();
	int MeshCollapseIterate(float a);

	void MyOpenMesh::PointArround(MyMesh::VertexHandle v_h);//for debug;
}; 

