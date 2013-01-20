using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Blink1Control
{
    // thanks to http://json2csharp.com/ for this
    public class IftttEvent
    {
        public string blink1_id { get; set; }
        public string name { get; set; }
        public string source { get; set; }
        public string date { get; set; }
    }

    public class IftttResponse
    {
        public List<IftttEvent> events { get; set; }
        public int event_count { get; set; }
        public string status { get; set; }
    }
}
