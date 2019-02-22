#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>

#include "Parser.hpp"

//---------------------------------------------

int main(int argc , char** argv)
{

    std::string s;

    bool ForwardPass = false,threadF = false,OFlag = false;

    int nthr = 1;
    int NFiles = 1;
    int offset = 0;

    for(int i = 0;i < argc;++i)
    {
        s = std::string(argv[i]);
        if(s == "-F")
        {
            ForwardPass = true;
            continue;
        }

        if(s == "-t")
        {
            threadF = true;
            continue;
        }
        if(threadF)
        {
            nthr = std::stoi(s);
            threadF = false;
            continue;
        }
        if (s == "-o")
        {
            OFlag = true;
            continue;
        }
        if (OFlag)
        {
            offset = std::stoi(s);
            OFlag = false;
            continue;
        }
    }

    std::string value = ForwardPass ? "true" : "false";

    std::cout << "Forward Pass = " << value << std::endl;
    std::cout << "Using " << nthr << " threads with offset " << offset << std::endl;

    NFiles = nthr;

    std::vector<std::shared_ptr<Parser>> PARSERS;
    PARSERS.reserve(nthr);

    std::vector<int> Ranges(2,0);

    int delimiter = NFiles / nthr;

    for(int i = 0;i < nthr;++i)
    {
        Ranges[0] = Ranges[1];
        Ranges[1] = Ranges[0] + delimiter;

        PARSERS.push_back(std::make_shared<Parser>(Ranges,i,ForwardPass,offset));
    }


    std::thread t[nthr];
    for(int i = 0;i < nthr;++i)
        t[i] = PARSERS[i]->threading();
    
    for(int i = 0;i < nthr;++i)
        t[i].join();

    std::cout << "--------------------\n"; 

    for(int i = 0;i < nthr;++i)
    {
        PARSERS[i]->PrintParsed();
        PARSERS.pop_back();
    }

    std::cout << "--------------------\n" << std::endl;

    return 0;

}

//---------------------------------------------