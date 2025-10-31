using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace dbCommon
{
    public partial class SettingsForm : Form
    {

        public delegate void SettingsDelegate(Settings settings);

        public event SettingsDelegate OnSettingsChanged;


        public SettingsForm(Settings settings,string filename)
        {
            InitializeComponent();
            currentWorking  = settings;
            settingsFile = filename;

            propertyGridSettings.SelectedObject = currentWorking;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SettingsManager.save(currentWorking, settingsFile);
            OnSettingsChanged(currentWorking);
            Close();
        }

        private string settingsFile;
        private Settings currentWorking;
    }
}
