#ifndef MESH_SIMPLIFIER_H
#define MESH_SIMPLIFIER_H


#include "../../octet.h"

class MeshSimplifier
{
public:
    MeshSimplifier();
    virtual ~MeshSimplifier() = 0{}

    virtual bool Simplify(octet::mesh& toSimplify) = 0{}
    virtual bool Simplify(const octet::mesh& toSimplify, octet::mesh* toWritten);
private:


};



#endif
