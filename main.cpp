#include <ModeInfo.h>
#include <vector>
#include <string>
#include <exception>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <reg_12.h>

bool checkArg(const std::string& sArgName, const std::string& sFileName)
{
    bool bRes = false;
    if (sArgName == sFileName)
        bRes = true;
    return bRes;
}

bool conversionPath(std::string& sPath)
{
    plug_key::CModeInfoPlug lib;
    bool bRes = lib.Load();
    std::cout << "RES>" << bRes << std::endl << std::endl;
    lib.ExpandString(sPath);
    std::cout << sPath << std::endl;

    std::cout << std::endl << std::endl;
    lib.Free();
    return bRes;
}

bool getNodeName(const std::string& sPath)
{
    registry::CXMLProxy xmlFile;
    bool bRes = false;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        std::string sNodeName;
        if (nodeRoot.isSubNode("Channels"))
        {
            bRes = true;
            registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
            std::cout << "Node count = " << nodeCount.getSubNodeCount() << std::endl << std::endl;
            for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
            {
                registry::CNode nodeValue = nodeCount.getSubNode(i);
                nodeValue.getValue("Name", sNodeName);
                std::cout << sNodeName << std::endl;

            }

        }

    }
    std::cout << std::endl << std::endl;
    return bRes;
}

bool cinArg(int ac, char* av[], boost::program_options::variables_map& vm)
{
    bool bRes = false;
    try
    {
        boost::program_options::options_description desc("Command Parser");
        desc.add_options()
                ("help,h", "show help")
                ("channels,c",
                 boost::program_options::value<std::string>(),
                 "set channel")
                ;
        boost::program_options::store(boost::program_options::parse_command_line(ac,av,desc), vm);
        std::cout << std::endl << std::endl;

        if(vm.count("help") || ( vm.size() == 0 ))
            std::cout << desc << std::endl << std::endl;
        else
        {
            boost::program_options::notify(vm);
            bRes = true;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!" << std::endl;
    }
    return bRes;
}

int main(int argc, char* argv[])
{
    std::string sPath = "$(NITAETC)/_System/ip_st/ip_st.xml";
    bool bCheckPath = conversionPath(sPath);
    if (!bCheckPath)
    {
        #ifdef WIN32
            sPath = "c:\\Nita\\Config";
        #else
            sPath = "/soft/etc";
        #endif
    }

    try
    {
       boost::filesystem::path pFileName(sPath);
       std::string sFileName = pFileName.filename().string();
       boost::program_options::variables_map vm;
       cinArg(argc, argv, vm);

       if (vm.count("channels"))
       {
           std::cout << "arg:" << vm["channels"].as<std::string>() << std::endl << std::endl << std::endl;
           std::string sArgName = vm["channels"].as<std::string>();

           if (checkArg(sArgName, sFileName))
               getNodeName(sPath);
           else
               std::cout << "ERR> Can't set channel by name: " << sArgName << std::endl << std::endl << std::endl;
       }

    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what();
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!" << std::endl;
    }
    return 0;
}
