#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <thread>
#include <iostream>
#include <fstream>
#include <cmath>

class Parser
{
private:

    const char* format = "%d %lf %lf %lf %lf %d";

    int Files_from,Files_to,threadNumber;
    bool type;

    double Egamma;

    int PhotonLength;

    std::string Folder;

    std::ofstream OUT;

    std::vector<std::vector<double>> Photon;

    void Process();
    void Write(int PhotonID);

    inline void FileEndingExc(int i);
    inline void FileExc(std::string name);

    inline std::string GetEnding(int i);

public:
    Parser(std::vector<int> &FileRange,
           int _threadNumber,
           bool _type);
    ~Parser();

    std::thread threading();
};



#endif