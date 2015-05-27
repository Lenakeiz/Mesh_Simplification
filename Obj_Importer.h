#ifndef OBJ_IMPORTER_H
#define OBJ_IMPORTER_H

///
// This class represent an importer of obj file.
// example structre is give in the following
//# Blender3D v249 OBJ File: untitled.blend
//# www.blender3d.org
// mtllib cube.mtl
// v 1.000000 - 1.000000 - 1.000000
// v 1.000000 - 1.000000 1.000000
// v - 1.000000 - 1.000000 1.000000
// vt 0.500149 0.750166
// vt 0.748355 0.998230
// vt 0.500193 0.998728
// vt 0.498993 0.250415
// vt 0.748953 0.250920
// vn 0.000000 0.000000 - 1.000000
// vn - 1.000000 - 0.000000 - 0.000000
// vn - 0.000000 - 0.000000 1.000000
// usemtl Material_ray.png
// s off
// f 5 / 1 / 1 1 / 2 / 1 4 / 3 / 1
// f 5 / 1 / 1 4 / 3 / 1 8 / 4 / 1
//
// 
///

namespace octet
{
	class Obj_Importer
	{

	private:


		std::vector<uint32_t> vertexIndices, uvIndices, normalIndices;
		std::vector<vec3> vertices;
		std::vector<vec2> uvs;
		std::vector<vec3> normals;
			
		ref<mesh> importedMesh;

		std::vector<mesh::vertex> out_vertices;
		hash_map < mesh::vertex, unsigned> duplicatesMap;
		std::vector<uint32_t> out_indices;
		std::vector<vec3> out_normals;
		const char* path;
		
	public:

		bool loadObj(const char* _path, ref<mesh> refMesh)
		{
			path = _path;

			if (&(*refMesh) == &(*importedMesh))
			{
				printf("Mesh already loaded");
				return false;
			}

			importedMesh = refMesh;

			FILE* file = fopen(path, "r");
			if (file == NULL)
			{
				printf("Impossible to open the file");
				return false;
			}

			unsigned int i = 0;

			while (true)
			{
				char headliner[128];
				//read the first word
				int res = fscanf(file, "%s", headliner);
				if (res == EOF) //can exit the loop
				{
					break;
				}

				//Loading the vertices
				if (strcmp(headliner, "v") == 0)
				{
					vec3 vtx;
					fscanf(file, "%f %f %f\n", &vtx.x(), &vtx.y(), &vtx.z());
					vertices.push_back(vtx);
				}
				else if (strcmp(headliner, "vt") == 0)
				{
					vec2 uv;
					fscanf(file, "%f %f\n", &uv.x(), &uv.y());
					uvs.push_back(uv);
				}
				else if (strcmp(headliner, "vn") == 0)
				{
					vec3 n;
					fscanf(file, "%f %f %f\n", &n.x(), &n.y(), &n.z());
					normals.push_back(n);
					out_normals.push_back(vec3(0, 0, 0));
				}
				else if (strcmp(headliner, "f") == 0)
				{
					unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
					int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
					if (matches != 9)
					{
						printf("Obj format is not good: check exporter options");
						return false;
					}


					vertexIndices.push_back(vertexIndex[0] - 1);
					vertexIndices.push_back(vertexIndex[1] - 1);
					vertexIndices.push_back(vertexIndex[2] - 1);
					uvIndices.push_back(uvIndex[0]);
					uvIndices.push_back(uvIndex[1]);
					uvIndices.push_back(uvIndex[2]);
					normalIndices.push_back(normalIndex[0]);
					normalIndices.push_back(normalIndex[1]);
					normalIndices.push_back(normalIndex[2]);

					vec3 calculatedNormal = (vertices[vertexIndex[1] - 1] - vertices[vertexIndex[0] - 1]).cross(vertices[vertexIndex[2] - 1] - vertices[vertexIndex[0] - 1]);

					//preparing normals calculation
					out_normals[vertexIndex[0] - 1] += calculatedNormal;
					out_normals[vertexIndex[1] - 1] += calculatedNormal;
					out_normals[vertexIndex[2] - 1] += calculatedNormal;

				}
			}
			return true;			
		}

		void buildMesh()
		{

			importedMesh->clear_attributes();

			for (uint32_t i = 0; i < vertices.size(); i++)
			{
				mesh::vertex out_vtx;
				out_vtx.pos = vertices[i];
				out_vtx.uv = vec2(0.0f, 0.0f);
				out_vtx.normal = -out_normals[i].normalize();
				out_vertices.push_back(out_vtx);
			}

			importedMesh->init(0, out_vertices.size(), vertexIndices.size());
			importedMesh->set_params(sizeof(mesh::vertex), vertexIndices.size(), out_vertices.size(), GL_TRIANGLES, GL_UNSIGNED_INT);

			gl_resource::rwlock vLock(importedMesh->get_vertices());
			gl_resource::rwlock iLock(importedMesh->get_indices());

			mesh::vertex* vtx = (mesh::vertex*) vLock.u32();
			uint32_t* idx = iLock.u32();
			
			memcpy(vtx, out_vertices.data(), sizeof(mesh::vertex) * out_vertices.size());
			memcpy(idx, vertexIndices.data(), sizeof(uint32_t)* vertexIndices.size());
						
		}

		Obj_Importer()
		{
		}

		~Obj_Importer()
		{
		}
	};
}

#endif

