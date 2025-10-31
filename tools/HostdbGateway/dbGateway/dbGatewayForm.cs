using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Net;
using System.Windows.Forms;
using NetworkServer;
using NetworkClient;
using ForemanDP;
using dbCommon;

namespace dbGateway
{
    public partial class dbGatewayForm : Form
    {
        private readonly NetworkServerBase databankServer =
           new NetworkServerBase(new TimeSpan(1000));

        private Settings settingsApp = new Settings();

        public IPHostEntry localhost;

        public string settingsFile;

        protected LogRecorder logRecorder = null;
        protected LogForm logForm = null;
        protected UsersView userForm = null;

               
        public void OnLogNewLine(string newline)
        {
            logForm.OnLogNewLine(newline);
        }
        
        public dbGatewayForm()
        {
            InitializeComponent();
            databankServer.OnServerNotify += databankServer_OnServerNotify;


            localhost = Network.DoGetHostEntry("");

            string text = localhost.HostName;
            if (localhost.AddressList.Length > 0)
                text += " -- IP:" + localhost.AddressList[0].ToString();
            labelInfo.Text = text;
            settingsFile = System.IO.Path.GetDirectoryName(Application.ExecutablePath) + @"\settings.xml";
            Settings settings = SettingsManager.open(settingsFile);
            OnSettingsChanged(settings);

            if (settingsApp.AutoStart)
                StartServer();

            logRecorder = new LogRecorder();
            logForm = new LogForm();

            logRecorder.OnNewLine += logForm.OnLogNewLine;
          
            logForm.Show();
        }

        public void AddUser(string user)
        {
            logRecorder.Add("User " + user + " connected");
        }

        void ExecuteCommand(NetworkServerClientWorker worker, string cmd)
        {
            if (cmd.Contains(BasicNetCmd.NetDisconnectionCmd))
            {
                string suser = cmd.Replace(BasicNetCmd.NetDisconnectionCmd, "");
                logRecorder.Add("User " + suser + " disconnected");
            }
            else
                if (cmd.Contains(BasicNetCmd.NetConnectionCmd))
               {
                   string value = cmd.Replace(BasicNetCmd.NetConnectionCmd, "");
                   worker.Client.Name = value;
                   AddUser(value.ToString());
                   labelClientsConnected.BeginInvoke(new ClientsUpdater(updateConnectedClientsAmount));
               }
        }

         
        void databankServer_OnServerNotify(NetworkServerClientWorker worker,
           NetworkServerClientWorker.NetworkClientNotification data)
        {

            string cmd = string.Empty;
            
            try
            {
                if (data.EventType == NetworkClientBase.ClientEventTypeEnum.RawDataRecieved ||
                    data.EventType == NetworkClientBase.ClientEventTypeEnum.Disconnected)
                {
                    System.Text.ASCIIEncoding enc = new System.Text.ASCIIEncoding();
                    cmd = enc.GetString((byte[])data.EventData);
                }
                else
                {
                    System.Text.UnicodeEncoding enc = new System.Text.UnicodeEncoding();
                    cmd = enc.GetString((byte[])data.EventData);
                }
            }
            catch { }


            switch (data.EventType)
            {
                    
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    labelClientsConnected.BeginInvoke(new ClientsUpdater(updateConnectedClientsAmount));
                    break;

                case NetworkClientBase.ClientEventTypeEnum.Disconnected:
                    labelClientsConnected.BeginInvoke(new ClientsUpdater(updateConnectedClientsAmount));
                    ExecuteCommand(worker,cmd);
                  
                    break;

                case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                    ExecuteCommand(worker, cmd);
                    break;

                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    ///((NetworkServerClientAccepter)worker).DataToSend = System.Text.Encoding.Unicode.GetBytes("ready");

                    foreach (NetworkServerClientAccepter accepter in databankServer.Accepters)
                    {
                        accepter.DataToSend = (byte[])data.EventData;
                    }

                    ExecuteCommand(worker, cmd);



                    // This would get a specific worker by key and will send Hello through it.
                    // chatServer.GetAccepter(worker.Key).DataToSend =
                    //     System.Text.Encoding.Unicode.GetBytes("Hello");

                    break;
            }

        }

        private delegate void ClientsUpdater();
        private void updateConnectedClientsAmount()
        {
            int i = 0;

            userForm.Clear();

            foreach (NetworkServerClientAccepter accepter in databankServer.Accepters)
            {
                i++;
                if (accepter.Client.Name.Length > 0)
                    userForm.AddUser(accepter.Client.Name);
            }

            labelClientsConnected.Text = i.ToString();
        }


        void StartServer()
        {
            IPAddress connectaddress = settingsApp.getIPAddress();

            databankServer.AddListener(
                new NetworkServerClientListener(
                new TimeSpan(2000),
                new System.Net.IPEndPoint(connectaddress, settingsApp.Port), 100));

            databankServer.Start();
         
            btnStartServer.Enabled  = false;
            btnStop.Enabled         = true;
            logRecorder.Add("Server started");

            userForm = new UsersView();
            userForm.Show();
        }

     

        private void btnStartServer_Click(object sender, EventArgs e)
        {
            StartServer();
        }

        public void StopServer()
        {
            if (userForm != null)
            {
                userForm.Close();
                userForm = null;
            }

            btnStartServer.Enabled = true;
            databankServer.Stop();
            btnStop.Enabled = false;

            logRecorder.Add("Server stopped");
        }

        public void OnSettingsChanged(Settings settings)
        {
            settingsApp = settings;
            labelServerIP.Text = settingsApp.Host+ " Port:" + settingsApp.Port.ToString();
        }

        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //Settings
            SettingsForm mySettingsForm = new SettingsForm(settingsApp, settingsFile);
            mySettingsForm.OnSettingsChanged += OnSettingsChanged;
            
            mySettingsForm.Show();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            StopServer();
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void testCommandToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TestCommandForm commandForm = new TestCommandForm(databankServer);
            commandForm.Show();
        }
    }
}
