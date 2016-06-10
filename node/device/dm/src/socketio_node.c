// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "emscripten.h"
#include "socketio_node.h"

// Marshal on_bytes_received event and callback from the nodejs socket implementation to the transpiled C code
EMSCRIPTEN_KEEPALIVE 
void marshal_on_bytes_received(void* callback, void* context, const unsigned char* buffer, size_t size)
{
    if (callback)
    {
        ON_BYTES_RECEIVED f = (ON_BYTES_RECEIVED)callback;

        // The callback is going to free this buffer, so we need to malloc and copy for them.
        unsigned char *newBuffer = malloc(size);
        if (newBuffer)
        {
            memcpy(newBuffer,buffer,size);
            f(context, newBuffer, size);
        }
    }
}

// Marshal on_send_complete event and callback from the nodejs socket implementation to the transpiled C code
EMSCRIPTEN_KEEPALIVE 
void marshal_on_send_complete(void *callback, void* context, IO_SEND_RESULT send_result)
{
    LogInfo("Send complete");
    if (callback)
    {
        ON_SEND_COMPLETE f = (ON_SEND_COMPLETE)callback;
        f(context, send_result);
    }
}

// Marshal on_io_open_complete event and callback from the nodejs socket implementation to the transpiled C code
EMSCRIPTEN_KEEPALIVE 
void marshal_on_io_open_complete(void *callback, void* context, IO_OPEN_RESULT open_result)
{
    if (callback)
    {
        ON_IO_OPEN_COMPLETE f = (ON_IO_OPEN_COMPLETE)callback;
        f(context, open_result);
    }
}

// Marshal on_io_close_complete event and callback from the nodejs socket implementation to the transpiled C code
EMSCRIPTEN_KEEPALIVE 
void marshal_on_io_close_complete(void *callback, void* context)
{
    if (callback)
    {
        ON_IO_CLOSE_COMPLETE f = (ON_IO_CLOSE_COMPLETE)callback;
        f(context);
    }
}

// Marshal on_io_error event and callback from the nodejs socket implementation to the transpiled C code
EMSCRIPTEN_KEEPALIVE 
void marshal_on_io_error(void *callback, void* context)
{
    if (callback)
    {
        ON_IO_ERROR f = (ON_IO_ERROR)callback;
        f(context);
    }
}

CONCRETE_IO_HANDLE socketio_create(void* io_create_parameters, LOGGER_LOG logger_log)
{
    SOCKETIO_CONFIG* socket_io_config = io_create_parameters;
    SOCKET_IO_INSTANCE* result;

    if (socket_io_config == NULL)
    {
        LogError("Invalid argument: socket_io_config is NULL");
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(SOCKET_IO_INSTANCE));
        if (result != NULL)
        {
            if (socket_io_config->hostname != NULL)
            {
                result->hostname = (char*)malloc(strlen(socket_io_config->hostname) + 1);
                if (result->hostname != NULL)
                {
                    (void)strcpy(result->hostname, socket_io_config->hostname);
                }

                result->socket = INVALID_SOCKET;
            }
            else
            {
                result->hostname = NULL;
                result->socket = *((int*)socket_io_config->accepted_socket);
            }

            if ((result->hostname == NULL) && (result->socket == INVALID_SOCKET))
            {
                LogError("Failure: hostname == NULL and socket is invalid.");
                free(result);
                result = NULL;
            }
            else
            {
                result->port = socket_io_config->port;
                result->on_bytes_received = NULL;
                result->on_io_error = NULL;
                result->on_bytes_received_context = NULL;
                result->on_io_error_context = NULL;
                result->logger_log = logger_log;
                result->node_object = 0;
            }
        }
        else
        {
            LogError("Allocation Failure: SOCKET_IO_INSTANCE");
        }
    }

    if (result)
    {
        EM_ASM(
            {
                if (!('socketmap' in Module))
                {
                    console.log('creating socket mapping');
                    Module.socketmap = new Array(1);    // Making it 1 item to start means the first index will be 1, which saves index==0 to mean "invalid"
                }
                if (!('marshal_on_bytes_received' in Module))
                {
                    Module.marshal_on_bytes_received = Module.cwrap('marshal_on_bytes_received', null, [ 'number', 'number', 'number', 'number' ]);
                    Module.marshal_on_send_complete = Module.cwrap('marshal_on_send_complete', null, ['number', 'number', 'number' ]);
                    Module.marshal_on_io_open_complete = Module.cwrap('marshal_on_io_open_complete', null, ['number', 'number', 'number' ]);
                    Module.marshal_on_io_close_complete = Module.cwrap('marshal_on_io_close_complete', null, ['number', 'number' ]);
                    Module.marshal_on_io_error = Module.cwrap('marshal_on_io_error', null, ['number', 'number' ]);
                }
            });
    }

    return result;
}

void socketio_destroy(CONCRETE_IO_HANDLE socket_io)
{    
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = __LINE__;
    }
    else
    {
        EM_ASM_INT(
            {
                var socket = Module.socketmap[$0];
                socket.destroy();
                socket.unref();
                Module.socketmap[$0] = null;
            }, socket_io_instance->node_object);
    }
}

int socketio_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
{
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = __LINE__;
    }
    else
    {
        socket_io_instance->node_object = EM_ASM_INT(
            {
                var host = Module.Pointer_stringify($0);
                var port = $1;
                var on_io_open_complete = $2;
                var on_io_open_complete_context = $3;
                var on_bytes_received = $4;
                var on_bytes_received_context = $5;
                var on_io_error = $6;
                var on_io_error_context = $7;

                if (!('net' in Module))
                {
                    console.log('loading network module');
                    Module.net = require('net');
                }
                
                console.log('allocating new socket');
                var node_object_index = Module.socketmap.length;
                var socket = new Module.net.Socket();
                Module.socketmap[node_object_index] = socket;

                socket.on('connect', function() {
                    console.log('connected');
                    Module.marshal_on_io_open_complete(on_io_open_complete, on_io_open_complete_context, 0);
                });

                socket.on('error', function() {
                    console.log(' io error');
                    // BKTODO: this loops forever.  Other implementations don't fire this event while connecting.
                    // Throw for now until we get a better solution
                    // Module.marshal_on_io_error(on_io_error, on_io_error_context);
                    throw new Error('io error');
                });

                socket.on('data', function(incomingBuffer) {
                    // Copy our buffer into the heap before calling the C function.
                    // Wouldn't it be neat if we could get incomingBuffer's byte array directly instead of copying it here?
                    var buf = Module._malloc(incomingBuffer.length);
                    Module.HEAPU8.set(incomingBuffer, buf);
                    Module.marshal_on_bytes_received(on_bytes_received, on_bytes_received_context, buf, incomingBuffer.length);
                    Module._free(buf);
                });
                
                socket.on('timeout', function() {
                    console.log('timeout');
                    Module.marshal_on_io_error(on_io_error, on_io_error_context);
                });
                
                console.log("connecting to host " + host + " port " + port);

                socket.connect(port, host);

                return node_object_index;
            }, 
            socket_io_instance->hostname,
            socket_io_instance->port, 
            on_io_open_complete,
            on_io_open_complete_context,
            on_bytes_received,
            on_bytes_received_context,
            on_io_error,
            on_io_error_context);
        result = 0;
    }

    return result;
}

int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context)
{
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = __LINE__;
    }
    else
    {
        EM_ASM_INT(
            {
                var socket = Module.socketmap[$0];
                var on_io_close_complete = $1;
                var callback_context = $2;

                socket.on('close', function() {
                    Module.marshal_on_io_close_complete(on_io_close_complete,callback_context);
                });
                socket.destroy();
  
            }, 
            socket_io_instance->node_object,
            on_io_close_complete,
            callback_context);
        result = 0;
    }
    
    return result;
 }

int socketio_send(CONCRETE_IO_HANDLE socket_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context)
{
    int result;

    SOCKET_IO_INSTANCE* socket_io_instance = (SOCKET_IO_INSTANCE*)socket_io;
    if (socket_io == NULL)
    {
        LogError("Invalid argument: SOCKET_IO_INSTANCE is NULL");
        result = __LINE__;
    }
    else
    {
         EM_ASM_INT(
            {
                var socket = Module.socketmap[$0];
                var buffer = $1;
                var size = $2;
                var on_send_complete = $3;
                var callback_context = $4;

                var array = Module.HEAP8.subarray(buffer, buffer+size);
                var buf = new Buffer(size);
                for (var i = 0; i < size; i++)
                {
                    // BKTODO: optimize
                    buf[i] = array[i];
                 }

                socket.write(buf, function() {
                    Module.marshal_on_send_complete(on_send_complete, callback_context, 0);
                });
            },
            socket_io_instance->node_object,
            buffer,
            size,
            on_send_complete,
            callback_context);
        result = 0;
    }
    
    return result;
}

void socketio_dowork(CONCRETE_IO_HANDLE socket_io)
{
    // nothing to do.  The node engine takes care of this for us.
}

int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value)
{
    // Nothing to do (for now)
    return 0;
}

static const IO_INTERFACE_DESCRIPTION socket_io_interface_description = 
{
    socketio_create,
    socketio_destroy,
    socketio_open,
    socketio_close,
    socketio_send,
    socketio_dowork,
    socketio_setoption
};

const IO_INTERFACE_DESCRIPTION* socketio_get_interface_description(void)
{
    return &socket_io_interface_description;
}


