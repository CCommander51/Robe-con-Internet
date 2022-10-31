
// Homework N.1 Telecomunicazioni 2022 | hw4
// Team N.27

// LINK PER CAPIRE IL "/24 E /31" NEGLI INDIRIZZI IP ---> https://it.wikipedia.org/wiki/Maschera_di_sottorete

// DEVONO ESSERE INSERITI GLI INCLUDE ADEGUATI, SUCCESSIVAMENTE ELIMINARE QUESTA RIGA DI COMMENTO PRIMA DELLA CONSEGNA

//------ DA RIVEDERE -----------------
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
//------------------------------------

//------ CSMA ------------------------
#include "ns3/csma-module.h"
//------------------------------------

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
//                                                    \    /
//                                                      n5
//
// ==========================================================================================================
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task_1_Team_27");

int
main(int argc, char* argv[])
{
    int configuration;  // Scelta della configurazione da eseguire da CMD | ns-3-tutorial P.68

    CommandLine cmd(__FILE__);
    cmd.AddValue("configuration", "Number of configuration type to run", configuration);    // Scelta della configurazione da eseguire da CMD | ns-3-tutorial P.68
    cmd.Parse(argc, argv);

    ///////////////////////////////////////////////////////////////////////////////

    uint32_t nodesNum = 9;
    //uint32_t nCsmaSX = 2;
    //uint32_t nCsmaDX = 2;

    NodeContainer allNodes;
    allNodes.Create(nodesNum);

    //------ CSMA SX -----------------------
   
    NodeContainer csmaSXNodes;  //Contenitore dei nodi CSMA
    csmaSXNodes.Add(allNodes.Get(0));
    csmaSXNodes.Add(allNodes.Get(1));
    csmaSXNodes.Add(allNodes.Get(2));     //Nodo n2 in testa al csmaSX, responsabile del csmaSX

    CsmaHelper csmaLNKSX;   //Caratteristiche del collegamento
    csmaLNKSX.SetChannelAttribute("DataRate", StringValue("25Mbps"));
    csmaLNKSX.SetChannelAttribute("Delay", TimeValue(NanoSeconds(10000)));

    NetDeviceContainer csmaSXDevices;   //Contenitore finale con nodi collegati con link
    csmaSXDevices = csmaLNKSX.Install(csmaSXNodes);

    //------ CSMA DX -----------------------

    NodeContainer csmaDXNodes;
    csmaDXNodes.Add(allNodes.Get(8));
    csmaDXNodes.Add(allNodes.Get(7));
    csmaDXNodes.Add(allNodes.Get(6));     //Nodo n6 in testa al csmaDX, responsabile del csmaDX

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

    //NON SAPENDO SE FARE n3-n4-n5-n6 COME COLLEGAMENTO A STELLA OPPURE COME SINGOLI, HO SCELTO LA SECONDA

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

    ///////////////////////////////////////////////////////////////////////////////

    InternetStackHelper allStack;
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

    ///////////////////////////////////////////////////////////////////////////////

    //Fino a qui sono solo stati creati i vari devices della rete, i canali di comunicazione con
    //relative specifiche, raggruppati i vari nodi ed assegnati gli  indirizzi ip.
    //Adesso bisogna inserire un protocollo di comunicazione, le applicazioni per la generazione 
    //del traffico di dati, definire client e server ed i Trace :)

    if(configuration == 0){

        uint32_t portSinkN0 = 2600;

        PacketSinkHelper sinkN0("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), portSinkN0));
        ApplicationContainer sinkAppsN0 = sinkN0.Install(allNodes.Get(0));
        sinkAppsN0.Start(Seconds(0.0));
        sinkAppsN0.Stop(Seconds(20.0));

        OnOffHelper onOffHelperN8("ns3::TcpSocketFactory", Address(InetSocketAddress(csmaDXInterfaces.GetAddress(2),portSinkN0)));
        onOffHelperN8.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        onOffHelperN8.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        //onOffHelperN8.SetAttribute("OnTime", TimeValue(NanoSeconds(3000)));
        //onOffHelperN8.SetAttribute("OffTime", TimeValue(NanoSeconds(15000)));
        onOffHelperN8.SetAttribute("PacketSize", UintegerValue(1500));        

        ApplicationContainer onOffAppN8 = onOffHelperN8.Install(allNodes.Get(8));
        onOffAppN8.Start(Seconds(3.0));
        onOffAppN8.Stop(Seconds(15.0));








        ///////////////////////////////////////////////////////////////////////////////

        PPP34.EnablePcap("task1-0-n3.pcap", PPP34Devices.Get(0), true);  // Pcap n3
        csmaLNKDX.EnablePcap("task1-0-n6.pcap", csmaDXDevices.Get(0), true);    // Pcap n6

    }
    else if(configuration == 1){









        ///////////////////////////////////////////////////////////////////////////////

        PPP34.EnablePcap("task1-1-n3.pcap", PPP34Devices.Get(0), true);  // Pcap n3
        csmaLNKDX.EnablePcap("task1-1-n6.pcap", csmaDXDevices.Get(0), true);    // Pcap n6


    }
    else if(configuration == 2){









        ///////////////////////////////////////////////////////////////////////////////

        PPP34.EnablePcap("task1-2-n3.pcap", PPP34Devices.Get(0), true);  // Pcap n3
        csmaLNKDX.EnablePcap("task1-2-n6.pcap", csmaDXDevices.Get(0), true);    // Pcap n6


    }











    ///////////////////////////////////////////////////////////////////////////////

    //AsciiTraceHelper myTracASCII;

    //SI ATTENDE LA CREAZIONE DI SERVER E CLIENT PER ABILITARE L'ASCII TRACING

    //TCP-SERVER.EnableAsciiAll(myTracASCII.CreateFileStream ("task1"-configuration-<id_del_nodo>".tr"));   // ns-3-manual | P.62
    //UDP-SERVER.EnableAsciiAll(myTracASCII.CreateFileStream ("task1"-configuration-<id_del_nodo>".tr"));

    //SI ATTENDE LA CREAZIONE DEI NODI PER ABILITARE IL PCAP TRACING, BISOGNA VEDERE COSA METTERE AL POSTO DI "NODE-TYPE"

    //PPP34.EnablePcap("task1-0-n3.pcap", PPP34Devices.Get(0), true);  // ns-3-manual | P.60
    //csmaLNKDX.EnablePcap("task1-0-n6.pcap", csmaDXDevices.Get(0), true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
