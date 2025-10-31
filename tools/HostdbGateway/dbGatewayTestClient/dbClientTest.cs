using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using NetworkClient;
using System.Net;
using dbCommon;
using ForemanDP;

namespace dbGatewayTestClient
{
    public partial class dbGatewayTestClient : Form
    {
        NetworkClientBase databankClient;
        private string settingsFile;
        private Settings settingsApp = new Settings();
        public IPHostEntry localhost;

        public dbGatewayTestClient()
        {
            InitializeComponent();
            btnDisconnect.Enabled = false;

            settingsFile = System.IO.Path.GetDirectoryName(Application.ExecutablePath) + @"\settings.xml";
            Settings settings = SettingsManager.open(settingsFile);
            OnSettingsChanged(settings);

            localhost = Network.DoGetHostEntry("");
        }

        
        private void Connect_Click(object sender, EventArgs e)
        {
            btnConnect.Enabled = false;

            databankClient =
                new NetworkClientBase(
                new CommunicationSettings(
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic,
                CommunicationSettings.SocketOperationFlow.Asynchronic, true));

            databankClient.OnClientEvent += chatClient_OnClientEvent;
            IPAddress connectaddress = settingsApp.getIPAddress();

            databankClient.Connect(new System.Net.IPEndPoint(connectaddress, settingsApp.Port));

            btnDisconnect.Enabled = true;

        }

        byte[] aliasCommandHost(string cmd)
        {
            return System.Text.Encoding.Unicode.GetBytes(cmd + localhost.HostName);
        }

        void sendInformationToServer()
        {
            databankClient.Send(aliasCommandHost(BasicNetCmd.NetConnectionCmd));
        }

        void chatClient_OnClientEvent(NetworkClientBase.ClientEventTypeEnum EventType, object EventData)
        {
            switch (EventType)
            {
                case NetworkClientBase.ClientEventTypeEnum.None:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Accepted:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.Connected:
                    databankClient.Recieve();
                    sendInformationToServer();
                    break;
                case NetworkClientBase.ClientEventTypeEnum.RawDataRecieved:
                    break;
                case NetworkClientBase.ClientEventTypeEnum.MessageRecieved:
                    break;
            }
        }

        private void Disconnect_Click(object sender, EventArgs e)
        {
            databankClient.Send(aliasCommandHost(BasicNetCmd.NetDisconnectionCmd));
            btnDisconnect.Enabled  = false;
            btnConnect.Enabled     = true;
        }

        private void editIP_TextChanged(object sender, EventArgs e)
        {

        }

        public void OnSettingsChanged(Settings settings)
        {
            settingsApp = settings;
            editIP.Text = settingsApp.Host;
            editPort.Text = settingsApp.Port.ToString();
        }

        private void optionsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //Settings
            SettingsForm mySettingsForm = new SettingsForm(settingsApp, settingsFile);
            mySettingsForm.OnSettingsChanged += OnSettingsChanged;

            mySettingsForm.Show();
        } 
    }
}
