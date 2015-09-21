#include "mesh.h"
#include <fstream>
#include <sstream>
#include "..\debug.h"

using namespace std;

bool Mesh::ReadTestFormat(string filename)
{
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            string object;
            std::istringstream iss(line);
            iss >> object;
            if (object == "o")
            {
                iss >> name;
            }
            else if (object == "v")
            {
                Vector3 temp;
                iss >> temp.x;
                iss >> temp.y;
                iss >> temp.z;
                vertices.push_back(temp);
            }
            else if (object == "f")
            {
                string facedef;
                Face temp;

                iss >> temp.a;
                iss >> facedef;

                iss >> temp.b;
                iss >> facedef;

                iss >> temp.c;

                // The object file index starts at 1
                --temp.a; --temp.b; --temp.c;

                faces.push_back(temp);
            }
            else
            {
                continue;
            }
        }
        file.close();
        CalculateNormals();
    }
    else
    {
        Debug::console("Unable to open file %s\n", filename.c_str());
        return false;
    }
}

Vector3 Normal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
    Vector3 a = v2 - v1;
    Vector3 b = v3 - v1;
    Vector3 normal = a.Cross(b);
    normal.Normalize();
    return normal;
}

void Mesh::CalculateNormals()
{
    for (int i = 0; i < faces.size(); ++i)
    {
        // Grab our face and vertices
        Face& face = faces[i];
        Vertex& v1 = vertices[face.a];
        Vertex& v2 = vertices[face.b];
        Vertex& v3 = vertices[face.c];

        // calculate the surface normal
        Vector3 a = v2.position - v1.position;
        Vector3 b = v3.position - v1.position;
        face.normal = a.Cross(b);
        face.normal.Normalize();

        // Add the normal to the verts
        v1.normal = v1.normal + face.normal;
        v2.normal = v2.normal + face.normal;
        v3.normal = v3.normal + face.normal;
    }

    // Normalize all of the vertex normals
    for (int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].normal.Normalize();
        Debug::console("Normal %f %f %f\n", vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z);
    }
}
