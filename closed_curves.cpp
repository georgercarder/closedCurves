#include <stdio.h>
#include <assert.h>

#include <bits/stdc++.h>

#include "./debug_data.h"
//#include "./parallel_hashmap/phmap.h"

//using phmap::flat_hash_map;
// on the server std unordered_map is actually faster!! 4s vs 10s on benchmark!

using namespace std;

int min(const int a, const int b) 
{
    if (a < b) return a;
    return b;
}

string myItoa(const int i, const int bufferLen)
{
    char buffer[min(100, bufferLen)];
    snprintf(buffer, sizeof(buffer), "%d", i);
    return buffer;
}

int compareRawCoordinates(const pair<int, int> &a, const pair<int, int> &b)
{
    if (a.second < b.second) 
    {
        return -1; 
    } 
    else if (a.second > b.second)
    {
        return 1;
    }
    else if (a.first < b.first)
    {
        return -1;
    }
    return 1;
}

typedef pair<pair<int, int>, pair<int, int>> RawCoordinates;

struct Coordinates {
    RawCoordinates coordinates;
    string sCoordinates;
};

typedef Coordinates Edge;

typedef vector<Edge> Loop;

bool compareFirstRawCoordinates(const Loop &loopA, const Loop &loopB)
{
    return compareRawCoordinates(loopA.at(0).coordinates.first, loopB.at(0).coordinates.first) < 0;
}

bool comparePathsFirstRawCoordinates(const pair<string, string> &pathA, const pair<string, string> &pathB)
{
    string ssPA = pathA.second;
    string ssPB = pathB.second;

    //ssPA = ssPA.substr(0, ssPA.find("v")); // since path is formatted to have "h" first!
    ssPA = ssPA.substr(0, ssPA.find("h"));

    string paX = ssPA.substr(ssPA.find("M")+1, ssPA.find(" "));
    string paY = ssPA.substr(ssPA.find(" ")+1, ssPA.size());

    //ssPB = ssPB.substr(0, ssPB.find("v")); // since path is formatted to have "h" first!
    ssPB = ssPB.substr(0, ssPB.find("h"));

    string pbX = ssPB.substr(ssPB.find("M")+1, ssPB.find(" "));
    string pbY = ssPB.substr(ssPB.find(" ")+1, ssPB.size());

    pair<int, int> pA = {atoi(paX.c_str()), atoi(paY.c_str())};
    pair<int, int> pB = {atoi(pbX.c_str()), atoi(pbY.c_str())};

    return (compareRawCoordinates(pA, pB) < 0);
}

struct DiffData {
    string direction;
    int magnitude;
};

DiffData getDiffData(const pair<int, int> &p0, const pair<int, int> &p1)
{
    DiffData ret;
    ret.direction = (p0.first == p1.first) ? "v" : "h";
    ret.magnitude = (p0.first == p1.first) ? (p1.second - p0.second) : (p1.first - p0.first);
    return ret;
}

enum Direction {UP, DOWN, LEFT, RIGHT};

Direction getDirection(const Edge edge)
{
    RawCoordinates rc = edge.coordinates;
    if (rc.first.first == rc.second.first) // up/down
    {
        if (rc.first.second < rc.second.second) { // up 

            //printf("up\n");
            return UP; 
        } 
        //printf("down\n");
        return DOWN;
    }
    // left right
    if (rc.first.first < rc.second.first)
    {
        //printf("left\n");
        return LEFT; 
    }
    //printf("right\n");
    return RIGHT;
}

string coordinatesToString(const RawCoordinates &c)
{
    int bufferSize = 2048;
    return "[[" + myItoa(c.first.first, bufferSize) + "," + myItoa(c.first.second, bufferSize) + "],[" + myItoa(c.second.first, bufferSize) + "," + myItoa(c.second.second, bufferSize) + "]]";
}

Edge reverseEdge(const Edge edge)
{
    RawCoordinates rc = {edge.coordinates.second, edge.coordinates.first};
    return Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});
}

void adjustForOrientation(Loop &loop)
{
    Edge edge = loop.at(0);
    pair<int, int> lastPoint = edge.coordinates.first;
    pair<int, int> currentPoint = edge.coordinates.second;
    DiffData data = getDiffData(lastPoint, currentPoint); 
    string d = data.direction;
    if (d == "v") {
        Loop newLoop;
        int s = loop.size();
        for (int i = 0; i < s; ++i) 
        {
            newLoop.push_back(reverseEdge(loop.at(s-i-1)));
        }
        loop = newLoop;
    }
}

string buildPathString(Loop &loop)
{
    int bufferSize = 1024;
    string ret;
    if (!(loop.size() > 3)) return ret;
    //assert(loop.size() > 3);
    int idx = 0;
    Edge edge = loop.at(idx++);
    pair<int, int> lastPoint = edge.coordinates.first;
    ret = "M" + myItoa(lastPoint.first, bufferSize) + " " + myItoa(lastPoint.second, bufferSize);
    pair<int, int> currentPoint = edge.coordinates.second;
    DiffData data = getDiffData(lastPoint, currentPoint); 
    int magnitude = data.magnitude;
    string d = data.direction;
    for (int i = 1; i < loop.size(); ++i) // note the unconventional bounds
    {
        lastPoint = currentPoint;
        edge = loop.at(i);
        currentPoint = edge.coordinates.second;
        data = getDiffData(lastPoint, currentPoint); 
        if (data.direction != d) 
        {
            ret += d + myItoa(magnitude, 2048);  
            d = data.direction;
            magnitude = data.magnitude;
        } else {
            magnitude += data.magnitude;
        }
    }
    ret += d + myItoa(magnitude, 2048);
    return ret;
}

int compareEdges(const Edge a, const Edge b)
{
    RawCoordinates ra = a.coordinates;
    RawCoordinates rb = b.coordinates;
    return compareRawCoordinates(ra.first, rb.first);
}

int getFirstEdgeIdx(const Loop &loop)
{
    assert(loop.size() > 0);
    int ret = 0;
    Edge bestEdge = loop.at(0);
    Edge other;
    for (int i = 1; i < loop.size(); ++i)
    {
        other = loop.at(i); 
        if (compareEdges(other, bestEdge) < 0) {
            ret = i;
            bestEdge = other;
        }
    }
    return ret;
}

vector<Edge> getOrientedBoundaryFromCoord(string coord)
{
    vector<Edge> ret;   
    vector<int> cellIdxArr;
    string token = "1";
    while (token.length() > 0)
    {
        int found = coord.find("_");
        token = coord.substr(0, found); 
        if (token.length() < 1) break;
        cellIdxArr.push_back(atoi(token.c_str()));
        if (found == -1) break;
        coord.erase(0, coord.find("_") + 1);
    }

    if (cellIdxArr.size() < 2) {
      printf("should not happen. cellIdxArr length wrong %d\n", cellIdxArr.size());
      return ret;
    }

    int y = cellIdxArr.at(0);
    int x = cellIdxArr.at(1);

    // note the unintuitive order!! it's correct, don't worry
    RawCoordinates rc = {{x, y}, {x+1, y}};
    Coordinates top = Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});

    rc = {{x+1, y}, {x+1, y+1}};
    Coordinates right = Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});

    rc = {{x+1, y+1}, {x, y+1}};
    Coordinates bottom = Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});

    rc = {{x, y+1}, {x, y}};
    Coordinates left = Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});

    ret.push_back(top);
    ret.push_back(right);
    ret.push_back(bottom);
    ret.push_back(left);

    return ret;
}

Edge getForwardEdge(const Edge edge)
{
    RawCoordinates rc;
    Direction direction = getDirection(edge);
    RawCoordinates ec = edge.coordinates;
    if (direction == UP)
    {
        rc = {{ec.first.first, ec.second.second}, {ec.first.first, ec.second.second+1}}; 
    }
    else if (direction == DOWN)
    {
        rc = {{ec.first.first, ec.second.second}, {ec.first.first, ec.second.second-1}}; 
    }
    else if (direction == LEFT)
    {
        rc = {{ec.second.first, ec.first.second}, {ec.second.first+1, ec.first.second}};
    }
    else // RIGHT
    {
        rc = {{ec.second.first, ec.first.second}, {ec.second.first-1, ec.first.second}};
    }
    return Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});
}

Edge getRightEdge(const Edge edge)
{
    RawCoordinates rc;
    Direction direction = getDirection(edge);
    RawCoordinates ec = edge.coordinates;
    if (direction == UP)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first-1, ec.second.second}}; 
    }
    else if (direction == DOWN)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first+1, ec.second.second}}; 
    }
    else if (direction == LEFT)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first, ec.second.second+1}};
    }
    else // RIGHT
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first, ec.second.second-1}}; 
    }
    return Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});
}

Edge getLeftEdge(const Edge edge)
{
    RawCoordinates rc;
    Direction direction = getDirection(edge);
    RawCoordinates ec = edge.coordinates;
    if (direction == UP)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first+1, ec.second.second}}; 
    }
    else if (direction == DOWN)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first-1, ec.second.second}}; 
    }
    else if (direction == LEFT)
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first, ec.second.second-1}};
    }
    else // RIGHT
    {
        rc = {{ec.second.first, ec.second.second}, {ec.second.first, ec.second.second+1}}; 
    }
    return Coordinates({coordinates: rc, sCoordinates: coordinatesToString(rc)});
}

unordered_map<string, Edge> getOrientedEdges(unordered_map<string, bool> coords)
{
    unordered_map<string, Edge> ret;
    for (auto x : coords) 
    {
        string coord = x.first;

        vector<Edge> boundary = getOrientedBoundaryFromCoord(coord);
        for (int i = 0; i < boundary.size(); ++i) {
            Edge edge = boundary.at(i); 
            Edge reversed = reverseEdge(edge);

            if (ret.find(reversed.sCoordinates) != ret.end()) {
                ret.erase(reversed.sCoordinates);   
            } else {
                ret[edge.sCoordinates] = edge;
            }
        }
    }
    return ret;
}

unordered_map<string, unordered_map<string, Edge>> getOrientedEdges(unordered_map<string, unordered_map<string, bool>> colorToCoords)
{
    unordered_map<string, unordered_map<string, Edge>> ret;
    
    for (auto x : colorToCoords) 
    {
        string color = x.first; 
        unordered_map<string, bool> coords = x.second;

        unordered_map<string, Edge> oee = getOrientedEdges(coords);

        ret[color] = oee;
    }
    return ret;
}

string pickOne(unordered_map<string, Edge> m) 
{
    string ret;
    for (auto x : m)
    {
        ret = x.first; 
        break;
    }
    return ret;
}

vector<Loop> getLoops(unordered_map<string, Edge> orientedEdgeExistence)
{
    vector<Loop> ret;
    unordered_map<string, Edge> oee = orientedEdgeExistence;

    if (oee.size() < 1) return ret;

    vector<string> keys;

    string sEdges = pickOne(oee);
    if (sEdges.length() < 1) return ret;

    Edge edge = oee.at(sEdges); 

    oee.erase(sEdges);

    Loop loop;
    loop.push_back(edge);

    // since every iteration one entry is removed from map, this while is tractable, and will not "run away" in an infinite loop
    while (oee.size() > 0) {
        Edge forward = getForwardEdge(edge);
        if (oee.find(forward.sCoordinates) != oee.end())
        {
            edge = forward;
            oee.erase(forward.sCoordinates);

            loop.push_back(edge);
            if (oee.size() < 1) ret.push_back(loop);
            continue;
        }
        Edge right = getRightEdge(edge);
        if (oee.find(right.sCoordinates) != oee.end()) {
            edge = right;
            oee.erase(right.sCoordinates);

            loop.push_back(edge);
            if (oee.size() < 1) ret.push_back(loop);
            continue;
        }
        Edge left = getLeftEdge(edge);
        if (oee.find(left.sCoordinates) != oee.end()) {
            edge = left;
            oee.erase(left.sCoordinates);

            loop.push_back(edge);
            if (oee.size() < 1) ret.push_back(loop);
            continue;
        }
        if (loop.size() > 0) ret.push_back(loop);
        Loop fresh;
        loop = fresh;

        sEdges = pickOne(oee);
        edge = oee.at(sEdges);
        oee.erase(sEdges);
    }
    return ret;
}

unordered_map<string, vector<Loop>> getLoops(unordered_map<string, unordered_map<string, Edge>> colorToOrientedEdgeExistence)
{
    unordered_map<string, vector<Loop>> ret;   
    for (auto x : colorToOrientedEdgeExistence) 
    {
        string color = x.first;
        unordered_map<string, Edge> orientedEdgeExistence = x.second;

        ret[color] = getLoops(orientedEdgeExistence);
    }
    return ret;
}

Loop canonicalLoop(Loop loop)
{
    Loop ret;
    int firstEdgeIdx = getFirstEdgeIdx(loop);
    for (int i = 0; i < loop.size(); ++i)
    {
        ret.push_back(loop.at((firstEdgeIdx + i) % loop.size())); 
    }
    return ret;
}

vector<pair<string, string>> getPaths(unordered_map<string, vector<Loop>> colorToLoops)
{
    vector<pair<string, string>> ret;
    for (auto x : colorToLoops)
    {
        string color = x.first;
        string pathScratch = "";
        vector<Loop> loops = x.second;
        for (int i = 0; i < loops.size(); ++i)
        {
            loops.at(i) = canonicalLoop(loops.at(i));
        }
        sort(loops.begin(), loops.end(), compareFirstRawCoordinates);
        for (int loopIdx = 0; loopIdx < loops.size(); ++loopIdx) 
        {
            Loop loop = loops.at(loopIdx);
            adjustForOrientation(loop);
            string sPath = buildPathString(loop); 
            pathScratch += sPath;
            if (loopIdx == loops.size()-1)
            {
                pair<string, string> p;
                p.first = color;
                p.second = pathScratch;
                ret.push_back(p);

                pathScratch = "";
            }
        }
    }
    return ret;
}

unordered_map<string, unordered_map<string, bool>> getColorClassToCoords(vector<string> keys, vector<string> values)
{
    assert(keys.size() == values.size());

    unordered_map<string, unordered_map<string, bool>> ret; 
    for (int i = 0; i < keys.size(); ++i) {
        string k = keys.at(i); 
        string v = values.at(i);

        ret[k][v] = true; 
    }
    return ret; 
}

vector<vector<string>> getVirtualArgs()
{
    vector<vector<string>> ret;   
    vector<string> keys;
    vector<string> values;

    vector<string> virtualArgs;
    string dataCp = debug_data;
    string token = "1";
    while (token.length() > 0)
    {
        int found = dataCp.find(" ");
        token = dataCp.substr(0, found); 
        virtualArgs.push_back(token);
        if (found == -1) break;
        dataCp.erase(0, found + 1);
    }
    for (int i = 0; i < virtualArgs.size(); i += 2) {
        string key = virtualArgs.at(i); // color
        string value = virtualArgs.at(i+1); // cellId

        keys.push_back(key);
        values.push_back(value);
    }
    ret.push_back(keys);
    ret.push_back(values);
    return ret;
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
    unordered_map<string, unordered_map<string, Edge>> colorToOrientedEdgeExistence = getOrientedEdges(colorToCoords);
    unordered_map<string, vector<Loop>> colorToLoops = getLoops(colorToOrientedEdgeExistence);
    vector<pair<string, string>> paths = getPaths(colorToLoops);
    sort(paths.begin(), paths.end(), comparePathsFirstRawCoordinates);
    for (auto p : paths) {
        printf("%s\n", p.first.c_str()); 
    }
    for (auto p : paths) {
        printf("%s\n", p.second.c_str()); 
    }

    return 0;
}
