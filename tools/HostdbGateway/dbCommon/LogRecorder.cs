using System;
using System.Collections.Generic;
using System.Text;

namespace dbCommon
{
    public class LogRecorder
    {

        public delegate void LogRecorderDelegateAddLine(string newline);

        public event LogRecorderDelegateAddLine OnNewLine =null;

        public LogRecorder()
        {
        
        }


        public void Add(string newline)       
        {
            if (OnNewLine != null)
            {
                DateTime saveNow = DateTime.Now;
                string finalline = saveNow.ToString()+ " :" +  newline;
                OnNewLine(finalline);
            }
        }
    }
}
