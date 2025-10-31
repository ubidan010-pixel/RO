using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace dbCommon
{
    public partial class LogForm : Form
    {
        private static Mutex mut = new Mutex();

        public LogForm()
        {
            InitializeComponent();

        }

        private delegate void LogFormUpdater();
        private void update()
        {
            mut.WaitOne();

            foreach (string line in listLinesToUpdate)
                listBoxOutput.Items.Add(line);

            listLinesToUpdate.Clear();
            mut.ReleaseMutex();

        }
           
        public void OnLogNewLine(string newline)
        {
            mut.WaitOne();
            listLinesToUpdate.Add(newline);
            listBoxOutput.BeginInvoke(new LogFormUpdater(update));
            mut.ReleaseMutex();
        }

        List<string> listLinesToUpdate = new List<string>();
    }
}
