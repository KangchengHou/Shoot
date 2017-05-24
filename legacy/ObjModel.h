// This file cope with the obj file, have utility function concerning with OBJ file
// Our project do not use obj file, so this file can be ommited.
// reference
// http://www.opengl-tutorial.org/cn/beginners-tutorials/tutorial-7-model-loading/
// https://github.com/ssloy/tinyrenderer            
#ifndef OBJ_MODEL_H_
#define OBJ_MODEL_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ObjModel {
private:
    std::vector<glm::vec3> verts_;

    std::vector<std::vector<glm::ivec3> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<glm::vec3> norms_;
    std::vector<glm::vec2> uv_;
    // TGAImage diffusemap_;
    // TGAImage normalmap_;
    // TGAImage specularmap_;
    // void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    ObjModel(const char *filename);
    ~ObjModel();
    int nverts();
    int nfaces();
    glm::vec3 normal(int iface, int nthvert);
    glm::vec3 normal(glm::vec2 uv);
    glm::vec3 vert(int i);
    glm::vec3 vert(int iface, int nthvert);
    glm::vec2 uv(int iface, int nthvert);
    // TGAColor diffuse(glm::vec2 uv);
    float specular(glm::vec2 uv);
    std::vector<int> face(int idx);
};

#endif