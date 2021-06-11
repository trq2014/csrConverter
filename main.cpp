#include "./globals.hpp"
#include <cstdlib>
#include <ctime>

bool IsWeightedFormat(string format)
{
    if((format == "bwcsr")	||
       (format == "wcsr")	||
       (format == "wel"))
        return true;
    return false;
}

string GetFileExtension(string fileName)
{
    if(fileName.find_last_of(".") != string::npos)
        return fileName.substr(fileName.find_last_of(".")+1);
    return "";
}


void convertTxtToByte(string input) {
    clock_t startTime, endTime;
    startTime = clock();
    ifstream infile;
    infile.open(input);
    std::ofstream outfile(input.substr(0, input.length()-3)+"toB", std::ofstream::binary);
    stringstream ss;
    uint max = 0;
    string line;
    ull edgeCounter = 0;
    vector<Edge> edges;
    Edge newEdge;
    ull testIndex = 0;
    while(getline( infile, line ))
    {
        testIndex++;
        ss.str("");
        ss.clear();
        ss << line;
        if (ss.str().empty() || ss.str()[0] == '#') continue;
        ss >> newEdge.source;
        ss >> newEdge.end;
        //cout << newEdge.source << "  " << newEdge.end << endl;
        edges.push_back(newEdge);
        edgeCounter++;
        if(max < newEdge.source)
            max = newEdge.source;
        if(max < newEdge.end)
            max = newEdge.end;
        if (testIndex % 1000000000 == 1)
        {
            int billionLines = testIndex / 1000000000;
            cout << billionLines << " billion lines " << endl;
            if (billionLines % 5 == 1)
            {
                outfile.write((char*)edges.data(), sizeof(Edge) * edges.size());
                edges.clear();
                cout << "clear edges = " << edges.size() << endl;
            }
        }

    }
    if (edges.size() > 0)
    {
        outfile.write((char*)edges.data(), sizeof(Edge) * edges.size());
        edges.clear();
    }
    cout << "write " << testIndex << " lines" << endl;
    cout << "max node " << max << endl;
    outfile.close();
    infile.close();
    endTime = clock();
    cout << setprecision(9)  << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s | " << "S0:Prepare [edges] OK" << endl;
}
uint friendsterEdgeSize = 34681189;
uint friendsterVertexSize = 4036530;
void convertByteToBCSR(string input) {
    cout << "convertByteToBCSR" << endl;
    clock_t startTime, endTime;
    startTime = clock();
    uint totalSize = friendsterEdgeSize;
    uint vertexSize = friendsterVertexSize;

    uint* nodePointers = new uint[vertexSize];
    uint* nodePointersAnkor = new uint [vertexSize];
    uint* degree = new uint[vertexSize];
    OutEdge* edges = new OutEdge[totalSize];
    Edge *byteEdges = new Edge[totalSize];
    ull counter = 0;
    for(uint i = 0; i < vertexSize; i++)
        degree[i] = 0;

    cout << "calculate degree " << endl;
    ifstream infile(input, ios::in | ios::binary);
    infile.seekg(0, std::ios::end);
    ull size = infile.tellg();
    totalSize = size / sizeof(Edge);
    cout << "total edge Size " << totalSize << endl;
    infile.clear();
    infile.seekg(0);
    uint maxNode = 0;
    while(counter < totalSize) {
        //cout << counter << " to " << counter + partSize << endl;
        //counter += partSize;
        infile.read((char *) byteEdges, sizeof(Edge) * totalSize);
        for (uint i = 0; i < totalSize; ++i)
        {
            degree[byteEdges[i].source]++;
            if (maxNode < byteEdges[i].source)
            {
                maxNode = byteEdges[i].source;
            }
        }
        counter += totalSize;
    }
    cout << "max node is " << maxNode << endl;
    vertexSize = maxNode + 1;
    ull tempPointer = 0;
    for(uint i=0; i<vertexSize; i++)
    {
        nodePointers[i] = tempPointer;
        tempPointer += degree[i];
    }

    infile.clear();
    infile.seekg(0);

    cout << "calculate edges " << endl;
    for (uint i = 0; i < vertexSize; ++i)
    {
        nodePointersAnkor[i] = 0;
    }
    counter = 0;
    while(counter < totalSize) {
        cout << counter << " to " << counter + totalSize << endl;
        counter += totalSize;
        infile.read((char *) byteEdges, sizeof(Edge) * totalSize);
        for (uint i = 0; i < totalSize; ++i)
        {
            uint location = nodePointers[byteEdges[i].source] + nodePointersAnkor[byteEdges[i].source];
            edges[location].end = byteEdges[i].end;
            nodePointersAnkor[byteEdges[i].source]++;
        }
    }
    //delete [] nodePointersAnkor;

    std::ofstream outfile(input.substr(0, input.length()-3)+"bcsr", std::ofstream::binary);
    outfile.write((char*)&vertexSize, sizeof(unsigned int));
    outfile.write((char*)&totalSize, sizeof(uint));
    outfile.write ((char*)degree, sizeof(uint)*vertexSize);
    outfile.write ((char*)nodePointers, sizeof(uint)*vertexSize);
    outfile.write ((char*)edges, sizeof(OutEdge)*totalSize);
    outfile.close();
    cout << "ull size is " << sizeof(ull) << endl;
    endTime = clock();
    cout << setprecision(9)  << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s | " << "S0:Prepare [edges] OK" << endl;
}

void convertByteToBCSC(string input) {
    cout << "convertByteToBCSC" << endl;
    clock_t startTime, endTime;
    startTime = clock();
    ull totalSize = 10000000000;
    uint partSize = 1000000000;
    uint vertexSize = 100000000;
    ull* nodePointers = new ull[vertexSize];
    ull* nodePointersAnkor = new ull[vertexSize];
    uint* degree = new uint[vertexSize];
    uint *inDegree = new uint[vertexSize];
    OutEdge* edges = new OutEdge[totalSize];
    Edge *byteEdges = new Edge[partSize];
    ull counter = 0;
    for(uint i = 0; i < vertexSize; i++) {
        degree[i] = 0;
        inDegree[i] = 0;
    }

    cout << "calculate degree " << endl;
    ifstream infile(input, ios::in | ios::binary);
    infile.seekg(0, std::ios::end);
    ull size = infile.tellg();
    totalSize = size / sizeof(Edge);
    cout << "total edge Size " << totalSize << endl;
    infile.clear();
    infile.seekg(0);
    uint maxNode = 0;
    while(counter < totalSize) {
        cout << counter << " to " << counter + partSize << endl;
        counter += partSize;
        infile.read((char *) byteEdges, sizeof(Edge) * partSize);
        for (uint i = 0; i < partSize; ++i)
        {
            degree[byteEdges[i].source]++;
            inDegree[byteEdges[i].end]++;
            if (maxNode < byteEdges[i].source)
            {
                maxNode = byteEdges[i].source;
            }
        }
    }
    cout << "max node is " << maxNode << endl;
    vertexSize = maxNode + 1;
    ull tempPointer = 0;
    for(uint i=0; i<vertexSize; i++)
    {
        nodePointers[i] = tempPointer;
        tempPointer += inDegree[i];
    }
    cout << "finish calculate nodePointers " << endl;
    // for (int i = 0; i < 100; ++i)
    // {
    // 	cout << i << " : " << nodePointers[i] << endl;
    // }

    // for (int i = 90000000; i < 90000000 + 100; ++i)
    // {
    // 	cout << i << " : " << nodePointers[i] << endl;
    // }

    infile.clear();
    infile.seekg(0);

    cout << "calculate edges " << endl;
    for (uint i = 0; i < vertexSize; ++i)
    {
        nodePointersAnkor[i] = 0;
    }
    counter = 0;
    while(counter < totalSize) {
        cout << counter << " to " << counter + partSize << endl;
        counter += partSize;
        infile.read((char *) byteEdges, sizeof(Edge) * partSize);
        for (uint i = 0; i < partSize; ++i)
        {
            ull location = nodePointers[byteEdges[i].source] + nodePointersAnkor[byteEdges[i].source];
            edges[location].end = byteEdges[i].end;
            nodePointersAnkor[byteEdges[i].source]++;
        }
    }
    //delete [] nodePointersAnkor;

    std::ofstream outfile(input.substr(0, input.length()-3)+"bcsc", std::ofstream::binary);
    outfile.write((char*)&vertexSize, sizeof(unsigned int));
    outfile.write((char*)&totalSize, sizeof(ull));
    outfile.write ((char*)degree, sizeof(uint)*vertexSize);
    outfile.write ((char*)nodePointers, sizeof(ull)*vertexSize);
    outfile.write ((char*)edges, sizeof(OutEdge)*totalSize);
    outfile.close();
    endTime = clock();
    cout << setprecision(9)  << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s | " << "S0:Prepare [edges] OK" << endl;
}

void convertByteToBWCSR(string input) {
    cout << "convertByteToBWCSR" << endl;
    srand(0);
    clock_t startTime, endTime;
    startTime = clock();
    ull totalSize = 6000000000;
    uint partSize = 1000000000;
    uint vertexSize = 60000000;
    ull* nodePointers = new ull[vertexSize];
    ull* nodePointersAnkor = new ull[vertexSize];
    ull* degree = new ull[vertexSize];
    OutEdgeWeighted* edges = new OutEdgeWeighted[totalSize];
    Edge *byteEdges = new Edge[partSize];
    ull counter = 0;
    for(uint i = 0; i < vertexSize; i++)
        degree[i] = 0;

    cout << "calculate degree " << endl;
    ifstream infile(input, ios::in | ios::binary);
    infile.seekg(0, std::ios::end);
    ull size = infile.tellg();
    totalSize = size / sizeof(Edge);
    cout << "total edge Size " << totalSize << endl;
    infile.clear();
    infile.seekg(0);
    uint maxNode = 0;

    while(counter < totalSize) {
        cout << counter << " to " << counter + partSize << endl;
        counter += partSize;
        infile.read((char *) byteEdges, sizeof(Edge) * partSize);
        for (uint i = 0; i < partSize; ++i)
        {
            degree[byteEdges[i].source]++;
            if (maxNode < byteEdges[i].source)
            {
                maxNode = byteEdges[i].source;
            }
        }
    }
    cout << "max node is " << maxNode << endl;
    vertexSize = maxNode + 1;

    ull tempPointer = 0;
    for(uint i=0; i<vertexSize; i++)
    {
        nodePointers[i] = tempPointer;
        tempPointer += degree[i];
    }

    infile.clear();
    infile.seekg(0);

    cout << "calculate edges " << endl;
    for (uint i = 0; i < vertexSize; ++i)
    {
        nodePointersAnkor[i] = 0;
    }
    counter = 0;
    while(counter < totalSize) {
        cout << counter << " to " << counter + partSize << endl;
        counter += partSize;
        infile.read((char *) byteEdges, sizeof(Edge) * partSize);
        for (uint i = 0; i < partSize; ++i)
        {
            ull location = nodePointers[byteEdges[i].source] + nodePointersAnkor[byteEdges[i].source];
            edges[location].end = byteEdges[i].end;
            nodePointersAnkor[byteEdges[i].source]++;
            edges[location].w8 = rand() % 20;
        }
    }
    //delete [] nodePointersAnkor;
    cout << "degree[0] " << degree[0] << endl;
    cout << "degree[1] " << degree[1] << endl;
    //cout << "degree[50000000] " << degree[50000000] << endl;

    std::ofstream outfile(input.substr(0, input.length()-3)+"bwcsr", std::ofstream::binary);
    outfile.write((char*)&vertexSize, sizeof(unsigned int));
    outfile.write((char*)&totalSize, sizeof(ull));
    outfile.write ((char*)nodePointers, sizeof(ull)*vertexSize);
    outfile.write ((char*)edges, sizeof(OutEdgeWeighted)*totalSize);
    outfile.close();
    endTime = clock();
    cout << setprecision(9)  << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s | " << "S0:Prepare [edges] OK" << endl;
}

void txtToByte() {
    string input;
    input = "./liveJournal.txt";
    convertTxtToByte(input);
}

void byteToCsr() {
    string input;
    input = "./liveJournal.toB";
    convertByteToBCSR(input);
}


int main(int argc, char** argv)
{

    byteToCsr();

    //convertByteToBCSC(input);
    return 0;
}
