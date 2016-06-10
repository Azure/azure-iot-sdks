// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _SOCKETIO_NODE_H_
#define _SOCKETIO_NODE_H_

#define INVALID_SOCKET          -1

typedef struct SOCKET_IO_INSTANCE_TAG
{
    int socket;
    LOGGER_LOG logger_log;
    ON_BYTES_RECEIVED on_bytes_received;
    ON_IO_ERROR on_io_error;
    void* on_bytes_received_context;
    void* on_io_error_context;
    char* hostname;
    int port;
    int node_object;
} SOCKET_IO_INSTANCE;

CONCRETE_IO_HANDLE socketio_create(void* io_create_parameters, LOGGER_LOG logger_log);
void socketio_destroy(CONCRETE_IO_HANDLE socket_io);
int socketio_open(CONCRETE_IO_HANDLE socket_io, ON_IO_OPEN_COMPLETE on_io_open_complete, void* on_io_open_complete_context, ON_BYTES_RECEIVED on_bytes_received, void* on_bytes_received_context, ON_IO_ERROR on_io_error, void* on_io_error_context);
int socketio_close(CONCRETE_IO_HANDLE socket_io, ON_IO_CLOSE_COMPLETE on_io_close_complete, void* callback_context);
int socketio_send(CONCRETE_IO_HANDLE socket_io, const void* buffer, size_t size, ON_SEND_COMPLETE on_send_complete, void* callback_context);
void socketio_dowork(CONCRETE_IO_HANDLE socket_io);
int socketio_setoption(CONCRETE_IO_HANDLE socket_io, const char* optionName, const void* value);

#endif // _SOCKETIO_NODE_H_

