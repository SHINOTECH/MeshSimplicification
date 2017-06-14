#include "stdafx.h"
#include "MyOpenMesh.h"

/*===========================================================================*\
*                                                                           *
*   $Revision$		v1.0                                                    *
*   $Date			2017.6.6												*
*   $Auther			Mr.chen												*
*                                                                           *
\*===========================================================================*/

MyOpenMesh::MyOpenMesh()
{
}

MyOpenMesh::~MyOpenMesh()
{
}

void MyOpenMesh::Readfile(const char * argg)
{
	// read mesh from stdin
	if (!OpenMesh::IO::read_mesh(mesh, argg, opt))
	{
		std::cerr << "Error: Cannot read mesh from " << std::endl;
		return;
	}
	//vertex_normals init!
	mesh.request_vertex_normals();

	// assure we have vertex normals
	if (!mesh.has_vertex_normals())
	{
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
		return;
	}
	if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	{
		cout << "Init Surface normal" << endl;
		//// we need face normals to update the vertex normals
		//mesh.request_face_normals();
		//// let the mesh update the normals
		//mesh.update_normals();
		//// dispose the face normals, as we don't need them anymore
		////mesh.release_face_normals();
	}
	 //init for mesh collapse
	
	mesh.request_face_normals();
	// let the mesh update the normals
	mesh.update_normals();

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();

	m_mesh_init = true;
}

void MyOpenMesh::Writefile(const char *argg, int i) {
	if (!OpenMesh::IO::write_mesh(mesh, argg, i)) //0 ascii 1 binary
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

//-----------------------------------------------------------------------------

int MyOpenMesh::MainLoop(float dest)
{
	if ((dest >= 1) || (dest <= 0)) {
		cout << "Your input is out of range, MainLoop" << endl;
		return 1;
	}
	if (!m_mesh_init) {
		cout << "Your mesh is not loaded successfully!" << endl;
		return 2;
	}

	int idest = mesh.n_vertices()*dest;//�и�ת��

	printf("Vertex : now:%d  aim:%d dif:%d\n", mesh.n_vertices(), mesh.n_vertices()-idest,idest);

	FillVertexPointMap();
	InitFillEdgePair();

	//MeshCollapseIterate(196);

	int temp =0;
	/*STRUCT_SET_EDGEPAIR::iterator ite;
	for (ite = m_EdgePair_Set.begin(); ite != m_EdgePair_Set.end(); ite++) {
		m_EdgePair_Map.erase(ite->i);
	}

	INT_MAP_EDGEPAIR::iterator itf;
	for (itf = m_EdgePair_Map.begin(); itf != m_EdgePair_Map.end(); itf++) {
		cout <<"No insert:"<< itf->first << endl;
		ite = m_EdgePair_Set.find(itf->second->ReturnEdgeSet());
		if (ite != m_EdgePair_Set.end()) {
			cout << ite->a << " " << ite->i << endl;
		}
	}
	*/
	while (idest--) {
		MeshCollapseIterate3();
		//if (!m_EdgePair_Set.size()) {
		//	cout << "error  zeror!" << endl;
		//}//
	}
	PointEdgeRelease();
	return 0;
}

//void MyOpenMesh::FillMeshPair() //�����������ʼ���ĵ�����ʼ������edge�����������edge����������
//{
//	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
//	{
//		//mesh.edge_handle(e_it->idx());
//		MyMesh::HalfedgeHandle h= mesh.halfedge_handle(mesh.edge_handle(e_it->idx()),0);
//		MyMesh::VertexHandle a[2];
//		a[0] = mesh.to_vertex_handle(h);
//		a[1] = mesh.from_vertex_handle(h);
//
//		EdgePair* mp = new EdgePair(a[0],a[1],mesh);
//		if (mp->AddVertexSurfaceCoe() == 1) {
//			delete mp;
//			continue;
//		}
//	}
//}


void MyOpenMesh::FillVertexPointMap() 
{
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		m_VertexPoint_Map[v_it->idx()] = new VertexPoint(mesh.vertex_handle(v_it->idx()),mesh);
		/*if (vp->QMSurfaceCoe()) {
			continue;
		}*/
		/*if (v_it->idx() == 109) {
			cout << "vertex break;" << endl;
		}*/
		//m_VertexPoint_Map[v_it->idx()]->QMSurfaceCoe();
		m_VertexPoint_Map[v_it->idx()]->UpdateVertexQ();
	}
}

void MyOpenMesh::InitFillEdgePair()
{
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
	{
		m_EdgePair_Map[e_it->idx()] = new EdgePair(mesh.edge_handle(e_it->idx()),mesh);

		m_EdgePair_Map[e_it->idx()]->UpdatePair(m_VertexPoint_Map);

		m_EdgePair_Set.insert(m_EdgePair_Map[e_it->idx()]->ReturnEdgeSet());
	}
}

MyMesh::EdgeHandle MyOpenMesh::FindMiniestEdge()
{
	INT_MAP_EDGEPAIR::iterator it(m_EdgePair_Map.begin());

	double xmin = it->second->ReturnQX(); double xtemp;
	MyMesh::EdgeHandle ehmin= it->second->ReturnEdge();
	for (it++; it != m_EdgePair_Map.end(); it++) {
		xtemp = it->second->ReturnQX();
		if (xmin > xtemp) {
			xmin = xtemp;
			ehmin = it->second->ReturnEdge();
		}
	}
	return ehmin;
}

//int MyOpenMesh::MeshCollapseIterate() //��ѭ��������ȥ����Сֵ
//{//һ�ε���ɾ��һ���㣬�����п����ֶ�����ɾ����
//	MyMesh::EdgeHandle e = FindMiniestEdge();
//	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
//	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();
//
//	vector<MyMesh::VertexHandle>		vmv;
//	map<int, int> s_medge;
//	cout << "This edge id:" << e.idx() << " Delete all;" << endl;
//	int count = 0;
//	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
//	{
//		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����b����Χ�Ķ��㣻
//		vmv.push_back(vtemp);
//
//		if (vtemp == ema) {
//			continue;
//		}
//		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
//		{
//			count++;
//			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
//			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
//
//			MyMesh::VertexHandle vtemp = mesh.to_vertex_handle(htemp);
//			//if (vtemp == ema) {//������b��a�Ĺ������㣬Ԥ��Ҫ��collapseed��ɾ��
//			//	cout << "The deleted edge is: " << etemp.idx() << endl;
//			//	delete m_EdgePair_Map[etemp.idx()];
//			//	m_EdgePair_Map.erase(etemp.idx());
//			//	continue;
//			//}
//			s_medge[etemp.idx()] = vtemp.idx();
//		}
//	}
//	//cout << count << endl;
//
//	map<int, int>::iterator set_it;
//
//	//for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
//	//	if (set_it->second == ema.idx()) {  
//	//		delete m_EdgePair_Map[set_it->first];
//	//		m_EdgePair_Map.erase(set_it->first);
//	//		s_medge.erase(set_it->first);
//
//	//		//cout << "Delete 1:" << set_it->first << endl;
//	//	}
//	//}
//
//	//��collapse��Ȼ���ٿ����ߵ���һ���ǲ��Ƿ���ԭ���ĵ㣻
//	//if ((e.idx() != 506905)&&(m_EdgePair_Map[e.idx()]!=NULL)) {
//		if (m_EdgePair_Map[e.idx()]->PairCollapse())
//		{
//			cout << "Collapse error, Remove the top!" << endl;
//		}
//	//}
//		
//		if (e.idx() == 506905) {
//			if (m_EdgePair_Map[e.idx()]->PairCollapse2())
//			{
//				cout << "Collapse2 error, Remove the top!" << endl;
//			}
//		}
//
//	//delete m_VertexPoint_Map[emb.idx()];	//һ�ε���ֻɾ��һ����
//	delete m_EdgePair_Map[e.idx()];			
//	//m_VertexPoint_Map.erase(emb.idx());	//��ǰcollapse��ɾ��
//	m_EdgePair_Map.erase(e.idx());		//��ǰ��ɾ����
//	//cout << "Delete 2:" << e.idx() << endl;
//
//	for (int i = 0; i < vmv.size();i++) {
//		m_VertexPoint_Map[vmv[i].idx()]->UpdateVertexQ(); //����B����Χ��ÿ�����Q
//	}
//
//	if (!s_medge.size())
//	{
//		cout << "Edge after collapse is zero!" << endl;
//		return mesh.n_vertices();
//	}
//
//	for (set_it=s_medge.begin(); set_it != s_medge.end();set_it++) {
//		if (m_EdgePair_Map[set_it->first] == NULL) {
//			cout <<"NULL :"<< set_it->first << endl;
//			m_EdgePair_Map.erase(set_it->first);
//			continue;
//		}
//		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
//	}
//
//	//cout << "````````````````````````````````" << endl;
//
//	return mesh.n_vertices();
//}

int MyOpenMesh::MeshCollapseIterate() //��ѭ��������ȥ����Сֵ
{//һ�ε���ɾ��һ���㣬�����п����ֶ�����ɾ����
	MyMesh::EdgeHandle e = FindMiniestEdge();
	/*if (e.idx() == -1) {
		if (m_EdgePair_Map[e.idx()] == NULL) {
			cout << "NULL" << endl;
		}
		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 3;
	}*/
	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	//vector<MyMesh::VertexHandle>		vmv;
	map<int, int> s_medge;

#ifdef OUTDEBUG
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	
	/*if (e.idx() == 196) {
		cout << "break 196:" <<emb.idx()<<" "<<ema.idx() << endl;

		PointArround(ema);
		PointArround(emb);
	}*/
	
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����b����Χ�Ķ��㣻
		//vmv.push_back(vtemp);
		if (vtemp == ema) {
			continue;
		}

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb)||(vbtemp==ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	//cout << count << endl;
	map<int, int>::iterator set_it;
	int count1 = 0,count2=0;
	//for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	//{
	//	MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����a����Χ�Ķ��㣻

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
		{ //196 :1
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			count1++;
			s_medge[etemp.idx()] = -1;//vtemp.idx();
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			count2++;
			s_medge[etemp.idx()] = -1;//vtemp.idx();
		}
	//}
		//if (s_medge.size()<=2)
		if((count1<2)||(count2<2))
		{
			//cout << "Edge after collapse is zero!" << endl;
			m_EdgePair_Map[e.idx()]->PairCollapse();
			delete m_EdgePair_Map[e.idx()];
			m_EdgePair_Map.erase(e.idx());
			return 1;// mesh.n_vertices();
		}

	//��collapse��Ȼ���ٿ����ߵ���һ���ǲ��Ƿ���ԭ���ĵ㣻
	/*if (e.idx() == 196) {
		if (m_EdgePair_Map[e.idx()]->PairCollapseInverse())
		{
			cout << "Collapse error, Remove the top!" << endl;
		}
	}
	else {*/
		if (m_EdgePair_Map[e.idx()]->PairCollapse())
		{
			cout << "Collapse error, Remove the top!" << endl;
		}
	//}
	
	/*if (e.idx() == 196) {
		PointArround(emb);
		PointArround(ema);
	}*/

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		//count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	//delete m_VertexPoint_Map[emb.idx()];	//һ�ε���ֻɾ��һ����
	//delete m_EdgePair_Map[e.idx()];
	//m_VertexPoint_Map.erase(emb.idx());	//��ǰcollapse��ɾ��
	//m_EdgePair_Map.erase(e.idx());		//��ǰ��ɾ����
	//cout << "Delete 2:" << e.idx() << endl;

	//for (int i = 0; i < vmv.size(); i++) {
	//	m_VertexPoint_Map[vmv[i].idx()]->UpdateVertexQ(); //����B����Χ��ÿ�����Q
	//}
	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//����A����Χ��ÿ����Qֵ
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (set_it->second == -1) {

#ifdef OUTDEBUG
	cout << "Delete :" << set_it->first << endl;
#endif // OUTDEBUG

			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}

		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
	}

	//cout << "````````````````````````````````" << endl;

	return 2;// mesh.n_vertices();
}

void MyOpenMesh::PointArround(MyMesh::VertexHandle m_vh) {
	cout << "m_vh:" << m_vh.idx() << endl;
	
	for (MyMesh::VertexOHalfedgeIter  voh_iter = mesh.voh_begin(m_vh); voh_iter.is_valid(); ++voh_iter) {
		cout << "the OH NO. is : " << voh_iter->idx() << endl;
		cout << "the OH NEXT NO. is: " << mesh.next_halfedge_handle(mesh.halfedge_handle(voh_iter->idx())) << endl;;

	}
	for (MyMesh::VertexIHalfedgeIter  vih_iter = mesh.vih_begin(m_vh); vih_iter.is_valid(); ++vih_iter) {
		cout << "the IH NO. is : " << vih_iter->idx() << endl;
		cout << "the OH NEXT NO. is: " << mesh.next_halfedge_handle(mesh.halfedge_handle(vih_iter->idx())) << endl;;
	}
	for (MyMesh::VertexVertexIter  vv_iter = mesh.vv_begin(m_vh); vv_iter.is_valid(); ++vv_iter) {
		cout << "the VV NO. is : " << vv_iter->idx() << endl;
	}
	for (MyMesh::VertexEdgeIter ve_iter = mesh.ve_begin(m_vh); ve_iter.is_valid(); ++ve_iter) {
		cout << "the edge NO. is : " << ve_iter->idx() << endl;
	}
}

int MyOpenMesh::MeshCollapseIterate(float dest) //��ѭ��������ȥ����Сֵ
{//һ�ε���ɾ��һ���㣬�����п����ֶ�����ɾ����
	int agdest = (int)dest;
	MyMesh::EdgeHandle e = mesh.edge_handle(agdest);
	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	//vector<MyMesh::VertexHandle>		vmv;
	map<int, int> s_medge;
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����b����Χ�Ķ��㣻
		//vmv.push_back(vtemp);
		if (vtemp == ema) {
			continue;
		}

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb) || (vbtemp == ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	map<int, int>::iterator set_it;
	int count = 0;

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}

	if (s_medge.size() <= 2)
	{
		cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return mesh.n_vertices();
	}

	//��collapse��Ȼ���ٿ����ߵ���һ���ǲ��Ƿ���ԭ���ĵ㣻
	if (m_EdgePair_Map[e.idx()]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//����A����Χ��ÿ����Qֵ
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (set_it->second == -1) {
			cout << "Delete :" << set_it->first << endl;
			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		if (m_EdgePair_Map[set_it->first] == NULL) {
			cout << "NULL :" << set_it->first << endl;
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
	}

	return mesh.n_vertices();
}

int MyOpenMesh::MeshCollapseIterate2() //��ѭ��������ȥ����Сֵ
{//һ�ε���ɾ��һ���㣬�����п����ֶ�����ɾ����
	
	MyMesh::EdgeHandle e = mesh.edge_handle(m_EdgePair_Set.begin()->i); //��С������

	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	map<int, int> s_medge;

#ifdef OUTDEBUG2
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����b����Χ�Ķ��㣻
		if (vtemp == ema) {
			continue;
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb) || (vbtemp == ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	//cout << count << endl;
	map<int, int>::iterator set_it;
	int count1 = 0, count2 = 0;

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{ //196 :1
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count1++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count2++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}

	if ((count1<2) || (count2<2))
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[e.idx()]->ReturnEdgeSet());

		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 1;// mesh.n_vertices();
	}

	//��collapse��Ȼ���ٿ����ߵ���һ���ǲ��Ƿ���ԭ���ĵ㣻

	if (m_EdgePair_Map[e.idx()]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		//count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	//delete m_EdgePair_Map[e.idx()];
	//m_EdgePair_Map.erase(e.idx());		//��ǰ��ɾ����
	//cout << "Delete 2:" << e.idx() << endl;

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//����A����Χ��ÿ����Qֵ
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG2
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Set.erase(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());

		if (set_it->second == -1) {
#ifdef OUTDEBUG2
			cout << "Delete :" << set_it->first << endl;
#endif // OUTDEBUG
			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}

		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
		m_EdgePair_Set.insert(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());
	}

	return 2;// mesh.n_vertices();
}

int MyOpenMesh::MeshCollapseIterate3() //��ѭ��������ȥ����Сֵ
{//һ�ε���ɾ��һ���㣬�����п����ֶ�����ɾ����

	MyMesh::EdgeHandle e = mesh.edge_handle(m_EdgePair_Set.begin()->i); //��С������
	if (m_EdgePair_Map[e.idx()] == NULL) {
		cout <<"NULL idx: "<<e.idx() << endl;
		m_EdgePair_Map.erase(e.idx());
	}

	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	//map<int, int> s_medge;

#ifdef OUTDEBUG2
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	map<int, int> s_medge;
	int count1 = 0, count2 = 0;
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{ //196 :1
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count1++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}
	if (count1<2)
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[e.idx()]->ReturnEdgeSet());

		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 1;// mesh.n_vertices();
	}

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		count2++;
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//����b����Χ�Ķ��㣻
		if (vtemp == ema) {
			continue;
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}

	
	//for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
	//{
	//	MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
	//	MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
	//	count2++;
	//	s_medge[etemp.idx()] = -1;//vtemp.idx();
	//}

	if (count2<2)
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[e.idx()]->ReturnEdgeSet());

		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 2;// mesh.n_vertices();
	}
	
	map<int, int>::iterator set_it;

	vector<MyMesh::EdgeHandle> v_del_half;
	v_del_half.push_back(e);
	MyMesh::HalfedgeHandle s_half = m_EdgePair_Map[e.idx()]->ReturnHalfEdge();
	MyMesh::HalfedgeHandle o_half = mesh.opposite_halfedge_handle(s_half);
	MyMesh::HalfedgeHandle s_del_half = mesh.next_halfedge_handle(mesh.next_halfedge_handle(s_half));
	MyMesh::HalfedgeHandle o_del_half = mesh.next_halfedge_handle(o_half);
	if (s_half == mesh.next_halfedge_handle(s_del_half)) {
		v_del_half.push_back(mesh.edge_handle(s_del_half));
	}
	if (o_half == mesh.next_halfedge_handle(mesh.next_halfedge_handle(o_del_half))) {
		v_del_half.push_back(mesh.edge_handle(o_del_half));
	}

	//��collapse��Ȼ���ٿ����ߵ���һ���ǲ��Ƿ���ԭ���ĵ㣻

	if (m_EdgePair_Map[e.idx()]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	//for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	//{
	//	MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
	//	MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
	//	//count++;
	//	s_medge[etemp.idx()] = 0;//vtemp.idx();
	//}

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//����A����Χ��ÿ����Qֵ
	}

	for (auto i : v_del_half) {
		m_EdgePair_Set.erase(m_EdgePair_Map[i.idx()]->ReturnEdgeSet());

#ifdef OUTDEBUG2
			cout << "Delete :" << i.idx() << endl;
#endif // OUTDEBUG
		delete m_EdgePair_Map[i.idx()];
		m_EdgePair_Map.erase(i.idx());
		s_medge.erase(i.idx());
		//m_EdgePair_Map[i.idx()]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
		//m_EdgePair_Set.insert(m_EdgePair_Map[i.idx()]->ReturnEdgeSet());
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG2
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Set.erase(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());

//		if (set_it->second == -1) {
//#ifdef OUTDEBUG2
//			cout << "Delete :" << set_it->first << endl;
//#endif // OUTDEBUG
//			delete m_EdgePair_Map[set_it->first];
//			m_EdgePair_Map.erase(set_it->first);
//			continue;
//		}

		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //����ÿ��������Ž�
		m_EdgePair_Set.insert(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());
	}
	
	return 3;// mesh.n_vertices();
}