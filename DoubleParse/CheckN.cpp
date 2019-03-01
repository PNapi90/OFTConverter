#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int main()
{

    std::string FileName = "Stored/Merged/GammaEvents_D.0000";

    std::ifstream DATA(FileName);

    if(DATA.fail())
    {
        std::cerr << "Could not find " << FileName << std::endl;
        return 1;
    }

    std::string line;

    bool dataComing = false;

    std::vector<double> x(6,0);
    int N = 0;

    std::ofstream OUT("N.dat");

    while(std::getline(DATA,line))
    {
        if(line[0] == '$')
        { 
            dataComing = true;
            continue;
        }
        
        if(!dataComing)
            continue;

        sscanf(line.c_str(), "%lf %lf %lf %lf %lf %lf", &x[0], &x[1], &x[2], &x[3], &x[4], &x[5]);

        if(x[0] == -1)
        {
            if(N > 0)
                OUT << N << std::endl;
            
            N = 0;
        }
        else
        {
            ++N;
        }
    }

    OUT.close();

    return 0;
}