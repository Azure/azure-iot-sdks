// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Common
{
    static class FxCop
    {
        public static class Category
        {
            public const string Design = "Microsoft.Design";
            public const string Globalization = "Microsoft.Globalization";
            public const string Maintainability = "Microsoft.Maintainability";
            public const string MSInternal = "Microsoft.MSInternal";
            public const string Naming = "Microsoft.Naming";
            public const string Performance = "Microsoft.Performance";
            public const string Reliability = "Microsoft.Reliability";
            public const string Security = "Microsoft.Security";
            public const string Usage = "Microsoft.Usage";
            public const string Configuration = "Configuration";
            public const string Xaml = "XAML";
            public const string Interoperability = "Microsoft.Interoperability";
            public const string ReliabilityBasic = "Reliability";
        }

        public static class Rule
        {
            public const string AptcaMethodsShouldOnlyCallAptcaMethods = "CA2116:AptcaMethodsShouldOnlyCallAptcaMethods";
            public const string AssembliesShouldHaveValidStrongNames = "CA2210:AssembliesShouldHaveValidStrongNames";
            public const string AttributeStringLiteralsShouldParseCorrectly = "CA2243:AttributeStringLiteralsShouldParseCorrectly";
            public const string AvoidCallingProblematicMethods = "CA2001:AvoidCallingProblematicMethods";
            public const string AvoidExcessiveClassCoupling = "CA1506:AvoidExcessiveClassCoupling";
            public const string AvoidExcessiveComplexity = "CA1502:AvoidExcessiveComplexity";
            public const string AvoidNamespacesWithFewTypes = "CA1020:AvoidNamespacesWithFewTypes";
            public const string AvoidOutParameters = "CA1021:AvoidOutParameters";
            public const string AvoidTypesThatRequireJitCompilationInPrecompiledAssemblies = "CA908:AvoidTypesThatRequireJitCompilationInPrecompiledAssemblies";
            public const string AvoidUncalledPrivateCode = "CA1811:AvoidUncalledPrivateCode";
            public const string AvoidUninstantiatedInternalClasses = "CA1812:AvoidUninstantiatedInternalClasses";
            public const string AvoidUnmaintainableCode = "CA1505:AvoidUnmaintainableCode";
            public const string AvoidUnusedPrivateFields = "CA1823:AvoidUnusedPrivateFields";
            public const string CollectionsShouldImplementGenericInterface = "CA1010:CollectionsShouldImplementGenericInterface";
            public const string ConfigurationPropertyAttributeRule = "Configuration102:ConfigurationPropertyAttributeRule";
            public const string ConfigurationValidatorAttributeRule = "Configuration104:ConfigurationValidatorAttributeRule";
            public const string ConsiderPassingBaseTypesAsParameters = "CA1011:ConsiderPassingBaseTypesAsParameters";
            public const string DefineAccessorsForAttributeArguments = "CA1019:DefineAccessorsForAttributeArguments";
            public const string DisposableFieldsShouldBeDisposed = "CA2213:DisposableFieldsShouldBeDisposed";
            public const string DoNotCallOverridableMethodsInConstructors = "CA2214:DoNotCallOverridableMethodsInConstructors";
            public const string DoNotCastUnnecessarily = "CA1800:DoNotCastUnnecessarily";
            public const string DoNotCatchGeneralExceptionTypes = "CA1031:DoNotCatchGeneralExceptionTypes";
            public const string DoNotDeclareReadOnlyMutableReferenceTypes = "CA2104:DoNotDeclareReadOnlyMutableReferenceTypes";
            public const string DoNotLockOnObjectsWithWeakIdentity = "CA2002:DoNotLockOnObjectsWithWeakIdentity";
            public const string DoNotNestGenericTypesInMemberSignatures = "CA1006:DoNotNestGenericTypesInMemberSignatures";
            public const string DoNotIndirectlyExposeMethodsWithLinkDemands = "CA2122:DoNotIndirectlyExposeMethodsWithLinkDemands";
            public const string DoNotInitializeUnnecessarily = "CA1805:DoNotInitializeUnnecessarily";
            public const string DoNotIgnoreMethodResults = "CA1806:DoNotIgnoreMethodResults";
            public const string DoNotPassLiteralsAsLocalizedParameters = "CA1303:DoNotPassLiteralsAsLocalizedParameters";
            public const string ExceptionsShouldBePublic = "CA1064:ExceptionsShouldBePublic";
            public const string GenericMethodsShouldProvideTypeParameter = "CA1004:GenericMethodsShouldProvideTypeParameter";
            public const string IdentifiersShouldBeSpelledCorrectly = "CA1704:IdentifiersShouldBeSpelledCorrectly";
            public const string IdentifiersShouldHaveCorrectSuffix = "CA1710:IdentifiersShouldHaveCorrectSuffix";
            public const string IdentifiersShouldNotMatchKeywords = "CA1716:IdentifiersShouldNotMatchKeywords";
            public const string InitializeReferenceTypeStaticFieldsInline = "CA1810:InitializeReferenceTypeStaticFieldsInline";
            public const string InstantiateArgumentExceptionsCorrectly = "CA2208:InstantiateArgumentExceptionsCorrectly";
            public const string InterfaceMethodsShouldBeCallableByChildTypes = "CA1033:InterfaceMethodsShouldBeCallableByChildTypes";
            public const string IsFatalRule = "Reliability108:IsFatalRule";
            public const string MarkBooleanPInvokeArgumentsWithMarshalAs = "CA1414:MarkBooleanPInvokeArgumentsWithMarshalAs";
            public const string MarkMembersAsStatic = "CA1822:MarkMembersAsStatic";
            public const string MovePInvokesToNativeMethodsClass = "CA1060:MovePInvokesToNativeMethodsClass";
            public const string NestedTypesShouldNotBeVisible = "CA1034:NestedTypesShouldNotBeVisible";
            public const string NormalizeStringsToUppercase = "CA1308:NormalizeStringsToUppercase";
            public const string OverrideLinkDemandsShouldBeIdenticalToBase = "CA2123:OverrideLinkDemandsShouldBeIdenticalToBase";
            public const string PInvokeEntryPointsShouldExist = "CA1400:PInvokeEntryPointsShouldExist";            
            public const string ReplaceRepetitiveArgumentsWithParamsArray = "CA1025:ReplaceRepetitiveArgumentsWithParamsArray";
            public const string ResourceStringsShouldBeSpelledCorrectly = "CA1703:ResourceStringsShouldBeSpelledCorrectly";
            public const string ReviewUnusedParameters = "CA1801:ReviewUnusedParameters";
            public const string ReviewSuppressUnmanagedCodeSecurityUsage = "CA2118:ReviewSuppressUnmanagedCodeSecurityUsage";
            public const string SecureAsserts = "CA2106:SecureAsserts";
            public const string SpecifyMarshalingForPInvokeStringArguments = "CA2101:SpecifyMarshalingForPInvokeStringArguments";
            public const string SpecifyCultureInfo = "CA1304:SpecifyCultureInfo"; 
            public const string SystemAndMicrosoftNamespacesRequireApproval = "CA:SystemAndMicrosoftNamespacesRequireApproval";            
            public const string TypeConvertersMustBePublic = "XAML1004:TypeConvertersMustBePublic";
            public const string TypesMustHaveXamlCallableConstructors = "XAML1007:TypesMustHaveXamlCallableConstructors";
            public const string TypesThatOwnDisposableFieldsShouldBeDisposable = "CA1001:TypesThatOwnDisposableFieldsShouldBeDisposable";
            public const string TypesThatOwnNativeResourcesShouldBeDisposable = "CA1049:TypesThatOwnNativeResourcesShouldBeDisposable";
            public const string UseApprovedGenericsForPrecompiledAssemblies = "CA908:UseApprovedGenericsForPrecompiledAssemblies";
            public const string UseManagedEquivalentsOfWin32Api = "CA2205:UseManagedEquivalentsOfWin32Api";
            public const string UsePropertiesWhereAppropriate = "CA1024:UsePropertiesWhereAppropriate";
            public const string UriPropertiesShouldNotBeStrings = "CA1056:UriPropertiesShouldNotBeStrings";
            public const string UseNewGuidHelperRule = "Reliability113:UseNewGuidHelperRule";
            public const string VariableNamesShouldNotMatchFieldNames = "CA1500:VariableNamesShouldNotMatchFieldNames";           
            public const string WrapExceptionsRule = "Reliability102:WrapExceptionsRule";
        }
    }
}
