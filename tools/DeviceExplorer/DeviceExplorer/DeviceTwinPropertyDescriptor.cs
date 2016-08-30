using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DeviceExplorer
{
    class DeviceTwinPropertyDescriptor : PropertyDescriptor
    {
        DeviceTwinProperty property;
        public DeviceTwinPropertyDescriptor(ref DeviceTwinProperty twinProperty, Attribute[] attrs) : base(twinProperty.Name, attrs)
		{
            property = twinProperty ;
        }

        public override Type ComponentType
        {
			get { return null; }
        }

        public override bool IsReadOnly
        {
			get { return property.ReadOnly; }
        }

        public override Type PropertyType
        {
			get { return property.Type; }
        }

        public override bool CanResetValue(object component)
        {
			return false;
        }

        public override object GetValue(object component)
        {
			return property.Value;
        }

        public override void ResetValue(object component)
        {
            property = null;
        }

        public override void SetValue(object component, object value)
        {
			property.Value = value;
        }

        public override bool ShouldSerializeValue(object component)
        {
			return false;
        }
    }
}
