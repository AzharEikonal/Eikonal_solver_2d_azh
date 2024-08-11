#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;

struct Vertex{
    double x,y,z;
};
struct triangles{
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

vector<int> onering(vector<triangles> &faces, int ind){
    vector<int> one_ring;
    set<int> st;
    for(int i=0; i<faces.size(); i++){
        if (ind ==faces[i].v1){
            st.insert(faces[i].v2);
            st.insert(faces[i].v3);
        }
        else if (ind == faces[i].v2){
            st.insert(faces[i].v1);
            st.insert(faces[i].v3); 
        }
        else if (ind == faces[i].v3){
            st.insert(faces[i].v1);
            st.insert(faces[i].v2);
        }
        else {
            continue;
        }
    }
    one_ring.assign(st.begin(), st.end());
    return one_ring;
}
double sq(double n){
    return n*n;
}
double norm(Vertex v){
    return sqrt(v.x*v.x+ v.y*v.y + v.z*v.z);
}

// 2D local solver
double localsolver_2D(Vertex v1, Vertex v2, Vertex v3, double phi1, double phi2){
    double tol = 1e-12;
    double phi= phi2-phi1;
    Vertex e13;
    e13.x= v3.x- v1.x;
    e13.y=v3.y- v1.y;
    e13.z= v3.z- v1.z;

    Vertex e12;
    e12.x = v2.x- v1.x;
    e12.y= v2.y- v1.y;
    e12.z= v2.z- v1.z;

    double a1= e13.x;
    double b1= -e12.x;
    double a2=e13.y;
    double b2= -e12.y;
    double a3= e13.z;
    double b3= -e12.z;

    Vertex e32;
    e32.x= v3.x-v2.x;
    e32.y= v3.y-v2.y;
    e32.z= v3.z-v2.z;

    if (phi1 == INFINITY && phi2 < INFINITY) {
        double phi3 = phi2 + norm(e32);
        return phi3;
    }
    else if (phi1 < INFINITY && phi2 == INFINITY) {
        double phi3 = phi1 + norm(e13);
        return phi3;
    }
    else if (phi1 == INFINITY && phi2 == INFINITY) {
        return INFINITY;
    } 
    else {
        double A= (sq(phi)-sq(b1))*sq(b1) + (sq(phi)-sq(b2))*sq(b2) + (sq(phi)- sq(b3))*sq(b3) - 2*sq(b1)*sq(b2) - 2*sq(b1)*sq(b3) - 2*sq(b2)*sq(b3);
        double B= (sq(phi)- sq(b1))*2*a1*b1 + (sq(phi)- sq(b2))*2*a2*b2 + (sq(phi)-sq(b3))*2*a3*b3 - sq(b3)*2*a2*b2 - sq(b2)*2*a3*b3 - sq(b1)*2*a3*b3 - sq(b3)*2*a1*b1 - sq(b2)*2*a1*b1 - sq(b1)*2*a2*b2;
        double C= (sq(phi)- sq(b1))*sq(a1) + (sq(phi)- sq(b2))*sq(a2) + (sq(phi)- sq(b3))*sq(a3) - 2*a1*a2*b1*b2 - 2*a2*a3*b2*b3 - 2*a1*a3*b1*b3;
        if (abs(A) <tol) {
            if (abs(B) < tol) {
                double phi3 = min(phi1 + norm(e13), phi2 + norm(e32));
                return phi3;
            } 
            else {
                double lambda1 = -C/B;
                Vertex t;
                t.x= v3.x- v1.x -lambda1*(v2.x-v1.x);
                t.y= v3.y- v1.y -lambda1*(v2.y-v1.y);
                t.z= v3.z- v1.z- lambda1*(v2.z-v1.z);
                double phi3 =lambda1 * phi + phi1 + norm(t);
                return phi3;
            }
        }
        if (B*B-4*A*C<0){
            double phi3 = min(phi1 + norm(e13), phi2 + norm(e32));
            return phi3;
        }
        else {
            double lambda1 = (-B+sqrt(B*B-4*A*C))/(2*A);
            double lambda2 = (-B-sqrt(B*B-4*A*C))/(2*A);

            if (lambda1 > 0 && lambda1<1 && lambda2 >0 && lambda2<1) {
                Vertex t1;
                t1.x= v3.x- v1.x -lambda1*(v2.x-v1.x);
                t1.y= v3.y-v1.y - lambda1*(v2.y-v1.y);
                t1.z= v3.z- v1.z - lambda1*(v2.z-v1.z);

                Vertex t2;
                t2.x= v3.x- v1.x - lambda2*(v2.x-v1.x);
                t2.y= v3.y-v1.y - lambda2*(v2.y-v1.y);
                t2.z= v3.z- v1.z -lambda2*(v2.z-v1.z);

                double phi3 =min(lambda1 * phi + phi1 + norm(t1), lambda2 * phi + phi1 + norm(t2));
                return phi3;
            }
            else if (lambda1 > 0 && lambda1<1) {
                Vertex t; 
                t.x= v3.x- v1.x -lambda1*(v2.x-v1.x);
                t.y= v3.y-v1.y - lambda1*(v2.y-v1.y);
                t.z= v3.z- v1.z - lambda1*(v2.z-v1.z);

                double phi3 = lambda1 * phi + phi1 + norm(t);
                return phi3;
            }
            else if (lambda2 > 0 && lambda2<1) {
                Vertex t; 
                t.x= v3.x- v1.x - lambda2*(v2.x-v1.x);
                t.y= v3.y - v1.y - lambda2*(v2.y-v1.y);
                t.z= v3.z- v1.z -lambda2*(v2.z-v1.z);

                double phi3 =lambda2 * phi +phi1 + norm(t);
                return phi3;
            }
            else {
                double phi3 =min(phi1 + norm(e13), phi2 + norm(e32));
                return phi3;
            }
        }
    }

}

double update(int v, vector<double> &values, vector<Vertex> &vertices, vector<triangles> &faces){
    vector<int> oner= onering(faces,v);
    vector<double> valueslist;
    for(int i=0; i<oner.size(); i++){
        int indexv1= oner[i];
        int indexv2 = oner[(i+1)%oner.size()];
        int indexv3= v;
        double phi1= values[indexv1];
        double phi2= values[indexv2];
        Vertex v1= vertices[indexv1];
        Vertex v2= vertices[indexv2];
        Vertex v3= vertices[indexv3];
        double val;
        val=localsolver_2D(v1, v2, v3, phi1, phi2);  
        valueslist.push_back(val);   
    }
    double phi3=*min_element(valueslist.begin(), valueslist.end());
    return phi3;
}

bool IsvinL (int v, vector<int> L){
    for(int i=0; i<L.size(); i++){
        if (v== L[i]){
            return true;
        }
        else continue;
    }
    return false;
}

int main(){
    // string filename1 = "vertices.txt";
    // string filename2= "faces.txt";
    // ifstream vertices_file(filename1);
    // ifstream faces_file(filename2);
    // double x,y,z;
    // while (vertices_file >> x>>y>>z){
    //     Vertex v;
    //     v.x=x;
    //     v.y=y;
    //     v.z=z;
    //     vertices.push_back(v);
    // }
    // vertices_file.close();

    // int a, b,c;
    // while (faces_file>> a>>b>>c){
    //     triangles t;
    //     t.v1=a;
    //     t.v2=b;
    //     t.v3=c;
    //     faces.push_back(t);
    // }
    // faces_file.close();

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

    // // read vtk file:
    // string filename= "square_refine.vtk";
    // readVTKfile(filename, vertices, faces);
    vector<int> B;
    B=onering(faces, 1000);
    // B={2,3,4,5,6,7,8};
    // ifstream infile_("bnd_vertices.txt");
    // if (infile_.is_open()){
    //     double val;
    //     while(infile_>> val){
    //         B.push_back(val);
    //     }
    //     infile_.close();
    // }

    // //switch:
    ofstream outfile1("bnd_vertices.txt");
    if(outfile1.is_open()){
        for (int i=0; i<B.size(); i++){
            outfile1<<B[i]<<"\n";
        }
        outfile1.close();
    }
    // return 0;

    vector <double> values;
    for(int i=0; i<vertices.size(); i++){
        if (IsvinL(i,B)){
            values.push_back(0);
        }
        else values.push_back(INFINITY);
    }
    
    cout<< "size of vertices is : "<<vertices.size()<<endl;
    cout<< "size of faces is : "<<faces.size()<<endl;
    vector<int> L;
    //initialise the active list
    for(int i=0; i<vertices.size(); i++){
        if (!IsvinL(i,B)){
            if (onering(faces,i).size()!=0){
                for(int j: onering(faces,i)){
                    if (IsvinL(j,B)){
                        if (!IsvinL(i,L)){
                            L.push_back(i);
                        }
                    }
                }
            }
        }
    }
    cout<<"sizeof L is: "<<L.size()<<endl;
    double tolerance=1e-12;
    clock_t start, end;
    start= clock();
    int count =0;
    while(!L.empty()){
        int ls_old=L.size();
        for(int i=0; i<vertices.size(); i++){
            if(!IsvinL(i,B)){
                double p=values[i];
                double q= update(i, values, vertices, faces);
                if (abs(p-q)<tolerance){
                    for (int j:onering(faces, i)){
                        if (!IsvinL(j,L)){
                            p=values[j];
                            q=update(j, values, vertices, faces);
                            if (p>q){
                                values[j]=q;
                                L.push_back(j);
                            }
                        }
                    }
                    if (IsvinL(i,L)){
                        L.erase(remove(L.begin(), L.end(), i), L.end());
                    }
                }
                else {
                    values[i]= min(p,q);
                }
            }
        }
        int ls_new= L.size();
        cout<<"size of L is: "<<L.size()<<endl;
        int diff= ls_old-ls_new;
        if (diff==0){
            count++;
        }
        if (count >=10){
            break;
        }
    }
    end=clock();

    double time_taken= double (end-start)/double (CLOCKS_PER_SEC);
    cout<< "Time taken by the cpu is "<<time_taken<<endl;
    ofstream outputfile1 ("valuescalc.txt");
    if (outputfile1.is_open()){
        for(int i=0; i<values.size(); i++){
            outputfile1<<values[i]<<endl;
        }
    }
    outputfile1.close();
    // // zero level set of the unsigned distance field
    // vector<int> zero_level_set;
    // for (int i=0; i<values.size(); i++){
    //     if (values[i]<2*1e-2) zero_level_set.push_back(i);
    // }

    // ofstream outputfilek ("zero_set.txt");
    //     if (outputfilek.is_open()){
    //         for (int i=0; i<zero_level_set.size(); i++){
    //             outputfilek<< zero_level_set[i]<<endl;
    //         }
    //     }
    // outputfilek.close();

    vector<double> values1;
    // for(int i=0; i<vertices.size(); i++){
    //     Vertex diff;
    //     diff.x= vertices[i].x-vertices[0].x;
    //     diff.y= vertices[i].y-vertices[0].y;
    //     diff.z=vertices[i].z-vertices[0].z;
    //     values1.push_back(norm(diff));
    // }

    // // ofstream outputfile2("valuestrue.txt");
    // // if (outputfile2.is_open()){
    // //     for(int i=0; i<vertices.size(); i++){
    // //         outputfile2<<values1[i]<<endl;
    // //     }
    // // }
    // // outputfile2.close();
    ifstream infile ("true_values.txt");
    if (infile.is_open()){
        double value;
        while(infile>> value){
            values1.push_back(value);
        }
        infile.close();
    }
    vector<double> errors;
    for(int i=0; i<vertices.size(); i++){
        errors.push_back(abs(values[i]-values1[i]));
    }

    ofstream outputfile3("errors.txt");
    if (outputfile3.is_open()){
        for (int i=0; i<vertices.size(); i++){
            outputfile3<<errors[i]<<endl;
        }
    }
    outputfile3.close();

    for(int i=0; i<vertices.size(); i++){
        cout<<values[i]<<" ";
    }
    cout<<endl;
    double sum=0;
    cout<<"max of error is : "<<endl;
    cout<< *max_element(errors.begin(), errors.end())<<endl;
    for(int i=0; i<errors.size(); i++){
        cout<<errors[i]<<" ";
        sum=sum+errors[i]*errors[i];
    }
    cout<< "norm of the error per vertex : "<<endl;
    cout<< sqrt(sum) / vertices.size()<<endl;
    cout<< "Time taken by the cpu is "<<time_taken<<endl;
    //Find the relatice error
    vector <double> relerror;

    for (int i=0; i<vertices.size(); i++){
        double rat;
        if (values1[i]==0){
            rat=0;
        }
        else rat= errors[i]/values1[i];
        relerror.push_back(rat);
    }

    double sum3=0;
    for (int i=0; i<vertices.size(); i++){
        sum3+=relerror[i]*relerror[i];
    }
    sum3=sqrt(sum3)/vertices.size();
    cout<< "relative error for : "<<filename<< " mesh with "<<vertices.size() <<endl;
    cout<< sum3<<endl;
    cout<<" max of the relative error: "<< *max_element(relerror.begin(), relerror.end());
    return 1;
    

}
