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
#include <ModeInfo/MergeXml.h>

bool outputNet(registry::CNode& nodeRoot)
{
    bool bRes = false;
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
    return bRes;
}

bool checkChannelName(registry::CNode& nodeRoot, const std::string& sChannel)
{
    bool bRes = false;
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
    return bRes;
}

bool outputChannelName(registry::CNode& nodeRoot, const std::string& sChannel)
{
    bool bRes = false;
    if (checkChannelName(nodeRoot, sChannel))
    {
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
    return bRes;
}

bool outputSourcesByChannel(registry::CNode& nodeRoot, const std::string& sChannel)
{
    bool bRes = false;
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
    return bRes;
}

bool checkSourceName(registry::CNode& nodeRoot, const std::string& sSource)
{
    bool bRes = false;
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
    return bRes;
}

bool outputSourceName(registry::CNode& nodeRoot, const std::string& sSource)
{
    bool bRes = false;
    if (checkSourceName(nodeRoot, sSource))
    {
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
    return bRes;
}

bool outputChannelInfo(registry::CNode& nodeRoot, const std::string sChannel)
{
    bool bRes = false;
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
    return bRes;
}

bool outputSourceInfo(registry::CNode& nodeRoot, const std::string& sSource)
{
    bool bRes = false;
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
    return bRes;
}

bool outputCountChannels(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("Channels"))
    {
        registry::CNode nodeCountChannels = nodeRoot.getSubNode("Channels");
        std::cout << "Channels: " << nodeCountChannels.getSubNodeCount() << std::endl;
        bRes = true;
    }
    return bRes;
}

bool outputCountSources(registry::CNode& nodeRoot)
{
    bool bRes = false;
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
    return bRes;
}

bool outputCountTypes(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("DataTypes"))
    {
        registry::CNode nodeCountTypes = nodeRoot.getSubNode("DataTypes");
        std::cout << "Types: " << nodeCountTypes.getSubNodeCount() << std::endl;
        bRes = true;
    }
    return bRes;
}

bool checkRepeatingPortNum(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("Channels"))
    {
        registry::CNode nodeCountChannels = nodeRoot.getSubNode("Channels");
        int iCountRepeat = 0;
        std::map <unsigned, std::string> map;
        std::cout << "* Duplicate channel SockPortUdp_h: " << std::endl;
        for (int i = 0; i < nodeCountChannels.getSubNodeCount(); i++)
        {
            registry::CNode nodeItem = nodeCountChannels.getSubNode(i);
            unsigned uChannelPortlId = 0;
            nodeItem.getValue("SockPortUdp_h", uChannelPortlId);
            std::string sItemName = nodeItem.getName();
            if(map.find(uChannelPortlId) == map.end())
                map[uChannelPortlId] = sItemName;
            else
            {
                std::cout << uChannelPortlId << ": " << map[uChannelPortlId] << ", " << sItemName << std::endl;
                iCountRepeat++;
            }
        }

        if (iCountRepeat == 0)
        {
            std::cout << "no errors." << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool checkRepeatingSourceId(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("Sources"))
    {
        std::cout << "* Duplicate source IDs: " << std::endl;
        registry::CNode nodeSourcesTypes = nodeRoot.getSubNode("Sources");
        int iCountRepeat = 0;
        for (int i = 0; i <nodeSourcesTypes.getSubNodeCount(); i++)
        {
            registry::CNode nodeSources = nodeSourcesTypes.getSubNode(i);
            std::map <unsigned, std::string> map;
            for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
            {
                registry::CNode nodeItem = nodeSources.getSubNode(j);
                unsigned uSourceId = 0;
                nodeItem.getValue("ID", uSourceId);
                std::string sItemName = nodeItem.getName();
                if(map.find(uSourceId) == map.end())
                    map[uSourceId] = sItemName;
                else
                {
                    std::cout << uSourceId << ": " << map[uSourceId] << ", " << sItemName << std::endl;
                    iCountRepeat++;
                }
            }

        }

        if(iCountRepeat == 0)
        {
            std::cout << "no errors." << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool checkRepeatingNameChannel(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("Channels"))
    {
        registry::CNode nodeCountChannels = nodeRoot.getSubNode("Channels");
        int iCountRepeat = 0;
        std::map <std::string, std::string> map;
        std::cout << "* Duplicate channels name: " << std::endl;
        for (int i = 0; i < nodeCountChannels.getSubNodeCount(); i++)
        {
            registry::CNode nodeItem = nodeCountChannels.getSubNode(i);
            std::string sChannelName;
            nodeItem.getValue("Name", sChannelName);
            std::string sItemName = nodeItem.getName();
            if(map.find(sChannelName) == map.end())
                map[sChannelName] = sItemName;
            else
            {
                std::cout << map[sChannelName] << ": " << sItemName << std::endl;
                iCountRepeat++;
            }
        }

        if (iCountRepeat == 0)
        {
            std::cout << "no errors." << std::endl;
            bRes = true;
        }
    }
    return bRes;
}

bool checkRepeatingNameSource(registry::CNode& nodeRoot)
{
    bool bRes = false;
    if (nodeRoot.isSubNode("Sources"))
    {
        std::cout << "* Duplicate sources name: " << std::endl;
        registry::CNode nodeSourcesTypes = nodeRoot.getSubNode("Sources");
        int iCountRepeat = 0;
        for (int i = 0; i <nodeSourcesTypes.getSubNodeCount(); i++)
        {
            registry::CNode nodeSources = nodeSourcesTypes.getSubNode(i);
            std::map <std::string, std::string> map;
            for (int j = 0; j < nodeSources.getSubNodeCount(); j++)
            {
                registry::CNode nodeItem = nodeSources.getSubNode(j);
                std::string sSourceName;
                nodeItem.getValue("Name", sSourceName);
                std::string sItemName = nodeItem.getName();
                if(map.find(sSourceName) == map.end())
                    map[sSourceName] = sItemName;
                else
                {
                    std::cout << map[sSourceName] << ": " << sItemName << std::endl;
                    iCountRepeat++;
                }
            }

        }

        if(iCountRepeat == 0)
        {
            std::cout << "no errors." << std::endl;
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

bool outputChannelsInfo(registry::CNode& nodeRoot, boost::program_options::variables_map& vm)
{
    bool bRes = false;
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
    return bRes;
}

bool outputSourcesInfo(registry::CNode& nodeRoot, boost::program_options::variables_map& vm)
{
    bool bRes = false;
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
    return bRes;
}

int main(int argc, char* argv[])
{
    std::string sPath = "$(NITAETC)/_System/ip_st/ip_st.xml";
    plug_key::CModeInfoPlug lib;
    lib.ExpandString(sPath);
    registry::CXMLProxy xmlFile;

    try
    {
        if(xmlFile.load(sPath))
        {
            std::string sClarifyingPath = "$(NITAETC)/templates/_Shared/ip_st.xml";
            if(lib.ExpandString(sClarifyingPath))
            {
                const char* pPathDyn = sClarifyingPath.c_str();
                MergeXml(lib, xmlFile, pPathDyn, NULL);
            }


            registry::CNode nodeRoot(&xmlFile, "");
            boost::program_options::variables_map vm;
            if(parseArgs(argc, argv, vm))
            {
                if(vm.count("net"))
                    outputNet(nodeRoot);

                if(vm.count("channel"))
                {
                    std::string sChannel = vm["channel"].as<std::string>();
                    if (checkChannelName(nodeRoot, sChannel))
                        outputChannelName(nodeRoot, sChannel);
                    else
                        std::cout << "ERR> Can't set channel by name: " << sChannel << std::endl << std::endl << std::endl;

                    if(vm.count("display"))
                    {
                       std::string sArgDisplay = vm["display"].as<std::string>();
                       if(sArgDisplay == "s")
                          outputSourcesByChannel(nodeRoot, sChannel);
                    }

                    if(vm.count("info"))
                        outputChannelInfo(nodeRoot, sChannel);
                }

                if(vm.count("source"))
                {
                    std::string sSource = vm["source"].as<std::string>();
                    if(checkSourceName(nodeRoot, sSource))
                        outputSourceName(nodeRoot, sSource);

                    if(vm.count("info"))
                       if(checkSourceName(nodeRoot, sSource))
                          outputSourceInfo(nodeRoot, sSource);
                }

                if (!vm.count("source"))
                {
                    if (vm.count("display"))
                    {
                        std::string sArgDisplay = vm["display"].as<std::string>();
                        if (sArgDisplay == "s")   
                            if (!vm.count("channel"))
                                outputSourcesInfo(nodeRoot, vm);
                        if (sArgDisplay == "c")
                            outputChannelsInfo(nodeRoot, vm);
                    }
                }

                if(vm.count("stat"))
                {
                    outputCountTypes(nodeRoot);
                    outputCountChannels(nodeRoot);
                    outputCountSources(nodeRoot);
                }

                if(vm.count("check"))
                {
                    checkRepeatingPortNum(nodeRoot);
                    checkRepeatingSourceId(nodeRoot);
                    checkRepeatingNameChannel(nodeRoot);
                    checkRepeatingNameSource(nodeRoot);
                }

            }            
        }
        else
            std::cout << "ERR> ip_st.xml not found" << std::endl;
    } 
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what();
    }
    catch(...)
    {
        std::cerr << "Exception of unknown type!" << std::endl;
    }
    lib.Free();
    return 0;
}
