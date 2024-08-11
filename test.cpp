#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;
struct Vertex{
    double x,y,z;
};
struct triangles {
    int v1, v2, v3;
};
vector<Vertex> vertices;
vector<triangles> faces;

vector<double> splitstring(const string& str){
    vector<double> values;
    istringstream iss(str);
    double val;
    while (iss >>val) {
        values.push_back(val);
    }
    return values;
}

bool readVTKfile(string filename, vector<Vertex> &vertices, vector<triangles> &tri){
    ifstream file(filename);
    if (!file.is_open()){
        cerr<< "error opening file: "<< filename<<endl;
        return false;
    }
    string line;
    while(getline(file, line)){
        if(line.find("POINTS")!= string::npos){
            int numvertices;
            istringstream iss(line);
            iss >> line>> numvertices;
            vertices.reserve(numvertices);

            for (int i=0; i<numvertices; i++){
                getline(file, line);
                vector<double> coords =splitstring(line);
                if (coords.size()!=3){
                    cerr<< "Not able to read the line"<< i+1<<endl;
                    return false;
                }

                Vertex vertex;
                vertex.x= coords[0];
                vertex.y= coords[1];
                vertex.z = coords[2];
                vertices.push_back(vertex);
            }
        }
        else if (line.find("CELLS") !=string:: npos){
            int numtets;
            istringstream iss(line);
            iss>> line>> numtets;
            tri.reserve(numtets);
            for(int i=0; i<numtets; i++){
                getline(file, line);
                vector<double> cellindices= splitstring(line);
                if (cellindices.size()!=4 && cellindices[0] !=3){
                    cerr<< "Not able to read tetrahedra at line: "<< i+1<<endl;
                }

                //we cast indices as integers.
                triangles T;
                T.v1= static_cast<int> (cellindices[1]);
                T.v2 =static_cast<int> (cellindices[2]);
                T.v3= static_cast<int> (cellindices[3]);
                tri.push_back(T);
            }
        }
    }
    return true;
}

int main(){
    // Read obj file
    string filename ="bunny.obj";
    ifstream objfile(filename);
    if(!objfile.is_open()){
        cerr<<" Faild to open the file";
        return -1;
    }
    string line;
    while (getline(objfile, line)){
        istringstream iss(line);
        string prefix;
        iss>> prefix;
        if (prefix=="v"){
            Vertex v;
            iss>> v.x>>v.y>>v.z;
            vertices.push_back(v);
        }
        else if (prefix== "f"){
            vector<int> facesind;
            string indexstr;
            while(iss>> indexstr){
                int vertexindex= stoi(indexstr);
                facesind.push_back(vertexindex-1);
            }
            triangles t;
            t.v1= facesind[0];
            t.v2= facesind[1];
            t.v3=facesind[2];
            faces.push_back(t);
        }
    }
    // string filename ="square_refine.vtk";
    // readVTKfile(filename, vertices, faces);
    
    // write .mesh file 
    ofstream meshfile("bunny.mesh");    
    if (!meshfile.is_open()){
        cerr<< "error opening file: bunny.mesh "<<endl;
        return 0;
    }
    meshfile<< "MeshVersionFormatted 2"<<endl;
    meshfile<< "Dimension 2"<<endl;
    meshfile<< "Vertices"<<endl;
    meshfile<< vertices.size()<<endl;
    for (int i=0; i<vertices.size(); i++){
        meshfile<< vertices[i].x<< " "<< vertices[i].y<<" "<<vertices[i].z<<endl;
    }
    meshfile<< "Triangles"<<endl;
    meshfile<< faces.size()<<endl;
    for (int i=0; i<faces.size(); i++){
        meshfile<< faces[i].v1<< " "<< faces[i].v2<< " "<< faces[i].v3<<endl;
    }
    meshfile.close();

    //write the .sol file
    ofstream solfile("bunny.sol");   
    if (!solfile.is_open()){
        cerr<< "error opening file: bunny.sol"<<endl;
        return 2;
    }
    solfile<< "MeshVersionFormatted 1"<<endl;
    solfile<< "Dimension 2"<<endl;
    solfile<< "SolAtVertices"<<endl;
    solfile<< vertices.size()<<endl;
    solfile<< "1 1"<<endl;
    //read values from valuescalc.txt file and put it in the sol file
    ifstream valuesfile("valuescalc.txt");
    if (!valuesfile.is_open()){
        cerr<< "error opening file: valuescalc.txt"<<endl;
        return 3;
    }
    string line1;
    // there are only vertices.size() values in the file 
    while(getline(valuesfile, line1)){
        solfile<< line1<<endl;
    }
    solfile.close();


    return 4;

}