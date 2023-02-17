#include <math.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

using vec = vector<double>;
using matrix = vector<vec>;

//======================================================================

matrix readCSV( string filename )
{
   char separator = ',';
   matrix result;
   string row, item;

   ifstream in( filename );
   while( getline( in, row ) )
   {
      vec R;
      stringstream ss( row );
      while ( getline ( ss, item, separator ) ) R.push_back( stod(item) );
      result.push_back( R );
   }
   in.close();
   return result;
}

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;
std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(FLOAT_MIN,
                                    FLOAT_MAX);

double get_cost(matrix &dMatrix,
                std::vector<int> tour)
{
    double distance = 0;
    for(int i=0;i<(tour.size()-1);i++){
        distance+=dMatrix[tour[i]][tour[i+1]];
    }
    distance+=dMatrix[tour[tour.size()-1]][tour[0]];
    return distance;
}

void add_trace(matrix &trace,
               std::vector<int> tour,
               double intensity)
{
    #pragma omp simd
    for(int i=0;i<(tour.size()-1);i++){
        trace[tour[i]][tour[i+1]]+=intensity;
    }
    trace[tour[tour.size()-1]][tour[0]]+=intensity;
}


void disipate(matrix &trace,
               double factor)
{
    #pragma omp simd
    for(int i=0;i<(trace.size()-1);i++){
        for(int j=i;j<(trace.size()-1);j++){
        trace[i][j]=trace[i][j]*factor;
        trace[j][i]=trace[j][i]*factor;
        }
    }

}

int select_next(matrix &dMatrix,
                matrix trace,
                std::vector<int> visited,
                std::vector<int> restant,
                int actual){
    double suma = 0; //total distance
    std::vector<double> probs;
    for(int i=0; i<restant.size();i++){
      probs.push_back(1/dMatrix[actual][restant[i]]);
      suma+=1/dMatrix[actual][restant[i]];
    }

    double last = 0;
    double coin_flip = distr(eng)*suma;
    int choice = 0;
    for(int i=0; i<restant.size();i++){
      if((coin_flip > last) and (coin_flip<=(last+probs[i]))){
          choice=i;
          last = last + probs[i];
      }else{
          last = last + probs[i];
      }
    }
    return restant[choice];
    };



int main(){

    matrix dMatrix = readCSV( "distances.csv" );
    matrix trace = readCSV( "trace.csv" );
    std::srand(std::time(nullptr));
    std::setprecision(6);

    int N = dMatrix.size();
    int rounds;
    int global_rounds;
    std::cout<<"Local rounds"<<std::endl;
    std::cin>>rounds;
    std::cout<<"Global rounds"<<std::endl;
    std::cin>>global_rounds;

    std::vector<std::vector<int>> global_travels;
    std::vector<double> global_dists;

    for(int k = 0;k<global_rounds;k++){
        std::vector<int> travel;
        std::vector<std::vector<int>> travels;
        std::vector<double> dists;
        for(int i = 0; i<rounds;i++){
            std::vector<int> r;
            for(int j=0;j<dMatrix.size();j++){
                r.push_back(j);
            }
            int actual=(int)(distr(eng)*(N-1));
            auto ref = std::find(r.begin(), r.end(), actual);
            r.erase(ref);
            std::vector<int> v = {actual};

            while(r.size()>=1){
                int choice = select_next(dMatrix,trace,v,r,actual);
                v.push_back(choice);
                auto ref = std::find(r.begin(), r.end(), choice);
                r.erase(ref);
                actual=choice;
            }
            double cost_t=get_cost(dMatrix,v);
            dists.push_back(cost_t);
            travels.push_back(v);
            add_trace(trace, v, 30000/cost_t);
            disipate(trace,0.92);

        }
        


        //get best
    int best_travel = 0;
    double best_cost = 100000000;
    for(int i=0; i<rounds;i++){
        if(dists[i]<best_cost){
            best_cost = dists[i];
            best_travel = i;
        }
    }
    global_travels.push_back(travels[best_travel]);
    global_dists.push_back(best_cost);
    add_trace(trace, travels[best_travel], 20000/best_cost);
    disipate(trace,0.96);
    }
    std::cout<<"------------------RESULT--------------------"<<std::endl;
    int best_global = 0;
    double best_global_cost = 100000000;
    for(int i=0; i<global_rounds;i++){
        if(global_dists[i]<best_global_cost){
            best_global_cost = global_dists[i];
            best_global = i;
        }
    }
    std::cout<<"Route: ";
    for(int i=0; i<global_travels[best_global].size();i++){
        std::cout<<global_travels[best_global][i]<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"Best distance: "<<best_global_cost<<std::endl;




    return 0;
}

