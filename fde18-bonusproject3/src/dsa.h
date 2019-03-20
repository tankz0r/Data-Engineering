#include "DistCalculator.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <unordered_map>
#include <unordered_set>

DistCalculator::DistCalculator(std::string edgeListFile)
{
    // TODO: implement graph parsing here
    // lseek(c_file, 0, SEEK_END);
    // int c_size = lseek(c_file, 0, SEEK_CUR);
    // char* c_data = (char*)mmap(0, c_size, PROT_READ, MAP_SHARED, c_file, 0);
    std::ifstream input_file(edgeListFile);
    Node actor, movie;
    std::string str;
    char c;
    std::getline(input_file, str);
    while (input_file >> actor && input_file >> c && input_file >> movie) {
        actors_to_movies.at(actor).push_back(movie);
        movies_to_actors.at(movie).push_back(actor);
    }
}

int64_t DistCalculator::Bidirectional_BFS(Node a, Node b)
{
    if (a == b) return 0;

    std::queue<Node> fwd_queue;
    std::queue<Node> bwd_queue;
    std::unordered_set<Node> movies_to_check;
    std::unordered_map<Node, int64_t> fwd_actors_to_check;
    std::unordered_map<Node, int64_t> bwd_actors_to_check;
    fwd_queue.push(a);
    bwd_queue.push(b);
    int64_t fwd_cur_dist = 0;
    int64_t bwd_cur_dist = 0;
    int64_t min_actual_dist = -1;
    int64_t min_possible_dist = 0;
    Node fwd_cur_actor = a;
    Node bwd_cur_actor = b;
    std::unordered_map<Node, int64_t>::iterator fwd_it;
    std::unordered_map<Node, int64_t>::iterator bwd_it;
    fwd_actors_to_check.insert({fwd_cur_actor, fwd_cur_dist});
    bwd_actors_to_check.insert({bwd_cur_actor, bwd_cur_dist});

    while (!fwd_queue.empty() && !bwd_queue.empty()) {

        min_possible_dist = fwd_actors_to_check[fwd_queue.front()] + bwd_actors_to_check[bwd_queue.front()] + 1;
        if ((min_actual_dist != -1)  && (min_actual_dist <= min_possible_dist)) return min_actual_dist;

        fwd_cur_actor = fwd_queue.front();
        fwd_queue.pop();
        fwd_cur_dist = fwd_actors_to_check[fwd_cur_actor];
        fwd_cur_dist += 1;
        for (auto const& movie: actors_to_movies[fwd_cur_actor]){
            if (movies_to_check.find(movie) == movies_to_check.end()) {
                movies_to_check.insert(movie);
                for (auto const& actor: movies_to_actors[movie]) {
                    if (fwd_actors_to_check.find(actor) == fwd_actors_to_check.end()) {
                        fwd_it = bwd_actors_to_check.find(actor);
                        if (fwd_it == bwd_actors_to_check.end()) {
                            fwd_actors_to_check.insert({actor, fwd_cur_dist});
                            fwd_queue.push(actor);
                        }
                        else {
                            fwd_actors_to_check.insert({actor, fwd_cur_dist});
                            min_actual_dist = fwd_it->second + fwd_cur_dist;
                            if (min_actual_dist <= min_possible_dist) return min_actual_dist;

                        }
                    }
                }
            }
        }

        bwd_cur_actor = bwd_queue.front();
        bwd_queue.pop();
        bwd_cur_dist = bwd_actors_to_check[bwd_cur_actor];
        bwd_cur_dist += 1;
        for (auto const& movie: actors_to_movies[bwd_cur_actor]){
            if (movies_to_check.find(movie) == movies_to_check.end()) {
                movies_to_check.insert(movie);
                for (auto const& actor: movies_to_actors[movie]) {
                    if (bwd_actors_to_check.find(actor) == bwd_actors_to_check.end()) {
                        bwd_it = fwd_actors_to_check.find(actor);
                        if (bwd_it == fwd_actors_to_check.end()) {
                            bwd_actors_to_check.insert({actor, bwd_cur_dist});
                            bwd_queue.push(actor);
                        }
                        else {
                            bwd_actors_to_check.insert({actor, bwd_cur_dist});
                            min_actual_dist = bwd_it->second + bwd_cur_dist;
                            if (min_actual_dist <= min_possible_dist) return min_actual_dist;
                        }
                    }
                }
            }
        }
    }

    return min_actual_dist;
}


int64_t DistCalculator::dist(Node a, Node b)
{
    // TODO: implement distance calculation here
    return Bidirectional_BFS(a, b);
}