#include <ModeInfo.h>
#include <vector>
#include <string>
#include <exception>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <reg_12.h>
#include <cctype>
#include <boost/algorithm/string/predicate.hpp>


bool convertPath(std::string& sPath)
{
    plug_key::CModeInfoPlug lib;
    bool bRes = lib.Load();
   // std::cout << "RES>" << bRes << std::endl << std::endl;
    lib.ExpandString(sPath);
   // std::cout << sPath << std::endl;

   // std::cout << std::endl << std::endl;
    lib.Free();
    return bRes;
}

std::string checkChannelName(const std::string& sPath, const std::string& Channel)
{
    std::string sCheckNonstrict;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("Sources"))
        {
            registry::CNode nodeSourceTypes = nodeRoot.getSubNode("Sources");
            for (int i = 0; i < nodeSourceTypes.getSubNodeCount(); i++)
            {
                 registry::CNode nodeSources = nodeSourceTypes.getSubNode(i);
                 for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                 {
                      registry::CNode nodeItem = nodeSources.getSubNode(j);
                      std::string sItemChannelName;
                      nodeItem.getValue("ChannelName", sItemChannelName);
                      if (boost::algorithm::iequals(sItemChannelName, Channel))
                      {
                          sCheckNonstrict = sItemChannelName;
                          break;
                      }
                 }
             }
         }
     }
     return sCheckNonstrict;
}

bool outputChannels(const std::string& sPath)
{
    registry::CXMLProxy xmlFile;
    bool bRes = false;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");    
        if (nodeRoot.isSubNode("Channels"))
        {
            registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
            for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
            {
                registry::CNode nodeValue = nodeCount.getSubNode(i);
                std::string sNodeName;
                nodeValue.getValue("Name", sNodeName);
                std::cout << sNodeName << std::endl;
            }
            std::cout << std::endl << std::endl;
            bRes = true;
        }

    }
    return bRes;
}

bool outputSourcesByChannel(const std::string& sPath, const std::string& sChannel)
{
    registry::CXMLProxy xmlFile;
    bool bRes = false;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        std::string sChannelNonstrict = checkChannelName(sPath, sChannel);
        std::string sTypeName;
        if (nodeRoot.isSubNode("Channels"))
        {
            registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
            for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
            {
                registry::CNode nodeValue = nodeCount.getSubNode(i);
                std::string sNodeName;
                nodeValue.getValue("Name", sNodeName);
                if (sChannelNonstrict == sNodeName)
                {
                    nodeValue.getValue("TypeName", sTypeName);
                    break;
                }
            }
        }

        if (nodeRoot.isSubNode("Sources"))
        {
            registry::CNode nodeSourceTypes = nodeRoot.getSubNode("Sources");
            for (int i = 0; i < nodeSourceTypes.getSubNodeCount(); i++)
            {
                registry::CNode nodeSources = nodeSourceTypes.getSubNode(i);
                std::string sSourcesType;
                nodeSources.getValue("TypeName", sSourcesType);

                if (sSourcesType == sTypeName)
                {
                    for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                    {
                        registry::CNode nodeItem = nodeSources.getSubNode(j);
                        std::string sItemChannelName;
                        nodeItem.getValue("ChannelName", sItemChannelName);

                        if (sChannelNonstrict == sItemChannelName)
                        {
                            std::string sItemSourceName;
                            nodeItem.getValue("Name", sItemSourceName);
                            std::cout << sItemSourceName << std::endl;
                        }
                    }
                    bRes = true;
                    break;
                }
            }
        }
    }
    std::cout << std::endl << std::endl;
    return bRes;
}

bool outputSources(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("Sources"))
        {
            registry::CNode nodeSourceType = nodeRoot.getSubNode("Sources");
            for (int i = 0; i < nodeSourceType.getSubNodeCount(); i++)
            {
                registry::CNode nodeSources = nodeSourceType.getSubNode(i);
                std::string sSourcesType;
                nodeSources.getValue("TypeName", sSourcesType);
                std::cout << sSourcesType << ":" << std::endl;
                for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                {
                    registry::CNode nodeItem = nodeSources.getSubNode(j);
                    std::string sItemChannelName;
                    nodeItem.getValue("ChannelName", sItemChannelName);
                    std::cout << nodeItem.getName() << ": ";
                    std::string sItemSourceName;
                    nodeItem.getValue("Name", sItemSourceName);
                    std::cout << sItemSourceName << std::endl;
                }
                std::cout << std::endl << std::endl;
            }
            bRes = true;
        }
    }
    return bRes;
}


bool parseArgs(int ac, char* av[], boost::program_options::variables_map& vm)
{
    bool bRes = false;
    try
    {
        boost::program_options::options_description desc("Command Parser");
        desc.add_options()
                ("help,h",      "show help")
                ("channels,c",  "show channel")
                ("sources",     "show sources")
                ("ch",
                 boost::program_options::value<std::string>(),
                 "show sources for specified channel")
                ;
        boost::program_options::store(boost::program_options::parse_command_line(ac,av,desc), vm);
        std::cout << std::endl << std::endl;

        if(vm.count("help") || ( vm.size() == 0 ))
        {
            std::cout << desc << std::endl << std::endl;
            exit(0);
        }
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

    try
    {
       boost::filesystem::path pFileName(sPath);
       std::string sFileName = pFileName.filename().string();
       boost::program_options::variables_map vm;
       parseArgs(argc, argv, vm);


       if(convertPath(sPath))
       {
          if (vm.count("channels"))
              outputChannels(sPath);

          if (vm.count("ch"))
          {
             std::cout << "Channel:" << vm["ch"].as<std::string>() << std::endl << std::endl << std::endl;
             std::string sChannel = vm["ch"].as<std::string>();
             std::string sChannelAfterCheck = checkChannelName(sPath, sChannel);
             if (!sChannelAfterCheck.empty())
                 outputSourcesByChannel(sPath, sChannel);
             else
                 std::cout << "ERR> Can't set channel by name: " << sChannel << std::endl << std::endl << std::endl;
          }

          if(vm.count("sources"))
             outputSources(sPath);
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
