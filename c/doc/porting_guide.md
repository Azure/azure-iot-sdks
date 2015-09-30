# How to Port the C Libraries to Other Platforms

The purpose of this document is to provide guidance on how to port the C Internet of Things (IoT) client library to platforms not supported out of the box. The document does cover the specifics of any particular platform.


# Table of Content
- [Background](#Background)
- [Porting steps](#Porting-steps)
- [HttpApi](#httpapi)
- [LockApi](#lockapi)
- [THreadApi](#threadapi)
- [Verify the client library port](#verify)

<a name="Background"/>
## Background

The C IoT client library is written in the C programming language for the purpose of portability to most platforms. However, several components rely on platform-specific resources in order to achieve the functionality required by the C IoT client library.

The following platform-specific components are required by the IoT client library and must be implemented for all new platforms:

| Component                | Interface declared in | Description                                                                                         |
|--------------------------|-----------------------|-----------------------------------------------------------------------------------------------------|
| Mutual exclusion locking | Lock.h                | This module implements the platform-specific locking mechanisms (mutex, etc.).                       |
| Threading                | ThreadAPI.h           | This module implements support for creating new threads, terminating threads, and sleeping threads. |

There are also a number of components that must be implemented, depending on the communication protocol used. You must implement at least one of the following communication protocol components:

| Component               | Interface declared in | Description                                                                                                                                                                                                                                                                                                                                                            |
|-------------------------|-----------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| HTTP protocol interface | HttpAPI.h             | This module implements the standard HTTP API used by the C IoT client library. For example, on a Windows platform the HTTP API code uses WinHTTP; for a Linux platform it uses curl, etc. HTTPAPI must support HTTPs (HTTP+SSL).                                                                                                                                       |
| AMQP protocol interface | Proton-C library      |  Proton-C is an open-source library that is used to communicate with the AMQP protocol. You can port this library to the target platform. For more information, see the proton-C website. Our AMQP samples and libraries require that you link with the Proton library, so after you port make sure your PATH variable and the build scripts work with the new library you created. |

<a name="Porting-steps"/>
## Porting steps

- After downloading the source code from GitHub, navigate to the Platform folder located at ..\\common\\adapters.

- In the adapters folder, create the following .c files that implement the platform-specific interfaces:

   -  HttpApi\_\<Library\>.c.

   - Lock\_\<Library\>.c

   - ThreadApi\_\<Library\>.c

- In each of these files, implement the functions provided in the header files, relying on the APIs of the libraries provided for that specific platform. You can see the implementation of these components provided for the platforms already supported in the SDK, under .\\common\\adapters. For more information, also refer to the specification for modules in the .\\common\\doc folder.

<a name="httpapi"/>
## HttpApi\_\<Library\>.c

### HTTPAPI\_RESULT HTTPAPI\_Init(void);

Global initialization for the HTTP API component.

### void HTTPAPI\_Deinit(void);

Frees resources allocated in HTTPAPI\_Init.

### HTTP\_HANDLE HTTPAPI\_CreateConnection(const char\* hostName);

Creates the HTTPS connection to the host specified by the *hostName* parameter. This function returns a handle to the newly created connection. You can use the handle in subsequent calls to execute specific HTTP calls using `HTTPAPI\_ExecuteRequest`.

### void HTTPAPI\_CloseConnection(HTTP\_HANDLE handle);

Closes a connection created with `HTTPAPI\_CreateConnection`. All resources allocated by `HTTPAPI\_CreateConnection` should be freed in `HTTPAPI\_CloseConnection`.

### HTTPAPI\_RESULT HTTPAPI\_ExecuteRequest(HTTP\_HANDLE handle, HTTPAPI\_REQUEST\_TYPE requestType, const char\* relativePath, HTTP\_HEADERS\_HANDLE httpHeadersHandle, const unsigned char\* content, size\_t contentLength, unsigned int\* statusCode, HTTP\_HEADERS\_HANDLE responseHeadersHandle, BUFFER\* responseContent);

Sends the HTTP request to the host and handles the response for the HTTP call.

-   *requestType* specifies which HTTP method is used (GET, POST, DELETE, PUT, PATCH).

-   *relativePath* specifies the relative path of the URL excluding the host name.

-   *httpHeadersHandle* specifies a set of HTTP headers name-value pairs to be added to the HTTP request. The *httpHeadersHandle* handle can be created and set up with the proper name-value pairs by using the **HTTPHeaders** APIs available in HTTPHeaders.h.

-   The *content* argument specifies a pointer to the request body. This value is optional and can be **NULL**.

-   *contentLength* specifies the request body size (this is typically added into the HTTP headers as the Content-Length header). This value is optional and can be 0.

-   *statusCode* is an out parameter, where `HTTPAPI\_ExecuteRequest` returns the status code from the HTTP response (200, 201, 400, 401, etc.)

-   *responseHeadersHandle* is an HTTP headers handle to which the `HTTPAPI\_ExecuteRequest` must add all the HTTP response headers so that the caller of `HTTPAPI\_ExecuteRequest` can inspect them. You can manipulate *responseHeadersHandle* by using the HTTPHeaders APIs available in HTTPHeaders.h.

-   *responseContent* is a buffer that must be filled by `HTTPAPI\_ExecuteRequest` with the contents of the HTTP response body. The buffer size must be increased by the `HTTPAPI\_ExecuteRequest` implementation in order to fit the response body. `HTTPAPI\_ExecuteRequest` must also handle chunked transfer for the HTTP responses. To manipulate the *responseContent* buffer, use the APIs available in Strings.h.

### HTTPAPI\_RESULT HTTPAPI\_CloneOption(const char\* optionName, const char\* value, const void\*\* savedValue);

Clones the option named *optionName* bearing the value *value* into the pointer *savedValue*

-   *optionName* is a NULL terminated string representing the name of the option

-   *value* is a pointer to the value of the option

-   *savedValue* receives the copy of the value of the option. The copy needs to be free-able.

### HTTPAPI\_RESULT HTTPAPI\_SetOption(HTTP\_HANDLE handle, const char\* optionName, const void\* value);

Sets the option named *optionName* bearing the value *value* for the HTTP_HANDLE *handle*

-   *handle* is an existing HTTP_HANDLE

-   *optionName* is a NULL terminated string representing the name of the option

-   *value* is a pointer to the value of the option


<a name="lockapi"/>
## Lock\_\<Library\>.c

### LOCK\_HANDLE Lock\_Init(void);

Initializes a new lock instance and returns a new lock handle that can be used in subsequent calls to Lock/Unlock.

### LOCK\_RESULT Lock(LOCK\_HANDLE handle);

Gets a lock for a specific lock handle.

-   *handle* is the lock handle instance returned from Lock\_Init().

### LOCK\_RESULT Unlock(LOCK\_HANDLE handle);

Releases a lock for a specific lock handle.

-   *handle* is the lock handle instance returned from Lock\_Init().

### LOCK\_RESULT Lock\_Deinit(LOCK\_HANDLE handle);

Frees the resources allocated for a lock handle instance.

-   *handle* is the lock handle instance returned from Lock\_Init().

<a name="threadapi"/>
## ThreadApi\_\<Library\>.c

### THREADAPI\_RESULT ThreadAPI\_Create(THREAD\_HANDLE\* threadHandle, THREAD\_START\_FUNC func, void\* arg)

Creates a thread with an entry point specified by the *func* argument.

-   *threadHandle* is a handle to the new thread.

-   *func* is a function pointer that indicates the entry point to the new thread.

-   *arg* is a void pointer that must be passed to the function pointed to by *func*.

### THREADAPI\_RESULT ThreadAPI\_Join(THREAD\_HANDLE threadHandle, int\* res);

Waits for the thread identified by the *threadHandle* argument to complete. When the *threadHandle* thread completes, all resources associated with the thread must be released and the thread handle will no longer be valid.

-   *threadHandle* is the handle of the thread to wait for completion.

-   *res* is the result returned by the thread, provided by the `ThreadAPI\_Exit` function.

### void ThreadAPI\_Exit(int res);

This function is called by a thread when the thread exits in order to return a result value to the caller of the *ThreadAPI\_Join* function. The *res* value must be copied into the *res* out argument passed to the `ThreadAPI\_Join` function.

-   *res* is an integer that represents the exit status of the thread.

### void ThreadAPI\_Sleep(unsigned int milliseconds);

Sleeps the current thread for a given number of milliseconds.

-   *milliseconds* is the number of milliseconds to sleep.

Now build the C IoT client library for your platform

<a name="verify"/>
## Verify the client library port

To verify that the port to the target platform is successful, ensure that data can be sent to the Event Hub endpoint. You can do this by running the samples in the following locations:

- iothub\_schema\_client/Samples/SimpleSample\_HTTP

- iothub\_schema\_client/Samples/SimpleSample\_AMQP

Both samples send temporary data to an Event Hubs endpoint using either the HTTP or AMQP protocols.

It may be necessary to create a build script specifically designed for your platform. To do so, follow these steps:

-  Create a folder under `serializer/samples/SimpleSample_HTTP/<Platform>`.

-  Generate a build script using Main.c as the entry point (see existing platform scripts as examples).

-  Build and run the sample.
