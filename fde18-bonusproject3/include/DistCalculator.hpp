#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <set>

using namespace std;

class DistCalculator{
public:
    using Node = uint64_t;
    DistCalculator(string edgeListFile);
    int64_t dist(Node a, Node b);
    vector<string> getWordByIndex(string* s, set<int>* index);
    const int num_actors = 1971697;
    const int num_movies = 1151759;
    vector<vector<Node>> actor2movie = vector<vector<Node>>(num_actors);
    vector<vector<Node>> movie2actor = vector<vector<Node>>(num_movies);
   };
