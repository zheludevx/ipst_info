#include <ModeInfo.h>
#include <vector>
#include <string>
#include <exception>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <reg_12.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/locale.hpp>

bool convertPath(std::string& sPath)
{
    plug_key::CModeInfoPlug lib;
    bool bRes = lib.Load();
    lib.ExpandString(sPath);
    lib.Free();
    return bRes;
}

bool outputNet(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
       registry::CNode nodeRoot(&xmlFile, "");
       std::string sValueSockAddr;
       nodeRoot.getValue("SockAddr", sValueSockAddr);
       std::cout << "SockAddr: " << sValueSockAddr << std::endl;
       if (nodeRoot.isSubNode("ConfigGroups"))
       {
           registry::CNode nodeConfigGroups = nodeRoot.getSubNode("ConfigGroups");
           std::vector<std::string> vInterfaces;
           for (int i = 0; i < nodeConfigGroups.getSubNodeCount(); i++)
           {
               registry::CNode nodeItemInterfaces = nodeConfigGroups.getSubNode(i);
               nodeItemInterfaces.getValue("SendInterfaces", vInterfaces);
               for (unsigned int j = 0; j < vInterfaces.size(); j++)
                    std::cout << "SendInterfaces: " << vInterfaces[j] << " ";
           }
       }
       std::cout << std::endl;
       bRes = true;
    }
    return bRes;
}

bool checkChannelName(const std::string& sPath, const std::string& sChannel)
{
    bool bRes = false;
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
                      if (boost::algorithm::iequals(sItemChannelName, sChannel))
                      {
                          bRes = true;
                          break;
                      }
                 }
             }
         }
     }
     return bRes;
}

bool outputSourcesByChannel(const std::string& sPath, const std::string& sChannel)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        std::string sTypeName;
        if (nodeRoot.isSubNode("Channels"))
        {
            registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
            for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
            {
                registry::CNode nodeValue = nodeCount.getSubNode(i);
                std::string sNodeName;
                nodeValue.getValue("Name", sNodeName);
                if (boost::algorithm::iequals(sChannel, sNodeName))
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

                        if (boost::algorithm::iequals(sChannel, sItemChannelName))
                        {
                            std::string sItemSourceName;
                            nodeItem.getValue("Name", sItemSourceName);
                            std::cout << "c:" << sItemSourceName << std::endl;
                        }
                    }
                    bRes = true;
                    break;
                }
            }
        }
    }
    return bRes;
}

bool outputSourceName(const std::string& sPath, const std::string sChannel)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
       registry::CNode nodeRoot(&xmlFile, "");
       std::string sTypeName;
       if (nodeRoot.isSubNode("Channels"))
       {
           registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
           for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
           {
               registry::CNode nodeValue = nodeCount.getSubNode(i);
               std::string sNodeName;
               nodeValue.getValue("Name", sNodeName);
               if (boost::algorithm::iequals(sChannel, sNodeName))
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

                       if (boost::algorithm::iequals(sChannel, sItemChannelName))
                       {
                           std::cout << "s:" << nodeSourceTypes.getSubNodeName(i) << std::endl;
                           bRes = true;
                           break;
                       }
                   }
               }
           }
       }
    }
    return bRes;
}

bool outputChannelInfo(const std::string& sPath, const std::string sChannel)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
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
               if (boost::algorithm::iequals(sChannel, sNodeName))
               {
                   for (int j = 0; j < nodeValue.getLeafCount(); j++)
                   {
                       if (nodeValue.getLeafName(j) != "DisplayName")
                       {
                           std::string sValueLeaf;
                           nodeValue.getValue(j, sValueLeaf);
                           std::cout << "   " << nodeValue.getLeafName(j) << ":" << "\t" << sValueLeaf << std::endl;
                       }
                       else
                       {
                           std::string sDisplayName;
                           nodeValue.getValue("DisplayName", sDisplayName);
                           std::string sRes = boost::locale::conv::between(sDisplayName, "utf-8", "cp-1251");
                           std::cout << "   " << nodeValue.getLeafName(j) << ":" << "\t" << sRes << std::endl;
                       }
                   }
                   bRes = true;
               }
           }
       }
    }
    return bRes;
}

bool outputSourceInfo(const std::string& sPath, const std::string& sChannel)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
       registry::CNode nodeRoot(&xmlFile, "");
       std::string sTypeName;
       if (nodeRoot.isSubNode("Channels"))
       {
           registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
           for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
           {
               registry::CNode nodeValue = nodeCount.getSubNode(i);
               std::string sNodeName;
               nodeValue.getValue("Name", sNodeName);
               if (boost::algorithm::iequals(sChannel, sNodeName))
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

                       if (boost::algorithm::iequals(sChannel, sItemChannelName))
                       {
                           for (int g = 0; g < nodeItem.getLeafCount(); g++)
                           {
                                std::string sValueLeaf;
                                nodeItem.getValue(g, sValueLeaf);
                                std::cout << "   " << nodeItem.getLeafName(g) << ":" << "\t" << sValueLeaf << std::endl;
                           }
                       }
                   }
                   bRes = true;
                   break;
               }
           }
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
                ("net",         "show net")
                ("display,d",
                 boost::program_options::value<std::string>(),
                 " arg is c or s: c - display channels, s - display sources")
                ("channel,c",
                 boost::program_options::value<std::string>(),
                 "show info for named channel")
                ("source,s",
                 boost::program_options::value<std::string>(),
                 "show info for named source")
                ("info,i",      "show full information - all parameters")
                ;
        boost::program_options::store(boost::program_options::parse_command_line(ac,av,desc), vm);

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

bool outputChannelsInfo(const std::string& sPath, boost::program_options::variables_map& vm)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
       registry::CNode nodeRoot(&xmlFile, "");
       if (nodeRoot.isSubNode("Channels"))
       {
           registry::CNode nodeCount = nodeRoot.getSubNode("Channels");
           for (int i = 0; i < nodeCount.getSubNodeCount(); i++)
           {
               std::cout << "c:" << nodeCount.getSubNodeName(i) << std::endl;
               if(vm.count("info"))
               {
                  registry::CNode nodeValue = nodeCount.getSubNode(i);
                  for (int j = 0; j < nodeValue.getLeafCount(); j++)
                  {
                       if (nodeValue.getLeafName(j) != "DisplayName")
                       {
                           std::string sValueLeaf;
                           nodeValue.getValue(j, sValueLeaf);
                           std::cout << "   " << nodeValue.getLeafName(j) << ":" << "\t" << sValueLeaf << std::endl;
                       }
                       else
                       {
                           std::string sDisplayName;
                           nodeValue.getValue("DisplayName", sDisplayName);
                           std::string sRes = boost::locale::conv::between(sDisplayName, "utf-8", "cp-1251");
                           std::cout << "   " << nodeValue.getLeafName(j) << ":" << "\t" << sRes << std::endl;
                       }
                  }
               }
           }
           bRes = true;
       }
    }
    return bRes;
}

bool outputSourcesInfo(const std::string& sPath, boost::program_options::variables_map& vm)
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
                for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                {
                     std::cout << "s:"<<nodeSources.getSubNodeName(j) << std::endl;
                     if (vm.count("info"))
                     {
                         registry::CNode nodeItem = nodeSources.getSubNode(j);
                         for (int g = 0; g < nodeItem.getLeafCount(); g++)
                         {
                           std::string sValueLeaf;
                           nodeItem.getValue(g, sValueLeaf);
                           std::cout << "   " << nodeItem.getLeafName(g) << ":" << "\t" << sValueLeaf << std::endl;
                         }
                     }
                }
           }
           bRes = true;
        }
    }
    return bRes;
}

int main(int argc, char* argv[])
{
    std::string sPath = "$(NITAETC)/_System/ip_st/ip_st.xml";

    try
    {
       boost::program_options::variables_map vm;

       if(parseArgs(argc, argv, vm))
       {
          if(convertPath(sPath))
          {
             if(vm.count("net"))
                outputNet(sPath);

             if(vm.count("channel"))
             {
                std::string sChannel = vm["channel"].as<std::string>();
                if (checkChannelName(sPath, sChannel))
                    outputSourcesByChannel(sPath, sChannel);
                else
                    std::cout << "ERR> Can't set channel by name: " << sChannel << std::endl << std::endl << std::endl;

                if(vm.count("info"))
                   outputChannelInfo(sPath, sChannel);
             }

             if(vm.count("source"))
             {
                std::string sChannel = vm["source"].as<std::string>();
                if (checkChannelName(sPath, sChannel))
                    outputSourceName(sPath, sChannel);
                else
                    std::cout << "ERR> Can't set channel by name: " << sChannel << std::endl << std::endl << std::endl;

                if(vm.count("info"))
                   outputSourceInfo(sPath, sChannel);
             }

             if (!vm.count("source"))
             {
                if (vm.count("display"))
                {
                    std::string sArgDisplay = vm["display"].as<std::string>();
                    if (sArgDisplay == "s")
                        outputSourcesInfo(sPath, vm);
                    if (sArgDisplay == "c")
                        outputChannelsInfo(sPath, vm);
                }
             }

          }
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
