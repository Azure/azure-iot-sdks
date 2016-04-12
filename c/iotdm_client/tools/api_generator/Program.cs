// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using api_generator.Templates;


namespace api_generator
{
    class Program
    {
        private static List<PropertyToGenerate> _propertiesToGenerate = null;
        public static List<PropertyToGenerate> GetListOfPropertiesToGenerate()
        {

            if (_propertiesToGenerate == null)
            {

                _propertiesToGenerate = Deserializer.GetPropertiesToGenerate();
            }

            return _propertiesToGenerate;
        }

        private static List<string> _objectsToGenerate = null;
        public static List<string> GetListOfObjectsToGenerate()
        {
            if (_objectsToGenerate == null)
            {
                _objectsToGenerate = Deserializer.GetListOfObjectsToGenerate();
            }

            return _objectsToGenerate;
        }

        public static List<PropertyToGenerate> GetListOfPropertiesToGenerate(string ObjectName)
        {
            List<PropertyToGenerate> output = new List<PropertyToGenerate>();
            foreach (var p in GetListOfPropertiesToGenerate())
            {
                if (p.ObjectName == ObjectName)
                {
                    output.Add(p);
                }
            }
            return output;
        }

        public static string GetIdForObject(string ObjectName)
        {
            foreach (var p in GetListOfPropertiesToGenerate())
            {
                if (p.ObjectName == ObjectName)
                {
                    return p.ObjectId;
                }
            }
            return "UNDEFINED_OBJECT";

        }

        private static int GetNumberOfResourcesForObject(string ObjectName)
        {
            int count = 0;
            foreach (var p in GetListOfPropertiesToGenerate())
            {
                if (p.ObjectName == ObjectName)
                {
                    count++;
                }
            }
            return count;
        }

        static string sample_directory = "../../../../lwm2m_objects/";


        private static void MakeSimpleSample()
        {
            // app_template_c app_template_c = new app_template_c();
            // System.IO.File.WriteAllText(sample_directory + "iotdm_simple_sample.c", app_template_c.TransformText());

            foreach (string s in GetListOfObjectsToGenerate())
            {
                blank_object_c ablank_object_c = new blank_object_c();
                ablank_object_c.ObjectName = s;
                ablank_object_c.ObjectId = Convert.ToUInt16(GetIdForObject(s));
                ablank_object_c.PropertyCount = GetNumberOfResourcesForObject(s);
                System.IO.File.WriteAllText(sample_directory + s.Replace(" ", "").ToLower() + "_object.c", ablank_object_c.TransformText());

                blank_object_x_c ablank_object_x_c = new blank_object_x_c();
                ablank_object_x_c.ObjectName = s;
                ablank_object_x_c.ObjectId = Convert.ToUInt16(GetIdForObject(s));
                ablank_object_x_c.PropertyCount = GetNumberOfResourcesForObject(s);
                System.IO.File.WriteAllText(sample_directory + s.Replace(" ", "").ToLower() + "_object_x.c", ablank_object_x_c.TransformText());

                blank_object_h ablank_object_h = new blank_object_h();
                ablank_object_h.ObjectName = s;
                ablank_object_h.ObjectId = Convert.ToUInt16(GetIdForObject(s));
                ablank_object_h.PropertyCount = GetNumberOfResourcesForObject(s);
                System.IO.File.WriteAllText(sample_directory + s.Replace(" ", "").ToLower() + "_object.h", ablank_object_h.TransformText());
            }
        }

        static void Main(string[] args)
        {
            GetListOfPropertiesToGenerate();

            MakeSimpleSample();
        }
    }
}
