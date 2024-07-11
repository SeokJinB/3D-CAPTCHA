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

// BVH ���
class BV
{
public:
    int level;
    BoundingBox box;
    vector<BoundingBox> boxes;
    vector<Face> faces;
    BV* left_, * right_;
    BV(vector<Face> faces, int lv, SurfaceMesh& mesh);
    bool IsLeaf();
};

// BVH Ʈ��
class BVH
{
private:

public:
    vector<BV> roots;

    BVH(vector<Face> allFaces, SurfaceMesh& mesh);
};

// BV ������
inline BV::BV(vector<Face> fcs, int lv, SurfaceMesh& mesh) {
    faces = fcs;
    level = lv;
    left_ = nullptr;
    right_ = nullptr;

    Point minPoint = Point(FLT_MAX, FLT_MAX, FLT_MAX);
    Point maxPoint = Point(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (auto& face : faces)
    {
        BoundingBox faceBox = AABB(face, mesh);

        minPoint = pmp::min(faceBox.min(), minPoint);
        maxPoint = pmp::max(faceBox.max(), maxPoint);
        boxes.push_back(faceBox);
    }

    box = BoundingBox(minPoint, maxPoint);

    auto lengths = maxPoint - minPoint;

    // x,y,z�� �� ���� �� ��
    int longestAxis = 0;
    if (lengths[1] > lengths[longestAxis]) longestAxis = 1;
    if (lengths[2] > lengths[longestAxis]) longestAxis = 2;


    // ���� ���� ��
    double splitValue = 0.5 * (minPoint[longestAxis] + maxPoint[longestAxis]);

    vector<Face> leftFaces, rightFaces;

    // ���� ���ؿ� ���� �з�
    for (size_t i = 0; i < faces.size(); ++i)
    {
        if (boxes[i].max()[longestAxis] < splitValue)
        {
            leftFaces.push_back(faces[i]);      // ���� �ຸ�� ������ left
        }
        else
        {
            rightFaces.push_back(faces[i]);     // ���� �ຸ�� ũ�� right
        }
    }

    if (!leftFaces.empty() && !rightFaces.empty())
    {
        if (leftFaces.size() > 2)
        {
            left_ = new BV(leftFaces, level + 1, mesh);
        }
        if (rightFaces.size() > 2)
        {
            right_ = new BV(rightFaces, level + 1, mesh);
        }
    }
}

// ���(BV)�� �������� �˻��ϴ� �Լ�
inline bool BV::IsLeaf()
{
    return left_ == nullptr && right_ == nullptr;
}

// BVH ������
inline BVH::BVH(vector<Face> allFaces, SurfaceMesh& mesh)
{
    roots.push_back(BV(allFaces, 0, mesh));
}
