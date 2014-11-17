//
//  main.cpp
//  obj2opengles
//
//  Created by Hugo Nordell on 7/11/14.
//  Copyright (c) 2014 hugo. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Representation of a .obj model
typedef struct Model {
    int vertices;
    int positions;
    int texels;
    int normals;
    int faces;
    int materials;
}
Model;

// Get information about the .obj model
Model getOBJinfo(string fp) {
    // Model representation
    Model model = {0};
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    
    if (!inOBJ.good()) {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while(!inOBJ.eof()) {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        // Check which type
        if (type.compare("v ") == 0) {
            model.positions++;
        } else if (type.compare("vt") == 0) {
            model.texels++;
        } else if (type.compare("vn") == 0) {
            model.normals++;
        } else if (type.compare("f ") == 0) {
            model.faces++;
        }
    }
    
    // Number of vertices in OBJ model
    model.vertices = model.faces*3;
    
    // Close OBJ file
    inOBJ.close();
    
    return model;
}

// Extract OBJ model data
void extractOBJdata(string fp, float positions[][3], float texels[][2], float normals[][3], int faces[][10], string *materials, int m_count) {
    // Counters
    int p = 0;
    int t = 0;
    int n = 0;
    int f = 0;
    int mtl = 0;
    
    // Open OBJ file
    ifstream inOBJ;
    inOBJ.open(fp);
    
    if (!inOBJ.good()) {
        cout << "ERROR OPENING OBJ FILE" << endl;
        exit(1);
    }
    
    // Read OBJ file
    while (!inOBJ.eof()) {
        string line;
        getline(inOBJ, line);
        string type = line.substr(0,2);
        
        // Materials
        if (type.compare("us") == 0) {
            // Extract token
            string l = "usemtl ";
            string material = line.substr(l.size());
            
            for (int i = 0; i < m_count; i++) {
                if (material.compare(materials[i]) == 0) {
                    mtl = i;
                }
            }
        }
        
        // Positions
        if (type.compare("v ") == 0) {
            // Copy line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1); // +1 for end of line char
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 3; i++) {
                positions[p][i] = atof(strtok(NULL, " "));
            }
            
            // Dealloc l and wrap up
            delete[] l;
            p++;
        }
        
        else if (type.compare("vt") == 0) {
            // Cope line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 2; i++) {
                texels[t][i] = atof(strtok(NULL, " "));
            }
            
            // Dealloc l and wrap up
            delete [] l;
            t++;
        }
        
        else if (type.compare("vn") == 0) {
            // Copy line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 3; i++) {
                normals[n][i] = atof(strtok(NULL, " "));
            }
            
            // Dealloc l
            delete [] l;
            n++;
        }
        
        else if (type.compare("f ") == 0) {
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 9; i++) {
                faces[f][i] = atof(strtok(NULL, " /"));
            }
            
            // Append material to face
            faces[f][9] = mtl;
            
            // Dealloc
            delete [] l;
            f++;
        }
    }
    
    // Close OBJ file
    inOBJ.close();
}

// Header creation
void writeH(string fp, string name, Model model) {
    // Create Header file
    ofstream outH;
    outH.open(fp);
    
    if (!outH.good()) {
        cout << "ERROR CREATING .h FILE" << endl;
        exit(1);
    }
    
    // Write to H file
    outH << "// This is a .h file for the model: " << name << endl;
    outH << endl;
    outH << "// Positions: " << model.positions << endl;
    outH << "// Texels: " << model.texels << endl;
    outH << "// Normals: " << model.normals << endl;
    outH << "// Faces: " << model.faces << endl;
    outH << "// Vertices: " << model.vertices << endl;
    outH << "// Materials: " << model.materials << endl;
    outH << endl;
    
    // Write declarations
    outH << "const int " << name << "Vertices;" << endl;
    outH << "const float " << name << "Positions[" << model.vertices*3 << "];" << endl;
    outH << "const float " << name << "Texels[" << model.vertices*2 << "];" << endl;
    outH << "const float " << name << "Normals[" << model.vertices*3 << "];" << endl;
    outH << endl;
    
    outH << "const int " << name << "Materials;" << endl;
    outH << "const int " << name << "Firsts[" << model.materials << "];" << endl;
    outH << "const int " << name << "Counts[" << model.materials << "];" << endl;
    outH << endl;

    outH << "const float " << name << "KDs[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << "const float " << name << "KSs[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << "const float " << name << "KAs[" << model.materials << "]" << "[" << 3 << "];" << endl;
    outH << "const float " << name << "NSs[" << model.materials << "];" << endl;
    outH << "const float " << name << "NIs[" << model.materials << "];" << endl;
    outH << "const float " << name << "Ds[" << model.materials << "];" << endl;
    outH << "const char *" << name << "MAPKDs[" << model.materials << "];" << endl; // NOT SURE IF THIS WORKS
    outH << "const float " << name << "NSs[" << model.materials << "];" << endl;
    outH << "const int " << name << "ILLUMs[" << model.materials << "];" << endl;
    outH << endl;
    
    // Close file
    outH.close();
}

// Write .c file of vertices
void writeCvertices(string fp, string name, Model model) {
    // Create .c file
    ofstream outC;
    outC.open(fp);
    
    if (!outC.good()) {
        cout << "ERROR CREATING .c FILE" << endl;
        exit(1);
    }
    
    // Write to .c file
    outC << "// This is a .c file for the model: " << name << endl;
    outC << endl;
    
    // Header
    outC << "#include " << "\"" << name << ".h" << "\"" << endl;
    outC << endl;
    
    // Vertices
    outC << "const int " << name << "Vertices = " << model.vertices << ";" << endl;
    outC << endl;
    
    // Close .c file
    outC.close();
}

// Write .c file of positions
void writeCpositions(string fp, string name, Model model, int faces[][10], float positions[][3], int counts[]) {
    // Append to .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Positions
    outC << "const float " << name << "Positions[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    
    for (int j = 0; j < model.materials; j++) {
        counts[j] = 0;
        
        for (int i = 0; i < model.faces; i++) {
            if (faces[i][9] == j) {
                int vA = faces[i][0] - 1;
                int vB = faces[i][3] - 1;
                int vC = faces[i][6] - 1;
            
                outC << positions[vA][0] << ", " << positions[vA][1] << ", " << positions[vA][2] << ", " << endl;
                outC << positions[vB][0] << ", " << positions[vB][1] << ", " << positions[vB][2] << ", " << endl;
                outC << positions[vC][0] << ", " << positions[vC][1] << ", " << positions[vC][2] << ", " << endl;
            
                // 3 vertices per triangular face
                counts[j] += 3;
            }
        }
    }
    
    outC << "};" << endl;
    outC << endl;
    
    // Close .c file
    outC.close();
}

// Write .c file of texels
void writeCtexels(string fp, string name, Model model, int faces[][10], float texels[][2]) {
    // Append to .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Texels
    outC << "const float " << name << "Texels[" << model.vertices*2 << "] = " << endl;
    outC << "{" << endl;
    
    for (int j = 0; j < model.materials; j++) {
        for (int i = 0; i < model.faces; i++) {
            if (faces[i][9] == j) {

                int vtA = faces[i][1] - 1;
                int vtB = faces[i][4] - 1;
                int vtC = faces[i][7] - 1;
            
                outC << texels[vtA][0] << ", " << texels[vtA][1] << ", " << endl;
                outC << texels[vtB][0] << ", " << texels[vtB][1] << ", " << endl;
                outC << texels[vtC][0] << ", " << texels[vtC][1] << ", " << endl;
            }
        }
    }
    
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

// Write .c file of normals
void writeCnormals(string fp, string name, Model model, int faces[][10], float normals[][3]) {
    // Append to .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Normals
    outC << "const float " << name << "Normals[" << model.vertices*3 << "] = " << endl;
    outC << "{" << endl;
    
    for (int j = 0; j < model.materials; j++) {
        for (int i = 0; i < model.faces; i++) {
            if (faces[i][9] == j) {
                int vnA = faces[i][2] - 1;
                int vnB = faces[i][5] - 1;
                int vnC = faces[i][8] - 1;
    
                outC << normals[vnA][0] << ", " << normals[vnA][1] << ", " << normals[vnA][2] << ", " << endl;
                outC << normals[vnB][0] << ", " << normals[vnB][1] << ", " << normals[vnB][2] << ", " << endl;
                outC << normals[vnC][0] << ", " << normals[vnC][1] << ", " << normals[vnC][2] << ", " << endl;
            }
        }
    }
    
    outC << "};";
    outC << endl;
    
    outC.close();
}

// Extract materials information from MTL file
int getMTLinfo(string fp) {
    int m = 0;
    
    // Open .mtl file
    ifstream inMTL;
    inMTL.open(fp);
    
    if (!inMTL.good()) {
        cout << "ERROR OPENING .MTL FILE" << endl;
        exit(1);
    }
    
    while (!inMTL.eof()) {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);
        
        if (type.compare("ne") == 0) {
            m++;
        }
    }
    
    inMTL.close();
    
    return m;
}

void extractMTLdata(string fp, string *materials, string *map_Kd, float kd[][3], float ks[][3], float ka[][3], float ns[], float ni[], float d[], int illum[]) {
    // Counters
    int m_count = 0;
    int kd_count = 0;
    int ks_count = 0;
    int ka_count = 0;
    int ns_count = 0;
    int map_kd_count = 0;
    int ni_count = 0;
    int illum_count = 0;
    int d_count = 0;
    
    // Open file
    ifstream inMTL;
    inMTL.open(fp);
    
    if (!inMTL.good()) {
        cout << "ERROR OPENING MTL FILE" << endl;
        exit(1);
    }
    
    // Read file
    while (!inMTL.eof()) {
        string line;
        getline(inMTL, line);
        string type = line.substr(0,2);
        
        // Names
        if (type.compare("ne") == 0) {
            string l = "newmtl ";
            materials[m_count] = line.substr(l.size());
            m_count++;
        }
        
        else if (type.compare("Kd") == 0) {
            // Copy line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 3; i++) {
                kd[kd_count][i] = atof(strtok(NULL, " "));
            }
            
            delete [] l;
            kd_count++;
        }
        
        else if (type.compare("Ks") == 0) {
            // Copy line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 3; i++) {
                ks[ks_count][i] = atof(strtok(NULL, " "));
            }
            
            delete [] l;
            ks_count++;
        }
        
        else if (type.compare("Ka") == 0) {
            // Copy line for parsing
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            
            // Extract tokens
            strtok(l, " ");
            for (int i = 0; i < 3; i++) {
                ka[ka_count][i] = atof(strtok(NULL, " "));
            }
            
            delete [] l;
            ka_count++;
        }
        
        else if (type.compare("Ns") == 0) {
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            strtok(l, " ");
            ns[ns_count] = atof(strtok(NULL, " "));
            ns_count++;
        }
        
        else if (type.compare("Ni") == 0) {
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            strtok(l, " ");
            ni[ni_count] = atof(strtok(NULL, " "));
            ni_count++;
        }
        
        else if (type.compare("d ") == 0) {
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            strtok(l, " ");
            d[d_count] = atof(strtok(NULL, " "));
            d_count++;
        }
        
        else if (type.compare("il") == 0) {
            char *l = new char[line.size()+1];
            memcpy(l, line.c_str(), line.size()+1);
            strtok(l, " ");
            illum[illum_count] = atof(strtok(NULL, " "));
            illum_count++;
        }
        
        else if (type.compare("ma") == 0) {
            string l = "map_Kd ";
            map_Kd[map_kd_count] = line.substr(l.size());
            map_kd_count++;
        }
    }
    
    inMTL.close();
}

void writeCmaterials(string fp, string name, Model model, int firsts[], int counts[]) {
    // Append to .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Materials
    outC << "const int " << name << "Materials = " << model.materials << ";" << endl;
    outC << endl;
    
    // Firsts
    outC << "const int " << name << "Firsts[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    
    for (int i = 0; i < model.materials; i++) {
        if (i == 0) {
            firsts[i] = 0;
        } else {
            firsts[i] = firsts[i-1] + counts[i-1];
        }
        
        outC << firsts[i] << ", " << endl;
    }
    
    outC << "};" << endl;
    outC << endl;
    
    // Counts
    outC << "const int " << name << "Counts[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    
    for (int i = 0; i < model.materials; i++) {
        outC << counts[i] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCkds(string fp, string name, Model model, float kd[][3]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Kds
    outC << "const float " << name << "KDs[" << model.materials << "][3] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << kd[i][0] << ", " << kd[i][1] << ", " << kd[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCkss(string fp, string name, Model model, float ks[][3]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // KSs
    outC << "const float " << name << "KSs[" << model.materials << "][3] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << ks[i][0] << ", " << ks[i][1] << ", " << ks[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCkas(string fp, string name, Model model, float ka[][3]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // KAs
    outC << "const float " << name << "KAs[" << model.materials << "][3] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << ka[i][0] << ", " << ka[i][1] << ", " << ka[i][2] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCds(string fp, string name, Model model, float d[]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // Ds
    outC << "const float " << name << "Ds[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << d[i] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCnss(string fp, string name, Model model, float ns[]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // NSs
    outC << "const float " << name << "NSs[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << ns[i] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCnis(string fp, string name, Model model, float ni[]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // NIs
    outC << "const float " << name << "NIs[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << ni[i] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCmapkds(string fp, string name, Model model, string* map_Kd) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // MAPKDs
    outC << "const char *" << name << "MAPKDs[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << "\"" << map_Kd[i] << "\"" << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

void writeCillums(string fp, string name, Model model, int illum[]) {
    // Append .c file
    ofstream outC;
    outC.open(fp, ios::app);
    
    // ILLUMs
    outC << "const int " << name << "ILLUMs[" << model.materials << "] = " << endl;
    outC << "{" << endl;
    for (int i = 0; i < model.materials; i++) {
        outC << illum[i] << ", " << endl;
    }
    outC << "};" << endl;
    outC << endl;
    
    outC.close();
}

int main(int argc, const char * argv[])
{
    // Arguments
    cout << argc << endl;
    cout << argv[0] << endl;
    cout << argv[1] << endl;
    
    // Filepaths to grab and generate
    string nameOBJ = argv[1];
    string filepathOBJ = "source/" + nameOBJ + ".obj";
    string filepathMTL = "source/" + nameOBJ + ".mtl";
    string filepathH = "product/" + nameOBJ + ".h";
    string filepathC = "product/" + nameOBJ + ".c";
    
    // Model info
    Model model = getOBJinfo(filepathOBJ);
    model.materials = getMTLinfo(filepathMTL);
    cout << "Model info" << endl;
    cout << "Positions: " << model.positions << endl;
    cout << "Texels: " << model.texels << endl;
    cout << "Normals: " << model.normals << endl;
    cout << "Faces: " << model.faces << endl;
    cout << "Vertices: " << model.vertices << endl;
    cout << "Materials: " << model.materials << endl;

    // Model data
    float positions[model.positions][3]; // XYZ
    float texels[model.texels][2]; // UV
    float normals[model.normals][3]; // XYZ
    int faces[model.faces][10]; // PTN PTN PTN M
    
    // Material data
    string *materials = new string[model.materials];
    string *map_Kd = new string[model.materials];
    float kd[model.materials][3];
    float ks[model.materials][3];
    float ka[model.materials][3];
    float ns[model.materials];
    float ni[model.materials];
    float d[model.materials];
    int illum[model.materials];
    
    extractMTLdata(filepathMTL, materials, map_Kd, kd, ks, ka, ns, ni, d, illum);
    cout << "Name1: " << materials[0] << endl;
    cout << "Kd1: " << kd[0][0] << "r " << kd[0][1] << "g " << kd[0][2] << "b " << endl;
    cout << "Ks1: " << ks[0][0] << "r " << ks[0][1] << "g " << ks[0][2] << "b " << endl;
    cout << "Ka1: " << ka[0][0] << "r " << ka[0][1] << "g " << ka[0][2] << "b " << endl;
    cout << "Ns1: " << ns[0] << endl;
    cout << "Ni1: " << ni[0] << endl;
    cout << "d1: " << d[0] << endl;
    cout << "illum1: " << illum[0] << endl;
    cout << "map_Kd1: " << map_Kd[0] << endl;
    
    extractOBJdata(filepathOBJ, positions, texels, normals, faces, materials, model.materials);
    cout << "Model data" << endl;
    cout << "P1: " << positions[0][0] << "x " << positions[0][1] << "y " << positions[0][2] << "z" << endl;
    cout << "T1: " << texels[0][0] << "U " << texels[0][1] << "V " << endl;
    cout << "N1: " << normals[0][0] << "x " << normals[0][1] << "y " << normals[0][2] << "z" << endl;
    cout << "F1v1: " << faces[0][0] << "p " << faces[0][1] << "t " << faces[0][2] << "n" << endl;
    
//    cout << "Material references" << endl;
//    for (int i = 0; i < model.faces; i++) {
//        int m = faces[i][9];
//        cout << "F" << i << "m: " << materials[m] << endl;
//    }
    
    // Write .h file
    writeH(filepathH, nameOBJ, model);
    
    // Materials matching to vertices and faces
    int firsts[model.materials];
    int counts[model.materials];

    // Write .c file
    writeCvertices(filepathC, nameOBJ, model);
    writeCpositions(filepathC, nameOBJ, model, faces, positions, counts);
    writeCtexels(filepathC, nameOBJ, model, faces, texels);
    writeCnormals(filepathC, nameOBJ, model, faces, normals);
    
    writeCmaterials(filepathC, nameOBJ, model, firsts, counts);
    writeCkds(filepathC, nameOBJ, model, kd);
    writeCkas(filepathC, nameOBJ, model, ka);
    writeCkss(filepathC, nameOBJ, model, ks);
    writeCnss(filepathC, nameOBJ, model, ns);
    writeCnis(filepathC, nameOBJ, model, ni);
    writeCds(filepathC, nameOBJ, model, d);
    writeCillums(filepathC, nameOBJ, model, illum);
    writeCmapkds(filepathC, nameOBJ, model, map_Kd); // MIGHT NOT WORK.
    
    return 0;
}

