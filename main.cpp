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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdlib.h>

bool GetLocalInterfaces(std::vector<std::string>& ip, const std::vector<std::string>& vInterfacesOptions)
{
    bool bRes = false;
    int fd;
    char buff[BUFSIZ];
    struct ifconf ifc;
    struct ifreq *ifr;
    ifr = 0;

    if((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
        std::cout << "UpdateLocalAddresses> Can't create socket" << std::endl;
    else
    {
        ifc.ifc_len = sizeof(buff);
        ifc.ifc_buf = buff;
        if(ioctl(fd, SIOCGIFCONF, &ifc) < 0)
            std::cout << "UpdateLocalAddresses> ioctl error" << std::endl;
        else
        {
            bRes = true;
            unsigned int nAddrCount = ifc.ifc_len/sizeof(struct ifreq);
            for(unsigned int i = 0; i < nAddrCount; i++)
            {
                ifr = &ifc.ifc_req[i];
                if (ifr)
                {
                    char ipDotBuf[16];
                    unsigned long ulAddr_n = ((struct sockaddr_in*)(&ifr->ifr_addr))->sin_addr.s_addr;
                    inet_ntop(AF_INET, &ulAddr_n, ipDotBuf, (socklen_t )sizeof(ipDotBuf));
                    unsigned long ulAddr_h = ntohl(ulAddr_n);

                    if(ulAddr_h != INADDR_LOOPBACK)
                        for(unsigned int j = 0; j < vInterfacesOptions.size(); j++)
                        {
                            const std::string sValueInterfaces = vInterfacesOptions[j];
                            if(sValueInterfaces == ipDotBuf)
                                ip.push_back(std::string(ipDotBuf));
                        }
                }
                else
                    break;
            }
        }
        close(fd);
    }
    if (!ip.empty())
    {
        std::cout << "Local interfaces: ";
        for (unsigned int i = 0; i < ip.size(); i++)
        {
            const std::string IpValue = ip[i];
            std::cout << IpValue << "  ";
        }
        std::cout << std::endl;
    }
    else
        std::cout << "Local interfaces: ERR" << std::endl;

    return bRes;
}

bool GetAllLocalInterfaces(std::vector<std::string>& vInterfacesOptions)
{
    bool bRes = false;
    int fd;
    char buff[BUFSIZ];
    struct ifconf ifc;
    struct ifreq *ifr;
    ifr = 0;

    if((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
        std::cout << "UpdateLocalAddresses> Can't create socket" << std::endl;
    else
    {
        ifc.ifc_len = sizeof(buff);
        ifc.ifc_buf = buff;
        if(ioctl(fd, SIOCGIFCONF, &ifc) < 0)
            std::cout << "UpdateLocalAddresses> ioctl error" << std::endl;
        else
        {
            bRes = true;
            unsigned int nAddrCount = ifc.ifc_len/sizeof(struct ifreq);
            for(unsigned int i = 0; i < nAddrCount; i++)
            {
                ifr = &ifc.ifc_req[i];
                if (ifr)
                {
                    char ipDotBuf[16];
                    unsigned long ulAddr_n = ((struct sockaddr_in*)(&ifr->ifr_addr))->sin_addr.s_addr;
                    inet_ntop(AF_INET, &ulAddr_n, ipDotBuf, (socklen_t )sizeof(ipDotBuf));
                    unsigned long ulAddr_h = ntohl(ulAddr_n);

                    if(ulAddr_h != INADDR_LOOPBACK)
                        for(unsigned int j = 0; j < vInterfacesOptions.size(); j++)
                            vInterfacesOptions.push_back(ipDotBuf);
                }
                else
                    break;
            }
        }
        close(fd);
    }
    return bRes;
}

bool CheckNet(const std::vector<std::string>& ip, registry::CNode& nodeRoot)
{
    bool bRes = false;
    if(!ip.empty())
    {
        if (nodeRoot.isSubNode("ConfigGroups"))
        {
            registry::CNode nodeConfigGroups = nodeRoot.getSubNode("ConfigGroups");
            std::vector<std::string> vInterfaces;
            for (int i = 0; i < nodeConfigGroups.getSubNodeCount(); i++)
            {
                registry::CNode nodeItemInterfaces = nodeConfigGroups.getSubNode(i);
                nodeItemInterfaces.getValue("SendInterfaces", vInterfaces);
            }

            std::vector<std::string> vSuccessfulCheckIp;
            if (!vInterfaces.empty())
            {
                for (unsigned int i = 0; i < vInterfaces.size(); i++)
                {
                    int iScrCheck = 0;
                    for (unsigned int j = 0; j < ip.size(); j++)
                    {
                        std::string sDelim = ".";
                        size_t sztPrev = 0;
                        size_t sztPrevIp = 0;
                        size_t sztNext;
                        size_t sztNextIp;
                        size_t sztDelta = sDelim.length();
                        std::string& sInterfacesValue = vInterfaces[i];
                        const std::string& sIpValue = ip[j];
                        while((sztNext = sInterfacesValue.find(sDelim, sztPrev)) != std::string::npos
                              &&((sztNextIp = sIpValue.find(sDelim, sztPrevIp)) != std::string::npos))
                        {
                            if(sInterfacesValue.substr(sztPrev, sztNext-sztPrev) != "255")
                            {
                                if (sInterfacesValue.substr(sztPrev, sztNext-sztPrev) == sIpValue.substr(sztPrevIp, sztNextIp-sztPrevIp))
                                    iScrCheck++;
                            }
                            if(sInterfacesValue.substr(sztPrev, sztNext-sztPrev) == "255")
                                iScrCheck++;

                            sztPrev = sztNext + sztDelta;
                            sztPrevIp = sztNextIp + sztDelta;
                        }

                        if ((sInterfacesValue.rfind(sDelim, sztPrev) != std::string::npos)
                                && ((sztNextIp = sIpValue.rfind(sDelim, sztPrevIp)) != std::string::npos))
                        {
                            if (sInterfacesValue.substr(sztPrev, sztNext-sztPrev) != "255")
                            {
                                if (sInterfacesValue.substr(sztPrev, sztNext-sztPrev) == sIpValue.substr(sztPrevIp, sztNextIp-sztPrevIp))
                                    iScrCheck++;
                            }
                            if (sInterfacesValue.substr(sztPrev, sztNext-sztPrev) == "255")
                                iScrCheck++;
                        }

                        if(iScrCheck == 4)
                            vSuccessfulCheckIp.push_back(sIpValue);
                    }
                }

                if(!vSuccessfulCheckIp.empty())
                    bRes = true;
            }
            else
            {
                for (unsigned i = 0; i < ip.size(); i++)
                {
                    const std::string& sIpValue = ip[i];
                    vSuccessfulCheckIp.push_back(sIpValue);
                }
                bRes = true;
            }

            if (vInterfaces.empty())
                std::cout << "SendInterfaces (config): ERR" << std::endl;
            else
            {
                std::cout << "SendInterfaces (config): ";
                for(unsigned int i = 0; i < vInterfaces.size(); i++)
                {
                    const std::string valueConfigInterfaces = vInterfaces[i];
                    std::cout << valueConfigInterfaces << " ";
                }
                std::cout<<std::endl;
            }

            if (vSuccessfulCheckIp.empty())
                std::cout << "SendInterfaces (local): ERR" << std::endl;
            else
            {
                std::cout << "SendInterfaces (local): ";
                for(unsigned int i = 0; i < vSuccessfulCheckIp.size(); i++)
                {
                    const std::string valueLocalInterfaces = vSuccessfulCheckIp[i];
                    std::cout << valueLocalInterfaces << " ";
                }
            }
        }
    }
    else
    {
        registry::CNode nodeConfigGroups = nodeRoot.getSubNode("ConfigGroups");
        std::vector<std::string> vInterfaces;
        for (int i = 0; i < nodeConfigGroups.getSubNodeCount(); i++)
        {
            registry::CNode nodeItemInterfaces = nodeConfigGroups.getSubNode(i);
            nodeItemInterfaces.getValue("SendInterfaces", vInterfaces);
        }

        if (vInterfaces.empty())
            std::cout << "SendInterfaces (config): ERR" << std::endl;
        else
        {
            std::cout << "SendInterfaces (config): ";
            for(unsigned int i = 0; i < vInterfaces.size(); i++)
            {
                const std::string valueConfigInterfaces = vInterfaces[i];
                std::cout << valueConfigInterfaces << " ";
            }
            std::cout<<std::endl;
        }
        std::cout << "SendInterfaces (local): ERR" << std::endl;
    }

    return bRes;
}

bool checkSockAddr(const std::string& sValueSockAddr)
{
    bool bRes = false;
    std::string sDelim = ".";
    size_t sztPrev = 0;
    size_t sztNext;
    size_t sztDelta = sDelim.length();
    int iScrCheck = 0;
    if ((sztNext = sValueSockAddr.find(sDelim, sztPrev)) != std::string::npos)
    {
        std::string sFirstByte = sValueSockAddr.substr(sztPrev, sztNext-sztPrev);
        int iValueFirstByte = boost::lexical_cast<int> (sFirstByte);
        if (iValueFirstByte >= 224 && iValueFirstByte <= 239)
        {
            sztPrev = sztNext + sztDelta;
            while ((sztNext = sValueSockAddr.find(sDelim, sztPrev)) != std::string::npos)
            {
                std::string sByte = sValueSockAddr.substr(sztPrev, sztNext-sztPrev);
                int iValueByte = boost::lexical_cast<int> (sByte);
                if (iValueByte > -1 && iValueByte < 256)
                    iScrCheck++;
                sztPrev = sztNext + sztDelta;
            }

            if((sValueSockAddr.rfind(sDelim, sztPrev)) != std::string::npos)
            {
                std::string sByte = sValueSockAddr.substr(sztPrev, sztNext-sztPrev);
                int iValueByte = boost::lexical_cast<int> (sByte);
                if (iValueByte > -1 && iValueByte < 256)
                    iScrCheck++;
            }

            if (iScrCheck == 3)
                bRes = true;
        }
    }
    return bRes;
}

bool checkInterface(const std::string sValueInterface)
{
     bool bRes = false;
     std::string sDelim = ".";
     size_t sztPrev = 0;
     size_t sztNext;
     size_t sztDelta = sDelim.length();
     int iScrCheck = 0;
     while((sztNext = sValueInterface.find(sDelim, sztPrev)) != std::string::npos)
     {
         std::string sByte = sValueInterface.substr(sztPrev, sztNext-sztPrev);
         int iValueByte = boost::lexical_cast<int> (sByte);
         if (iValueByte > -1 && iValueByte < 256)
             iScrCheck++;

         sztPrev = sztNext + sztDelta;
     }

     if((sValueInterface.rfind(sDelim, sztPrev)) != std::string::npos)
     {
         std::string sByte = sValueInterface.substr(sztPrev, sztNext-sztPrev);
         int iValueByte = boost::lexical_cast<int> (sByte);
         if (iValueByte > -1 && iValueByte < 256)
             iScrCheck++;
     }

     if (iScrCheck == 4)
         bRes = true;

     return bRes;
}

bool setSockAddr(const std::string& sValueSockAddr, const std::string& sPath)
{
    bool bRes = false;
    if(checkSockAddr(sValueSockAddr))
    {
        registry::CXMLProxy xmlFile;
        if(xmlFile.load(sPath))
        {
            registry::CNode nodeRoot(&xmlFile);
            std::string sBeforeSockAddr;
            nodeRoot.getValue("SockAddr", sBeforeSockAddr);
            std::cout << "SockAddr: " << sBeforeSockAddr << " -> " << sValueSockAddr << std::endl;
            nodeRoot.setValue("SockAddr", sValueSockAddr);

            if(xmlFile.save(sPath))
                bRes = true;
            else
                std::cout << "ERR>> changes have not been saved" << std::endl;
        }
    }
    else
        std::cout << "SockAddr: ERR" << std::endl;

    return bRes;
}

bool setInterfaces(const std::vector<std::string>& sInterfaces, const std::string& sPath)
{
    bool bRes = false;
    registry::CXMLProxy xmlFile;
    if(xmlFile.load(sPath))
    {
        registry::CNode nodeRoot(&xmlFile);
        if(nodeRoot.isSubNode("ConfigGroups"))
        {
            registry::CNode nodeConfigGroups = nodeRoot.getSubNode("ConfigGroups");
            std::vector<std::string> sBeforeInterfaces;
            for (int i = 0; i < nodeConfigGroups.getSubNodeCount(); i++)
            {
                registry::CNode nodeItemInterfaces = nodeConfigGroups.getSubNode(i);
                nodeItemInterfaces.getValue("SendInterfaces", sBeforeInterfaces);
            }

            std::cout << "Interfaces: ";
            if(!sBeforeInterfaces.empty())
            {
                for (unsigned int i = 0; i < sBeforeInterfaces.size(); i++)
                {
                    const std::string sValueInterface = sBeforeInterfaces[i];
                    std::cout << sValueInterface << " ";
                }
            }
            else
                std::cout << "ERR ";

            std::vector<std::string> v_sNewInterfaces;
            for (unsigned int i = 0; i < sInterfaces.size(); i++)
            {
                const std::string sValueInterface = sInterfaces[i];
                if(checkInterface(sValueInterface))
                    v_sNewInterfaces.push_back(sValueInterface);
            }

            if(nodeConfigGroups.isSubNode("__Default"))
            {
                registry::CNode nodeItemDefault = nodeConfigGroups.getSubNode("__Default");
                nodeItemDefault.setValue("SendInterfaces", v_sNewInterfaces);
            }

            std::cout << "-> ";
            if(!v_sNewInterfaces.empty())
            {
                for (unsigned int i = 0; i < v_sNewInterfaces.size(); i++)
                {
                    const std::string sValueInterface = v_sNewInterfaces[i];
                    std::cout << sValueInterface << " ";
                }
            }
            else
                std::cout << "ERR";
            std::cout << std::endl;

            if(xmlFile.save(sPath))
                bRes = true;
            else
                std::cout << "ERR>> changes have not been saved" << std::endl;

        }
    }
    return bRes;
}

bool outputNet(const std::vector<std::string>& ip, registry::CNode& nodeRoot, boost::program_options::variables_map& vm)
{
    bool bRes = false;
    std::string sValueSockAddr;
    nodeRoot.getValue("SockAddr", sValueSockAddr);
    if (checkSockAddr(sValueSockAddr))
        std::cout << "SockAddr: " << sValueSockAddr << std::endl;

    if(vm.count("interfaces"))
        CheckNet(ip, nodeRoot);
    else
    {
        if (nodeRoot.isSubNode("ConfigGroups"))
        {
            registry::CNode nodeConfigGroups = nodeRoot.getSubNode("ConfigGroups");
            std::vector<std::string> vInterfaces;
            for (int i = 0; i < nodeConfigGroups.getSubNodeCount(); i++)
            {
                registry::CNode nodeItemInterfaces = nodeConfigGroups.getSubNode(i);
                nodeItemInterfaces.getValue("SendInterfaces", vInterfaces);
            }
            if(!vInterfaces.empty())
            {
                std::cout << "SendInterfaces (config): ";
                for (unsigned int i = 0; i < vInterfaces.size(); i++)
                {
                    const std::string& sInterfacesValue = vInterfaces[i];
                    std::cout << sInterfacesValue << " ";
                }
            }
            else
                 std::cout << "SendInterfaces (config): ERR";

            std::cout << std::endl;
        }
    }

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
                ("interfaces,m",
                 boost::program_options::value< std::vector<std::string> >()->multitoken(),
                 "show local interfaces")
                ("mcast",
                 boost::program_options::value<std::string>(),
                 "set SockAddr")
                ("ip",
                 boost::program_options::value< std::vector<std::string> >()->multitoken(),
                 "set Interfaces")
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
    lib.Load();
    lib.ExpandString(sPath);
    registry::CXMLProxy xmlFile;

    try
    {
        if(xmlFile.load(sPath))
        {
            std::string sClarifyingPath = "$(NITAETC)/[CFG]/_Shared/ip_st.xml";
            lib.ExpandString(sClarifyingPath);
            registry::CXMLProxy proxyOrg;
            if(proxyOrg.load(sClarifyingPath))
                registry::merge( &proxyOrg, proxyOrg.get_root(), &xmlFile, xmlFile.get_root() );

            registry::CNode nodeRoot(&xmlFile, "");
            boost::program_options::variables_map vm;
            std::vector<std::string> vInterfacesOptions;
            if(parseArgs(argc, argv, vm))
            {
                std::vector<std::string> ip;
                if(vm.count("interfaces"))
                {
                    vInterfacesOptions = vm["interfaces"].as<std::vector<std::string> >();
                    GetLocalInterfaces(ip, vInterfacesOptions);
                }
                else
                    GetAllLocalInterfaces(vInterfacesOptions);

                if (vm.count("mcast"))
                {
                    std::string sSockAddr = vm["mcast"].as<std::string>();
                    setSockAddr(sSockAddr, sClarifyingPath);
                }

                if(vm.count("ip"))
                {
                    std::vector<std::string> sInterfaces = vm["ip"].as<std::vector<std::string> >();
                    setInterfaces(sInterfaces, sClarifyingPath);
                }

                if(vm.count("net"))
                    outputNet(ip, nodeRoot, vm);

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
