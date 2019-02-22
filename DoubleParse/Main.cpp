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

    bool ForwardPass = false;

    for(int i = 0;i < 2;++i)
    {
        s = std::string(argv[i]);
        if(s == "-F")
        {
            ForwardPass = true;
            continue;
        }
    }

    std::string value = ForwardPass ? "true" : "false";

    std::cout << "Forward Pass = " << value << std::endl;

    int nthr = 1;
    int NFiles = 1;

    std::vector<std::shared_ptr<Parser>> PARSERS;
    PARSERS.reserve(nthr);

    std::vector<int> Ranges(2,0);

    int delimiter = NFiles / nthr;

    for(int i = 0;i < nthr;++i)
    {
        Ranges[0] = Ranges[1];
        Ranges[1] = Ranges[0] + delimiter;

        PARSERS.push_back(std::make_shared<Parser>(Ranges,i,ForwardPass));
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