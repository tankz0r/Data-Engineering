#include "JoinQuery.hpp"
#include <assert.h>
#include <fstream>
#include <thread>
#include <iostream>
#include <zconf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>

//---------------------------------------------------------------------------
///home/denys/Code/ML/courses/Foundations_in_Data_Engineering/HW/HW2/files
JoinQuery::JoinQuery(std::string lineitem, std::string order,
                     std::string customer)
{
    f_lineitem = lineitem;
    f_order = order;
    f_customer = customer;
}
//---------------------------------------------------------------------------
size_t JoinQuery::avg(std::string segmentParam)
{
    int handle = open(f_customer.c_str(), O_RDONLY);
    if (handle<0) {
        std::cerr << "unable to open" << f_customer << std::endl;
        return 1;
    }
    long fileSize = lseek(handle, 0, SEEK_END);
    auto fileBegin = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, handle, 0));
    auto fileEnd=fileBegin+fileSize;

    std::unordered_set<int> v_customer;
    std::vector<std::string> wordsbyindex;
    int o_first, o_second;
    std::set<int> v_index = {0, 6};

    auto startLine=fileBegin;
    auto endLine=fileBegin;
    while((endLine != fileEnd) && (startLine != fileEnd)){
        endLine = (char *) memchr(startLine, '\n', startLine - fileEnd);
        char newLine[endLine - startLine];
        memcpy(newLine, startLine, endLine - startLine);
        std::string newLineString(newLine);
        wordsbyindex = getWordByIndex(&newLineString, &v_index);
        o_first = std::stoi(wordsbyindex[0]);
        if (wordsbyindex[1] == segmentParam)
            v_customer.insert(o_first);
        startLine = endLine+1;
    }

    if (munmap(fileBegin, fileSize) == -1)
    {
        close(handle);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    close(handle);
// ----------------------------------------------------
    std::set<int> o_index = {0, 1};
    std::unordered_set<int> v_orders;

    handle = open(f_order.c_str(), O_RDONLY);
    if (handle<0) {
        std::cerr << "unable to open" << f_order << std::endl;
        return 1;
    }
    fileSize = lseek(handle, 0, SEEK_END);
    fileBegin = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, handle, 0));
    fileEnd=fileBegin+fileSize;

    startLine=fileBegin;
    endLine=fileBegin;
    while((endLine != fileEnd) && (startLine != fileEnd)){
        endLine = (char *) memchr(startLine, '\n', startLine - fileEnd);
        char newLine[endLine - startLine];
        memcpy(newLine, startLine, endLine - startLine);
        std::string newLineString(newLine);
        wordsbyindex = getWordByIndex(&newLineString, &o_index);
        o_first = std::stoi(wordsbyindex[0]);
        o_second = std::stoi(wordsbyindex[1]);
        if (v_customer.find(o_second) != v_customer.end())
            v_orders.insert(o_first);
        startLine = endLine+1;
    }

    if (munmap(fileBegin, fileSize) == -1)
    {
        close(handle);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    close(handle);
//---------------------------------------------------------------------------
    uint64_t avg = 0;
    uint64_t counter = 0;

    handle = open(f_lineitem.c_str(), O_RDONLY);
    if (handle<0) {
        std::cerr << "unable to open" << f_lineitem << std::endl;
        return 1;
    }
    fileSize = lseek(handle, 0, SEEK_END);
    fileBegin = static_cast<char*>(mmap(nullptr, fileSize, PROT_READ, MAP_SHARED, handle, 0));
    fileEnd=fileBegin+fileSize;

    unsigned threadCount = std::thread::hardware_concurrency();
    auto getThreadBounds = [fileBegin, threadCount, fileSize](unsigned index){
        return fileBegin+(index*fileSize/threadCount);
    };
    std::vector<std::thread> threads;
    for (unsigned index=0;index!=threadCount;++index){
        threads.push_back(std::thread([&avg, &counter, &getThreadBounds, fileEnd, index, &v_orders, this](){
            auto result = this->computeAvg(getThreadBounds(index), getThreadBounds(index+1), fileEnd, &v_orders, index);
            avg += result.first;
            counter += result.second;
        }));
    }

    for (auto& t:threads) t.join();

    if (munmap(fileBegin, fileSize) == -1)
    {
        close(handle);
        perror("Error un-mmapping the file");
        exit(EXIT_FAILURE);
    }
    close(handle);

    return avg*100/counter;
}

std::pair<uint64_t, uint64_t> JoinQuery::computeAvg(char* partitionBegin, char* partitionEnd, char* fileEnd, std::unordered_set<int>* v_orders, unsigned index){
    std::vector<std::string> wordsbyindex;
    std::set<int> l_index = {0, 4};
    int o_first;
    uint64_t o_second;
    uint64_t avg=0;
    uint64_t counter=0;

    if (index == 0){
        partitionEnd = (char*) memchr(partitionEnd, '\n', partitionEnd - fileEnd);
    }
    else {
        partitionBegin = (char*) memchr(partitionBegin, '\n', partitionBegin - partitionEnd)+1;
        if (partitionEnd == fileEnd)
            partitionEnd = fileEnd;
        else
            partitionEnd = (char*) memchr(partitionEnd, '\n', partitionEnd - fileEnd);
    }

    auto startLine = partitionBegin;
    auto endLine = partitionBegin;

    while((endLine != partitionEnd) && (startLine != partitionEnd)){
        endLine = (char *) memchr(startLine, '\n', startLine - partitionEnd);
        char newLine[endLine - startLine];
        memcpy(newLine, startLine, endLine - startLine);
        std::string newLineString(newLine);
        wordsbyindex = getWordByIndex(&newLineString, &l_index);
        o_first = std::stoi(wordsbyindex[0]);
        o_second = (uint64_t) std::stod(wordsbyindex[1]);
        if (v_orders->find(o_first) != v_orders->end()) {
            avg += o_second;
            counter++;
        }
        startLine = endLine+1;
    }
    return std::pair<uint64_t, uint64_t >(avg, counter);
}

std::vector<std::string> JoinQuery::getWordByIndex(std::string* s, std::set<int>* index){
        std::string delimiter = "|";
        size_t pos = 0;
        int counter = 0;
        int counter_pos = 0;
        std::string token;
        std::vector<std::string> result;
        result.reserve(index->size());
        while (((pos = s->find(delimiter)) != std::string::npos) && (counter_pos!=2)) {
            token = s->substr(0, pos);
            if (index->find(counter) != index->end()) {
                result.push_back(token); // here we can reduce number of checks two times
                counter_pos ++;
            }
            s->erase(0, pos + delimiter.length());
            counter++;
        }
        return result;
};

//---------------------------------------------------------------------------
size_t JoinQuery::lineCount(std::string rel)
{
   std::ifstream relation(rel);
   assert(relation);  // make sure the provided string references a file
   size_t n = 0;
   for (std::string line; std::getline(relation, line);)
       n++;
   return n;
}
//---------------------------------------------------------------------------
