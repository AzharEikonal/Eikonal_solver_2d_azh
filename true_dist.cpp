#include <iostream>
#include <fstream>
#include <vector> 
#include <unordered_set>
#include <set>
#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
using namespace std;
vector<vector<double>> vertices;
vector<vector<int>> faces;
set <vector<int>> edges;
double distanceToedge(const vector<double> &v, const vector<int> &edge){
    double x1= vertices[edge[0]][0];
    double y1= vertices[edge[0]][1];
    double x2= vertices[edge[1]][0];
    double y2= vertices[edge[1]][1];
    double numerator = abs((y2- y1) * v[0]-(x2 - x1)*v[1] +x2 *y1 -y2 * x1);
    double denominator = sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
    return numerator/ denominator;
}
int main(){
    ifstream infile("flower.obj");
    string line;
    while(getline(infile, line)){
        istringstream iss(line);
        string prefix;
        iss>>prefix;
        if (prefix =="v"){
            vector<double> vt(2);
            iss>> vt[0]>>vt[1];
            vertices.push_back(vt);
        }
        else if (prefix=="f"){
            vector<int> vf;
            string indexstr;
            while(iss>>indexstr){
                int vindex= stoi(indexstr);
                vf.push_back(vindex-1);
            }
            faces.push_back(vf);
        }
    }
    cout<<vertices.size()<< " "<< faces.size()<<endl;
    map <vector<int>, set<int>> etofMap;
    for (int i=0; i<faces.size(); i++){
        vector<int> edge1={faces[i][0], faces[i][1]};
        sort(edge1.begin(), edge1.end());
        edges.insert(edge1);
        etofMap[edge1].insert(i);
        vector<int> edge2= {faces[i][1], faces[i][2]};
        sort(edge2.begin(), edge2.end());
        edges.insert(edge2);
        etofMap[edge2].insert(i);
        vector<int> edge3= {faces[i][2], faces[i][0]};
        sort(edge3.begin(), edge3.end());
        edges.insert(edge3);
        etofMap[edge3].insert(i);
    }
    set <vector<int>> bd_edges;
    set <int> bd_vertices;
    for (auto & i : etofMap){
        if (i.second.size()==1){
            bd_edges.insert({i.first[0], i.first[1]});
            bd_vertices.insert(i.first[0]);
            bd_vertices.insert(i.first[1]);
        }
    }
    ofstream outfile1("bnd_vertices.txt");
    if (!outfile1.is_open()){
        cerr<<"error opening bnd_v file"<< endl;
        return -1;
    }
    for(const auto &i: bd_vertices){
        outfile1<<i<<"\n";
    }
    outfile1.close();
    // for (auto i: bd_edges){
    //     cout<<i[0]<<" "<<i[1]<<endl;
    // }
    vector<double> distances;
    ofstream outfile("true_values.txt");
    if (!outfile.is_open()){
        cerr<<"error opening file"<<endl;
        return 0;
    }
    for (int i=0; i<vertices.size(); i++){
        double min_distance= numeric_limits<double>::max();
        for(const auto & j: bd_edges){
            double dist= distanceToedge(vertices[i], j);
            min_distance= min(dist, min_distance);
        } 
        distances.push_back(min_distance);
        outfile<<min_distance<<"\n";
        cout<<min_distance<<endl;
    }
    outfile.close();

    return 1;

}

