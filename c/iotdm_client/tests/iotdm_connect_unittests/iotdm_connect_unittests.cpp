// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "azure_c_shared_utility/gballoc.h"

#include "mocks.h"
#include "testrunnerswitcher.h"

#ifdef __cplusplus
extern "C"
{
#endif
    bool append_object_link(uint16_t oid, void *ctx);
    STRING_HANDLE iotdmc_get_registration_query(const char* endpointName, const char* iotHubName, const char* deviceKey);
    void iotdmc_handle_registration_reply(lwm2m_transaction_t *transacP, void *message);
    STRING_HANDLE iotdmc_SAS_create(const char* iotHubName, const char* deviceKey);
    void on_bytes_received(void *context, const unsigned char *buffer, size_t size);
    uint32_t parse_int(uint8_t *bytes, size_t length);
    uint16_t prv_min(uint16_t x, uint16_t y, uint16_t z);
    void reset_input_buffer(CLIENT_DATA* cd);

    bool for_each_oid(for_each_oid_predicate pred, void* context)
    {
        return false;
    }
#ifdef __cplusplus
}
#endif

struct test_transaction
{
    lwm2m_transaction_t transaction;
    lwm2m_server_t server;
    test_transaction() : transaction(), server()
    {
        server.status = STATE_REG_PENDING;
        transaction.peerP = &server;
    }
};

coap_packet_t make_packet(uint8_t code = CREATED_2_01)
{
    coap_packet_t packet = { 0 };
    packet.code = code;
    return packet;
}

class client_data {
    CLIENT_DATA data_;
public:
    client_data& and_;
    client_data() : data_(), and_(*this) {}
    client_data& with_state(IOTHUB_CLIENT_STATE state) {
        data_.state = state;
        return *this;
    }
    client_data& with_message_length(uint16_t len) {
        data_.input.length = len;
        data_.input.available = SIZEOF_MESSAGE_LENGTH_FIELD;
        data_.input.buffer[0] = (len >> 8) & 0xFF;
        data_.input.buffer[1] = len & 0xFF;
        return *this;
    }
    client_data& with_buffer(const uint8_t* buf, uint16_t size) {
        data_.input.length = (size >= SIZEOF_MESSAGE_LENGTH_FIELD) ? ((uint16_t)buf[0] << 8) + buf[1] : 0;
        data_.input.available = size;
        memcpy(data_.input.buffer, buf, size);
        return *this;
    }
    operator CLIENT_DATA() { return data_; }
};

client_data make_client_data() {
    return client_data();
}

const char nonEmpty[] = "irrelevant";

BEGIN_TEST_SUITE(iotdm_connect_unittests)

    void BeforeEachTest()
    {
        STRING_construct_failer.reset();
        STRING_concat_failer.reset();
        STRING_new_failer.reset();
    }

    MICROMOCK_BOILERPLATE

    TEST_FUNCTION(iotdmc_SAS_create_returns_a_sas_token)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        STRING_HANDLE h = iotdmc_SAS_create(nonEmpty, nonEmpty);

        ASSERT_ARE_EQUAL(char_ptr, sasTokenValue, REAL::STRING_c_str(h));

        STRING_delete(h);
    }

    TEST_FUNCTION(iotdmc_SAS_create_returns_null_if_iothubname_is_null)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        ASSERT_IS_NULL(iotdmc_SAS_create(NULL, nonEmpty));
    }

    TEST_FUNCTION(iotdmc_SAS_create_returns_null_if_device_key_is_null)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
    
        ASSERT_IS_NULL(iotdmc_SAS_create(nonEmpty, NULL));
    }

    TEST_FUNCTION(iotdmc_SAS_create_returns_null_if_STRING_construct_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        STRING_construct_failer.fail_on(1);
        ASSERT_IS_NULL(iotdmc_SAS_create(nonEmpty, nonEmpty));

        STRING_construct_failer.fail_on(2);
        ASSERT_IS_NULL(iotdmc_SAS_create(nonEmpty, nonEmpty));
    }

    TEST_FUNCTION(iotdmc_SAS_create_returns_null_if_STRING_new_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        EXPECTED_CALL(mocks, STRING_new())
            .SetReturn((STRING_HANDLE)NULL);

        STRING_new_failer.fail_on(1);
        ASSERT_IS_NULL(iotdmc_SAS_create(nonEmpty, nonEmpty));
    }
    

    TEST_FUNCTION(iotdmc_get_registration_query_returns_the_registration_query_string)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        STRING_HANDLE query = iotdmc_get_registration_query("endpoint", "iothub", "key");

        ASSERT_ARE_EQUAL(char_ptr, "?ep=endpoint&b=T&tk=token", STRING_c_str(query));

        STRING_delete(query);
    }

    TEST_FUNCTION(iotdmc_get_registration_query_returns_null_if_it_cannot_construct_the_return_string)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        STRING_construct_failer.fail_on(1);
        ASSERT_IS_NULL(iotdmc_get_registration_query(nonEmpty, nonEmpty, nonEmpty));
    }

    TEST_FUNCTION(iotdmc_get_registration_query_returns_null_if_it_cannot_generate_the_sas_token)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        ASSERT_IS_NULL(iotdmc_get_registration_query(nonEmpty, NULL, NULL));
    }

    TEST_FUNCTION(iotdmc_get_registration_query_returns_null_if_string_concatenation_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        STRING_concat_failer.fail_on(1);
        ASSERT_IS_NULL(iotdmc_get_registration_query(nonEmpty, nonEmpty, nonEmpty));

        STRING_concat_failer.fail_on(2);
        ASSERT_IS_NULL(iotdmc_get_registration_query(nonEmpty, nonEmpty, nonEmpty));

        STRING_concat_failer.reset();
        EXPECTED_CALL(mocks, STRING_concat_with_STRING(NULL, NULL))
            .SetReturn(1);

        ASSERT_IS_NULL(iotdmc_get_registration_query(nonEmpty, nonEmpty, nonEmpty));
    }

    TEST_FUNCTION(iotdmc_handle_registration_reply_sets_the_transaction_state)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        auto tr = test_transaction();
        auto packet = make_packet();

        iotdmc_handle_registration_reply(&tr.transaction, (void*)&packet);

        ASSERT_IS_TRUE(tr.server.registration > 0);
        ASSERT_ARE_EQUAL(int, STATE_REGISTERED, tr.server.status);
        ASSERT_ARE_EQUAL(char_ptr, optionValue, tr.server.location);
    }

    TEST_FUNCTION(iotdmc_handle_registration_reply_fails_if_the_message_arg_is_null)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        auto tr = test_transaction();

        iotdmc_handle_registration_reply(&tr.transaction, NULL);

        ASSERT_IS_TRUE(tr.server.registration > 0);
        ASSERT_ARE_EQUAL(int, STATE_REG_FAILED, tr.server.status);
        ASSERT_IS_NULL(tr.server.location);
    }

    TEST_FUNCTION(iotdmc_handle_registration_reply_fails_if_the_message_arg_has_the_wrong_code)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        auto tr = test_transaction();
        auto packet = make_packet(DELETED_2_02);

        iotdmc_handle_registration_reply(&tr.transaction, (void*)&packet);

        ASSERT_IS_TRUE(tr.server.registration > 0);
        ASSERT_ARE_EQUAL(int, STATE_REG_FAILED, tr.server.status);
        ASSERT_IS_NULL(tr.server.location);
    }

    TEST_FUNCTION(iotdmc_handle_registration_reply_succeeds_even_if_it_fails_to_set_the_location)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        EXPECTED_CALL(mocks, coap_get_multi_option_as_string(NULL))
            .SetReturn((char*)NULL);

        auto tr = test_transaction();
        auto packet = make_packet();

        iotdmc_handle_registration_reply(&tr.transaction, (void*)&packet);

        ASSERT_IS_TRUE(tr.server.registration > 0);
        ASSERT_ARE_EQUAL(int, STATE_REGISTERED, tr.server.status);
        ASSERT_IS_NULL(tr.server.location);
    }

    TEST_FUNCTION(append_object_link_appends_an_object_link)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        STRING_HANDLE str = STRING_construct("prefix:");
        uint16_t objectId = 42;

        bool result = append_object_link(objectId, str);

        ASSERT_IS_TRUE(result);
        ASSERT_ARE_EQUAL(char_ptr, "prefix:</42/0>,", STRING_c_str(str));

        STRING_delete(str);
    }

    TEST_FUNCTION(append_object_link_does_not_append_if_STRING_concat_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        STRING_HANDLE str = STRING_construct("prefix:");
        uint16_t objectId = 42;

        STRING_concat_failer.fail_on(1);
        bool result = append_object_link(objectId, str);

        ASSERT_IS_FALSE(result);
        ASSERT_ARE_EQUAL(char_ptr, "prefix:", STRING_c_str(str));

        STRING_delete(str);
    }

    TEST_FUNCTION(iotdmc_register_fails_if_xio_create_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        EXPECTED_CALL(mocks, xio_create(NULL, NULL, NULL))
            .SetReturn((XIO_HANDLE)NULL);
        CLIENT_DATA data;

        IOTHUB_CLIENT_RESULT result = iotdmc_register(&data, NULL, NULL);

        ASSERT_ARE_EQUAL(int, (int)IOTHUB_CLIENT_ERROR, result);
    }

    TEST_FUNCTION(iotdmc_register_fails_if_xio_open_fails)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        EXPECTED_CALL(mocks, xio_open(NULL, NULL, NULL, NULL, NULL, NULL, NULL))
            .SetReturn(1)
            .ExpectedTimesExactly(16);
        CLIENT_DATA data = { 0 };

        IOTHUB_CLIENT_RESULT result = iotdmc_register(&data, NULL, NULL);

        ASSERT_ARE_EQUAL(int, result, (int)IOTHUB_CLIENT_ERROR);
        ASSERT_ARE_EQUAL(void_ptr, data.ioHandle, NULL);
    }

    TEST_FUNCTION(iotdmc_register_succeeds_when_xio_open_succeeds)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        EXPECTED_CALL(mocks, xio_open(NULL, NULL, NULL, NULL, NULL, NULL, NULL));
        CLIENT_DATA data;

        IOTHUB_CLIENT_RESULT result = iotdmc_register(&data, NULL, NULL);

        ASSERT_ARE_EQUAL(int, result, (int)IOTHUB_CLIENT_OK);
        ASSERT_ARE_NOT_EQUAL(void_ptr, data.ioHandle, NULL);
    }

    TEST_FUNCTION(prv_min_returns_the_lesser_of_three_integers)
    {
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(1, 2, 3));
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(1, 3, 2));
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(2, 1, 3));
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(2, 3, 1));
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(3, 1, 2));
        ASSERT_ARE_EQUAL(uint32_t, 1, prv_min(3, 2, 1));
    }

    TEST_FUNCTION(parse_int_converts_a_numeric_byte_sequence_to_an_integer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t bytes[] = { 0x42, 0x42 };

        ASSERT_ARE_EQUAL(uint32_t, 0x4242, parse_int(bytes, sizeof(bytes)));
    }

    TEST_FUNCTION(parse_int_parses_the_last_four_bytes)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t bytes[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

        ASSERT_ARE_EQUAL(uint32_t, 0x02030405, parse_int(bytes, sizeof(bytes)));
    }

    TEST_FUNCTION(reset_input_buffer_resets_the_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        CLIENT_DATA data = make_client_data()
            .with_state(RECEIVING).and_
            .with_buffer(buffer, sizeof(buffer));

        reset_input_buffer(&data);

        ASSERT_ARE_EQUAL(int, BLOCKED, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.available);

        uint8_t cmp[sizeof(CLIENT_DATA::input.buffer)] = { 0 };
        ASSERT_IS_TRUE(memcmp(cmp, data.input.buffer, sizeof(cmp)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_does_nothing_if_client_state_is_not_valid)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        CLIENT_DATA before = make_client_data()
            .with_state((IOTHUB_CLIENT_STATE)42);
        CLIENT_DATA after = before;

        on_bytes_received(&after, NULL, 0);

        ASSERT_IS_TRUE(memcmp(&before, &after, sizeof(CLIENT_DATA)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_interprets_the_first_byte_of_the_incoming_message_as_part_of_the_payload_length)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED);
        uint8_t buffer[] = { 0x01 };

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, LENGTH, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.length);
        ASSERT_ARE_EQUAL(uint32_t, 1, data.input.available);
        ASSERT_ARE_EQUAL(uint8_t, buffer[0], data.input.buffer[0]);
    }

    TEST_FUNCTION(on_bytes_received_interprets_the_second_byte_of_the_incoming_message_as_part_of_the_payload_length)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x01, 0x02 };
        CLIENT_DATA data = make_client_data()
            .with_state(LENGTH).and_
            .with_buffer(&buffer[0], 1);

        on_bytes_received(&data, &buffer[1], 1);

        ASSERT_ARE_EQUAL(int, RECEIVING, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0x0102, data.input.length);
        ASSERT_ARE_EQUAL(uint32_t, sizeof(buffer), data.input.available);
        ASSERT_IS_TRUE(memcmp(data.input.buffer, buffer, sizeof(buffer)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_can_receive_the_payload_length_in_one_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x01, 0x02 };
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, RECEIVING, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0x0102, data.input.length);
        ASSERT_ARE_EQUAL(uint32_t, sizeof(buffer), data.input.available);
        ASSERT_IS_TRUE(memcmp(data.input.buffer, buffer, sizeof(buffer)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_can_receive_the_payload_length_and_part_of_the_payload_in_one_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x01, 0x02, 42, 42 };
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, RECEIVING, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0x0102, data.input.length);
        ASSERT_ARE_EQUAL(uint32_t, sizeof(buffer), data.input.available);
        ASSERT_IS_TRUE(memcmp(data.input.buffer, buffer, sizeof(buffer)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_can_receive_part_of_the_payload_length_and_part_of_the_payload_in_one_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x01, 0x02, 42, 42 };
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED).and_
            .with_buffer(buffer, 1);

        on_bytes_received(&data, buffer + 1, sizeof(buffer) - 1);

        ASSERT_ARE_EQUAL(int, RECEIVING, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0x0102, data.input.length);
        ASSERT_ARE_EQUAL(uint32_t, sizeof(buffer), data.input.available);
        ASSERT_IS_TRUE(memcmp(data.input.buffer, buffer, sizeof(buffer)) == 0);
    }

    TEST_FUNCTION(on_bytes_received_keeps_receiving_until_the_incoming_message_is_complete)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 1, 2, 3 };
        CLIENT_DATA data = make_client_data()
            .with_state(RECEIVING).and_
            .with_message_length(sizeof(buffer) * 2);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, RECEIVING, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 5, data.input.available);
    }

    TEST_FUNCTION(on_bytes_received_flushes_the_message_when_it_is_complete)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 1, 2, 3 };
        CLIENT_DATA data = make_client_data()
            .with_state(RECEIVING).and_
            .with_message_length(sizeof(buffer));

        EXPECTED_CALL(mocks, lwm2m_handle_packet(NULL, NULL, sizeof(buffer), NULL))
            .ValidateArgumentBuffer(2, buffer, sizeof(buffer))
            .ValidateArgument(3);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, BLOCKED, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.available);
    }

    TEST_FUNCTION(on_bytes_received_can_receive_the_entire_message_in_one_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x00, 0x02, 42, 42 };
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED);

        EXPECTED_CALL(mocks, lwm2m_handle_packet(NULL, NULL, sizeof(buffer) - SIZEOF_MESSAGE_LENGTH_FIELD, NULL))
            .ValidateArgumentBuffer(2, buffer + SIZEOF_MESSAGE_LENGTH_FIELD, sizeof(buffer) - SIZEOF_MESSAGE_LENGTH_FIELD)
            .ValidateArgument(3);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, BLOCKED, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.available);
    }

    TEST_FUNCTION(on_bytes_received_can_parse_more_than_one_message_from_a_buffer)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;
        uint8_t buffer[] = { 0x00, 0x02, 42, 42, 0x00, 0x02, 41, 41 };
        CLIENT_DATA data = make_client_data()
            .with_state(BLOCKED);

        EXPECTED_CALL(mocks, lwm2m_handle_packet(NULL, NULL, SIZEOF_MESSAGE_LENGTH_FIELD, NULL))
            .ValidateArgumentBuffer(2, buffer + SIZEOF_MESSAGE_LENGTH_FIELD, SIZEOF_MESSAGE_LENGTH_FIELD)
            .ValidateArgument(3);
        EXPECTED_CALL(mocks, lwm2m_handle_packet(NULL, NULL, SIZEOF_MESSAGE_LENGTH_FIELD, NULL))
            .ValidateArgumentBuffer(2, buffer + 6, SIZEOF_MESSAGE_LENGTH_FIELD)
            .ValidateArgument(3);

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, BLOCKED, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.available);
    }
        
    TEST_FUNCTION(on_bytes_received_DISCARDS_the_input_buffer_if_it_is_larger_than_the_destination)
    {
        CNiceCallComparer<iotdm_connect_mocks> mocks;

        uint8_t buffer[sizeof(CLIENT_DATA::input.buffer) - SIZEOF_MESSAGE_LENGTH_FIELD + 1]; // payload one byte larger than CLIENT_DATA::input.buffer
        memset(buffer, 0x42, sizeof(buffer));

        CLIENT_DATA data = make_client_data()
            .with_state(RECEIVING).and_
            .with_message_length((uint16_t)sizeof(buffer));

        EXPECTED_CALL(mocks, lwm2m_handle_packet(NULL, NULL, 0, NULL))
            .NeverInvoked();

        on_bytes_received(&data, buffer, sizeof(buffer));

        ASSERT_ARE_EQUAL(int, BLOCKED, data.state);
        ASSERT_ARE_EQUAL(uint32_t, 0, data.input.available);
    }

END_TEST_SUITE(iotdm_connect_unittests)
