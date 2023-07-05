using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class MyZEDStreamingClient : MonoBehaviour
{
    UdpClient clientData;

    IPEndPoint ipEndPointData;

    public bool useMulticast = true;

    public int port = 20000;
    public string multicastIpAddress = "230.0.0.1";

    public bool showZEDFusionMetrics = false;

    private object obj = null;
    private System.AsyncCallback AC;
    byte[] receivedBytes;

    int bufferSize = 10;
    LinkedList<byte[]> receivedDataBuffer;

    bool newDataAvailable = false;
    sl.DetectionData data;

    public delegate void onNewDetectionTriggerDelegate(sl.Bodies bodies);
    public event onNewDetectionTriggerDelegate OnNewDetection;

    public delegate void onPerformanceHandlerDelegate(int id, string sn, string fps);
    public event onPerformanceHandlerDelegate OnPerformance;

    public delegate void onFusionPerformanceHandlerDelegate(string fps);
    public event onFusionPerformanceHandlerDelegate OnFusionPerformance;

    private float startTime;

    void Start()
    {
        startTime = Time.time;
        InitializeUDPListener();
    }
    public void InitializeUDPListener()
    {
        receivedDataBuffer = new LinkedList<byte[]>();

        ipEndPointData = new IPEndPoint(IPAddress.Any, port);
        clientData = new UdpClient();
        clientData.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, optionValue: true);
        clientData.ExclusiveAddressUse = false;
        clientData.EnableBroadcast = false;

        if (useMulticast)
        {
            clientData.JoinMulticastGroup(IPAddress.Parse(multicastIpAddress));

        }
        clientData.Client.Bind(ipEndPointData);

        clientData.DontFragment = true;
        AC = new System.AsyncCallback(ReceivedUDPPacket);
        clientData.BeginReceive(AC, obj);
        Debug.Log("UDP - Start Receiving..");
    }

    void ReceivedUDPPacket(System.IAsyncResult result)
    {
        //stopwatch.Start();
        receivedBytes = clientData.EndReceive(result, ref ipEndPointData);
        ParsePacket();
        clientData.BeginReceive(AC, obj);
    } // ReceiveCallBack

    void ParsePacket()
    {

        if (receivedDataBuffer.Count == bufferSize) receivedDataBuffer.RemoveFirst();
        receivedDataBuffer.AddLast(receivedBytes);
        newDataAvailable = true;
    }

    public bool IsNewDataAvailable()
    {
        return newDataAvailable;
    }

    public sl.Bodies GetLastBodiesData()
    {
        data = sl.DetectionData.CreateFromJSON(receivedDataBuffer.Last.Value);

        return data.bodies;
    }

    public sl.FusionMetrics GetLastFusionMetrics()
    {
        return data.fusionMetrics;
    }

    public bool ShowFusionMetrics()
    {
        return showZEDFusionMetrics && data.fusionMetrics != null;
    }


    private void Update()
    {
        if (IsNewDataAvailable())
        {
            OnNewDetection(GetLastBodiesData());
            newDataAvailable = false;

            float duration = Time.time - startTime;
            float fusion_fps = 1.0f / duration;
            startTime = Time.time;
            OnFusionPerformance(fusion_fps.ToString("0.000"));

            sl.FusionMetrics metrics;

            if(data.fusionMetrics != null) {
                metrics = GetLastFusionMetrics();
                int id = 0;
                foreach (var camera in metrics.camera_individual_stats)
                {
                    OnPerformance(id, camera.sn.ToString(), camera.received_fps.ToString("0.000"));
                    id++;
                }
            }

            if (ShowFusionMetrics())
            {
                metrics = GetLastFusionMetrics();
                string tmpdbg = "";
                foreach (var camera in metrics.camera_individual_stats)
                {
                    tmpdbg += "SN : " + camera.sn + " Synced Latency: " + camera.synced_latency + "FPS : " + camera.received_fps + "\n";
                }
                Debug.Log(tmpdbg);
            }
        }
    }

    void OnDestroy()
    {
        if (clientData != null)
        {
            Debug.Log("Stop receiving ..");
            clientData.Close();
        }
    }
}