# ServiceAuthenticationWithSharedAccessPolicyToken Requirements

## Overview

Extend AuthenticationMethod class, provide getters for protected properties and implement populate function to set ServiceAuthenticationWithSharedAccessPolicyToken type policy on given IotHubConnectionString object. 

## References

([IoT Hub SDK.doc](https://microsoft.sharepoint.com/teams/Azure_IoT/_layouts/15/WopiFrame.aspx?sourcedoc={9A552E4B-EC00-408F-AE9A-D8C2C37E904F}&file=IoT%20Hub%20SDK.docx&action=default))

## Exposed API

### ServiceAuthenticationWithSharedAccessPolicyKey

```java
public class ServiceAuthenticationWithSharedAccessPolicyKey extends AuthenticationMethod
{
    public String populate();
}
```
**SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_001: [** Provide access to the following properties: policyName, token **]**

### populate

```java
protected populate();
```
**SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_002: [** The function shall throw IllegalArgumentException if the input object is null **]**

**SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_003: [** The function shall save the policyName and token to the target object **]**

**SRS_SERVICE_SDK_JAVA_SERVICEAUTHENTICATIONWITHSHAREDACCESSTOKEN_12_004: [** The function shall set the access key to null **]**
