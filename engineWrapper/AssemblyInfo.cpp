using namespace System;
using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;
using namespace System::Security::Permissions;

//
// General Information about an assembly is controlled through the following
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
//
[assembly:AssemblyTitleAttribute("engineWrapper")];
[assembly:AssemblyDescriptionAttribute("")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("Ubisoft")];
[assembly:AssemblyProductAttribute("engineWrapper")];
[assembly:AssemblyCopyrightAttribute("Copyright (c) Ubisoft 2009")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// Version information for an assembly consists of the following four values:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// You can specify all the value or you can default the Revision and Build Numbers
// by using the '*' as shown below:

[assembly:AssemblyVersionAttribute("1.0.*")];

[assembly:ComVisible(false)];

[assembly:CLSCompliantAttribute(true)];

// Upgrading to .Net8.0 we get:
// error C4950: 'System::Security::Permissions::SecurityAction': marked as obsolete
// Message: 'Code Access Security is not supported or honored by the runtime.'
// So removed the following line:
// [assembly:SecurityPermission(SecurityAction::RequestMinimum, UnmanagedCode = true)];
