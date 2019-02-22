#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <cstdlib>
#include <memory>
#include <cmath>

//---------------------------------------------

struct Gamma
{
    double ETrack,ESum;
    std::vector<double> E;
    std::vector<std::vector<double>> X;
    int Length,ID;

    Gamma()
    {
        ETrack = 0;
        ESum = 0;
        E = std::vector<double>(100,0);
        X = std::vector<std::vector<double>>(100,
            std::vector<double>(3,0));
        Length = 0;
        ID = -1;
    };

};

//---------------------------------------------

class Parser
{
private:

    const char* format = "%lf %lf %lf %lf %lf %d";

    bool type;

    int FilesStart,FilesEnd,thrN,Parsed,hit;
    std::string Folder;

    std::ofstream OUT;

    std::vector<std::shared_ptr<Gamma>> DoubleGamma;

    void DoIt(int FileNumber);
    bool Process();

    inline std::string GetEnding(int FileNumber);
    inline bool IDCheck(int ID1,int ID2);

public:
    Parser(std::vector<int> &Ranges,
           int _thrN,
           bool _type);
    ~Parser();

    std::thread threading();
    void PrintParsed();
};

//---------------------------------------------

#endif

