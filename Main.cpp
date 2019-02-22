#include <iostream>
#include <string>
#include <memory>
#include <thread>

#include "Parser.hpp"

//--------------------------------------------

struct FlagsAndValues
{
    int nthr = 1;
    int nFiles = 1;
    bool type = false;
};

//--------------------------------------------

void GetFlags(int argc,
              char** argv,
              FlagsAndValues &F);

//--------------------------------------------

int main(int argc,
         char** argv)
{
    FlagsAndValues F;

    GetFlags(argc,argv,F);

    int nthr = F.nthr;
    int nFiles = F.nFiles;

    std::vector<int> FileRange(2,0);

    std::vector<std::shared_ptr<Parser>> Parsers;

    Parsers.reserve(nthr);

    int FilesPerThread = nFiles/nthr;

    double CommaVal = ((double) nFiles)/((double) nthr);

    if(nFiles % nthr)
    {
        std::cerr << "Uneven amount of files per thread : " << CommaVal;
        std::cerr << " -> nthr = " << nthr << " files_in_total = " << nFiles << std::endl;
        std::cerr << "Please change either of them" << std::endl;
        return 1;
    }

    for(int i = 0;i < nthr;++i)
    {
        FileRange[0] = FileRange[1];
        FileRange[1] = FileRange[0] + FilesPerThread;

        Parsers.push_back(std::make_shared<Parser>(FileRange,i,F.type));
    }

    std::thread t[nthr];
    for(int i = 0;i < nthr;++i)
        t[i] = Parsers[i]->threading();
    for (int i = 0; i < nthr; ++i)
        t[i].join();

    std::cout << "\n---------------------------------\n" << std::endl;

    return 0;
}

//--------------------------------------------

void GetFlags(int argc,
              char** argv,
              FlagsAndValues &F)
{
    std::string s;

    bool threadFlag = false,FileFlag = false;

    for(int i = 0;i < argc;++i)
    {
        s = std::string(argv[i]);
        
        if(s == "-t")
        {
            threadFlag = true;
            continue;
        }
        if(threadFlag)
        {
            F.nthr = std::stoi(s);
            threadFlag = false;
            continue;
        }

        if (s == "-n")
        {
            FileFlag = true;
            continue;
        }
        if (FileFlag)
        {
            F.nFiles = std::stoi(s);
            FileFlag = false;
            continue;
        }
        if (s == "-d")
        {
            F.type = true;
            continue;
        }
    }
}

//--------------------------------------------