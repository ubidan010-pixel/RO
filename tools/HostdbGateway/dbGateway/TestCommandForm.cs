using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using NetworkServer;

namespace dbGateway
{
    public partial class TestCommandForm : Form
    {
        private readonly NetworkServerBase databankServer = null;

        List<ClientTalker> listClients = new List<ClientTalker>();

        public TestCommandForm(NetworkServerBase _databankServer)
        {
            InitializeComponent();

            databankServer = _databankServer;
            update_listclients();
        }

        void update_listclients()
        {
            foreach (NetworkServerClientAccepter accepter in databankServer.Accepters)
            {
                if (accepter.Client.Name.Length > 0)
                {
                    ClientTalker talker = new ClientTalker(accepter);
                    listClients.Add(talker);

                    listBoxClients.Items.Add(talker);
                }
            }

        }


        public static byte[] StrToByteArray(string str)
        {
            System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            return encoding.GetBytes(str);
        }


        private void btnSend_Click(object sender, EventArgs e)
        {
            //Send all the chat to the selected clients...
            byte[] data = StrToByteArray(textBoxChat.Text);
            foreach (ClientTalker clienttalker in listBoxClients.SelectedItems)
            {
                
                clienttalker.Accepter.Client.Send(data);

            }
           
        }

        private void listBoxClients_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBoxClients.SelectedItems.Count != 0)
                btnSend.Enabled = true;
            else
                btnSend.Enabled = false;
        }
    }

    public class ClientTalker
    {
        

        public ClientTalker(NetworkServerClientAccepter _accepter)
        {
            accepter = _accepter;
        }

        public override string ToString()
        {
            return Name;
        }

        public String Name
        {
            get { return Accepter.Client.Name; }
        }

        public NetworkServerClientAccepter Accepter
        {
            get { return accepter; }
            set { accepter = value; }
        }

        private NetworkServerClientAccepter accepter;
    }
}
