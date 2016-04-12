// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;
using System.Runtime.Serialization.Json;
using System.IO;

namespace api_generator
{
    class Deserializer
    {

        private static List<LWM2M> XmlFiles = null;
        private static List<PropertyMapEntry> PropertyMap = null;

        private static void EnsureFilesLoaded()
        {
            if (XmlFiles == null)
            {
                XmlFiles = LoadOmaXmlFiles();
            }
            if (PropertyMap == null)
            {
                PropertyMap = LoadPropertyMap();
            }
        }

        private static List<LWM2M> LoadOmaXmlFiles()
        {
            var o = new List<LWM2M>();

            DirectoryInfo di = new DirectoryInfo("../../lwm2m_objects/");
            var fi = di.GetFiles("*.xml");
            foreach (var f in fi)
            {
                Console.WriteLine("Reading " + f.FullName);
                XmlSerializer serializer = new XmlSerializer(typeof(LWM2M));
                StreamReader reader = new StreamReader(f.FullName);
                o.Add((LWM2M)serializer.Deserialize(reader));
                reader.Close();
            }

            return o;
        }

        private static List<PropertyMapEntry> LoadPropertyMap()
        {
            DataContractJsonSerializer js = new DataContractJsonSerializer(typeof(List<PropertyMapEntry>));
            StreamReader reader = new StreamReader("../../lwm2m_objects/PropertyMap.json");
            return (List<PropertyMapEntry>)js.ReadObject(reader.BaseStream);
        }

        private static PropertyMapEntry GetPropertyMapEntryForResource(string ObjectName, string ResourceName)
        {
            foreach (var pm in PropertyMap)
            {
                if (string.Equals(pm.Object, ObjectName) &&
                    string.Equals(pm.Resource, ResourceName))
                {
                    return pm;
                } 
            }
            return null;
        }
        
        public static List<PropertyToGenerate> GetPropertiesToGenerate()
        {
            var output = new List<PropertyToGenerate>();
            EnsureFilesLoaded();

            foreach (var xmlFile in XmlFiles)
            {
                int index = 0;
                foreach (var obj in xmlFile.Object)
                {
                    foreach (var res in obj.Resources)
                    {
                        var pm = GetPropertyMapEntryForResource(obj.Name, res.Name);
                        if (pm != null)
                        { 
                            PropertyToGenerate o = new PropertyToGenerate();
                            o.ObjectName = obj.Name;
                            o.ObjectId = obj.ObjectID;
                            o.PropertyIndex = index++;
                            o.ResourceFromXml = res;
                            if (String.IsNullOrEmpty(pm.ApiPropertyOverride) == false)
                            {
                                o.PropertyName = pm.ApiPropertyOverride;
                            }
                            output.Add(o);
                            pm.Mapped = true;
                        }
                    }
                }
            }

            bool doThrow = false;
            foreach (var pm in PropertyMap)
            {
                if (pm.Mapped == false)
                {
                    Console.WriteLine("Property " + pm.ServiceName + " not mapped");
                    doThrow = true;
                }
            }
            if (doThrow)
            {
                throw new Exception("Some properties not mapped");
            }

            return output;
        }

        public static List<string>GetListOfObjectsToGenerate()
        {
            var output = new List<string>();
            EnsureFilesLoaded();

            foreach (var pm in PropertyMap)
            {
                if ( !output.Contains(pm.Object) )
                {
                    output.Add(pm.Object);
                }
            }
            return output;
        }


    }
}
