using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DeviceExplorer
{
    public class DeviceTwinProperties : CollectionBase, ICustomTypeDescriptor
    {

        public void Add(DeviceTwinProperty value)
        {
            base.List.Add(value);
        }

        public void Remove(string name)
        {
            foreach (DeviceTwinProperty deviceTwinProperty in base.List)
            {
                if (deviceTwinProperty.Name == name)
                {
                    base.List.Remove(deviceTwinProperty);
                    return;
                }
            }
        }

        public DeviceTwinProperty this[int index]
        {
            get
            {
                return (DeviceTwinProperty)base.List[index];
            }
            set
            {
                base.List[index] = (DeviceTwinProperty)value;
            }
        }

        #region ICustomTypeDescriptor interface implementation

        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            PropertyDescriptor[] newProps = new PropertyDescriptor[this.Count];
            for (int i = 0; i < this.Count; i++)
            {
                DeviceTwinProperty deviceTwinProperties = (DeviceTwinProperty)this[i];
                newProps[i] = new DeviceTwinPropertyDescriptor(ref deviceTwinProperties, attributes);
            }

            return new PropertyDescriptorCollection(newProps);
        }

        public AttributeCollection GetAttributes()
        {
			return TypeDescriptor.GetAttributes(this,true);
        }

        public string GetClassName()
        {
			return TypeDescriptor.GetClassName(this,true);
        }

        public string GetComponentName()
        {
			return TypeDescriptor.GetComponentName(this, true);
        }

        public TypeConverter GetConverter()
        {
			return TypeDescriptor.GetConverter(this, true);
        }

        public EventDescriptor GetDefaultEvent()
        {
			return TypeDescriptor.GetDefaultEvent(this, true);
        }

        public PropertyDescriptor GetDefaultProperty()
        {
			return TypeDescriptor.GetDefaultProperty(this, true);
        }

        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents()
        {
			return TypeDescriptor.GetEvents(this, true);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
			return TypeDescriptor.GetEvents(this, true);
        }

        public PropertyDescriptorCollection GetProperties()
        {
			return TypeDescriptor.GetProperties(this, true);
        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
			return this;
        }
        #endregion
    }
}
