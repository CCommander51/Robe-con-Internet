// Homework N.2 Telecomunicazioni 2022 | hwUNICO
// Team N.27

//------------------------------------
#include "ns3/applications-module.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/gnuplot.h"
#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
//------ RNG -------------------------
// #include "rng-seed-manager.h"
// Il parametro RngRun viene passato da cmd ( --RngRun=<..>)
// come indicato da una risposta per email, ad RngRun si deve
// passare la somma delle matricole dei componenti del gruppo;
// per questo gruppo il parametro risulta:
// 1938802 + 1959164 + 1922069 = 5820035    -->     --RngRun=5820035
//------ WiFi ------------------------
#include "ns3/mobility-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
// #include "wifi-phy-standard.h"
//------ NetAnim ---------------------
#include "ns3/netanim-module.h"
//------ Trace -----------------------
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
//------------------------------------

/*
// ============================================ Network Topology ============================================
//
//                                               __ n0 __
//                                           __ /   |    \__                 AP MODE                                     
//                                          /       |       \                                               
//                                       n4 ------- n5 ----- n1                                              
//                                         \     /     \     /                                              
//                                          \  /        \   /                                              
//                                           n3 _________ n2                                                 
//                                                                                                          
//                                           192.168.0.1/24                                                            
//                                                                                                                                                                                                                  
// ==========================================================================================================
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("HW2_Task_2_Team_27");      // AP MODE

int main(int argc, char* argv[]){

    bool useRtsCts = false;  // Forza l'utilizzo dell'handshake RTS/CTS
    bool verbose = false; // Abilita l'uso dei LOG (SRV e CLT) per UDP application
    bool useNetAnim = false; // Genera file per l'analisi su NetAnim
    std::string ssid; // Parametro SSID AP

    CommandLine cmd(__FILE__);
    cmd.AddValue("useRtsCts", "Force the use of Rts and Cts", useRtsCts);    // Scelta di useRtsCts da CMD
    cmd.AddValue("verbose", "Enable the use of Logs on SRV and CLI", verbose);    // Scelta di verbose da CMD
    cmd.AddValue("useNetAnim", "Enable file generation for NetAnim", useNetAnim);    // Scelta di useNetAnim da CMD
    cmd.AddValue("ssid", "Enable file generation for NetAnim", ssid);    // Parametro SSID AP

    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);      //Riferito a NS_LOG_INFO

    ///////////////////////////////////////////////////////////////////////////////

    UintegerValue ctsThreshold = (useRtsCts ? UintegerValue(100) : UintegerValue(2346)) ;                              
    Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThreshold);

    ///////////////////////////////////////////////////////////////////////////////

    if(ssid == ""){

        ssid = "TLC2022";       // Impostazione parametro di default a ssid (se non indicato da cmd)

    }

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("Creazione dei nodi e dei relativi container");        //STATUS LOG INFO LEVEL

    uint32_t nodesNum = 6;      // Numero di nodi totali

    NodeContainer allWifiApModNodes;     // Contenitore di tutti i nodi della rete WiFi AP
    allWifiApModNodes.Create(nodesNum);

    NodeContainer apWifiApModNodes;     // Contenitore nodi AP della rete WiFi AP
    apWifiApModNodes.Add(allWifiApModNodes.Get(5));     // Nodo n5 AP

    NodeContainer staWifiApModNodes;     // Contenitore nodi STA della rete WiFi AP
    for (uint32_t i = 0; i < (nodesNum - 1); ++i){

        staWifiApModNodes.Add(allWifiApModNodes.Get(i));    // Nodo n-iesimo STA
        
    }

    YansWifiChannelHelper channelApMod = YansWifiChannelHelper::Default();   // Definizione canale di comunicazione tra i nodi
    YansWifiPhyHelper phyApMod;   // Definizione physical layer tra i nodi
    phyApMod.SetChannel(channelApMod.Create());

    WifiHelper wifiApMod;    // Helper per creare ed installare WiFi devices
    wifiApMod.SetStandard(WifiStandard(WIFI_STANDARD_80211g)); // Definizione di standard da usare, andrà a sovrascrivere i valori di default impostati precedentemente
    wifiApMod.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper macApMod;
    Ssid ssidAp = Ssid(ssid);       // Impostazione SSID

    macApMod.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidAp), "ActiveProbing", BooleanValue(false));  // Definizione modalità operativa per STA

    NetDeviceContainer staApModDevices;      // Contenitore finale con nodi STA collegati con link
    staApModDevices = wifiApMod.Install(phyApMod, macApMod, staWifiApModNodes);

    macApMod.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidAp));    // Definizione modalità operativa per AP (side-effect)

    NetDeviceContainer apApModDevices;
    apApModDevices = wifiApMod.Install(phyApMod, macApMod, apWifiApModNodes);

    NS_LOG_INFO("Fine creazione topologia di rete");        //STATUS LOG INFO LEVEL

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("Creazione del mobility model");

    MobilityHelper mobilityApMod;

    mobilityApMod.SetPositionAllocator(
        "ns3::GridPositionAllocator",       // Allocate positions on a rectangular 2d grid
        "MinX", DoubleValue(0.0),           // The x coordinate where the grid starts
        "MinY", DoubleValue(0.0),           // The y coordinate where the grid starts
        "DeltaX", DoubleValue(5.0),         // The x space between objects
        "DeltaY", DoubleValue(10.0),        // The y space between objects
        "GridWidth", UintegerValue(3),      // The number of objects layed out on a line
        "LayoutType", StringValue("RowFirst")   // The type of layout
    );

    mobilityApMod.SetMobilityModel(
        "ns3::RandomWalk2dMobilityModel",
        "Bounds", RectangleValue(Rectangle(-90, 90, -90, 90))   // xMin, xMax, yMin, yMax
    );

    mobilityApMod.Install(staWifiApModNodes);   //Installazione mobility model su STA nodes

    mobilityApMod.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityApMod.Install(apWifiApModNodes);   //Installazione mobility model su AP node

    AnimationInterface::SetConstantPosition(apWifiApModNodes.Get(0), 5, 5);   // Assegnazione posizione statica ad AP node

    NS_LOG_INFO("Fine creazione del mobility model");

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("Creazione del blocco di indirizzi IP per ogni container definito");

    InternetStackHelper allstack;   //InternetStackHelper su tutti i nodi 
    allstack.Install(allWifiApModNodes);

    //------ IP Address -------------
    
    Ipv4AddressHelper ipAddApModNodes;      //Definisco blocco di indirizzi
    ipAddApModNodes.SetBase("192.168.1.0", "255.255.255.0");    //Struttura del blocco di indirizzi

    Ipv4InterfaceContainer staApModNodesInterfaces;    //Definisco un container con STA devices e IP Set
    staApModNodesInterfaces = ipAddApModNodes.Assign(staApModDevices);   //Assegno il blocco di indirizzi a STA devices

    Ipv4InterfaceContainer apApModNodesInterfaces;    //Definisco un container con AP devices e IP Set
    apApModNodesInterfaces = ipAddApModNodes.Assign(apApModDevices);   //Assegno il blocco di indirizzi a AP devices

    NS_LOG_INFO("Fine definizione blocco di indirizzi IP");        //STATUS LOG INFO LEVEL

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("START");        //STATUS LOG INFO LEVEL

    uint32_t UportSrvN0 = 21;       // UDP Echo Server Port n0
    uint32_t pkSize = 512;          // Packet size UDP Client

    //--------------------------------------

    UdpEchoServerHelper echoServerN0(UportSrvN0);     // UDP Echo Server
    ApplicationContainer srvAppN0 = echoServerN0.Install(staWifiApModNodes.Get(0));     // UDP Echo Server installato su n0
    srvAppN0.Start(Seconds(0.0));
    srvAppN0.Stop(Seconds(10.0));       //Tempo di run del server
    
    //--------------------------------------  

    UdpEchoClientHelper echoClientN3(staApModNodesInterfaces.GetAddress(0), UportSrvN0);    // UDP Echo Client verso n0
    echoClientN3.SetAttribute("MaxPackets", UintegerValue(2));
    echoClientN3.SetAttribute("Interval", TimeValue(Seconds(2.0)));
    echoClientN3.SetAttribute("PacketSize", UintegerValue(pkSize));

    ApplicationContainer cltAppN3 = echoClientN3.Install(staWifiApModNodes.Get(3));     // UDP Echo Client installato su n3
    cltAppN3.Start(Seconds(2.0));
    cltAppN3.Stop(Seconds(4.5));

    //--------------------------------------  

    UdpEchoClientHelper echoClientN4(staApModNodesInterfaces.GetAddress(0), UportSrvN0);    // UDP Echo Client verso n0
    echoClientN4.SetAttribute("MaxPackets", UintegerValue(2));
    echoClientN4.SetAttribute("Interval", TimeValue(Seconds(3.0)));
    echoClientN4.SetAttribute("PacketSize", UintegerValue(pkSize));

    ApplicationContainer cltAppN4 = echoClientN4.Install(staWifiApModNodes.Get(4));     // UDP Echo Client installato su n4
    cltAppN4.Start(Seconds(1.0));
    cltAppN4.Stop(Seconds(4.5));

    ///////////////////////////////////////////////////////////////////////////////

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    ///////////////////////////////////////////////////////////////////////////////

    if(verbose){
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);     //LOG abilitato per UDP SERVER (n0)
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);     //LOG abilitato per UDP CLIENT (n3, n4)
    }

    // phyApMod.EnablePcap("task2-off(on)-n4.pcap", staApModDevices.Get(4), true, true);           //Pcap su n4 [ task(1|2)-<state>-<id_del_nodo>.<formato_file_richiesto> ]

    NS_LOG_INFO("END");        //STATUS LOG INFO LEVEL

    ///////////////////////////////////////////////////////////////////////////////

    if(useRtsCts){

        phyApMod.EnablePcap("task2-on-n4.pcap", staApModDevices.Get(4), true, true);           //Pcap su n4 [ task(1|2)-<state>-<id_del_nodo>.<formato_file_richiesto> ]
        phyApMod.EnablePcap("task2-on-n5.pcap", apApModDevices.Get(0), true, true);           //Pcap su n5 [ task(1|2)-<state>-<id_del_nodo>.<formato_file_richiesto> ]


    }

    else{

        phyApMod.EnablePcap("task2-off-n4.pcap", staApModDevices.Get(4), true, true);           //Pcap su n4 [ task(1|2)-<state>-<id_del_nodo>.<formato_file_richiesto> ]
        phyApMod.EnablePcap("task2-off-n5.pcap", apApModDevices.Get(0), true, true);           //Pcap su n5 [ task(1|2)-<state>-<id_del_nodo>.<formato_file_richiesto> ]


    }

    ///////////////////////////////////////////////////////////////////////////////

    if(useNetAnim){              

        if(useRtsCts){

            AnimationInterface netAnimApMode ("wireless-task2-rts-on.xml");

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(0), "SRV-0"); // Nodo n0 SRV           --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(0), 255, 0, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(1), "STA-1"); // Nodo n1               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(1), 0, 0, 255);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(2), "STA-2"); // Nodo n2               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(2), 0, 0, 255);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(3), "CLI-3"); // Nodo n3 CLI               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(3), 0, 255, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(4), "CLI-4"); // Nodo n4 CLI & PCAP    --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(4), 0, 255, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(5), "AP-5"); // Nodo n5 AP            --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(5), 66, 49, 137);   // R, G, B
        
            netAnimApMode.EnablePacketMetadata();    // Packet Metadata

            netAnimApMode.EnableWifiMacCounters(Seconds(0), Seconds(10)); // Tracing MAC
            netAnimApMode.EnableWifiPhyCounters(Seconds(0), Seconds(10)); // Tracing PHY

            // Ipv4GlobalRoutingHelper::PopulateRoutingTables();
            Simulator::Stop(Seconds(12.0));

            Simulator::Run();
            Simulator::Destroy();

            return 0;

        }

        else{
        
            AnimationInterface netAnimApMode ("wireless-task2-rts-off.xml");

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(0), "SRV-0"); // Nodo n0 SRV           --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(0), 255, 0, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(1), "STA-1"); // Nodo n1               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(1), 0, 0, 255);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(2), "STA-2"); // Nodo n2               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(2), 0, 0, 255);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(3), "CLI-3"); // Nodo n3 CLI               --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(3), 0, 255, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(4), "CLI-4"); // Nodo n4 CLI & PCAP    --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(4), 0, 255, 0);   // R, G, B

            netAnimApMode.UpdateNodeDescription(allWifiApModNodes.Get(5), "AP-5"); // Nodo n5 AP            --- STA
            netAnimApMode.UpdateNodeColor(allWifiApModNodes.Get(5), 66, 49, 137);   // R, G, B
        
            netAnimApMode.EnablePacketMetadata();    // Packet Metadata

            netAnimApMode.EnableWifiMacCounters(Seconds(0), Seconds(10)); // Tracing MAC
            netAnimApMode.EnableWifiPhyCounters(Seconds(0), Seconds(10)); // Tracing PHY


            // Ipv4GlobalRoutingHelper::PopulateRoutingTables();
            Simulator::Stop(Seconds(12.0));

            Simulator::Run();
            Simulator::Destroy();

            return 0;
        }
    
    }

    else{

        // Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        Simulator::Stop(Seconds(12.0));

        Simulator::Run();
        Simulator::Destroy();

        return 0;
        
    }    

}
 
