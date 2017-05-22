#include <iostream>
#include <fstream>
#include <sstream>
#include "ObjModel.h"

ObjModel::ObjModel(const char *filename) : verts_(), faces_(), norms_(), uv_(){
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        // string s;
        // s.compare(pos, len, anotherString)
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            glm::vec3 v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            // normal point
            iss >> trash >> trash;
            glm::vec3 n;
            for (int i=0;i<3;i++) iss >> n[i];
            norms_.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {
            // texture point
            iss >> trash >> trash;
            glm::vec2 uv;
            for (int i=0;i<2;i++) iss >> uv[i];
            uv_.push_back(uv);
        }  else if (!line.compare(0, 2, "f ")) {
            // face 
            // every face is a vector of a int vector
            std::vector<glm::ivec3> f;
            glm::ivec3 tmp;
            iss >> trash;
            // a face has many point 
            // a point is a three element tuple
            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                // the first point is 1 
                for (int i=0; i<3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << " vt# " << uv_.size() << " vn# " << norms_.size() << std::endl;
    // load_texture(filename, "_diffuse.tga", diffusemap_);
    // load_texture(filename, "_nm_tangent.tga",      normalmap_);
    // load_texture(filename, "_spec.tga",    specularmap_);
}

ObjModel::~ObjModel() {}

int ObjModel::nverts() {
    return (int)verts_.size();
}

int ObjModel::nfaces() {
    return (int)faces_.size();
}

std::vector<int> ObjModel::face(int idx) {
    std::vector<int> face;
    for (int i=0; i<(int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
    return face;
}

glm::vec3 ObjModel::vert(int i) {
    return verts_[i];
}

glm::vec3 ObjModel::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

// void ObjModel::load_texture(std::string filename, const char *suffix, TGAImage &img) {
//     std::string texfile(filename);
//     size_t dot = texfile.find_last_of(".");
//     if (dot!=std::string::npos) {
//         texfile = texfile.substr(0,dot) + std::string(suffix);
//         std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
//         img.flip_vertically();
//     }
// }

// TGAColor ObjModel::diffuse(glm::vec2 uvf) {
//     glm::ivec2 uv(uvf[0]*diffusemap_.get_width(), uvf[1]*diffusemap_.get_height());
//     return diffusemap_.get(uv[0], uv[1]);
// }

// glm::vec3 ObjModel::normal(glm::vec2 uvf) {
//     glm::ivec2 uv(uvf[0]*normalmap_.get_width(), uvf[1]*normalmap_.get_height());
//     TGAColor c = normalmap_.get(uv[0], uv[1]);
//     glm::vec3 res;
//     for (int i=0; i<3; i++)
//         res[2-i] = (float)c[i]/255.f*2.f - 1.f;
//     return res;
// }

// glm::vec2 ObjModel::uv(int iface, int nthvert) {
//     return uv_[faces_[iface][nthvert][1]];
// }

// float ObjModel::specular(glm::vec2 uvf) {
//     glm::ivec2 uv(uvf[0]*specularmap_.get_width(), uvf[1]*specularmap_.get_height());
//     return specularmap_.get(uv[0], uv[1])[0]/1.f;
// }

// glm::vec3 ObjModel::normal(int iface, int nthvert) {
//     int idx = faces_[iface][nthvert][2];
//     return norms_[idx].normalize();
// }
