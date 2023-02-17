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

constexpr int FLOAT_MIN = 0;
constexpr int FLOAT_MAX = 1;
std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> distr(FLOAT_MIN,
                                    FLOAT_MAX);

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
        trace[tour[i+1]][tour[i]]+=intensity;
    }
    trace[tour[tour.size()-1]][tour[0]]+=intensity;
    trace[tour[0]][tour[tour.size()-1]]+=intensity;
}


void disipate(matrix &trace,
               double factor)
{   
    #pragma omp parallel for
    for(int i=0;i<(trace.size()-1);i++){
        #pragma omp simd
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

class ants
{
    public:
    std::vector<int> visited;
    std::vector<int> restant;
};




int main(){

    matrix dMatrix = readCSV( "distances.csv" );
    matrix trace = readCSV( "trace.csv" );
    std::srand(std::time(nullptr));
    std::setprecision(6);

    int N = dMatrix.size();
    int rounds = 5000;
    int n_ants = 500;
    //std::cout<<"Colony size"<<std::endl;
    //std::cin>>n_ants;
    //std::cout<<"Rounds"<<std::endl;
    //std::cin>>rounds;
    

    std::vector<std::vector<int>> travels;
    std::vector<double> dists;



    for(int i = 0; i<rounds;i++){
        // Initilize positions
        std::vector<ants> colony(n_ants);
        for(int j = 0; j<n_ants;j++){
            std::vector<int> v;
            std::vector<int> r(dMatrix.size());
            #pragma omp parallel for
            for(int k=0;k<dMatrix.size();k++){
                r[k]=k;
            }
            int actual=(int)(distr(eng)*(N-1));
            auto ref = std::find(r.begin(), r.end(), actual);
            r.erase(ref);
            colony[j].visited.push_back(actual);
            colony[j].restant = r;
        }
        // trace complete tours for all ants
        for(int p = 0;p<dMatrix.size()-1;p++){
            omp_set_num_threads(12);
            #pragma omp parallel for
            for(int j = 0;j<n_ants;j++){
                int next = select_next(dMatrix,
                                       trace,
                                       colony[j].visited,
                                       colony[j].restant,
                                       colony[j].visited[colony[j].visited.size()-1]);
                colony[j].visited.push_back(next);
                auto ref = std::find(colony[j].restant.begin(), colony[j].restant.end(), next);
                colony[j].restant.erase(ref);
            }
        }

        //deposite trace and get best
        double best_cost = 100000000;
        int best_travel = 0;
        for(int j = 0; j<n_ants;j++){
            double travel_cost = get_cost(dMatrix,colony[j].visited);
            add_trace(trace,colony[j].visited,50000/travel_cost);
            if(travel_cost<best_cost){
                best_cost = travel_cost;
                best_travel = j;
            }
        }
        disipate(trace,0.95);
        travels.push_back(colony[best_travel].visited);
        dists.push_back(best_cost);
    }
    std::cout<<"------------------------------"<<std::endl;
    std::cout<<"Route: ";
    int best = 10000000;
    int optimal_travel = 0;
    for(int j=0; j<dists.size();j++){
        if(dists[j]<best){
                best = dists[j];
                optimal_travel = j;
            }
    }
    std::cout<<std::endl;
    std::cout<<"Best distance: "<<best<<std::endl;

    for(int j=0; j<travels[0].size();j++){
        std::cout<<travels[optimal_travel][j]<<" ";
    }
    return 0;
}

