// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "azure_c_shared_utility/xio.h"
#include "azure_c_shared_utility/socketio.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/gballoc.h"
#include "emscripten.h"
#include "socketio_node.h"

int tlsio_node_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context)
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

                if (!('tls' in Module))
                {
                    console.log('loading network module');
                    Module.tls = require('tls');
                }
                
                console.log("TLS: connecting to host " + host + " port " + port);
                var socket = Module.tls.connect(port,host, function() {
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
                
                var node_object_index = Module.socketmap.length;
                Module.socketmap[node_object_index] = socket;
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

static const IO_INTERFACE_DESCRIPTION tlsio_node_io_interface_description = 
{
    socketio_create,
    socketio_destroy,
    tlsio_node_open,
    socketio_close,
    socketio_send,
    socketio_dowork,
    socketio_setoption
};

const IO_INTERFACE_DESCRIPTION* tlsio_node_get_interface_description(void)
{
    return &tlsio_node_io_interface_description;
}


