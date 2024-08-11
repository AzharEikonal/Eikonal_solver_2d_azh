#include<fstream>
#include<vector>
#include<string>
#include<sstream>
#include<cmath>
#include<iostream>

using namespace std;
vector<double> splitstring(const string& str){
    vector<double> values;
    istringstream iss(str);
    double val;
    while (iss >>val) {
        values.push_back(val);
    }
    return values;
}

int main(int argc, char* argv[]){
    vector<vector<double>> vertices;
    vector<vector<int>> tetras;
    vector<double> values;
    vector<double> valuestrue;
    vector<double> errors;
    
    // ifstream infile("vertices2.txt");
    // string line;
    // while(getline(infile, line)){
    //     vector<double> vertex;
    //     double value;
    //     istringstream iss(line);
    //     while(iss >> value){
    //         vertex.push_back(value);
    //     }
    //     vertices.push_back(vertex);
    // }
    // infile.close();

    // ifstream infile2("tets2.txt");
    // string line2;
    // while(getline(infile2, line2)){
    //     vector<int> tetra;
    //     int val;
    //     istringstream iss(line2);
    //     while(iss >>val){
    //         tetra.push_back(val);
    //     }
    //     tetras.push_back(tetra);
    // }
    // infile2.close();

    // // copy the vertices and tets from the vtk file to the vectors of vertices and tets: 
    // ifstream file("torus.vtk");
    // if (!file.is_open()){
    //     cerr<< "error opening file: "<<endl;
    //     return false;
    // }
    // string line;
    // while(getline(file, line)){
    //     if(line.find("POINTS")!= string::npos){
    //         int numvertices;
    //         istringstream iss(line);
    //         iss >> line>> numvertices;
    //         // vertices.reserve(numvertices);

    //         for (int i=0; i<numvertices; i++){
    //             getline(file, line);
    //             vector<double> coords =splitstring(line);
    //             if (coords.size()!=3){
    //                 cerr<< "Not able to read the line"<< i+1<<endl;
    //                 return false;
    //             }

    //             vector<double> vertex;
    //             vertex= {coords[0], coords[1], coords[2]};
    //             vertices.push_back(vertex);
    //         }
    //     }
    //     else if (line.find("CELLS") !=string:: npos){
    //         int numtets;
    //         istringstream iss(line);
    //         iss>> line>> numtets;
    //         //Tetrahedra.reserve(numtets);
    //         for(int i=0; i<numtets; i++){
    //             getline(file, line);
    //             vector<double> cellindices= splitstring(line);
    //             if (cellindices.size()!=4 && cellindices[0] !=3){
    //                 cerr<< "Not able to read tetrahedra at line: "<< i+1<<endl;
    //                 return false;
    //             }

    //             //we cast indices as integers.
    //             vector<int> T(3);
    //             T[0]= static_cast<int> (cellindices[1]);
    //             T[1]=static_cast<int> (cellindices[2]);
    //             T[2]= static_cast<int> (cellindices[3]);
    //             tetras.push_back(T);
    //         }
    //     }
    // }

    // Read obj file and write vtk file
    string filename = "bunny.obj";
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
            vector<double> v(3);
            iss>> v[0]>>v[1]>>v[2];
            vertices.push_back(v);
        }
        else if (prefix== "f"){
            vector<int> facesind;
            string indexstr;
            while(iss>> indexstr){
                int vertexindex= stoi(indexstr);
                facesind.push_back(vertexindex-1);
            }
            vector<int> t(3);
            t[0]= facesind[0];
            t[1]= facesind[1];
            t[2]=facesind[2];
            tetras.push_back(t);
        }
    }

    ifstream infile3("valuescalc.txt");
    double value3; 
    while(infile3 >> value3){
        values.push_back(value3);
    }
    infile3.close();
    
    ifstream infile4("true_values.txt");
    double val4; 
    while(infile4>>val4){
        valuestrue.push_back(val4);
    }
    infile4.close();

    ifstream infile5("errors.txt");
    double val5;
    while(infile5 >> val5){
        errors.push_back(val5);
    }
    infile5.close();

    ofstream outfile("mesh0calc.vtk");
    ofstream outfile3("mesherror.vtk");
    ofstream outfile2("mesh0true.vtk");
    outfile<<"# vtk DataFile Version 2.0"<<endl;
    outfile2<<"# vtk DataFile Version 2.0"<<endl;
    outfile3<< "# vtk DataFile Version 2.0"<<endl;
    outfile3<< "Unstructured Grid Example"<<endl;
    outfile<<"Unstructured Grid Example"<<endl;
    outfile2<<"Unstructured Grid Example"<<endl;
    outfile<<"ASCII"<<endl;
    outfile2<<"ASCII"<<endl;
    outfile3<<"ASCII"<<endl;
    outfile<<" "<<endl;
    outfile2<<" "<<endl;
    outfile3<<" "<<endl;
    outfile<< "DATASET UNSTRUCTURED_GRID"<<endl;
    outfile2<< "DATASET UNSTRUCTURED_GRID"<<endl;
    outfile3<< "DATASET UNSTRUCTURED_GRID"<<endl;
    outfile<<"POINTS "<<vertices.size()<<" double"<<endl;
    outfile2<<"POINTS "<<vertices.size()<<" double"<<endl;
    outfile3<<"POINTS "<<vertices.size()<<" double"<<endl;
    for(int i=0; i<vertices.size(); i++){
        outfile<<vertices[i][0]<<" "<<vertices[i][1]<<" "<<vertices[i][2]<<endl;
        outfile2<<vertices[i][0]<<" "<<vertices[i][1]<<" "<<vertices[i][2]<<endl;
        outfile3<<vertices[i][0]<<" "<<vertices[i][1]<<" "<<vertices[i][2]<<endl;
    }
    outfile<<" "<<endl;
    outfile2<<" "<<endl;
    outfile3<<" "<<endl;
    outfile<<"CELLS "<<tetras.size()<<" "<<4*tetras.size()<<endl;
    outfile2<<"CELLS "<<tetras.size()<<" "<<4*tetras.size()<<endl;
    outfile3<<"CELLS "<<tetras.size()<<" "<<4*tetras.size()<<endl;
    for(int i=0; i<tetras.size(); i++){
        outfile<<"3 "<< tetras[i][0]<<" "<< tetras[i][1]<<" "<<tetras[i][2]<<endl;
        outfile2<<"3 "<<tetras[i][0]<<" "<< tetras[i][1]<<" "<< tetras[i][2]<<endl;
        outfile3<<"3 "<<tetras[i][0]<<" "<< tetras[i][1]<<" "<< tetras[i][2]<<endl;
    }
    outfile<<" "<<endl;
    outfile2<<" "<<endl;
    outfile3<<" "<<endl;
    outfile<<"CELL_TYPES "<< tetras.size()<<endl;
    outfile2<<"CELL_TYPES "<<tetras.size()<<endl;
    outfile3<<"CELL_TYPES "<<tetras.size()<<endl;
    for(int i=0; i<tetras.size(); i++){
        outfile<< "5"<<endl;
        outfile2<<"5"<<endl;
        outfile3<<"5"<<endl;
    }
    outfile<<" " <<endl;
    outfile2<<" "<<endl;
    outfile3<<" "<<endl;
    outfile<< "POINT_DATA "<< vertices.size()<<endl;
    outfile2<<"POINT_DATA "<<vertices.size()<<endl;
    outfile3<<"POINT_DATA "<<vertices.size()<<endl;
    outfile<<"SCALARS sample_scalars double"<<endl;
    outfile2<<"SCALARS sample_scalars double" <<endl;
    outfile3<<"SCALARS sample_scalars double" <<endl;
    outfile<<"LOOKUP_TABLE default"<<endl;
    outfile2<<"LOOKUP_TABLE default"<<endl;
    outfile3<<"LOOKUP_TABLE default"<<endl;
    for(int i=0; i<values.size(); i++){
        outfile<<values[i]<<endl;
        outfile2<<valuestrue[i]<<endl;
        outfile3<<errors[i]<<endl;
    }
    outfile.close();
    outfile2.close();
    outfile3.close();
    return 0;
}
