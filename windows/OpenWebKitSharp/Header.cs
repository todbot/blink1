using System;
using System.Collections.Generic;
using System.Text;

namespace WebKit
{
    public class Header
    {
        public string Field { get; internal set; }
        public string Value { get; internal set; }
        public Header(string field, string value)
        {
            this.Field = field;
            this.Value = value;
        }
    }
}
