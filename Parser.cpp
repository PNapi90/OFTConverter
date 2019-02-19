#include "Parser.hpp"

//--------------------------------------------

Parser::Parser(std::vector<int> &FileRange,
               int _threadNumber)
    : Files_from(FileRange[0]),
      Files_to(FileRange[1]),
      threadNumber(_threadNumber)
{
    Folder = "Gamma_Single_Cs/OFT/";
    Photon = std::vector<std::vector<double>>(100,std::vector<double>(4,0));

    std::string OutputName = "Stored/OFT/Gamma_"+std::to_string(Files_from);
    OutputName += "_" + std::to_string(Files_to) + ".rawSpec";

    OUT.open(OutputName);

    if(OUT.fail())
        FileExc(OutputName);
}

//--------------------------------------------

Parser::~Parser()
{
    OUT.close();
}

//--------------------------------------------

void Parser::Process()
{
    std::string name,line;
    std::ifstream DATA;

    double E = 0 , x[3] = {0};
    int ID = 0,dummy = 0;
    
    int PhotonID = 0;

    PhotonLength = 0;

    for(int i = Files_from;i < Files_to;++i)
    {
        name = Folder + "GammaOFT." + GetEnding(i);

        DATA.open(name);

        if(DATA.fail())
            FileExc(name);

        PhotonID = 0;
        PhotonLength = 0;


        while(std::getline(DATA,line))
        {
            sscanf(line.c_str(), format, &ID, &E, &x[0], &x[1], &x[2], &dummy);

            if(ID == -1)
            {
                if(PhotonLength > 0)
                    Write(PhotonID);

                
                ++PhotonID;
                PhotonLength = 0;
            }
            else
            {
                Photon[PhotonLength][0] = E;
                for(int k = 0;k < 3;++k)
                    Photon[PhotonLength][k+1] = x[k];
                ++PhotonLength;
            }
            
            if(PhotonID % 50000 == 0 && threadNumber == 0)
            {
                std::cout << "\r";
                std::cout << "Thread 0 at photon "  << PhotonID << "\t\t\t";
                std::cout.flush();
            }
        }

        DATA.close();
        DATA.clear();

        if(threadNumber == 0)
        {
            std::cout << "\nFile # " << i << " from [" << Files_from;
            std::cout << "," << Files_to << ") processed" << std::endl;
        }
    }
}

//--------------------------------------------

void Parser::Write(int PhotonID)
{
    for(int i = 0;i < PhotonLength;++i)
    {
        for(auto Values : Photon[i])
            OUT << Values << " ";
        OUT << 661.7 << " " << PhotonLength << " " << PhotonID << std::endl;
    }

    for(int i = 0;i < 7;++i)
        OUT << -99999 << " ";
    OUT << std::endl;
}

//--------------------------------------------

std::thread Parser::threading()
{
    return std::thread([=]{Process();});
}

//--------------------------------------------

inline std::string Parser::GetEnding(int i)
{

    int power = 0;
    int leadingZeros = 0;
    bool calledExit = false;

    if(i > 0)
    {

        for(int k = 3;k > 0;--k)
        {
            power = pow(10,k);
            if(i / power > 0)
            {
                leadingZeros = 3 - k; 
                calledExit = true;
                break;
            }
        }
        
        if(!calledExit)
            FileEndingExc(i);
    }

    else
        leadingZeros = 3;

    std::string FileNameEnding = "";

    for(int k = 0;k < leadingZeros;++k)
        FileNameEnding += "0";

    return FileNameEnding + std::to_string(i);
}

//--------------------------------------------

inline void Parser::FileEndingExc(int i)
{
    std::cerr << "=======================================" << std::endl;
    std::cerr << "File Ending exception encountered!" << std::endl;
    std::cerr << "File Name value = " << i << std::endl;
    std::cerr << "=======================================" << std::endl;
    exit(1);
}

//--------------------------------------------

inline void Parser::FileExc(std::string name)
{
    std::cerr << "=======================================" << std::endl;
    std::cerr << "File Exception encountered!" << std::endl;
    std::cerr << "File " << name << " could not be opened!" << std::endl;
    std::cerr << "=======================================" << std::endl;
    exit(1);
}

//--------------------------------------------
