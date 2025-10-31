using System;
using System.Text;
using System.Net;

namespace dbCommon
{
    public class Network
    {
        static public IPHostEntry DoGetHostEntry(string hostname)
        {
            IPHostEntry host;

            host = Dns.GetHostEntry(hostname);
            return host;
        }

    }
}
