//------------------------------------------------------------------
// Declaration: Copyright (c), by i_dovelemon, 2017. All right reserved.
// Author: i_dovelemon[1322600812@qq.com]
// Date: 2017 / 03 / 04
// Brief: Read the .obj file generated by 3dmax, then generate the tangent-binormal coordinates
// Version: 1.0
//------------------------------------------------------------------
#include "glb.h"

#include <stdint.h>

#include <fstream>
#include <iostream>

//-----------------------------------------------------------------------------------
// TYPE DECLARATION
//-----------------------------------------------------------------------------------
static const int32_t kMaxFilePathLen = 256;
static const int32_t kMaxCharLen = 256;

//----------------------------------------------------------------------------------
// CLASS DECLARATION
//---------------------------------------------------------------------------------
struct TangentSpace {
    glb::Vector tangent;
    glb::Vector binormal;
};

//-----------------------------------------------------------------------------------
// FUNCTION DECLARATION
//-----------------------------------------------------------------------------------
//------------------------------------------------------------------
// @brief: Check arguments if valid
// @param: argc The number of arguments
// @param: argv The arguments array
// @out: file_path The buffer to store .obj file path
// @ret: If fail, return false, otherwise return true.
//------------------------------------------------------------------
bool glb_check_args(int argc, char* argv[], char* file_path);

//------------------------------------------------------------------
// @brief: Calculate tangent and binormal coordinate of a list of faces
// @param: face_num The number of faces
// @param: vertex_buf The buffer hold the face's vertex data
// @param: texcoord_buf The buffer hold the face's texcoord data
// @out: tangents The list hold all the vertex's tangents space coordinate
// @ret: If faild return false, otherwise return true.
//------------------------------------------------------------------
bool glb_calc_tangent_binormal_coordinates(int32_t face_num, float* vertex_buf, float* texcoord_buf, std::vector<TangentSpace> &tangents);

//------------------------------------------------------------------
// @brief: Calculate tangent and binormal coordinate of a face
// @param: v0-v2 The vertex of face
// @param: t0-t2 The texture coordinate of face
// @out: space The tangent space coordinate
//------------------------------------------------------------------
void glb_calc_tangent_binormal(glb::Vector v0, glb::Vector v1, glb::Vector v2
                               , glb::Vector t0, glb::Vector t1, glb::Vector t2
                               , TangentSpace& space);

//------------------------------------------------------------------
// @brief: Write the generated tangents space coordinates to .obj file
// @param: obj_file_path The path name of .obj file
// @param: tangents The tangent space coordinate
// @ret: If faild return false, otherwise return true.
//------------------------------------------------------------------
bool glb_write_tangent_space(char* obj_file_path, std::vector<TangentSpace> tangents);

//-----------------------------------------------------------------------------------
// FUNCTION DEFINITION
//-----------------------------------------------------------------------------------
int main(int argc,char* argv[]) {
    // Check if arguments is valid
    char obj_file_path[kMaxFilePathLen];
    bool is_valid_arguments = glb_check_args(argc, argv, obj_file_path);
    if (!is_valid_arguments) {
        std::cout << "Invalid arguments!" << std::endl;
#if _DEBUG
        system("pause");
#endif
        return 0;
    }

    // Extract face data from file
    glb::ModelEffectParam effect_param;
    glb::ModelMaterialParam material_param;
    float* vertex_buf = NULL;
    float* texcoord_buf = NULL;
    float* normal_buf = NULL;
    float* tangent_buf = NULL;
    int32_t face_num = glb::ModelFile::ExtractModelData(obj_file_path, effect_param, material_param
        , &vertex_buf, &texcoord_buf, &normal_buf, &tangent_buf);

    if (vertex_buf == NULL || texcoord_buf == NULL) {
        glb::ModelFile::RelaseBuf(&vertex_buf, &texcoord_buf, &normal_buf, &tangent_buf);
        std::cout << "There is no texture coordinate, so there is no need for tangent-binormal coordinates!" << std::endl;

#if _DEBUG
        system("pause");
#endif
        return 0;
    }

    if (tangent_buf != NULL) {
        glb::ModelFile::RelaseBuf(&vertex_buf, &texcoord_buf, &normal_buf, &tangent_buf);
        std::cout << "There is already tangent space coordinate, so there is no need for tangent-binormal coordinates!" << std::endl;

#if _DEBUG
        system("pause");
#endif
        return 0;
    }

    // Calculate tangent and binormal coordinates
    std::vector<TangentSpace> tangents;
    bool is_tangent_space_generated = glb_calc_tangent_binormal_coordinates(face_num, vertex_buf, texcoord_buf, tangents);
    glb::ModelFile::RelaseBuf(&vertex_buf, &texcoord_buf);
    if (!is_tangent_space_generated) {
        std::cout << "Generate tangent space coordinate error!" << std::endl;

#if _DEBUG
        system("pause");
#endif
        return 0;
    }

    // Write tangent and binormal coordinates to file
    bool is_write_new_file_ok = glb_write_tangent_space(obj_file_path, tangents);
    if (!is_write_new_file_ok) {
        std::cout << "Write new data to file error!" << std::endl;
#if _DEBUG
        system("pause");
#endif
        return 0;
    }

    std::cout << "New tangent space coordinate has generated successfully!" << std::endl;

#if _DEBUG
    system("pause");
#endif

    return 0;
}

bool glb_check_args(int argc, char* argv[], char* file_path) {
    bool result = true;

    if (argc <= 1) {
        result = false;
        return result;
    }

    if (argv == NULL) {
        result = false;
        return false;
    }

    int32_t len = strlen(argv[1]);
    if (len >= kMaxFilePathLen) {
        result = false;
        return result;
    }

    std::ifstream input;
    input.open(argv[1]);
    if (input.fail()) {
        result = false;
        return result;
    }
    input.close();

    memcpy(file_path, argv[1], len);
    file_path[len] = '\0';

    return result;
}

bool glb_calc_tangent_binormal_coordinates(int32_t face_num, float* vertex_buf, float* texcoord_buf, std::vector<TangentSpace> &tangents) {
    bool result = true;
    if (vertex_buf == NULL || texcoord_buf == NULL) {
        result = false;
        return result;
    }

    glb::Vector v0, v1, v2, t0, t1, t2;
    for (int32_t i = 0; i < face_num; i++) {
         v0.x = vertex_buf[i * 3 * 3 + 0 * 3 + 0];
         v0.y = vertex_buf[i * 3 * 3 + 0 * 3 + 1];
         v0.z = vertex_buf[i * 3 * 3 + 0 * 3 + 2];
         v1.x = vertex_buf[i * 3 * 3 + 1 * 3 + 0];
         v1.y = vertex_buf[i * 3 * 3 + 1 * 3 + 1];
         v1.z = vertex_buf[i * 3 * 3 + 1 * 3 + 2];
         v2.x = vertex_buf[i * 3 * 3 + 2 * 3 + 0];
         v2.y = vertex_buf[i * 3 * 3 + 2 * 3 + 1];
         v2.z = vertex_buf[i * 3 * 3 + 2 * 3 + 2];
         t0.x = texcoord_buf[i * 3 * 2 + 0 * 2 + 0];
         t0.y = texcoord_buf[i * 3 * 2 + 0 * 2 + 1];
         t1.x = texcoord_buf[i * 3 * 2 + 1 * 2 + 0];
         t1.y = texcoord_buf[i * 3 * 2 + 1 * 2 + 1];
         t2.x = texcoord_buf[i * 3 * 2 + 2 * 2 + 0];
         t2.y = texcoord_buf[i * 3 * 2 + 2 * 2 + 1];
         TangentSpace space;
         glb_calc_tangent_binormal(v0, v1, v2, t0, t1, t2, space);
         tangents.push_back(space);
    }

    return result;
}

void glb_calc_tangent_binormal(glb::Vector v0, glb::Vector v1, glb::Vector v2
                               , glb::Vector t0, glb::Vector t1, glb::Vector t2
                               , TangentSpace& space) {
    glb::Vector vedge0 = v1 - v0;
    glb::Vector vedge1 = v2 - v0;
    glb::Vector tedge0 = t1 - t0;
    glb::Vector tedge1 = t2 - t0;
    float temp = 1.0f / (tedge0.x * tedge1.y - tedge0.y * tedge1.x);
    space.tangent = (vedge0 * tedge1.y - vedge1 * tedge0.y) * temp;
    space.binormal = (vedge1 * tedge0.x - vedge0 * tedge1.x) * temp;
    space.tangent.Normalize();
    space.binormal.Normalize();
}

bool glb_write_tangent_space(char* obj_file_path, std::vector<TangentSpace> tangents) {
    bool result = true;
    if (obj_file_path == NULL || tangents.size() == 0) {
        result = false;
        return result;
    }

    // Open file and read all data
    std::ifstream input;
    input.open(obj_file_path);
    if (input.fail()) {
        result = false;
        return result;
    }

    std::string data;
    char line[256];
    while (!input.eof()) {
        memset(line, 0, sizeof(line));
        input.getline(line, sizeof(line));
        data += line;
        data += '\n';
    }

    // Close file
    input.close();

    char temp[64];

    // Build vtan string data
    std::string vtan_str;
    for (int32_t i = 0; i < tangents.size(); i++) {
        std::string vtan;
        vtan += "vtan ";

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f ", tangents[i].tangent.x);
        vtan += temp;

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f ", tangents[i].tangent.y);
        vtan += temp;

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f\n", tangents[i].tangent.z);
        vtan += temp;
        vtan_str += vtan;
    }
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "# %d tangents (Gen by glb_tbn_gen)\n\n", tangents.size());
    vtan_str += temp;

    // Build vbi string data
    std::string vbi_str;
    for (int32_t i = 0; i < tangents.size(); i++) {
        std::string vbi;
        vbi += "vbi ";

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f ", tangents[i].binormal.x);
        vbi += temp;

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f ", tangents[i].binormal.y);
        vbi += temp;

        memset(temp, 0, sizeof(temp));
        sprintf(temp, "%f\n", tangents[i].binormal.z);
        vbi += temp;
        vbi_str += vbi;
    }
    memset(temp, 0, sizeof(temp));
    sprintf(temp, "# %d binormals (Gen by glb_tbn_gen)\n\n", tangents.size());
    vbi_str += temp;

    // Build tangent space data
    std::string vtangent_space_str;
    vtangent_space_str += vtan_str;
    vtangent_space_str += vbi_str;

    // Insert string
    int32_t last_normal_pos = 0;
    while (true) {
        int32_t pos = data.find("vn", last_normal_pos);
        if (pos == -1) {
            break;
        } else {
            last_normal_pos = pos + 2;
        }
    }

    while (true) {
        if (data[last_normal_pos] == '\n'
            && data[last_normal_pos + 1] == '\n') {
            last_normal_pos += 2;
            break;
        } else {
            last_normal_pos++;
        }
    }

    data.insert(last_normal_pos, vtangent_space_str);

    // Insert face
    int32_t face_pos = 0;
    while (true) {
        if (face_pos + 1< data.length()) {
            if (data[face_pos] == '\n' && data[face_pos + 1] == 'f') {
                break;
            } else {
                face_pos++;
            }
        } else {
            break;
        }
    }
    int32_t face_num = tangents.size();
    for (int32_t i = 0; i < face_num; i++) {
        face_pos += 3;

        // Insert tan0, bin0
        while (true) {
            if (data[face_pos] == ' ') {
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "%d", i + 1);
                int32_t len = strlen(temp);
                data.insert(face_pos, 1, '/');
                data.insert(face_pos + 1, temp);
                data.insert(face_pos + 1 + len, 1, '/');
                data.insert(face_pos + 2 + len, temp);
                face_pos += (3 + len * 2);
                break;
            } else {
                face_pos++;
            }
        }

        // Insert tan1, bi1
        while (true) {
            if (data[face_pos] == ' ') {
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "%d", i + 1);
                int32_t len = strlen(temp);
                data.insert(face_pos, 1, '/');
                data.insert(face_pos + 1, temp);
                data.insert(face_pos + 1 + len, 1, '/');
                data.insert(face_pos + 2 + len, temp);
                face_pos += (3 + len * 2);
                break;
            } else {
                face_pos++;
            }
        }

        // Insert tan2, bi2
        while (true) {
            if (data[face_pos] == ' ') {
                memset(temp, 0, sizeof(temp));
                sprintf(temp, "%d", i + 1);
                int32_t len = strlen(temp);
                data.insert(face_pos, 1, '/');
                data.insert(face_pos + 1, temp);
                data.insert(face_pos + 1 + len, 1, '/');
                data.insert(face_pos + 2 + len, temp);
                face_pos += (3 + len * 2);
                break;
            } else {
                face_pos++;
            }
        }
    }

    face_pos += 1;
    data.insert(face_pos, "# Reorganize by glb_tbn_gen\n");

    // Open file to write new obj data
    std::ofstream output;
    output.open(obj_file_path);
    if (output.fail()) {
        result = false;
        return result;
    }

    output << data.c_str();

    // Close file
    output.close();

    return result;
}