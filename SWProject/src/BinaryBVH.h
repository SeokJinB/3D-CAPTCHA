#pragma once
#include "pmp/types.h"
#include "DrawComponent.h"
#include "pmp/bounding_box.h"
#include "pmp/algorithms/normals.h"

using namespace pmp;
using namespace std;

// 한 개의 면(face)을 AABB로 감싸는 함수
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

// BVH 노드
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

// BVH 트리
class BVH
{
private:

public:
    vector<BV> roots;

    BVH(vector<Face> allFaces, SurfaceMesh& mesh);
};

// BV 생성자
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

    // x,y,z축 중 가장 긴 축
    int longestAxis = 0;
    if (lengths[1] > lengths[longestAxis]) longestAxis = 1;
    if (lengths[2] > lengths[longestAxis]) longestAxis = 2;


    // 분할 기준 축
    double splitValue = 0.5 * (minPoint[longestAxis] + maxPoint[longestAxis]);

    vector<Face> leftFaces, rightFaces;

    // 분할 기준에 따라 분류
    for (size_t i = 0; i < faces.size(); ++i)
    {
        if (boxes[i].max()[longestAxis] < splitValue)
        {
            leftFaces.push_back(faces[i]);      // 기준 축보다 작으면 left
        }
        else
        {
            rightFaces.push_back(faces[i]);     // 기준 축보다 크면 right
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

// 노드(BV)가 리프인지 검사하는 함수
inline bool BV::IsLeaf()
{
    return left_ == nullptr && right_ == nullptr;
}

// BVH 생성자
inline BVH::BVH(vector<Face> allFaces, SurfaceMesh& mesh)
{
    roots.push_back(BV(allFaces, 0, mesh));
}
