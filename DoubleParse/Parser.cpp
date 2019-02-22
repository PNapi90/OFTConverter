#include "Parser.hpp"

//---------------------------------------------

Parser::Parser(std::vector<int> &Ranges,
               int _thrN,
               bool _type,
               int _offset)
    : FilesStart(Ranges[0]+_offset),
      FilesEnd(Ranges[1]+_offset),
      thrN(_thrN),
      Parsed(0),
      type(_type),
      hit(0),
      offset(_offset)
{
    Folder = "Gamma_Double_Cs/";
    Folder += !type ? "OFT/" : "";

    DoubleGamma.reserve(2);
    for(int i = 0;i < 2;++i)
        DoubleGamma.push_back(std::make_shared<Gamma>());

    std::string outName = "Stored/";
    outName += !type ? "OFT/" : "Merged/";
    
    if(!type)
    {
        outName +=  "GammaDouble_" + std::to_string(FilesStart);
        outName += "_" + std::to_string(FilesEnd) + ".rawSpec";
    }
    else
        outName += "GammaEvents_D." + GetEnding(FilesStart);
    
    
    OUT.open(outName);
    if(OUT.fail())
    {
        std::cerr << "Could not open Output file " << outName << std::endl;
        exit(1);
    }
}

//---------------------------------------------

Parser::~Parser()
{
    for (int i = 0; i < 2; ++i)
        DoubleGamma.pop_back();

    OUT.close();
}

//---------------------------------------------

void Parser::DoIt(int FileNumber)
{
    std::string Name,line;

    Name = Folder + "GammaEvents." + GetEnding(FileNumber);

    std::ifstream DATA(Name);

    if(DATA.fail())
    {
        std::cerr << "Could not find " << Name << std::endl;
        exit(1);
    }

    bool dataComing = false;
    double x[6] =  {0};

    int ID = 0,gammaCount = 0,iter = 0;

    int lineIter = 0;
    bool skipGamma0 = true;

    while(std::getline(DATA,line))
    {
        
        

        if (line[0] == '$')
        {
            dataComing = true;
            
            if(type)
                OUT << line << std::endl;

            continue;
        }

        //write header of original file to output
        if (!dataComing)
        {
            if (type)
                OUT << line << std::endl;

            continue;
        }

        sscanf(line.c_str(), format, &x[0], &x[1], &x[2], &x[3], &x[4], &ID);

        if(x[0] < 0)
        {
            skipGamma0 = (ID == 0);
            
            if(skipGamma0)
                continue;
            
            //pair of photons loaded
            // -> check if true double gamma event
            if(gammaCount == 2)
            {
                int ID1 = DoubleGamma[0]->ID;
                int ID2 = DoubleGamma[1]->ID;

                bool processed = false;

                if(IDCheck(ID1,ID2))
                    processed = Process();

                gammaCount = 0;
                ++lineIter;

                if (thrN == 0 && lineIter % 100000 == 0)
                {
                    std::cout << "Thread 0 in File " << FileNumber;
                    std::cout <<  " @ photon " << lineIter << std::endl;
                }
            }

            DoubleGamma[gammaCount]->ETrack = x[1];
            DoubleGamma[gammaCount]->ESum = 0;
            DoubleGamma[gammaCount]->ID = ID;
            iter = 0;
            ++gammaCount;
        }
        else
        {
            if(skipGamma0)
                continue;

            DoubleGamma[gammaCount-1]->E[iter] = x[1];
            for(int i = 0;i < 3;++i)
                DoubleGamma[gammaCount-1]->X[iter][i] = x[2+i];
            DoubleGamma[gammaCount-1]->ESum += x[1];
            
            ++iter;
            DoubleGamma[gammaCount-1]->Length = iter;
        }
        
    }

}

//---------------------------------------------

bool Parser::Process()
{
    bool Tracked = true,TrackTmp = false;
    for(int i = 0;i < 2;++i)
    {
        TrackTmp = std::abs(DoubleGamma[i]->ETrack - DoubleGamma[i]->ESum) <= 2; 
        Tracked = Tracked && TrackTmp;
    }
    double E1 = DoubleGamma[0]->ESum;
    double E2 = DoubleGamma[1]->ESum;

    if(E1 > 0 && E2 > 0)
        ++hit;

    int ID1 = DoubleGamma[0]->ID;
    int ID2 = DoubleGamma[1]->ID;

    double ET1 = DoubleGamma[0]->ETrack;
    double ET2 = DoubleGamma[1]->ETrack;
    //sum of energies = 661.7 keV
    bool Summed = std::abs(DoubleGamma[0]->ESum + DoubleGamma[1]->ESum - 661.7) <= 2;


    //only proceed with both gammas correctly in summed dep. energy (or !tracked)
    bool abortProcess = !type ? (!Tracked || !Summed) : !Summed;
    
    if(abortProcess)
        return false;

    //Save DoubleGamma in compY's GammaTracker output format
    if(!type)
    {
        for(int i = 0; i < 2;++i)
        {
            OUT << -1 << " " << DoubleGamma[i]->ETrack <<" ";
            for(int j = 0;j < 4;++j)
                OUT << 0 << " ";
            OUT << std::endl;

            for(int j = 0;j < DoubleGamma[i]->Length;++j)
            {
                OUT << 13 << " " << DoubleGamma[i]->E[j] << " ";
                for(auto X : DoubleGamma[i]->X[j])
                    OUT << X << " ";
                OUT << " " << DoubleGamma[i]->ESum << " ";
                OUT << DoubleGamma[i]->Length << std::endl;
            }
        }
    }
    else
    {
        OUT << "   " << -1 << "   " << 661.7 << " ";
        for(int i = 0;i < 3;++i)
            OUT << 0 << " ";
        OUT << Parsed << std::endl;
        for(int i = 0;i < 2;++i)
        {
            for(int j = 0;j < DoubleGamma[i]->Length;++j)
            {
                OUT << "   " << 13 << "   ";
                OUT << DoubleGamma[i]->E[j] << " ";
                for(int k = 0;k < 3;++k)
                    OUT << DoubleGamma[i]->X[j][k] << " ";
                OUT << "01" << std::endl;
            }
        }
    }
    

    ++Parsed;
    return true;
}

//---------------------------------------------

std::thread Parser::threading()
{
    return std::thread(
        [=]
        {
            for(int i = FilesStart;i < FilesEnd;++i)
                DoIt(i);
        }
    );
}

//---------------------------------------------

inline std::string Parser::GetEnding(int FileNumber)
{
    
    std::string leadingZeros = "";

    if(FileNumber > 999)
        leadingZeros = "";
    else if(FileNumber > 99)
        leadingZeros = "0";
    else if(FileNumber > 9)
        leadingZeros = "00";
    else
        leadingZeros = "000";

    return leadingZeros + std::to_string(FileNumber);
    
}

//---------------------------------------------

inline bool Parser::IDCheck(int ID1,int ID2)
{

    //First gamma has odd ID,second has even ID
    bool odd = (ID1 % 2);
    bool even = !(ID2 % 2);

    //Difference between IDs has to be 1
    int Diff = ID2 - ID1;

    return (Diff == 1) && even && odd;
} 

//---------------------------------------------

void Parser::PrintParsed()
{
    std::cout << "Thread # " << thrN << " parsed " << Parsed << " photon pairs" << std::endl;
    std::cout << "2 gamma hits -> " << hit << std::endl;
}

//---------------------------------------------