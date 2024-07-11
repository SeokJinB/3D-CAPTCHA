#pragma once
#include "pmp/surface_mesh.h"


class DrawComponent
{
public:
    void Init();

    void Draw();

    pmp::SurfaceMesh mesh;
};

