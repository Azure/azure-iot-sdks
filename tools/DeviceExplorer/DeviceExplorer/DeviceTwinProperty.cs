using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DeviceExplorer
{
    public class DeviceTwinProperty
    {
		private Type type;

        private string propertyName = string.Empty;
        private object propertyValue = null;
        private bool isReadOnly = false;
        private bool isVisible = true;

        public DeviceTwinProperty(string name, object value, Type type, bool isReadOnly)
        {
            this.propertyName = name;
            this.propertyValue = value;
			this.type = type;
            this.isReadOnly = isReadOnly;
        }

        public string Name
        {
            get
            {
                return propertyName;
            }
        }

        public object Value
        {
            get
            {
                return propertyValue;
            }
            set
            {
                propertyValue = value;
            }
        }
        public Type Type
        {
            get { return type; }
        }

        public bool ReadOnly
        {
            get
            {
                return isReadOnly;
            }
        }

        public bool Visible
        {
            get
            {
                return isVisible;
            }
        }
    }
}
