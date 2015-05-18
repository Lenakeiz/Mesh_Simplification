#ifndef EQ_MESH_SIMPLIFIER_H
#define EQ_MESH_SIMPLIFIER_H


#include "MeshSimplifier.h"

class EQMeshSimplifier: public MeshSimplifier
{
public:
    EQMeshSimplifier(){}
    ~EQMeshSimplifier()override {}

    bool Simplify(octet::mesh& toSimplify)override
    {}

private:
};



#endif