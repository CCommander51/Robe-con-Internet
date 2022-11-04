
// Homework N.1 Telecomunicazioni 2022 | hw4
// Team N.27

/*
// ====== ATTENZIONE =================
//
// La corretta esecuzione del seguente codice dipende da una modifica nei scr file di ns-3.
// Nello specifico si richiede di svolgere la segente modifica:
//  1) spostarsi nella seguente directory:
//      /home/student/ns-3-dev-git/src/internet/model/global-route-manager-impl.cc
//  2) modificare all'interno del file " global-route-manager-impl.cc " le seguenti righe:
//       n. 325
//       n. 326
//     Procedendo al commento delle stesse, mediante " // "
//
// ===================================
*/

//------------------------------------
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
//------ CSMA ------------------------
#include "ns3/csma-module.h"
//------ Trace -----------------------
#include "ns3/object.h"
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
//------------------------------------

/*
// ============================================ Network Topology ============================================
//
//                                                      n4
//                                                    /    \
//                                  10.0.1.0/30   ___/      \___   10.0.4.0/30
//                                            ___/              \___
//                                           /                      \
//                       10.1.1.0/30        /                        \   
//  n0 --- n1 --- n2 ------------------- n3                            n6 ----------------- n7 -- n8
//   |     |      |     point-to-point      \                        /  |                   |     |
//  ================                         \___                ___/  ==============================
// LAN 192.148.1.0/24                            \___        ___/                  LAN 192.148.2.0/24
//                                  10.0.2.0/30      \      /      10.0.3.0/30
//      CSMA SX                                       \    /                        CSMA DX
//                                                      n5
//
// ==========================================================================================================
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1_Team_27");

int main(int argc, char* argv[]){

    int configuration;  // Scelta della configurazione da eseguire da CMD

    CommandLine cmd(__FILE__);
    cmd.AddValue("configuration", "Number of configuration type to run", configuration);    // Scelta della configurazione da eseguire da CMD
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);      //Riferito a NS_LOG_INFO

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("Creazione dei nodi e dei relativi container");        //STATUS LOG INFO LEVEL

    uint32_t nodesNum = 9;      // Numero di nodi totali

    NodeContainer allNodes;     //Contenitore di tutti i nodi della rete
    allNodes.Create(nodesNum);

    //------ CSMA SX -----------------------
   
    NodeContainer csmaSXNodes;  //Contenitore dei nodi CSMASX
    csmaSXNodes.Add(allNodes.Get(0));       //Nodo n0 --- Posizione 0
    csmaSXNodes.Add(allNodes.Get(1));       //Nodo n1 --- Posizione 1
    csmaSXNodes.Add(allNodes.Get(2));       //Nodo n2 --- Posizione 2

    CsmaHelper csmaLNKSX;   //Caratteristiche del collegamento
    csmaLNKSX.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csmaLNKSX.SetChannelAttribute("Delay", TimeValue(NanoSeconds(10000)));

    NetDeviceContainer csmaSXDevices;   //Contenitore finale con nodi collegati con link
    csmaSXDevices = csmaLNKSX.Install(csmaSXNodes);

    //------ CSMA DX -----------------------

    NodeContainer csmaDXNodes;  //Contenitore dei nodi CSMASX
    csmaDXNodes.Add(allNodes.Get(6));       //Nodo n6 --- Posizione 0
    csmaDXNodes.Add(allNodes.Get(7));       //Nodo n7 --- Posizione 1
    csmaDXNodes.Add(allNodes.Get(8));       //Nodo n8 --- Posizione 2

    CsmaHelper csmaLNKDX;   //Caratteristiche del collegamento
    csmaLNKDX.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csmaLNKDX.SetChannelAttribute("Delay", TimeValue(NanoSeconds(10000)));

    NetDeviceContainer csmaDXDevices;   //Contenitore finale con nodi collegati con link
    csmaDXDevices = csmaLNKDX.Install(csmaDXNodes);

    //------ n2-n3 LINK --------------------

    NodeContainer NC23;     //Creo un sottogruppo con n2 e n3
    NC23.Add(allNodes.Get(2));
    NC23.Add(allNodes.Get(3));

    PointToPointHelper PPP23;   //Caratteristiche del collegamento n2-n3
    PPP23.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    PPP23.SetChannelAttribute("Delay", StringValue("0.015ms"));

    NetDeviceContainer PPP23Devices;    //Contenitore finale con nodi collegati con link
    PPP23Devices = PPP23.Install(NC23);

    //------ n3-n4 LINK --------------------

    NodeContainer NC34;     //Creo un sottogruppo con n3 e n4
    NC34.Add(allNodes.Get(3));
    NC34.Add(allNodes.Get(4));

    PointToPointHelper PPP34;   //Caratteristiche del collegamento n3-n4
    PPP34.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    PPP34.SetChannelAttribute("Delay", StringValue("0.005ms"));

    NetDeviceContainer PPP34Devices;    //Contenitore finale con nodi collegati con link
    PPP34Devices = PPP34.Install(NC34);

    //------ n3-n5 LINK --------------------

    NodeContainer NC35;     //Creo un sottogruppo con n3 e n5
    NC35.Add(allNodes.Get(3));
    NC35.Add(allNodes.Get(5));

    PointToPointHelper PPP35;   //Caratteristiche del collegamento n3-n5
    PPP35.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    PPP35.SetChannelAttribute("Delay", StringValue("0.005ms"));

    NetDeviceContainer PPP35Devices;    //Contenitore finale con nodi collegati con link
    PPP35Devices = PPP35.Install(NC35);

    //------ n6-n4 LINK --------------------

    NodeContainer NC64;     //Creo un sottogruppo con n6 e n4
    NC64.Add(allNodes.Get(6));
    NC64.Add(allNodes.Get(4));

    PointToPointHelper PPP64;   //Caratteristiche del collegamento n6-n4
    PPP64.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    PPP64.SetChannelAttribute("Delay", StringValue("0.005ms"));

    NetDeviceContainer PPP64Devices;    //Contenitore finale con nodi collegati con link
    PPP64Devices = PPP64.Install(NC64);

    //------ n6-n5 LINK --------------------

    NodeContainer NC65;     //Creo un sottogruppo con n6 e n5
    NC65.Add(allNodes.Get(6));
    NC65.Add(allNodes.Get(5));

    PointToPointHelper PPP65;   //Caratteristiche del collegamento n6-n5
    PPP65.SetDeviceAttribute("DataRate", StringValue("80Mbps"));
    PPP65.SetChannelAttribute("Delay", StringValue("0.005ms"));

    NetDeviceContainer PPP65Devices;    //Contenitore finale con nodi collegati con link
    PPP65Devices = PPP65.Install(NC65);

    NS_LOG_INFO("Fine creazione topologia di rete");        //STATUS LOG INFO LEVEL

    ///////////////////////////////////////////////////////////////////////////////

    NS_LOG_INFO("Creazione del blocco di indirizzi IP per ogni container definito");        //STATUS LOG INFO LEVEL

    InternetStackHelper allStack;   //InternetStackHelper su tutti i nodi 
    allStack.Install(allNodes);

    //------ IP Address csmaSX -------------
    
    Ipv4AddressHelper ipAddCsmaSX;      //Definisco blocco di indirizzi
    ipAddCsmaSX.SetBase("192.148.1.0", "255.255.255.0");    //Struttura del blocco di indirizzi
    Ipv4InterfaceContainer csmaSXInterfaces;    //Definisco un container con devices e IP Set
    csmaSXInterfaces = ipAddCsmaSX.Assign(csmaSXDevices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address csmaDX -------------
    
    Ipv4AddressHelper ipAddCsmaDX;      //Definisco blocco di indirizzi
    ipAddCsmaDX.SetBase("192.148.2.0", "255.255.255.0");    //Struttura del blocco di indirizzi     
    Ipv4InterfaceContainer csmaDXInterfaces;    //Definisco un container con devices e IP Set
    csmaDXInterfaces = ipAddCsmaDX.Assign(csmaDXDevices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address n2-n3 LINK ---------

    Ipv4AddressHelper ipAddNd23;      //Definisco blocco di indirizzi
    ipAddNd23.SetBase("10.1.1.0", "255.255.255.252");    //Struttura del blocco di indirizzi    
    Ipv4InterfaceContainer nd23Interfaces;    //Definisco un container con devices e IP Set
    nd23Interfaces = ipAddNd23.Assign(PPP23Devices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address n3-n4 LINK ---l0----

    Ipv4AddressHelper ipAddNd34;      //Definisco blocco di indirizzi
    ipAddNd34.SetBase("10.0.1.0", "255.255.255.252");    //Struttura del blocco di indirizzi    
    Ipv4InterfaceContainer nd34Interfaces;    //Definisco un container con devices e IP Set
    nd34Interfaces = ipAddNd34.Assign(PPP34Devices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address n3-n5 LINK ---l1----

    Ipv4AddressHelper ipAddNd35;      //Definisco blocco di indirizzi
    ipAddNd35.SetBase("10.0.2.0", "255.255.255.252");    //Struttura del blocco di indirizzi    
    Ipv4InterfaceContainer nd35Interfaces;    //Definisco un container con devices e IP Set
    nd35Interfaces = ipAddNd35.Assign(PPP35Devices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address n6-n4 LINK ---l3----

    Ipv4AddressHelper ipAddNd64;      //Definisco blocco di indirizzi
    ipAddNd64.SetBase("10.0.4.0", "255.255.255.252");    //Struttura del blocco di indirizzi    
    Ipv4InterfaceContainer nd64Interfaces;    //Definisco un container con devices e IP Set
    nd64Interfaces = ipAddNd64.Assign(PPP64Devices);   //Assegno il blocco di indirizzi ai devices

    //------ IP Address n6-n5 LINK ---l2----

    Ipv4AddressHelper ipAddNd65;      //Definisco blocco di indirizzi
    ipAddNd65.SetBase("10.0.3.0", "255.255.255.252");    //Struttura del blocco di indirizzi    
    Ipv4InterfaceContainer nd65Interfaces;    //Definisco un container con devices e IP Set
    nd65Interfaces = ipAddNd65.Assign(PPP65Devices);   //Assegno il blocco di indirizzi ai devices

    NS_LOG_INFO("Fine definizione blocco di indirizzi IP");        //STATUS LOG INFO LEVEL

    ///////////////////////////////////////////////////////////////////////////////

    if(configuration == 0){

        NS_LOG_INFO("START");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 0");        //STATUS LOG INFO LEVEL

        uint32_t TportSinkN0 = 2600;     // TCP Sink Port n0

        //--------------------------------------  

        PacketSinkHelper sinkN0("ns3::TcpSocketFactory", InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0));    //Definisco TCP Sink su n0
        ApplicationContainer sinkAppsN0 = sinkN0.Install(csmaSXNodes.Get(0));   //PacketSink installato su n0
        sinkAppsN0.Start(Seconds(0.0));
        sinkAppsN0.Stop(Seconds(20.0));         //Tempo di run della simulazione

        //--------------------------------------  

        OnOffHelper onOffHelperN8("ns3::TcpSocketFactory", Address(InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0)));      //OnOffClient verso n0
        onOffHelperN8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperN8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelperN8.SetAttribute("PacketSize", UintegerValue(1500));
                
        ApplicationContainer onOffAppN8;    //OnOffClient installato su n8
        onOffAppN8.Add(onOffHelperN8.Install(csmaDXNodes.Get(2)));      
        onOffAppN8.Start(Seconds(3.0));
        onOffAppN8.Stop(Seconds(15.0));

        ///////////////////////////////////////////////////////////////////////////////

        LogComponentEnable("PacketSink", LOG_LEVEL_INFO);           //LOG abilitato per TCP PKSINK  (N0)
        LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);     //LOG abilitato per TCP ONOFFAPP    (N8)

        PPP23.EnablePcap("task1-0-n3.pcap", PPP23Devices.Get(1), true, true);           //Pcap su n3
        csmaLNKDX.EnablePcap("task1-0-n6.pcap", csmaDXDevices.Get(0), true, true);      //Pcap su n6
        
        csmaLNKSX.EnableAscii("task1-0-n0.tr",csmaSXDevices.Get(0),true);               //AsciiTracing su n0
        csmaLNKDX.EnableAscii("task1-0-n8.tr",csmaDXDevices.Get(2),true);               //AsciiTracing su n8

        NS_LOG_INFO("END");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 0");        //STATUS LOG INFO LEVEL
        
    }
    else if(configuration == 1){

        NS_LOG_INFO("START");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 1");        //STATUS LOG INFO LEVEL

        uint32_t TportSinkN0 = 2600;     // TCP Sink Port n0
        uint32_t TportSinkN7 = 7777;     // TCP Sink Port n7

        //--------------------------------------  

        PacketSinkHelper sinkN0("ns3::TcpSocketFactory", InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0));    //Definisco TCP Sink su n0
        ApplicationContainer sinkAppsN0 = sinkN0.Install(csmaSXNodes.Get(0));   //PacketSink installato su n0
        sinkAppsN0.Start(Seconds(0.0));
        sinkAppsN0.Stop(Seconds(20.0));     //Tempo di run della simulazione

        PacketSinkHelper sinkN7("ns3::TcpSocketFactory", InetSocketAddress(csmaDXInterfaces.GetAddress(1), TportSinkN7));    //Definisco TCP Sink su n7
        ApplicationContainer sinkAppsN7 = sinkN7.Install(csmaDXNodes.Get(1));   //PacketSink installato su n7
        sinkAppsN7.Start(Seconds(0.0));
        sinkAppsN7.Stop(Seconds(20.0));     //Tempo di run della simulazione

        //--------------------------------------
        
        OnOffHelper onOffHelperN8("ns3::TcpSocketFactory", Address(InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0)));      //OnOffClient verso n0
        onOffHelperN8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperN8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelperN8.SetAttribute("PacketSize", UintegerValue(2500));
                
        ApplicationContainer onOffAppN8;    //OnOffClient installato su n8
        onOffAppN8.Add(onOffHelperN8.Install(csmaDXNodes.Get(2)));      
        onOffAppN8.Start(Seconds(5.0));
        onOffAppN8.Stop(Seconds(15.0));

        //-------------------------------------- 

        OnOffHelper onOffHelperN1("ns3::TcpSocketFactory", Address(InetSocketAddress(csmaDXInterfaces.GetAddress(1), TportSinkN7)));      //OnOffClient TCP verso n7
        onOffHelperN1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperN1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelperN1.SetAttribute("PacketSize", UintegerValue(5000));
                
        ApplicationContainer onOffAppN1;    //OnOffClient installato su n1
        onOffAppN1.Add(onOffHelperN1.Install(csmaSXNodes.Get(1)));     
        onOffAppN1.Start(Seconds(2.0));
        onOffAppN1.Stop(Seconds(9.0));

        ///////////////////////////////////////////////////////////////////////////////

        LogComponentEnable("PacketSink", LOG_LEVEL_INFO);           //LOG abilitato per TCP PKSINK  (N0, N7)
        LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);     //LOG abilitato per TCP ONOFFAPP    (N8, N1)

        PPP23.EnablePcap("task1-1-n3.pcap", PPP23Devices.Get(1), true, true);           //Pcap su n3
        csmaLNKDX.EnablePcap("task1-1-n6.pcap", csmaDXDevices.Get(0), true, true);      //Pcap su n6
        
        csmaLNKSX.EnableAscii("task1-1-n0.tr",csmaSXDevices.Get(0),true);               //AsciiTracing su n0
        csmaLNKSX.EnableAscii("task1-1-n1.tr",csmaSXDevices.Get(1),true);               //AsciiTracing su n1
        csmaLNKDX.EnableAscii("task1-1-n7.tr",csmaDXDevices.Get(1),true);               //AsciiTracing su n7
        csmaLNKDX.EnableAscii("task1-1-n8.tr",csmaDXDevices.Get(2),true);               //AsciiTracing su n8

        NS_LOG_INFO("END");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 0");        //STATUS LOG INFO LEVEL

    }
    else if(configuration == 2){

        NS_LOG_INFO("START");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 2");        //STATUS LOG INFO LEVEL

        uint32_t UportSrvN2 = 63;           // UDP Echo Server Port n2
        uint32_t TportSinkN0 = 2600;        // TCP Sink Port n0
        uint32_t UportSinkN7 = 2500;        // UCP Sink Port n0

        //--------------------------------------

        uint32_t pkSizeN8 = 2560;
        uint8_t mat[] = {'0', 'x', '5', '8', 'c', 'e', '8', '3'};     //Somma delle matricole in esadecimale
        uint32_t matSize = sizeof(mat);

        //-------------------------------------- 

        UdpEchoServerHelper echoServerN2(UportSrvN2);     // UDP Echo Server

        ApplicationContainer srvAppN2 = echoServerN2.Install(csmaSXNodes.Get(2));     // UDP Echo Server installato su n2
        srvAppN2.Start(Seconds(0.0));
        srvAppN2.Stop(Seconds(20.0));       //Tempo di run della simulazione

        //--------------------------------------  

        UdpEchoClientHelper echoClientN8(csmaSXInterfaces.GetAddress(2), UportSrvN2);    // UDP Echo Client verso n2
        echoClientN8.SetAttribute("MaxPackets", UintegerValue(1));
        echoClientN8.SetAttribute("Interval", TimeValue(Seconds(2.0)));
        echoClientN8.SetAttribute("PacketSize", UintegerValue(2560));

        ApplicationContainer cltAppN8 = echoClientN8.Install(allNodes.Get(8));     // UDP Echo Client installato su n8
        cltAppN8.Start(Seconds(3.0));
        cltAppN8.Stop(Seconds(12.99));
        
        echoClientN8.SetFill(cltAppN8.Get(0), mat, matSize, pkSizeN8);      //Payload Pacchetti UDP
        
        //--------------------------------------  

        PacketSinkHelper sinkN0("ns3::TcpSocketFactory", InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0));    //Definisco TCP Sink su n0
        ApplicationContainer sinkAppsN0 = sinkN0.Install(csmaSXNodes.Get(0));   //PacketSink installato su n0
        sinkAppsN0.Start(Seconds(0.0));
        sinkAppsN0.Stop(Seconds(20.0));     //Tempo di run della simulazione

        PacketSinkHelper sinkN7("ns3::UdpSocketFactory", InetSocketAddress(csmaDXInterfaces.GetAddress(1), UportSinkN7));    //Definisco UDP Sink su n7
        ApplicationContainer sinkAppsN7 = sinkN7.Install(csmaDXNodes.Get(1));   //PacketSink installato su n7
        sinkAppsN7.Start(Seconds(0.0));
        sinkAppsN7.Stop(Seconds(20.0));     //Tempo di run della simulazione

        //--------------------------------------
        
        OnOffHelper onOffHelperTN8("ns3::TcpSocketFactory", Address(InetSocketAddress(csmaSXInterfaces.GetAddress(0), TportSinkN0)));      //OnOffClient TCP verso n0
        onOffHelperTN8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperTN8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelperTN8.SetAttribute("PacketSize", UintegerValue(3000));
                
        ApplicationContainer onOffAppTN8;    //OnOffClient TCP installato su n8
        onOffAppTN8.Add(onOffHelperTN8.Install(csmaDXNodes.Get(2)));      
        onOffAppTN8.Start(Seconds(3.0));
        onOffAppTN8.Stop(Seconds(9.0));    

        //--------------------------------------
        
        OnOffHelper onOffHelperUN8("ns3::UdpSocketFactory", Address(InetSocketAddress(csmaDXInterfaces.GetAddress(1), UportSinkN7)));      //OnOffClient UDP verso n7
        onOffHelperUN8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperUN8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        onOffHelperUN8.SetAttribute("PacketSize", UintegerValue(3000));
                
        ApplicationContainer onOffAppUN8;    //OnOffClient UDP installato su n8
        onOffAppUN8.Add(onOffHelperUN8.Install(csmaDXNodes.Get(2)));      
        onOffAppUN8.Start(Seconds(5.0));
        onOffAppUN8.Stop(Seconds(15.0));

        ///////////////////////////////////////////////////////////////////////////////

        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);     //LOG abilitato per UDP SERVER (N2)
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);     //LOG abilitato per UDP CLIENT (N8)
        LogComponentEnable("PacketSink", LOG_LEVEL_INFO);       //LOG abilitato per TCP PKSINK (N0) & UDP PKSINK (N7)
        LogComponentEnable("OnOffApplication", LOG_LEVEL_INFO);     //LOG abilitato per TCP ONOFFAPP (N8) & UDP ONOFFAPP (N8)

        PPP23.EnablePcap("task1-2-n3.pcap", PPP23Devices.Get(1), true, true);           //Pcap su n3
        csmaLNKDX.EnablePcap("task1-2-n6.pcap", csmaDXDevices.Get(0), true, true);      //Pcap su n6

        csmaLNKSX.EnableAscii("task1-2-n0.tr",csmaSXDevices.Get(0),true);               //AsciiTracing su n0
        csmaLNKSX.EnableAscii("task1-2-n2.tr",csmaSXDevices.Get(2),true);               //AsciiTracing su n2
        csmaLNKDX.EnableAscii("task1-2-n7.tr",csmaDXDevices.Get(1),true);               //AsciiTracing su n7
        csmaLNKDX.EnableAscii("task1-2-n8.tr",csmaDXDevices.Get(0),true);               //AsciiTracing su n8

        NS_LOG_INFO("END");        //STATUS LOG INFO LEVEL
        NS_LOG_INFO("Configuration 2");        //STATUS LOG INFO LEVEL

    }

    ///////////////////////////////////////////////////////////////////////////////

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
