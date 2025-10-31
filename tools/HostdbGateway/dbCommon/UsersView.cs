using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace dbCommon
{
    public partial class UsersView : Form
    {
        public UsersView()
        {
            InitializeComponent();
        }

        public void Clear()
        {
            listUsers.Items.Clear();
        }

        public void AddUser(string user)
        {
            listUsers.Items.Add(user);
        }
    }
}
