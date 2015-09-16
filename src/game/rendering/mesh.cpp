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
    }
    else
    {
        Debug::console("Unable to open file %s\n", filename.c_str());
        return false;
    }
}