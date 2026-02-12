// copyleft George R. Carder 2026, GNU Affero V3 

#include <stdio.h>
#include <assert.h>
#include <thread>
#include <mutex>

#include <bits/stdc++.h>

#include "./common.cpp"

using namespace std;

struct DataFeed {
    mutex mut;
    unordered_map<string, unordered_map<string, bool>> colorToCoords;
};

struct ReturnData {
    mutex mut;
    vector<pair<string, string>> paths;
};

void tGetPathsFromCoordsLoop(shared_ptr<DataFeed> sDataFeed, shared_ptr<ReturnData> sReturnData)
{
    while (1) 
    {
        sDataFeed->mut.lock(); 
        if (sDataFeed->colorToCoords.size() < 1) {
            sDataFeed->mut.unlock(); 
            break;
        }

        string color;
        unordered_map<string, bool> coords;
        for (auto x : sDataFeed->colorToCoords) 
        {
            color = x.first; 
            coords = x.second;
            break;
        }
        sDataFeed->colorToCoords.erase(color);
        sDataFeed->mut.unlock();

        vector<pair<string, string>> ps = getPathsFromCoords(color, coords);
        sReturnData->mut.lock();
        sReturnData->paths.insert(sReturnData->paths.end(), ps.begin(), ps.end());
        sReturnData->mut.unlock();
    }
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

    int maxThreads = thread::hardware_concurrency();
    if (maxThreads < 2) {
        printf("not enough concurrency\n");
        return 1;
    }
    --maxThreads;

    shared_ptr<DataFeed> sDataFeed = make_shared<DataFeed>();
    sDataFeed->colorToCoords = colorToCoords;

    shared_ptr<ReturnData> sReturnData = make_shared<ReturnData>();
    sReturnData->paths.reserve(values.size());

    vector<thread> threads;

    for (int i = 0; i < maxThreads; ++i) {
        thread t(tGetPathsFromCoordsLoop, sDataFeed, sReturnData); 
        threads.push_back(move(t));
    }
    while (threads.size() > 0) {
        threads.back().join();
        threads.pop_back();
    }

    vector<pair<string, string>> paths = sReturnData->paths;
    sort(paths.begin(), paths.end(), comparePathsFirstRawCoordinates);
    for (auto p : paths) {
        printf("%s\n", p.first.c_str()); 
    }
    for (auto p : paths) {
        printf("%s\n", p.second.c_str()); 
    }

    return 0;
}
