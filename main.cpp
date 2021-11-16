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
#include <map>

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
                {
                    const std::string& sInterfacesValue = vInterfaces[j];
                    std::cout << "SendInterfaces: " << sInterfacesValue << " ";
                }
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

                if(bRes)
                    break;
            }
        }
    }
    return bRes;
}

bool outputChannelName(const std::string& sPath, const std::string& sChannel)
{
    bool bRes = false;
    if (checkChannelName(sPath, sChannel))
    {
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
                             std::cout << "c:" << sItemChannelName << std::endl;
                             bRes = true;
                             break;
                         }
                     }

                     if(bRes)
                        break;
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
                            std::cout << "s:" << sItemSourceName << std::endl;
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

bool checkSourceName(const std::string& sPath, const std::string& sSource)
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
                    std::string sItemSourceName;
                    nodeItem.getValue("Name", sItemSourceName);
                    if (boost::algorithm::iequals(sItemSourceName, sSource))
                    {
                        bRes = true;
                        break;
                    }
                }

                if(bRes)
                    break;
            }
        }
    }
    return bRes;
}

bool outputSourceName(const std::string& sPath, const std::string& sSource)
{
    bool bRes = false;
    if (checkSourceName(sPath, sSource))
    {
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
                        std::string sItemSourceName;
                        nodeItem.getValue("Name", sItemSourceName);
                        if (boost::algorithm::iequals(sItemSourceName, sSource))
                        {
                            std::cout << "s:" << sItemSourceName << std::endl;
                            bRes = true;
                            break;
                        }
                    }

                    if(bRes)
                        break;
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
                std::string sItemChannelName;
                nodeValue.getValue("Name", sItemChannelName);
                if (boost::algorithm::iequals(sChannel, sItemChannelName))
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
                    break;
                }
            }
        }
    }
    return bRes;
}

bool outputSourceInfo(const std::string& sPath, const std::string& sSource)
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
                    std::string sItemSourceName;
                    nodeItem.getValue("Name", sItemSourceName);
                    if (boost::algorithm::iequals(sItemSourceName, sSource))
                    {
                        for (int g = 0; g < nodeItem.getLeafCount(); g++)
                        {
                            std::string sValueLeaf;
                            nodeItem.getValue(g, sValueLeaf);
                            std::cout << "   " << nodeItem.getLeafName(g) << ":" << "\t" << sValueLeaf << std::endl;
                        }
                        bRes = true;
                        break;
                    }
                }

                if(bRes)
                   break;
            }
        }
    }
    return bRes;
}

bool outputCountChannels(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("Channels"))
        {
            registry::CNode nodeCountChannels = nodeRoot.getSubNode("Channels");
            std::cout << "Channels: " << nodeCountChannels.getSubNodeCount() << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool outputCountSources(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("Sources"))
        {
            registry::CNode nodeSources = nodeRoot.getSubNode("Sources");
            int iSourceCount = 0;
            for (int i = 0; i <nodeSources.getSubNodeCount(); i++)
            {
                registry::CNode nodeCountSources = nodeSources.getSubNode(i);
                iSourceCount += nodeCountSources.getSubNodeCount();
            }
            std::cout << "Sources: " << iSourceCount << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool outputCountTypes(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("DataTypes"))
        {
            registry::CNode nodeCountTypes = nodeRoot.getSubNode("DataTypes");
            std::cout << "Types: " << nodeCountTypes.getSubNodeCount() << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool checkRepeatingPortNum(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");
        if (nodeRoot.isSubNode("Channels"))
        {
            registry::CNode nodeCountChannels = nodeRoot.getSubNode("Channels");
            unsigned int size = nodeCountChannels.getSubNodeCount();
            unsigned int* iSockportValue = new unsigned int [size];
            for (int i = 0; i < nodeCountChannels.getSubNodeCount(); i++)
            {
                 registry::CNode nodeItem = nodeCountChannels.getSubNode(i);
                 unsigned int& iValueI = iSockportValue[i];
                 nodeItem.getValue("SockPortUdp_h", iValueI);
            }

            std::map <unsigned, std::string> mSockPortValue;
            for (int i = 0; i < nodeCountChannels.getSubNodeCount(); i++)
            {
                for (int j = 0; j < nodeCountChannels.getSubNodeCount(); j++)
                {
                    unsigned int& iValueJ = iSockportValue[j];
                    unsigned int& iValueI = iSockportValue[i];
                    if ((i != j) && (iValueI == iValueJ))
                        mSockPortValue.insert(std::make_pair(iValueI, nodeCountChannels.getSubNodeName(i)));
                }
            }


            if (mSockPortValue.empty())
            {
                bRes = true;
                std::cout << "Duplicate channel SockPortUdp_h: no errors." << std::endl;
            }
            else
            {
                std::map <unsigned, std::string> :: iterator it = mSockPortValue.begin();
                std::map <unsigned, std::string> mRepeatSockPortValue;
                std::cout << "Duplicate channel SockPortUdp_h:" << std::endl;
                for (int i = 0; it != mSockPortValue.end();it++, i++)
                {
                    for (int j = 0; j < nodeCountChannels.getSubNodeCount(); j++)
                    {
                        unsigned int& iValueJ = iSockportValue[j];
                        if ((it->first == iValueJ) && (it->second != nodeCountChannels.getSubNodeName(j)))
                        {
                            mRepeatSockPortValue.insert(std::make_pair(iSockportValue[j], nodeCountChannels.getSubNodeName(j)));
                            std::map <unsigned, std::string> :: iterator itForRepeat = mRepeatSockPortValue.begin();
                            if(it->first == itForRepeat->first)
                                std::cout << it->first << ": " << it->second << ", " << itForRepeat->second << std::endl;
                            mRepeatSockPortValue.erase(itForRepeat);

                        }

                    }
                }
            }

            delete []iSockportValue;
        }
    }
    return bRes;
}

bool checkRepeatingSourceId(const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile, "");    
        if (nodeRoot.isSubNode("Sources"))
        {
            std::cout << "Duplicate source IDs:" << std::endl;
            registry::CNode nodeSourceTypes = nodeRoot.getSubNode("Sources");
            std::map <unsigned, std::string> mIdValue;
            std::map <unsigned, std::string> mRepeatIdValue;
            int iScrRepeat = 0;
            for (int i = 0; i < nodeSourceTypes.getSubNodeCount(); i++)
            {
                registry::CNode nodeSources = nodeSourceTypes.getSubNode(i);
                unsigned int iSourceCount = nodeSources.getSubNodeCount();
                unsigned int* iItemSourceId = new unsigned int [iSourceCount];
                for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                {
                    registry::CNode nodeItem = nodeSources.getSubNode(j);
                    nodeItem.getValue("ID", iItemSourceId[j]);
                }

                std::string* sNodeName = new std::string [iSourceCount];
                for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
                {
                    for (int g = 0; g < nodeSources.getSubNodeCount(); g++)
                    {
                        unsigned int& iValueJ = iItemSourceId[j];
                        unsigned int& iValueG = iItemSourceId[g];
                        if ((j != g) && (iValueJ == iValueG))
                        {
                            registry::CNode nodeItem = nodeSources.getSubNode(j);
                            mIdValue.insert(std::make_pair(iItemSourceId[j], nodeSources.getSubNodeName(j)));
                            nodeItem.getValue("ChannelName", sNodeName[j]);
                            break;
                        }

                    }
                }

                std::map <unsigned, std::string> :: iterator it = mIdValue.begin();
                for (int j = 0; it != mIdValue.end();it++, j++)
                {
                    for (int g = 0; g < nodeSources.getSubNodeCount(); g++)
                    {
                        unsigned int& iValueG = iItemSourceId[g];
                        registry::CNode nodeItem = nodeSources.getSubNode(g);
                        std::string sNode;
                        nodeItem.getValue("ChannelName", sNode);
                        if ((it->first == iValueG) && (it->second != nodeSources.getSubNodeName(g)) && (sNode == sNodeName[g]))
                        {
                            mRepeatIdValue.insert(std::make_pair(iItemSourceId[g], nodeSources.getSubNodeName(g)));
                            std::map <unsigned, std::string> :: iterator itForRepeat = mRepeatIdValue.begin();
                            if(it->first == itForRepeat->first)
                            {
                                std::cout << it->first << ": " << it->second << ", " << itForRepeat->second << std::endl;
                                iScrRepeat++;
                            }
                            mRepeatIdValue.erase(itForRepeat);
                        }
                    }
                }

                delete[] sNodeName;
                delete[] iItemSourceId;
            }

            if(iScrRepeat == 0)
            {
                std::cout << "Duplicate source IDs: no errors." << std::endl;
                bRes = true;
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
                ("help,h",        "show help")
                ("net,n",         "show net")
                ("display,d",
                 boost::program_options::value<std::string>(),
                 " arg is c or s: c - display channels, s - display sources")
                ("channel,c",
                 boost::program_options::value<std::string>(),
                 "show info for named channel")
                ("source,s",
                 boost::program_options::value<std::string>(),
                 "show info for named source")
                ("info,i",        "show full information - all parameters")
                ("stat",          "show statistics types, channels, sources")
                ("check",         "checks the integrity of ip_st configurations - duplicate port numbers, source IDs, etc")
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
    std::string sPath = "$(NITAETC)/_System/ip_st/x.xml";

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
                        outputChannelName(sPath, sChannel);
                    else
                        std::cout << "ERR> Can't set channel by name: " << sChannel << std::endl << std::endl << std::endl;

                    if(vm.count("display"))
                    {
                       std::string sArgDisplay = vm["display"].as<std::string>();
                       if(sArgDisplay == "s")
                          outputSourcesByChannel(sPath, sChannel);
                    }

                    if(vm.count("info"))
                        outputChannelInfo(sPath, sChannel);
                }

                if(vm.count("source"))
                {
                    std::string sSource = vm["source"].as<std::string>();
                    if (checkSourceName(sPath, sSource))
                        outputSourceName(sPath, sSource);

                    if(vm.count("info"))
                       if(checkSourceName(sPath, sSource))
                          outputSourceInfo(sPath, sSource);
                }

                if (!vm.count("source"))
                {
                    if (vm.count("display"))
                    {
                        std::string sArgDisplay = vm["display"].as<std::string>();
                        if (sArgDisplay == "s")   
                            if (!vm.count("channel"))
                                outputSourcesInfo(sPath, vm);
                        if (sArgDisplay == "c")
                            outputChannelsInfo(sPath, vm);
                    }
                }

                if(vm.count("stat"))
                {
                    outputCountTypes(sPath);
                    outputCountChannels(sPath);
                    outputCountSources(sPath);
                }

                if(vm.count("check"))
                {
                    checkRepeatingPortNum(sPath);
                    checkRepeatingSourceId(sPath);
                    /*if (checkRepeatingPortNum(sPath) && checkRepeatingSourceId(sPath))
                        std::cout << "Check: success" << std::endl;
                    else
                        std::cout << "Check: failed" << std::endl << std::endl;*/
                }

            }
            else
                std::cout << "ERR> ip_st.xml not found" << std::endl;
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
