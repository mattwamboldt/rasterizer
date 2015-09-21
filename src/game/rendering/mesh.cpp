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
        Vector3& v1 = vertices[face.a];
        Vector3& v2 = vertices[face.b];
        Vector3& v3 = vertices[face.c];

        // calculate the surface normal
        Vector3 a = v2 - v1;
        Vector3 b = v3 - v1;
        face.normal = a.Cross(b);
        face.normal.Normalize();

        // TODO: Add the surface normal to the vertex normal
    }

    // TODO: Normalize all of the vertex normals
}
