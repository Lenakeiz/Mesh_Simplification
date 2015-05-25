#ifndef MESH_SIMPLIFIER_H
#define MESH_SIMPLIFIER_H


#include "../../octet.h"

class MeshSimplifier
{
public:
    MeshSimplifier(){}
    virtual ~MeshSimplifier() = 0{}



    virtual bool Init(octet::ref<octet::mesh> toSimplify) = 0{return false;}

    virtual bool Simplify(octet::ref<octet::mesh> toSimplify) = 0{return false;}
    virtual bool Simplify(const octet::ref<octet::mesh> toSimplify, octet::mesh* toWritten){return false;}
private:
};



#endif
