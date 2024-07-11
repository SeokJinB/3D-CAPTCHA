#pragma once
#pragma once
#include "pmp/types.h"
#include "DrawComponent.h"
#include "pmp/bounding_box.h"
#include "pmp/algorithms/normals.h"

using namespace pmp;
using namespace std;

// �� ���� ��(face)�� AABB�� ���δ� �Լ�
BoundingBox AABB(Face face, SurfaceMesh& mesh)
{
    BoundingBox box;
    for (auto v : mesh.vertices(face))
    {
        auto& p = mesh.position(v);
        box += p;
    }
    return box;
}

// ��Ʈ�� ���
class OctreeNode
{
public:
    int level;
    BoundingBox box;
    vector<BoundingBox> faceBoxes;
    vector<Face> faces;
    OctreeNode* children[8];

    OctreeNode(vector<Face> faces, int lv, SurfaceMesh& mesh);
    bool IsLeaf();
    ~OctreeNode();
};

// ��Ʈ��
class Octree
{
public:
    OctreeNode* root;

    Octree(vector<Face> allFaces, SurfaceMesh& mesh);
    ~Octree();
};

// OctreeNode ������
inline OctreeNode::OctreeNode(vector<Face> fcs, int lv, SurfaceMesh& mesh) {
    faces = fcs;
    level = lv;
    for (int i = 0; i < 8; ++i) children[i] = nullptr;

    Point minPoint(FLT_MAX, FLT_MAX, FLT_MAX);
    Point maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (auto& face : faces)
    {
        BoundingBox faceBox = AABB(face, mesh);
        minPoint = pmp::min(faceBox.min(), minPoint);
        maxPoint = pmp::max(faceBox.max(), maxPoint);
        faceBoxes.push_back(faceBox);
    }

    box = BoundingBox(minPoint, maxPoint);

    // ���� ������
    Point center = 0.5 * (minPoint + maxPoint);

    // ���� ���� ����� �й�
    vector<Face> childFaces[8];

    for (size_t i = 0; i < faces.size(); ++i)
    {
        Point faceCenter = 0.5 * (faceBoxes[i].min() + faceBoxes[i].max());
        int octant = 0;
        if (faceCenter[0] >= center[0]) octant |= 4;
        if (faceCenter[1] >= center[1]) octant |= 2;
        if (faceCenter[2] >= center[2]) octant |= 1;
        childFaces[octant].push_back(faces[i]);
    }

    // ���� ��� ����
    for (int i = 0; i < 8; ++i)
    {
        if (!childFaces[i].empty())
        {
            children[i] = new OctreeNode(childFaces[i], level + 1, mesh);
        }
    }
}

// ��尡 �������� �˻��ϴ� �Լ�
inline bool OctreeNode::IsLeaf()
{
    for (int i = 0; i < 8; ++i)
    {
        if (children[i] != nullptr) return false;
    }
    return true;
}

// OctreeNode �Ҹ���
inline OctreeNode::~OctreeNode()
{
    for (int i = 0; i < 8; ++i)
    {
        delete children[i];
    }
}

// Octree ������
inline Octree::Octree(vector<Face> allFaces, SurfaceMesh& mesh)
{
    root = new OctreeNode(allFaces, 0, mesh);
}

// Octree �Ҹ���
inline Octree::~Octree()
{
    delete root;
}