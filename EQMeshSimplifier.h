#ifndef EQ_MESH_SIMPLIFIER_H
#define EQ_MESH_SIMPLIFIER_H


#include "MeshSimplifier.h"
#include <assert.h>
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>

class EQMeshSimplifier : public MeshSimplifier
{
private:
    //Three indexes representing a triangle
    struct IndexTriangle
    {
        IndexTriangle(int a, int b, int c) :
            indexA(a), indexB(b), indexC(c)
        {}
        bool Contains(int argA)
        {
            return (indexA==argA||
                indexB==argA||
                indexC==argA);
        }

        bool ContainsBoth(int argA, int argB)
        {
            return ((indexA == argA || indexB == argA ||
                indexC == argA) && (indexA == argB || indexB == argB ||
                indexC == argB));
        }
        void Replace(int argA, int data)
        {
            if (indexA == argA){indexA = data;}
            else if (indexB == argA){ indexB = data; }
            else if (indexC == argA){ indexC = data; }
        }

        unsigned int indexA,
            indexB,
            indexC;
    };
    //Two indexes representing an endge, and a vector with indexes to the triangles it belongs to
    struct IndexEdge{
        IndexEdge(int a, int b):indexA(a),indexB(b)
        {}
        
        bool operator <(const IndexEdge& e) const
        {
            return indexA<e.indexA ||
                (e.indexA==indexA&&indexB<e.indexB);
        }

        bool IsEdge(int a, int b)
        {
            return (indexA==a && indexB==b)||
                (indexB==a&&indexA==b);
        }

        bool IsBorder()
        {
            return triangles.size()!=0;
        }

        

    int indexA,
        indexB;
    std::vector<int> triangles;
    };
    //A valid contraction with the cost (lowest=best), new position and indexes to contract
    struct Contraction
    {
        Contraction():Contraction(-1,octet::vec3(),0,0)
        {}
        Contraction(float c, octet::vec3 vB, int v1, int v2) :
            cost(c), vBar(vB), v1Index(v1), v2Index(v2){}

        bool operator <(const Contraction& c) const
        {
            return cost > c.cost;
        }

        bool Contains(int argA, int argB)
        {
            return (v1Index==argA&&v2Index==argB)||
                (v2Index==argA&&v1Index==argB);
        }

        bool Replace(int argA, int data)
        {
            if (v1Index == argA)v1Index = data;
            else if (v2Index == argA)v2Index = data;
            else { return false; }
            return true;
        }
        float cost;

        octet::vec3 vBar;

        int v1Index;
        int v2Index;
    };
    //A memory alignment struct (currently only for default vertexes)
    struct Vertex{
        Vertex()
        {
            assert(size>0);
        }
        Vertex(float* f)
        {
            arr[0]=  f[0];
            arr[1] = f[1];
            arr[2] = f[2];
            arr[3] = f[3];
            arr[4] = f[4];
            arr[5] = f[5];
            arr[6] = f[6];
            arr[7] = f[7];
        }
        
        void Write(float*& f)
        {
            f[0] = arr[0];
            f[1] = arr[1];
            f[2] = arr[2];
            f[3] = arr[3];
            f[4] = arr[4];
            f[5] = arr[5];
            f[6] = arr[6];
            f[7] = arr[7];
            f+=8;
        }

        float arr[8];


        static int posOffset;
        static int normalOffset;
        static int UVOffset;
        static int size;

        static void SetOffsets(int s, int posOff, int normOff, int uvOff)
        {
            size=s;
            posOffset=posOff;
            assert(posOffset+2<=size);
            normalOffset=normOff;
            assert(normalOffset+2<=size);
            UVOffset=uvOff;
            assert(UVOffset+1<=size);
        }

        octet::vec3 GetPos(){ return octet::vec3(arr[posOffset],arr[posOffset+1],arr[posOffset+2]);}
        octet::vec3 GetNorm(){ return octet::vec3(arr[normalOffset], arr[normalOffset+1], arr[normalOffset+2]); }
    };
public:
    EQMeshSimplifier() :currentMesh(NULL), pairThresholdSqrd(0),contractionMaxCost(FLT_MAX),
    triangleTargetNumber(0),vertexTargetNumber(0),removalsPerStep(INT_MAX){}
    ~EQMeshSimplifier()override {}



    void SetPairThreshold(float t)
    {
        pairThresholdSqrd = t*t;
    }
    void SetContractionMaxCost(float cost)
    {
        contractionMaxCost=cost;
    }
    void SetRemovalsPerSimplification(int numRemovals)
    {
    removalsPerStep=numRemovals;
    }
    void SetTargetNumVertices(int desiredNum)
    {
    vertexTargetNumber=desiredNum;
    }
    void SetTargetNumTriangles(int desiredNum)
    {
    triangleTargetNumber=desiredNum;
    }

    bool Init(octet::ref<octet::mesh> toSimplify)override
    {
        if (&(*toSimplify) == &(*currentMesh))
        {
            return false;
        }
        Reset();
        currentMesh = toSimplify;
        unsigned int* idx = 0;
        float * vtx = 0;
        octet::gl_resource::rwlock vLock(currentMesh->get_vertices());
        octet::gl_resource::rwlock iLock(currentMesh->get_indices());

        vtx = (float*)vLock.u16();
        idx = (unsigned int*)iLock.u16();
        InitMeshAttribOffsets();

            
        if (currentMesh->get_num_indices() > 0)
        {
            BuildVertices(vtx);
            BuildTriangles(idx);
            BuildMatrices(idx, vtx);
            BuildPairs(idx, vtx);
            ComputeCosts(idx, vtx);
        }
        return true;
    }

    bool Simplify(octet::ref<octet::mesh> toSimplify)override
    {
        if (&(*toSimplify) != &(*currentMesh))
        {
            if (!Init(toSimplify))
            {
                return false;
            }
        }
        unsigned int * idx = 0;
        float * vtx = 0;
        GetResources(idx, vtx);

        RemovePairs(idx, vtx);

        RebuildMesh();
        return true;
    }


private:
    void Reset()
    {
        currentMesh = nullptr;

        pairs.clear();
        triangles.clear();
        contractions.clear();
        errorMatrices.clear();

    }
    void GetResources(unsigned int* & idx, float* & vtx)
    {
            vtx = &vertices[0].arr[0];
            idx = (unsigned int*)triangles.data();
    }

    void InitMeshAttribOffsets()
    {
        //we only care about vertex position for now, so extract the position offest from the 
        //vertex struct in the mesh, as well as the total size
        int numAttributes = currentMesh->get_num_slots();
        int vertexStructSize=0;
        int posOffset=-1;
        int normalOffset=-1;
        int uvOffset=-1;
        for (int i = 0; i < numAttributes; ++i)
        {
            if (posOffset < 0 && currentMesh->get_attr(i) == octet::attribute_pos)
            {
                posOffset = vertexStructSize;
            }
            if (normalOffset < 0 && currentMesh->get_attr(i) == octet::attribute_normal)
            {
                normalOffset=vertexStructSize;
            }
            if (uvOffset < 0 && currentMesh->get_attr(i) == octet::attribute_texcoord)
            {
                uvOffset = vertexStructSize;
            }
            vertexStructSize += currentMesh->get_size(i);
        }
        Vertex::SetOffsets(vertexStructSize,posOffset,normalOffset,uvOffset);
    }

    void BuildMatrices(const unsigned int* idx, const float* vtx)
    {
        if (currentMesh == nullptr)
        {
            return;
        }
        //clear and reset matrix vector
        errorMatrices.clear();
        errorMatrices.resize(currentMesh->get_num_vertices());
        //matrix default initilise to identity matrix which is not what we want
        for (int i = 0; i < errorMatrices.size(); ++i)
        {
            errorMatrices[i].init(octet::vec4(),
                octet::vec4(),
                octet::vec4(),
                octet::vec4());
        }

        //per face of mesh add data to vertex matrix
        for (int i = 0; i < currentMesh->get_num_indices(); i += 3)
        {
            int indexA = idx[i],
                indexB = idx[i + 1],
                indexC = idx[i + 2];

            octet::vec3 va(GetVertexPos(indexA)),
                vb(GetVertexPos(indexB)),
                vc(GetVertexPos(indexC));
            //calculate triangle normal to act as plane for triangle
            octet::vec3 planeNorm = (vb - va).cross(vc - va);
            planeNorm = planeNorm.normalize();
            float d = planeNorm.dot(vc);

            AddPlaneToMatrix(planeNorm, d, errorMatrices[indexA]);
            AddPlaneToMatrix(planeNorm, d, errorMatrices[indexB]);
            AddPlaneToMatrix(planeNorm, d, errorMatrices[indexC]);
        }
    }
    
    octet::vec3 GetVertexPos(int index)
    {
        assert(vertices.size() > index);
        
            return vertices[index].GetPos();
        
    }

    octet::vec3 CalculateTriangleNormal(int index)
    {
        return (GetVertexPos(triangles[index].indexB)-GetVertexPos(triangles[index].indexA)).cross(
            GetVertexPos(triangles[index].indexC)-GetVertexPos(triangles[index].indexA));
    
    }

    float* GetVertexPosPointer( int index)
    {
        assert(vertices.size() > index);
        
            return &vertices[index].arr[Vertex::posOffset];
        
    }
    void AddPlaneToMatrix(octet::vec3& norm, float dist, octet::mat4t& matrix)
    {
        /*
        Plane = [a,b,c,d] where a^2 + b^2 + c^2 =1 (eg normalised)
        d= distance from oritin = dist
                 _____________________
        Matrix = |a^2 | ab | ac | ad |
                 | ab |b^2 | bc | bd |
                 | ac | bc |c^2 | cd |
                 | ad | bd | cd |d^2 |
                 ---------------------
        */
        matrix += octet::mat4t(
            octet::vec4(norm.x()*norm.x(), norm.x()*norm.y(), norm.x()*norm.z(), norm.x()*dist),
            octet::vec4(norm.x()*norm.y(), norm.y()*norm.y(), norm.y()*norm.z(), norm.y()*dist),
            octet::vec4(norm.x()*norm.z(), norm.y()*norm.z(), norm.z()*norm.z(), norm.z()*dist),
            octet::vec4(norm.x()*dist    , norm.y()*dist    , norm.z()*dist    , dist*dist));
    }

    void BuildTriangles(const unsigned int* idx)
    {
        for (int i = 0; i < currentMesh->get_num_indices(); i += 3)
        {
            triangles.push_back(IndexTriangle(idx[i], idx[i + 1], idx[i + 2]));
        }
    }
    void BuildVertices(float * vtx)
    {
        vertices.resize(currentMesh->get_num_vertices());
        memcpy(vertices.data(), vtx, currentMesh->get_num_vertices()*Vertex::size*sizeof(float));
    }

    void BuildPairs(unsigned int* idx, float* vtx)
    {
        //look through all proximity pairs based on threshold
        if (pairThresholdSqrd > 0)
        {
            for (int i = 0; i < currentMesh->get_num_vertices() - 1; ++i)
            {
                octet::vec3 a = GetVertexPos(i);
                for (int j = i + 1; j < currentMesh->get_num_vertices(); ++j)
                {
                    octet::vec3 b = GetVertexPos(j);
                    if ((a - b).squared() <= pairThresholdSqrd)
                    {
                        pairs.insert(IndexEdge(i, j));
                    }
                }
            }
        }
        //
        for (int i = 0;i < triangles.size(); ++i)
        {
           int indexA=triangles[i].indexA,
               indexB=triangles[i].indexB,
               indexC=triangles[i].indexC;
           //a self sorting method designed to stop duplicates, we always construct objects in lowest to highest
           indexA<indexB ? BorderEdgeInsertionCheck(indexA, indexB, i) : BorderEdgeInsertionCheck(indexB, indexA, i);
           indexA<indexC ? BorderEdgeInsertionCheck(indexA, indexC, i) : BorderEdgeInsertionCheck(indexC, indexA, i);
           indexC<indexB ? BorderEdgeInsertionCheck(indexC, indexB, i) : BorderEdgeInsertionCheck(indexB, indexC, i);
        }
        for (std::set<IndexEdge>::iterator it = pairs.begin(); it != pairs.end(); ++it)
        {
            if (it->triangles.size() == 1)
            {
                octet::vec3 norm=CalculateTriangleNormal(it->triangles[0]);
                octet::vec3 vec=GetVertexPos(it->indexB)-GetVertexPos(it->indexA);
                octet::vec3 perpNorm=norm.cross(vec);
                perpNorm=perpNorm.normalize();
                float dist=perpNorm.dot(GetVertexPos(it->indexA));

                AddPlaneToMatrix(perpNorm,dist,errorMatrices[it->indexA]);
                AddPlaneToMatrix(perpNorm,dist,errorMatrices[it->indexB]);
            }
        }
    }
    
    void BorderEdgeInsertionCheck(int indexA, int indexB,int triangleIndex)
    {
        //here we check if the edge already exists
        std::set<IndexEdge>::iterator it;
        IndexEdge edge(indexA,indexB);
        edge.triangles.push_back(triangleIndex);
        it=pairs.find(edge);
        //if it already exits then we need to add the triangle to it
        if (it!=pairs.end())
        {
            //however sets only return const objects to stop sorting problems,
            //as we have overloaded the < operator we know that changing .triangles wont effect the sort order
            const IndexEdge* iePtr=&(*it);
            const_cast<IndexEdge*>(iePtr)->triangles.push_back(triangleIndex);
        }
        else
        {
            pairs.insert(edge);
        }
    }

    void ComputeCosts(unsigned int* idx, float* vtx)
    {
        //look at each pair
        std::set<IndexEdge>::iterator it;
        int numContractions=0;
        contractions.push_back(Contraction());
        for (it = pairs.begin(); it != pairs.end(); ++it)
        {
                //sum the plane matricies
            if(CalcContraction(vtx,it->indexA,it->indexB,&contractions.back()))
            {
                    contractions.push_back(Contraction());
                ++numContractions;
            }
        }
        contractions.pop_back();
        //sort the contractions to keep the smallest at the start of the vector
        std::sort(contractions.begin(), contractions.end());
    }

    bool CalcContraction(float* vtx, int indexA, int indexB,Contraction* c)
    {
        octet::mat4t q = errorMatrices[indexA] + errorMatrices[indexB];
        octet::mat4t qInv = q;
        qInv[3] = octet::vec4(0, 0, 0, 1);
        
        //if they are invertable then we can find a perfect position!
        if (qInv.det4x4() != 0)
        {
            qInv = qInv.inverse4x4();
            octet::vec4 vBar = octet::vec4(-qInv[0].w(),-qInv[1].w(),-qInv[2].w(),1);
            float cost = (vBar*q).squared();
            if (!CheckInversion(vBar, indexA, indexB))
            {
                *c=Contraction(cost, vBar, indexA, indexB);
                return true;
            }
            return false;
        }

        //if not we make an approximation [A] REPLACE THIS WITH CALCULUS TO FIND BEST vBar

        octet::vec3 a = GetVertexPos(indexA);
        octet::vec3 b = GetVertexPos(indexB);
        octet::vec4 vBar = ((a+b)/2).xyz1();
        float cost = (vBar*q).squared();
        if (!CheckInversion(vBar, indexA, indexB))
        {
            *c = Contraction(cost, vBar, indexA, indexB);
            return true;
        }
        return false;
    }

    bool CheckInversion(octet::vec4& vBar, int a, int b)
    {
        for (int i = 0; i < triangles.size(); ++i)
        {
            int targetReplace = -1;
            if (triangles[i].Contains(a))
            {
                targetReplace = a;
            }
            else if (triangles[i].Contains(b))
            {
                targetReplace=b;
            }
            if(targetReplace!=-1)
            {
                IndexTriangle t=triangles[i];
                octet::vec3 startingNorm=CalculateTriangleNormal(i);
                octet::vec3 v1,v2,v3;
                t.indexA== targetReplace? v1=vBar.xyz() : v1=GetVertexPos(t.indexA);
                t.indexB == targetReplace ? v2 = vBar.xyz() : v2 = GetVertexPos(t.indexB);
                t.indexC == targetReplace ? v3 = vBar.xyz() : v3 = GetVertexPos(t.indexC);

                octet::vec3 endNorm=(v2-v1).cross(v3-v1);
                float test=endNorm.dot(startingNorm);
                if (endNorm.dot(startingNorm) < 0)
                {
                    return true;
                }
            }
        } 
        return false;
    }

    void RemovePairs(unsigned int *idx, float* vtx){

        int totalNumVertex = currentMesh->get_num_vertices();
        int totalNumFaces = triangles.size();


        int numRemove=removalsPerStep;
        while (contractions.size() > 0)
        {
            if (numRemove == 0||totalNumVertex<=vertexTargetNumber||triangles.size()<=triangleTargetNumber)
            {
                return;
            }
            Contraction curr = contractions.back();
            contractions.pop_back();
            if (curr.cost > contractionMaxCost)
            {
                return;
            }


            int newIndex = Merge(vtx, curr.v1Index, curr.v2Index, curr.vBar);
            //Merge() -into slot of lowest index


            //remove triangles who have both vertices being removed, these triangles would dissapear
            RemoveTris(curr.v1Index, curr.v2Index);

            //ReplaceIndex() replace all references to x1 and x2 with x-
            ReplaceIndex(curr.v1Index, curr.v2Index, newIndex);

            //UpdateCosts() update all pair costs relevent to x-
            UpdateContractions(vtx, curr.v1Index, curr.v2Index, newIndex);
            std::sort(contractions.begin(), contractions.end());
            totalNumVertex--;

           numRemove--;
            
        }
    }

    int Merge(float * vtx, int indexA, int indexB, octet::vec3& newAB)
    {
        octet::vec3p* aPos =(octet::vec3p*)&vertices[indexA].arr[Vertex::posOffset];
        octet::vec3p* aNormalAligned = (octet::vec3p*)&vertices[indexA].arr[Vertex::normalOffset];
        octet::vec3 aNormal=vertices[indexA].GetNorm();
        octet::vec3 bNormal=vertices[indexB].GetNorm();
        *aPos = newAB;
        *aNormalAligned = (aNormal+bNormal).normalize();
        return indexA;
    }

    void RemoveTris(int indexA, int indexB)
    {
        for (int i = 0; i < triangles.size(); ++i)
        {
            if (triangles[i].ContainsBoth(indexA, indexB))
            {
                std::swap(triangles[i], triangles.back());
                triangles.pop_back();
                i--;
            }
        }
    }

    void ReplaceIndex(int targetIndexA, int targetIndexB, int replacement, bool calculateMatrix=true)
    {
        for (int i = 0; i < triangles.size(); ++i)
        {
            triangles[i].Replace(targetIndexA, replacement);
            triangles[i].Replace(targetIndexB, replacement);
        }
        if (calculateMatrix)
        {
            errorMatrices[replacement] = errorMatrices[targetIndexA] + errorMatrices[targetIndexB];
        }
    }

    void UpdateContractions(float* vtx, int targetIndexA, int targetIndexB, int replacement)
    {
        
        for (int i = 0; i < contractions.size(); ++i)
        {
            bool remove=false;
            if (contractions[i].Contains(replacement,targetIndexB))
            {
                remove=true;

            }
            if (contractions[i].Replace(targetIndexA, replacement)&&!remove)
            {
                if (!CalcContraction(vtx, contractions[i].v1Index, contractions[i].v2Index, &contractions[i]))
                {
                    remove=true;
                }
            }
            if (contractions[i].Replace(targetIndexB, replacement)&&!remove)
            {
                if (!CalcContraction(vtx, contractions[i].v1Index, contractions[i].v2Index, &contractions[i]))
                {
                    remove=true;
                }
            }
            if (remove)
            {
                std::swap(contractions[i], contractions[contractions.size() - 1]);
                contractions.pop_back();
                i--;
                continue;
            }
        }
    }


    void RebuildMesh(){
            octet::gl_resource::rwlock vLock(currentMesh->get_vertices());
            octet::gl_resource::rwlock iLock(currentMesh->get_indices());
            unsigned int * idx;
            float * vtx;

            vtx = (float*)vLock.u16();
            idx = (unsigned int*)iLock.u16();

            std::map<int,int> indexToRealMap;
            std::vector<IndexTriangle> meshTris;
            meshTris.assign(triangles.begin(),triangles.end());

            int writeIndex=0;
            std::map<int,int>::iterator it;
            for (int i = 0; i < meshTris.size(); ++i)
            {
                it = indexToRealMap.find(meshTris[i].indexA);
                if (it!= indexToRealMap.end())
                {
                    meshTris[i].indexA=it->second;
                }
                else
                {
                    vertices[meshTris[i].indexA].Write(vtx);
                    indexToRealMap[meshTris[i].indexA]=writeIndex;
                    meshTris[i].indexA = writeIndex;
                    writeIndex++;
                }

                it = indexToRealMap.find(meshTris[i].indexB);
                if (it != indexToRealMap.end())
                {
                    meshTris[i].indexB = it->second;
                }
                else
                {
                    vertices[meshTris[i].indexB].Write(vtx);
                    indexToRealMap[meshTris[i].indexB] = writeIndex;
                    meshTris[i].indexB = writeIndex;
                    writeIndex++;
                }

                it = indexToRealMap.find(meshTris[i].indexC);
                if (it != indexToRealMap.end())
                {
                    meshTris[i].indexC = it->second;
                }
                else
                {
                    vertices[meshTris[i].indexC].Write(vtx);
                    indexToRealMap[meshTris[i].indexC] = writeIndex;
                    meshTris[i].indexC = writeIndex;
                    writeIndex++;
                }
            }
            memcpy(idx,meshTris.data(),sizeof(IndexTriangle)*meshTris.size());

           currentMesh->set_num_indices(triangles.size()*3);
           currentMesh->set_num_vertices(writeIndex);
        
    }
private:
    float pairThresholdSqrd;
    float contractionMaxCost;
    int vertexTargetNumber;
    int triangleTargetNumber;
    int removalsPerStep;

    octet::ref<octet::mesh> currentMesh;

    std::vector<Vertex> vertices;
    std::vector<IndexTriangle> triangles;

    std::set<IndexEdge> pairs;
    std::vector<Contraction> contractions;
    std::vector<octet::mat4t> errorMatrices;
};

int EQMeshSimplifier::Vertex::posOffset = 0;
int EQMeshSimplifier::Vertex::normalOffset = 0;
int EQMeshSimplifier::Vertex::UVOffset=0;
int EQMeshSimplifier::Vertex::size=0;

#endif