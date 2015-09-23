// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef _WIN32
#include <CTest.h>
#include <MicroMock_C.h>
#else
#include "ctest.h"
#include "micromock_c.h"
#endif

BEGIN_MOCKS

//Methods for my int pointer type
int int_ptr_compare(void* a, void* b)
{
	return *(int*)a != *(int*)b;
}

int int_ptr_to_string(char* string, size_t bufferSize, void* arg)
{
	size_t strlength = strlen(string);
	if (strlength >= bufferSize){
		fprintf(stderr, "Buffer size is too small.\n");
		exit(EXIT_FAILURE);
	}
	size_t maxLen = bufferSize - strlen(string);
	char* str = micromock_malloc(maxLen, "char*");
	sprintf_s(str, maxLen, "%d", *(int*)arg);
	int ret = strcat_s(string, bufferSize, str);
	free(str);
	return ret;
}

int* int_ptr_Clone(int* val)
{
	int* newValue = malloc(sizeof(int));
	*newValue = *val;
	return newValue;
}

//The following 3 macros are necessary. Only include parameters if you are using your own comparison, to string, or cloning functions. 
//Parameters should be ordered with the type followed by the function to which it is bound for the specified operation (comparison, stringify, cloning)
COMPARISONS(int*, int_ptr_compare)
TO_STRINGS(int*, int_ptr_to_string)
CLONES(int*, int_ptr_Clone)

/*Mocks Section*/
MOCK_FUNCTION(void, hello)
printf("Hello!\n");
MOCK_FUNCTION_END(void, )

MOCK_FUNCTION(void, goodbye)
printf("Goodbye!\n");
MOCK_FUNCTION_END(void, )

MOCK_FUNCTION(void, manyArgs, char*, name, int, value, float, anotherValue, double, thirdValue)
printf("Got:\n\tName: %s\n\tValue: %d\n\tVal2: %.02f\n\tVal3: %.02f\n", name, value, anotherValue, thirdValue);
MOCK_FUNCTION_END(void, )

MOCK_FUNCTION(void, charArg, char, letter)
printf("Got: %c\n", letter);
MOCK_FUNCTION_END(void, )

MOCK_FUNCTION(int, hi, char*, name, int, num)
for (int i = 0; i < num; i++){
	printf("Hello, %s!\n", name);
}
MOCK_FUNCTION_END(int, 0)

MOCK_FUNCTION(void, int_ptr_function, int*, argument)
printf("My int is: %d\n", *argument);
MOCK_FUNCTION_END(void, )
/*End Mocks Section*/

END_MOCKS

void foo(void)
{
	manyArgs("Andy", 2, 3.0, 3.14159);
	charArg('f');
}

void int_foo(void)
{
	int x = 100;
	int* myIntPointer = &x;
	int_ptr_function(myIntPointer);
}

CTEST_BEGIN_TEST_SUITE(MicroMock_C_Test_Suite)

CTEST_FUNCTION(FOO_TEST_WITH_CALLS_OUT_OF_ORDER_AND_UNORDERED_COMPARISON_PASSES)
{
	//Arrange
	STRICT_EXPECTED_CALL(charArg('f'));
	STRICT_EXPECTED_CALL(manyArgs("Andy", 2, 3.0, 3.14159));
	
	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(UnorderedComparison);

	RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_CALLS_OUT_OF_ORDER_AND_ORDERED_COMPARISON_FAILS)
{
	//Arrange
	STRICT_EXPECTED_CALL(charArg('f'));
	STRICT_EXPECTED_CALL(manyArgs("Andy", 2, 3.0, 3.14159));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison)

	RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_CALLS_IN_ORDER_AND_UNORDERED_COMPARISON_PASSES)
{
	//Arrange
	STRICT_EXPECTED_CALL(manyArgs("Andy", 2, 3.0, 3.14159));
	STRICT_EXPECTED_CALL(charArg('f'));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(UnorderedComparison)

	RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_CALLS_IN_ORDER_AND_ORDERED_COMPARISON_PASSES)
{
	//Arrange
	STRICT_EXPECTED_CALL(manyArgs("Andy", 2, 3.0, 3.14159));
	STRICT_EXPECTED_CALL(charArg('f'));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison)

		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_IGNORED_ARGS_AND_INVALID_PARAMETERS_WITH_UNORDERED_COMPARISON_PASSES)
{
	//Arrange
	IGNORE_ARGS(STRICT_EXPECTED_CALL(manyArgs("Andrew", 2, 3.0, 3.14159)), 1);
	IGNORE_ALL(STRICT_EXPECTED_CALL(charArg('f')));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(UnorderedComparison)
		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_IGNORED_ARGS_AND_INVALID_PARAMETERS_WITH_ORDERED_COMPARISON_PASSES)
{
	//Arrange
	IGNORE_ARGS(STRICT_EXPECTED_CALL(manyArgs("Andrew", 2, 3.0, 3.14159)), 1);
	IGNORE_ALL(STRICT_EXPECTED_CALL(charArg('f')));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison)
		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_INVALID_PARAMETERS_AND_UNORDERED_COMPARISON_FAILS)
{
	//Arrange
	STRICT_EXPECTED_CALL(manyArgs("Andrew", 2, 3.0, 3.14159));
	STRICT_EXPECTED_CALL(charArg('f'));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(UnorderedComparison)
		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_INVALID_PARAMETERS_AND_ORDERED_COMPARISON_FAILS)
{
	//Arrange
	STRICT_EXPECTED_CALL(manyArgs("Andrew", 2, 3.0, 3.14159));
	STRICT_EXPECTED_CALL(charArg('f'));

	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison)
		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_INVALID_PARAMETER_WITH_USER_DEFINED_TYPES_AND_UNORDERED_COMPARISON_FAILS)
{
	//Arrange
	int f = 99;
	int* intptr = &f;
	STRICT_EXPECTED_CALL(int_ptr_function(intptr));

	//Act
	int_foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(UnorderedComparison)

		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_INVALID_PARAMETER_WITH_USER_DEFINED_TYPES_AND_ORDERED_COMPARISON_FAILS)
{
	//Arrange
	int f = 99;
	int* intptr = &f;
	STRICT_EXPECTED_CALL(int_ptr_function(intptr));

	//Act
	int_foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison)

		RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_ACTUAL_CALLS_AND_NO_EXPECTED_CALLS_AND_ORDERED_COMPARISON_FAILS)
{
	//Act
	foo();

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison);

	RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_FUNCTION(FOO_TEST_WITH_EXPECTED_CALLS_AND_NO_ACTUAL_CALLS_AND_ORDERED_COMPARISON_FAILS)
{
	//Arrange
	STRICT_EXPECTED_CALL(manyArgs("Andrew", 2, 3.0, 3.14159));
	STRICT_EXPECTED_CALL(charArg('f'));

	//Assert
	COMPARE_EXPECTED_AND_ACTUAL_CALLS(OrderedComparison);
	
	RESET_EXPECTED_AND_ACTUAL_CALLS
}

CTEST_END_TEST_SUITE(MicroMock_C_Test_Suite)