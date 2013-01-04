using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using System.Runtime.Serialization;

namespace Blink1Control
{
//    [Serializable]
    public class Blink1Input
    {
        /// <summary>
        /// Name of the input 
        /// </summary>
        public string iname { get; set; }
        /// <summary>
        /// name of the color pattern to play if this input is triggered
        /// </summary>
        public string pname { get; set; }
        /// <summary>
        ///  type of input, can be: "ifttt", "url", "file", "script"
        /// </summary>
        public string type { get; set; }
        public string arg1 { get; set; }
        public string arg2 { get; set; }
        public string arg3 { get; set; }
        public string lastVal { get; set; }

        public Blink1Input(string name, string type, string arg1, string arg2, string arg3)
        {
            this.iname = name; this.type = type;
            this.arg1 = arg1; this.arg2 = arg2; this.arg3 = arg3;
        }


        public void update()
        {
        }

        public void stop()
        {
        }

    }
}
