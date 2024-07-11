#include "DrawComponent.h"
#include "gl/freeglut.h"
#include "GL/GLU.h"
//#include "BinaryBVH.h"
#include "TernaryBVH.h"

#include "pmp/io/io.h"
#include "pmp/algorithms//normals.h"
#include "pmp/bounding_box.h"
#include "pmp/mat_vec.h"

#include <queue>
#include <random>

#define LINE_WIDTH 2.0	// Wireframe ������ �� �׷����� ���� ���� (������ ����)
#define POINT_SIZE 2.0	// Point Cloud ������ �� �׷����� ���� ũ�� (������ ����)
#define SPHERE_SLICES 15	// �� slice ���� ��
#define SPHERE_STACKS 15	// �� stack ���� ��
#define NOISE_PROBABILITY 0.1 // Noise ������ Ȯ��
#define NOISE_POINT_SIZE 7.0 // Noise point�� ũ��
#define NOISE_POINT_NUM 100000	// Noise point�� ��ü ����

using namespace std;
using namespace pmp;

BVH* bvh = nullptr;
vector<Face> faces;
int lv;
int currentLevel;
int nodeCount;
int leafLevel;

int treeHeight(BV& node)
{
	if (node.IsLeaf())
	{
		return 0;
	}
	else
	{
		int leftHeight = 0;
		int rightHeight = 0;
		int midHeight = 0;

		if (node.left_ != nullptr)
		{
			leftHeight = treeHeight(*node.left_);
		}

		if (node.right_ != nullptr)
		{
			rightHeight = treeHeight(*node.right_);
		}

		if (node.mid_ != nullptr)
		{
			midHeight = treeHeight(*node.mid_);
		}

		return 1 + max(leftHeight, max(rightHeight, midHeight));
	}
}

// Ʈ���� ��ü ���� ��� �Լ�
int maxHeight(BVH& bvh)
{
	int maxHeight = 0;

	// �� ��Ʈ ��帶�� Ʈ���� ���̸� ����ϰ� ���� ū ���� ����
	for (size_t i = 0; i < bvh.roots.size(); ++i)
	{
		int height = treeHeight(bvh.roots[i]);
		maxHeight = max(maxHeight, height);
	}

	return maxHeight;
}

GLfloat* randomRGB(float prob)
{
	GLfloat rgb[3];

	if (prob <= 0.33)
	{
		rgb[0] = 1; rgb[1] = 0; rgb[2] = 0;
	}
	else if (prob > 0.33 && prob <= 0.66)
	{
		rgb[0] = 0; rgb[1] = 1; rgb[2] = 0;
	}
	else
	{
		rgb[0] = 0; rgb[1] = 0; rgb[2] = 1;
	}

	return rgb;
}

void DrawWireAABB(BoundingBox& box)
{
	Point minPoint = box.min();
	Point maxPoint = box.max();

	glLineWidth(LINE_WIDTH);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> colorProb(0.0, 1.0);
	auto prob = colorProb(gen);
	GLfloat* rgb = randomRGB(prob);
	glColor3fv(rgb);

	// �Ʒ��� Drawing
	glBegin(GL_LINE_LOOP);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glEnd();

	// ���� Drawing
	glBegin(GL_LINE_LOOP);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glEnd();


	// ���� Drawing
	glBegin(GL_LINE_LOOP);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glEnd();
}

// AABB Drawing
void DrawSolidAABB(BoundingBox& box)
{
	Point minPoint = box.min();
	Point maxPoint = box.max();

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> colorProb(0.0, 1.0);
	auto prob = colorProb(gen);
	GLfloat* rgb = randomRGB(prob);
	glColor3fv(rgb);

	// �Ʒ��� Drawing
	glBegin(GL_POLYGON);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glEnd();

	// ���� Drawing
	glBegin(GL_POLYGON);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glEnd();


	// ���� Drawing
	glBegin(GL_POLYGON);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3d(maxPoint[0], minPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3d(maxPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(maxPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3d(minPoint[0], minPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], maxPoint[2]);
	glVertex3d(minPoint[0], maxPoint[1], minPoint[2]);
	glVertex3d(minPoint[0], minPoint[1], minPoint[2]);
	glEnd();
}

// Bounding Sphere Drawing
void DrawSphere(BoundingBox& box)
{
	Point centerPoint = box.center();
	Point maxPoint = box.max();

	double radius = distance(centerPoint, maxPoint);

	GLUquadric* quad = gluNewQuadric();

	glLineWidth(LINE_WIDTH);
	glPointSize(POINT_SIZE);

	// 1) �� �������� ������
	gluQuadricDrawStyle(quad, GLU_LINE);

	// 2) �� �������� ������(����� �����ϴ� �����ڸ��� �׷����� ����)
	//gluQuadricDrawStyle(quad, GLU_SILHOUETTE);

	// 3) ����Ʈ �������� ������
	//gluQuadricDrawStyle(quad, GLU_POINT);

	// 4) �ٰ��� �⺻ �������� ������
	//gluQuadricDrawStyle(quad, GLU_FILL);

	glPushMatrix();
	glTranslated(centerPoint[0], centerPoint[1], centerPoint[2]);
	gluSphere(quad, radius, SPHERE_SLICES, SPHERE_STACKS);
	glPopMatrix();

	gluDeleteQuadric(quad);
}

// Mesh Drawing
void DrawMesh(BV*& bv, SurfaceMesh& mesh, auto& normals)
{
	for (auto& f : bv->faces)
	{
		glBegin(GL_TRIANGLES);
		for (auto v : mesh.vertices(f))
		{
			auto& p = mesh.position(v);
			auto& n = normals[v];
			glNormal3dv(n.data());
			glVertex3dv(p.data());
		}
		glEnd();
	}
}

// Noise Drawing
void DrawRandomNoise(BV*& bv)
{
	BoundingBox box = bv->box;
	Point centerPoint = box.center();
	Point maxPoint = box.max();

	double radius = distance(centerPoint, maxPoint);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> disTheta(0.0, 2.0 * M_PI);
	uniform_real_distribution<> disPi(0.0, 1.0);
	uniform_real_distribution<> pointProb(0.0, 1.0);
	uniform_real_distribution<> colorProb(0.0, 1.0);

	for (int i = 0; i < NOISE_POINT_NUM; i++)
	{
		// ���� Ȯ���� ������ ����
		if (pointProb(gen) <= NOISE_PROBABILITY)
		{
			auto theta = disTheta(gen);
			auto pi = acos(2.0 * disPi(gen) - 1.0);

			auto x = centerPoint[0] + (1.5 * radius) * sin(pi) * cos(theta);
			auto y = centerPoint[1] + (1.5 * radius) * sin(pi) * sin(theta);
			auto z = centerPoint[2] + (1.5 * radius) * cos(pi);

			auto prob = colorProb(gen);

			GLfloat* rgb = randomRGB(prob);

			glBegin(GL_POINTS);
			glColor3fv(rgb);
			glVertex3f(x, y, z);
		}
	}
	glEnd();
}

// mesh�� ���δ� Bounding Volume �׸���
void DrawBVbyLevel(BVH* bvh, SurfaceMesh& mesh, auto& normals, int lv)
{
	std::queue<BV*> q;

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> colorProb(0.0, 1.0);

	for (auto& bv : bvh->roots)
	{
		q.push(&bv);
	}

	while (!q.empty()) {
		auto& bv = q.front();

		// Noise ����
		if (bv->level == 0)
		{
			glPointSize(NOISE_POINT_SIZE);
			DrawRandomNoise(bv);
		}

		// lv ���� ��带 Drawing
		if (bv->level == lv)
		{
			BoundingBox bvBox = bv->box;
			//DrawMesh(bv, mesh, normals);

			// ���� �ڽ� BV Drawing
			if (bv->left_ != nullptr)
			{
				auto prob = colorProb(gen);
				GLfloat* rgb = randomRGB(prob);
				//glColor3fv(rgb);

				BV* leftBV = bv->left_;

				// 1) Wireframe AABB
				//DrawWireAABB(leftBV->box);

				// 2) Solid AABB
				//DrawSolidAABB(leftBV->box);

				// 3) Sphere
				//DrawSphere(leftBV->box);

				q.push(leftBV);
			}

			// ������ �ڽ� BV Drawing
			if (bv->right_ != nullptr)
			{
				auto prob = colorProb(gen);
				GLfloat* rgb = randomRGB(prob);
				//glColor3fv(rgb);

				BV* rightBV = bv->right_;

				// 1) Wireframe AABB
				//DrawWireAABB(rightBV->box);

				// 2) Solid AABB
				//DrawSolidAABB(rightBV->box);

				// 3) Sphere
				//DrawSphere(rightBV->box);

				q.push(rightBV);
			}

			// ��� �ڽ� BV Drawing
			if (bv->mid_ != nullptr)
			{
				auto prob = colorProb(gen);
				GLfloat* rgb = randomRGB(prob);
				glColor3fv(rgb);

				BV* midBV = bv->mid_;

				// 1) Wireframe AABB
				//DrawWireAABB(midBV->box);

				// 2) Solid AABB
				//DrawSolidAABB(midBV->box);

				// 3) Sphere
				DrawSphere(midBV->box);

				q.push(midBV);
			}
		}
		else
		{
			if (bv->left_ != nullptr)
				q.push(bv->left_);
			if (bv->right_ != nullptr)
				q.push(bv->right_);
			if (bv->mid_ != nullptr)
				q.push(bv->mid_);
		}

		q.pop();

	}
}

void DrawComponent::Init()
{
	// load the model
	pmp::read(mesh, "models\\horse.obj");
	pmp::vertex_normals(mesh);

	auto nf = mesh.n_faces();
	auto nv = mesh.n_vertices();
	std::cout << "#f " << nf << " #v " << nv << std::endl;

	for (auto f : mesh.faces())
	{
		faces.push_back(f);
	}

	bvh = new BVH(faces, mesh);
	lv = maxHeight(*bvh);
	currentLevel = bvh->GetMinLeafLevel();

	cout << "Ʈ���� ��ü ���� : " << lv << endl;
	cout << "���� ���� : " << currentLevel << endl;
}

void DrawComponent::Draw()
{
	auto normals = mesh.vertex_property<pmp::Normal>("v:normal");

	DrawBVbyLevel(bvh, mesh, normals, currentLevel);
}