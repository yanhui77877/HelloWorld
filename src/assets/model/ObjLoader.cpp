#include <ObjLoader.h>

#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<Vertex>
loadObj(const std::string& path)
{
    std::ifstream file{ path };

    if (!file)
        throw std::runtime_error("Cannot open OBJ: " + path);

    std::vector<Vec3> positions;
    std::vector<Vec3> texcoords;
    std::vector<Vec3> normals;
    std::vector<Vertex> vertices;

    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream stream{ line };
        std::string type;
        stream >> type;

        if (type == "v")
        {
            Vec3 position{};

            if (!(stream >> position.x >> position.y >> position.z))
                throw std::runtime_error("Invalid OBJ position");

            positions.push_back(position);
        }
        else if (type == "vt")
        {
            Vec3 uv{};

            if (!(stream >> uv.x >> uv.y))
                throw std::runtime_error("Invalid OBJ UV");

            texcoords.push_back(uv);
        }
        else if (type == "vn")
        {
            Vec3 normal{};

            if (!(stream >> normal.x >> normal.y >> normal.z))
                throw std::runtime_error("Invalid OBJ normal");

            normals.push_back(normal);
        }
        else if (type == "f")
        {
            for (int i = 0; i < 3; ++i)
            {
                std::string token;

                if (!(stream >> token))
                    throw std::runtime_error("OBJ face needs three vertices");

                std::istringstream indices{ token };

                int positionIndex;
                int uvIndex;
                int normalIndex;
                char slash1;
                char slash2;

                if (!(indices >> positionIndex >> slash1
                    >> uvIndex >> slash2 >> normalIndex)
                    || slash1 != '/' || slash2 != '/'
                    || positionIndex <= 0
                    || uvIndex <= 0
                    || normalIndex <= 0)
                {
                    throw std::runtime_error("Expected positive OBJ v/vt/vn indices");
                }

                const Vec3& uv = texcoords.at(uvIndex - 1);

                Vertex vertex{
                    positions.at(positionIndex - 1),
                    uv.x,
                    uv.y,
                    normals.at(normalIndex - 1)
                };

                vertices.push_back(vertex);
            }

            std::string extra;

            if (stream >> extra && extra[0] != '#')
                throw std::runtime_error("Triangulate OBJ before loading");
        }
    }

    if (vertices.empty())
        throw std::runtime_error("OBJ contains no triangles");

    return vertices;
}