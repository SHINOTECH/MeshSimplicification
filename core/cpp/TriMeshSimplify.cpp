#include "stdafx.h"
#include "TriMeshSimplify.h"


TriMeshSimplify::TriMeshSimplify(MyMesh&m, MyMesh::FaceHandle b):
	mesh(m),
	m_Mesh_Face(b)
{
}

TriMeshSimplify::~TriMeshSimplify()
{
}

void TriMeshSimplify::UpdateTriQ() {
	m_Face_Q << 0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0;

	m_Face_Arround.clear();

	set<int> atface;
	
	//m_Collapse_Half = mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx()); //�ȸ�һ�����Ժ��õĻ���ɾ�����������ԣ� ������

	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		MyMesh::VertexHandle vhp = mesh.vertex_handle(fv_iter->idx());
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vhp); vf_iter.is_valid(); ++vf_iter)
		{
			atface.insert(vf_iter->idx());
		}

	}
	
	MyMesh::Point arv[3];
	set<int>::iterator sit(atface.begin());
	for (; sit != atface.end(); sit++) {
		m_Face_Arround[*sit] = 1; //record
		int i = 0;
		for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(mesh.face_handle(*sit)); fv_iter.is_valid(); ++fv_iter)
		{
			if (i >= 3) {
				cout << "Up three!" << endl;
				break;
			}
			arv[i] = mesh.point(*fv_iter);
			i++;
		}
		ComputeSurfaceQ(arv);
	}

	m_Q_Equation = m_Face_Q;
	m_Q_Equation(3, 0) = 0;
	m_Q_Equation(3, 1) = 0;
	m_Q_Equation(3, 2) = 0;
	m_Q_Equation(3, 3) = 1;

	ComputeMaxPoint();
	m_QX_Set.a = m_x;
	m_QX_Set.i = m_Mesh_Face.idx();
}

bool TriMeshSimplify::UpdateHalfEdge(MyMesh::HalfedgeHandle cc) {
	//m_Collapse_Half = cc; //mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx());
	for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(mesh.face_handle(cc)); fh_iter.is_valid(); fh_iter++) {
		MyMesh::HalfedgeHandle ah = mesh.halfedge_handle(fh_iter->idx());

		MyMesh::HalfedgeHandle ah_a = mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(ah));//opposit next

		MyMesh::HalfedgeHandle ah_b = mesh.prev_halfedge_handle(mesh.opposite_halfedge_handle(mesh.prev_halfedge_handle(ah)));

		//if (m_Mesh_Face.idx() == 654954) { //309486
		//	cout << "break: " << ah.idx()<<" "<<ah_a.idx()<<" "<<ah_b.idx()<<" "<<mesh.opposite_halfedge_handle(ah_b).idx() << endl;
		//	cout << "face: " << mesh.face_handle(ah).idx() << " " << mesh.face_handle(ah_a).idx() << " " << mesh.face_handle(ah_b).idx() << " " << mesh.face_handle(mesh.opposite_halfedge_handle(ah_b)).idx() << endl;
		//}
		
		if (ah_a.idx() == mesh.opposite_halfedge_handle(ah_b).idx()) {
			//m_Collapse_Half = ah;
			//break;
			return true;
		}
	}
	return false;
}

bool TriMeshSimplify::is_Three_Tri(MyMesh::FaceHandle cc,MyMesh::HalfedgeHandle &hh) {

	for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(cc); fh_iter.is_valid(); fh_iter++) {
		MyMesh::HalfedgeHandle ah = mesh.halfedge_handle(fh_iter->idx());

		MyMesh::HalfedgeHandle ah_a = mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(ah));//opposit next

		MyMesh::HalfedgeHandle ah_b = mesh.prev_halfedge_handle(mesh.opposite_halfedge_handle(mesh.prev_halfedge_handle(ah)));

		if (ah_a.idx() == mesh.opposite_halfedge_handle(ah_b).idx()) {
			hh = ah;
			return true;
		}
	}
	return false;
}

void TriMeshSimplify::ComputeSurfaceQ(MyMesh::Point *vp) //������Ķ����������Qֵ
{
	Vector3d a((*vp)[0], (*vp)[1], (*vp)[2]);
	Vector3d b((*(vp + 1))[0], (*(vp + 1))[1], (*(vp + 1))[2]);
	Vector3d c((*(vp + 2))[0], (*(vp + 2))[1], (*(vp + 2))[2]);

	Vector3d ab = a - b;
	ab = (a - c).cross(ab);
	ab.normalize();
	double df = ab.dot(Vector3d(0, 0, 0) - a);

	Vector4d coe(ab[0], ab[1], ab[2], df); //surface equation cofficient!

	m_Face_Q += coe*coe.transpose(); //update Q
}

void TriMeshSimplify::ComputeMaxPoint()//(MyMesh::VertexHandle tmpa, MyMesh::VertexHandle tmpb)
{
	Vector4d ori(0, 0, 0, 1);
	//Vector4d m_v=m_Q_Equation.fullPivHouseholderQr().solve(ori); //�ĳɾֲ�������
	Vector4d m_v = m_Q_Equation.colPivHouseholderQr().solve(ori); //ʹ��col�᲻������һ�㣨������ûɶ�����ٶȶ���࣡��
																  //check;
	double temp = (m_Q_Equation*m_v - ori).norm();
	if ((!temp) || (temp < SOLUTIONACCURANCY)) { //��������̵������
		m_v_mesh = MyMesh::Point(m_v[0], m_v[1], m_v[2]); //������η���������ŵ�
		m_x = m_v.transpose()*m_Face_Q*m_v;
		return;
	}

	//����û�н�������̵�����£���Ҫ�����߸���Ĳ���ȡ���е������Сֵ
	vector<Vector4d> ptemp;
	STRUCT_SET_EDGEPAIR iter;
	int i = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		if (i == 3) {
			cout << "3 break;" << endl;
			break;
		}
		MyMesh::Point p= mesh.point(*fv_iter);
		ptemp.push_back(Vector4d(p[0], p[1], p[2], 1));
		i++;
	}
	ptemp.push_back( (ptemp[0] + ptemp[1]) / 2);
	ptemp.push_back( (ptemp[1] + ptemp[2]) / 2);
	ptemp.push_back( (ptemp[0] + ptemp[2]) / 2);
	ptemp.push_back( (ptemp[0] + ptemp[1] + ptemp[2]) / 3);

	for (i = 0; i < ptemp.size();i++) {
		MESHSETEDGE git;
		git.a=ptemp[i].transpose()*m_Face_Q*ptemp[i];
		git.i = i;
		iter.insert(git);
	}
	STRUCT_SET_EDGEPAIR::iterator it(iter.begin());
	m_x = it->a;
	m_v_mesh = MyMesh::Point(ptemp[it->i][0], ptemp[it->i][1], ptemp[it->i][2]);
}

int TriMeshSimplify::TriCollapse()
{
	//UpdateHalfEdge();//ÿ�ζ���Ҫ���м�����һ�£������Ƿ���ִ���ĵ㣻
	
	if (!is_Three_Tri(m_Mesh_Face, m_Collapse_Half)) {
		for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(m_Mesh_Face); fh_iter.is_valid(); fh_iter++) {
			m_Collapse_Half = mesh.halfedge_handle(fh_iter->idx());
			if (UpdateHalfEdge(mesh.opposite_halfedge_handle(m_Collapse_Half))) {//������������Ӧ��������ṹ��
				return mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx();
			}
		}
	}
	if (!DebugInfo2()) {
		return m_Mesh_Face.idx();
	}

	MyMesh::HalfedgeHandle halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));


	MyMesh::VertexHandle vp = mesh.to_vertex_handle(m_Collapse_Half);
	//MyMesh::VertexHandle vpf = mesh.from_vertex_handle(mesh.prev_halfedge_handle(halfh));

	//MyMesh::HalfedgeHandle halfn = mesh.prev_halfedge_handle(halfh);//�ڵ�һ��collapseʱ���Ѿ���ɾ���ˣ�

	//if (m_Mesh_Face.idx() == 259514) {
	//	//m_TriMesh_Map[it->i]->TriCollapse3();
	//	DebugInfo();
	//}
	

	mesh.collapse(m_Collapse_Half);

	//if (m_Mesh_Face.idx() == 259514) {
	//	//m_TriMesh_Map[it->i]->TriCollapse3();
	//	DebugInfo();
	//}

	mesh.collapse(halfn);
	
	mesh.set_point(vp, m_v_mesh);

	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
		m_Face_Arround[vf_iter->idx()]++;
	}
	return -1;
}

int TriMeshSimplify::TriCollapse3()
{
	//UpdateHalfEdge();
	MyMesh::VertexHandle vp = mesh.to_vertex_handle(m_Collapse_Half);

	MyMesh::HalfedgeHandle halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
	//MyMesh::HalfedgeHandle halfn = mesh.prev_halfedge_handle(halfh);//�ڵ�һ��collapseʱ���Ѿ���ɾ���ˣ�
	DebugInfo();
	mesh.collapse(m_Collapse_Half);

	DebugInfo();
	mesh.collapse(halfn);

	DebugInfo();
	mesh.set_point(vp, m_v_mesh);

	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
		m_Face_Arround[vf_iter->idx()]++;
	}
	return 0;
}


int TriMeshSimplify::is_TriMesh(MyMesh::VertexHandle vh) { //�����ж�һ������Χ�Ƿ���������������Σ�
	int i = 1;
	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vh); vf_iter.is_valid(); vf_iter++) {
		i++;
	}
	if (i == 3) {
		return 1;
	}
	if (i < 3) {
		return 2;
	}
	return 0;
}

bool TriMeshSimplify::is_TriMesh() {
	int i = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		i++;
	}
	if (i < 3) {
		return false;
	}
	return true;
}

void TriMeshSimplify::DebugInfo()
{
	cout << "Debug mesh Tri "<< m_Mesh_Face.idx()<<" :" << endl;
	cout<< "This collapse edge: " << m_Collapse_Half.idx() << "-"<<mesh.opposite_halfedge_handle(m_Collapse_Half)<<" "<<mesh.next_halfedge_handle(m_Collapse_Half)<<"-"<<mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))<<" "<<mesh.next_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))<<"-"<<mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))) << endl;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); fv_iter++) {
		cout << "Face Vertex: " << fv_iter->idx() << endl;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(mesh.vertex_handle(fv_iter->idx())); vf_iter.is_valid(); vf_iter++) {
			cout << "Face " << vf_iter->idx() << " halfedge: ";
			for (MyMesh::FaceHalfedgeCWIter fh_iter = mesh.fh_cwbegin(mesh.face_handle(vf_iter->idx())); fh_iter.is_valid(); fh_iter++) {
				cout << fh_iter->idx() << "-" << mesh.opposite_halfedge_handle(mesh.halfedge_handle(fh_iter->idx())) << " ";
			}
			cout << endl;
		}
	}
	cout << endl;
}

bool TriMeshSimplify::DebugInfo2() {
	int cc = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); fv_iter++) {
		int i = 0;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(mesh.vertex_handle(fv_iter->idx())); vf_iter.is_valid(); vf_iter++) {
			i++;
		}
		if (i == 3) {
			cc++;
		}
		else if (i<3) {
			return false;
		}
	}
	if (cc > 1) {
		return false;
	}
	return true;
}

MyTriOpenMesh::MyTriOpenMesh()
{
}
MyTriOpenMesh::~MyTriOpenMesh()
{
}

int MyTriOpenMesh::Readfile(const char * argg)
{
	// read mesh from stdin
	if (!OpenMesh::IO::read_mesh(mesh, argg, opt))
	{
		std::cerr << "Error: Cannot read mesh from " << std::endl;
		return 1;
	}

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();
	cout << "Read File Over!" << endl;
	return 0;
}

void MyTriOpenMesh::Writefile(const char *argg, int i) {
	cout << "Now is Writing File..." << endl;
	if (!OpenMesh::IO::write_mesh(mesh, argg, i)) //0 ascii 1 binary
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

void MyTriOpenMesh::Writefile(const char *argg) {
	if (!OpenMesh::IO::write_mesh(mesh, argg)) //obj file
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

int MyTriOpenMesh::MeshSimplification(float dest)
{
	if ((dest >= 1) || (dest <= 0)) {
		cout << "Your input is out of range, MainLoop" << endl;
		return 1;
	}
	int idest = mesh.n_faces()*dest;//�и�ת��
	FillTriMeshMap();
	printf("������Ŀͳ�� : ��ǰ:%d  Ŀ��:%d  ��ֵ:%d\n", mesh.n_faces(), mesh.n_faces() - idest, idest);

	while (idest>0) {
		idest -=CollapseIterator();
	}
	Release();
}

void MyTriOpenMesh::FillTriMeshMap()
{
	cout << "Fill the Map :"<<mesh.n_faces() << endl;
	int i = 0;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		i = f_it->idx();
		m_TriMesh_Map[i] = new TriMeshSimplify(mesh,mesh.face_handle(i));

		m_TriMesh_Map[i]->UpdateTriQ();

		m_TriMesh_Set.insert(m_TriMesh_Map[i]->ReturnTriSet());

		/*if (i == 65480) {
			m_TriMesh_Map[i]->DebugInfo();
		}*/
	}
}

int MyTriOpenMesh::CollapseIterator()
{
	STRUCT_SET_EDGEPAIR::iterator it(m_TriMesh_Set.begin());
	//cout << it->i<<":"<< endl;
	//if (it->i == 259523) {
	//	//m_TriMesh_Map[259523]->DebugInfo2();
	//}
	/*if (it->i == 258266) {
		m_TriMesh_Map[it->i]->DebugInfo();
	}
*/
	//309493
	int judge = m_TriMesh_Map[it->i]->TriCollapse();
	if (judge != -1) {
		//m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());//Ӧ����������������ɾ��������ɾ������
		//cout << "-1 collapse : " << it->i << " " << judge << endl;
		if (it->i == judge) {
			m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());
			return 0;
		}
		return CollapseIterator(judge);
	}

	INT_INT_MAP Face_Arround = m_TriMesh_Map[it->i]->ReturnTriMap();

	map<int, int>::iterator itm(Face_Arround.begin());
	int fm = 0;
	for (; itm != Face_Arround.end(); itm++) {
		m_TriMesh_Set.erase(m_TriMesh_Map[itm->first]->ReturnTriSet());
		//cout << "iftri:" << itm->first << " " << itm->second << ":" << m_TriMesh_Map[itm->first]->is_TriMesh() << endl;
		//if (itm->first == 65459) {
		//	cout << "1:0map " << it->i << " : " << itm->first<<" "<< itm->second << ":" << m_TriMesh_Map[itm->first]->is_TriMesh() << endl;
		//	m_TriMesh_Map[itm->first]->DebugInfo(); //65480
		//}
		if (itm->second > 1) {
			m_TriMesh_Map[itm->first]->UpdateTriQ();
			m_TriMesh_Set.insert(m_TriMesh_Map[itm->first]->ReturnTriSet());

			if (!m_TriMesh_Map[itm->first]->is_TriMesh()) {
				cout << "2:0 map: " << it->i <<" : "<< itm->first << endl; //250635   258266
				m_TriMesh_Map[itm->first]->DebugInfo();
			}
		}
		else {
			fm++;
		}
	}
	return fm;
}

int MyTriOpenMesh::CollapseIterator(int imf) {

	
	int judge = m_TriMesh_Map[imf]->TriCollapse();
	if (judge != -1) {
		m_TriMesh_Set.erase(m_TriMesh_Map[imf]->ReturnTriSet());//Ӧ����������������ɾ��������ɾ������
		cout << "-2 collapse : " << imf << endl;
		return 0;
	}

	INT_INT_MAP Face_Arround = m_TriMesh_Map[imf]->ReturnTriMap();

	map<int, int>::iterator itm(Face_Arround.begin());
	int fm = 0;
	for (; itm != Face_Arround.end(); itm++) {
		m_TriMesh_Set.erase(m_TriMesh_Map[itm->first]->ReturnTriSet());

		if (itm->second > 1) {
			m_TriMesh_Map[itm->first]->UpdateTriQ();
			m_TriMesh_Set.insert(m_TriMesh_Map[itm->first]->ReturnTriSet());

			if (!m_TriMesh_Map[itm->first]->is_TriMesh()) {
				cout << "iter2 2:0 map: " << imf << " : " << itm->first << endl; //250635   258266
				m_TriMesh_Map[itm->first]->DebugInfo();
			}
		}
		else {
			fm++;
		}
	}
	return fm;
}