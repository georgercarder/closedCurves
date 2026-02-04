// copyleft George R. Carder 2026, GNU Affero V3 

#include <stdio.h>
#include <assert.h>
#include <thread>

#include <bits/stdc++.h>

#include "./common.cpp"

using namespace std;

void tGetPathsFromCoords(string color, unique_ptr<unordered_map<string, bool>> ptrCoords, vector<pair<string, string>>* ret) 
{
    //printf("%s\n", color.c_str());
    unordered_map<string, bool> coords = *(ptrCoords.get());
    vector<pair<string, string>> ps = getPathsFromCoords(color, coords);
    ret->insert(ret->end(), ps.begin(), ps.end());
}

int main(int argc, char* argv[])
{

    //flat_hash_map
    //unordered_map
    // on the server std unordered_map is actually faster!! 4s vs 10s on benchmark!
    vector<string> keys;
    vector<string> values;

    if (argc < 2) {
        printf("no args. exiting\n"); 
        return 1;
    } else if (argc == 2) {
        string a = argv[1]; 
        if (a != "demo") {
            printf("invalid input. Solo arg must be 'demo'.\n");
            return 1;
        }
        vector<vector<string>> virtualArgs = getVirtualArgs();
        assert(virtualArgs.size() == 2);
        keys = virtualArgs.at(0);
        values = virtualArgs.at(1);
    } else {
        for (int i = 1; i < argc; i += 2) { // offset by 1 cuz executable
            string key = argv[i];
            string value = argv[i+1];

            keys.push_back(key);
            values.push_back(value);
        }
    }

    unordered_map<string, unordered_map<string, bool>> colorToCoords = getColorClassToCoords(keys, values);
    vector<pair<string, string>> paths;
    
    int maxThreads = thread::hardware_concurrency();
    vector<vector<pair<string, string>>*> subPaths;
    vector<thread> threads;
    for (auto x : colorToCoords) { // can just be independent "thread" for each color
        string color = x.first;
        unordered_map<string, bool> coords = x.second;
   
        vector<pair<string, string>> *ps = new vector<pair<string, string>>();
        subPaths.push_back(ps);

        thread t(tGetPathsFromCoords, ref(color), make_unique<unordered_map<string, bool>>(coords), ps);
        threads.push_back(move(t));

        if (threads.size() > maxThreads-1) {
            while (threads.size() > 0) {
                //printf("%d\n", threads.size());
                threads.at(threads.size() - 1).join();
                threads.pop_back();
            }
        }
    }
    while (threads.size() > 0) {
        //printf("%d\n", threads.size());
        threads.at(threads.size() - 1).join();
        threads.pop_back();
    }
    while (subPaths.size() > 0) {
        vector<pair<string, string>>* ptr = subPaths.back(); 
        subPaths.pop_back();

        vector<pair<string, string>> ps = *ptr;

        paths.insert(paths.end(), ps.begin(), ps.end());

        delete ptr;
    }

    sort(paths.begin(), paths.end(), comparePathsFirstRawCoordinates);
    for (auto p : paths) {
        printf("%s\n", p.first.c_str()); 
    }
    for (auto p : paths) {
        printf("%s\n", p.second.c_str()); 
    }

    return 0;
}
