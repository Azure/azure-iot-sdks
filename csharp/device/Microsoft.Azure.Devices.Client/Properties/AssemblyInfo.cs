// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle("Microsoft.Azure.Devices.Client")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]

// Setting ComVisible to false makes the types in this assembly not visible
// to COM components.  If you need to access a type in this assembly from
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]

// The following GUID is for the ID of the typelib if this project is exposed to COM
[assembly: Guid("4680cfbc-a5f8-4d62-9aa4-44a7b62a02d3")]

[assembly: AssemblyVersion("1.0.0.0")]

#if (RELEASE_DELAY_SIGN)
[assembly: AssemblyDelaySignAttribute(true)]
[assembly: AssemblyKeyFileAttribute("35MSSharedLib1024.snk")]
#else
[assembly: InternalsVisibleTo("Microsoft.Azure.Devices.Client.Test")]
[assembly: InternalsVisibleTo("DynamicProxyGenAssembly2")]
#endif

// Version information for an assembly follows semantic versioning 1.0.0 (because
// NuGet didn't support semver 2.0.0 before VS 2015). See semver.org for details.
[assembly: AssemblyInformationalVersion("1.0.19")]