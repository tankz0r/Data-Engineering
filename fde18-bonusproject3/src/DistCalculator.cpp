#include "DistCalculator.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include<fstream>
#include<sstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <set>
#include <assert.h>
#include <iostream>
#include <zconf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>

using namespace std;

DistCalculator::DistCalculator(string edgeListFile)
{
        int handle = open(edgeListFile.c_str(), O_RDONLY);
//        if (handle<0) {
//            cerr << "unable to open"  << endl;
//        }
        long fileSize = lseek(handle, 0, SEEK_END);
        auto fileBegin = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, handle, 0));
        auto fileEnd=fileBegin+fileSize;

        vector<string> wordsbyindex;
        Node first, second;
        set<int> v_index = {0, 2};

        auto startLine=fileBegin;
        auto endLine=fileBegin;
        int i = 0;
        while((endLine != fileEnd) && (startLine != fileEnd)){
            endLine = (char *) memchr(startLine, '\n', startLine - fileEnd);
            //char newLine[endLine - startLine+1]={0};
            //memcpy(newLine, startLine, endLine - startLine);
            string newLineString(startLine, endLine);
            if (i == 0) {
                i = i + 1;
                startLine = endLine+1;
                continue;
            }
            wordsbyindex = getWordByIndex(&newLineString, &v_index);
            first = stoull(wordsbyindex[0]);
            second = stoull(wordsbyindex[1]);
            actor2movie[first].push_back(second);
            movie2actor[second].push_back(first);
            startLine = endLine+1;
        }

        if (munmap(fileBegin, fileSize) == -1)
        {
            close(handle);
//            perror("Error un-mmapping the file");
            exit(EXIT_FAILURE);
        }
        close(handle);
}

int64_t DistCalculator::dist(Node a, Node b) {
    if (a == b)
        return 0;

    vector<bool> a_visited_actors = vector<bool>(num_actors, false);
    vector<bool> b_visited_actors = vector<bool>(num_actors, false);
    vector<bool> visited_movies = vector<bool>(num_movies,false);
//    unordered_set<Node> visited_movies;
    vector<Node> a_actor_dist = vector<Node>(num_actors, 0);
    vector<Node> b_actor_dist = vector<Node>(num_actors, 0);
    queue<Node> a_queue;
    queue<Node> b_queue;

    a_queue.push(a);
    b_queue.push(b);

    a_visited_actors[a] = true;
    b_visited_actors[b] = true;

    while (!a_queue.empty() || !b_queue.empty()) {
        long a_queue_len = a_queue.size();
        for (int i =0; i<a_queue_len; ++i)
        {
            Node front = a_queue.front();
            a_queue.pop();
            vector<Node>& a_front_movies = actor2movie[front];
            Node front_dist = a_actor_dist[front];
            if (!a_front_movies.empty())
            {
                for (auto movie = a_front_movies.begin(); movie < a_front_movies.end(); ++movie)
                {
                    if(!visited_movies[*movie])
                    {
                        visited_movies[*movie] = true;
                        vector<Node>& actors = movie2actor[*movie];
                        for (auto actor = actors.begin(); actor < actors.end(); ++actor)
                        {
                            if(!a_visited_actors[*actor])
                            {
                                a_visited_actors[*actor] = true;
                                a_actor_dist[*actor] = front_dist + 1;
                                a_queue.push(*actor);
                            }
                        }
                    }
                }
            }
        }

        long b_queue_len = b_queue.size();
        for (int i =0; i<b_queue_len; ++i)
        {
            Node front = b_queue.front();
            b_queue.pop();
            vector<Node>& b_front_movies = actor2movie[front];
            Node front_dist = b_actor_dist[front];
            if (!b_front_movies.empty())
            {
                for (auto movie = b_front_movies.begin(); movie < b_front_movies.end(); ++movie)
                {
                    if(!visited_movies[*movie])
                    {
                        visited_movies[*movie] = true;
                        vector<Node>& actors = movie2actor[*movie];
                        for (auto actor = actors.begin(); actor < actors.end(); ++actor)
                        {
                            if(!b_visited_actors[*actor])
                            {
                                b_visited_actors[*actor] = true;
                                b_actor_dist[*actor] = front_dist + 1;
                                b_queue.push(*actor);
                            }
                        }
                    }
                }
            }
        }

        Node minimum = 1000000;

        for (auto i =0; i<a_visited_actors.size(); ++i)
        {
            if (a_visited_actors[i] && b_visited_actors[i])
            {
                auto curr_dist = a_actor_dist[i] + b_actor_dist[i];
                if (curr_dist < minimum) {
                    minimum = curr_dist;
                }
            }
        }
        if (minimum != 1000000) return minimum;
    }
    return -1;
}

vector<string> DistCalculator::getWordByIndex(string* s, set<int>* index){
    string delimiter = ",";
    size_t pos = 0;
    int counter = 0;
    int counter_pos = 0;
    string token;
    vector<string> result;
    result.reserve(index->size());
    while (((pos = s->find(delimiter)) != string::npos) && (counter_pos!=3)) {
        token = s->substr(0, pos);
        if (index->find(counter) != index->end()) {
            result.push_back(token);
            counter_pos ++;
        }
        s->erase(0, pos + delimiter.length());
        result.push_back(*s);
        counter++;
    }
    return result;
};


