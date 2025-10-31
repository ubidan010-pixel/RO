using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Xml;
using System.Xml.Serialization;
using System.Net;

namespace dbCommon
{
    
    public class SettingsManager
    {
        static public readonly int DefaultPort = 3456;

        static public Settings open(string filename)
            {
                if (File.Exists(filename))
                {
                    XmlSerializer s = new XmlSerializer(typeof(Settings));
                    TextReader r = new StreamReader(filename);
                    Settings settings = (Settings)s.Deserialize(r);
                    r.Close();
                    return settings;
                }

                //default settings
                return new Settings();
            }

            static public bool save(Settings settings, string filename)
            {
                XmlSerializer s = new XmlSerializer(typeof(Settings));
                TextWriter w = new StreamWriter(filename);
                s.Serialize(w, settings);
                w.Close();
                return true;
            }
    }
   

    [XmlRoot("Settings")]
    public class Settings
    {

        [XmlAttribute("Host")]
        public System.String Host
        {
            set {host = value;}
            get {return host;}
        }

        [XmlAttribute("PortName")]
        public int Port
        {
            set {port = value;}
            get {return port;}
        }

        [XmlAttribute("AutoStart")]
        public bool AutoStart
        {
            set { autostart = value; }
            get { return autostart; }
        }


        //The server start at startup application
        private bool autostart = false;
        //Default IP
        private System.String host  = "127.0.0.1";
        //Default Port
        private int port = SettingsManager.DefaultPort;

        public IPAddress getIPAddress()
        {
            string sHost = Host.ToString();
            if (sHost.Length > 0 && sHost.ToLower() != "local")
                return IPAddress.Parse(sHost);
            else
                return System.Net.IPAddress.Loopback;
        }
    }
}