// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef MICROMOCK_C_H
#define MICROMOCK_C_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef _WIN32
#include "ctest.h"
#include "macro_utils.h"
#include <crt_abstractions.h>
#else
#include "ctest.h"
#include "macro_utils.h"
#include "crt_abstractions.h"
#endif

#define CALL_SIZE 100
#define CALL_STRING_SIZE 100

typedef enum {OrderedComparison, UnorderedComparison} comparisonType;

typedef struct{
	bool result;
	char* expected;
	char* actual;
	char* message;
} MicroMock_C_Result;

typedef struct{
	bool ignored;
	void* argValue;
	const char* argType;
} ArgInfo;

typedef struct{
	void *retValuePtr;
	ArgInfo **args;
	unsigned int argc;
	const char* fName;
} FunctionInfo;

extern char* make_function_call_string(const FunctionInfo *info, char* dest, size_t buffsize);
extern int function_info_name_cmpr(const void* a, const void* b);
extern int function_info_cmpr(const FunctionInfo* info_a, const FunctionInfo* info_b);
extern int to_string_args(const FunctionInfo* info_a, int index, char* dest, size_t buffsize);
extern int compare_args(const FunctionInfo* info_a, const FunctionInfo* info_b, int index);
extern void* clone_arg(const char* type, ...);
extern void reset_expected_and_actual_calls(FunctionInfo** expected, int* lastExpectedIndex, FunctionInfo** actual, int* lastActualIndex);
extern void* micromock_malloc(size_t _Size, char* _Type);
extern int custom_compare_args(void* arg1, void* arg2, const char* type);
extern int custom_to_string(void* arg, const char* type, char** dest, size_t buffSize);
extern void* custom_clone_arg(const char* type, va_list* args);
extern void** default_clone(void* val);

#define BEGIN_MOCKS													\
	static FunctionInfo* expectedCalls[CALL_SIZE];					\
	static FunctionInfo* actualCalls[CALL_SIZE];					\
	static int lastExpectedCallIndex = 0;							\
	static int lastActualCallIndex = 0;								\
	static MicroMock_C_Result* result;								\
	GENERATE_COMPARE_EXPECTED_AND_ACTUAL_CALLS

#define END_MOCKS

#define EXTRACT_FIRST(item, ...) item

#define MAKE_PARAM(N,a,b) a b IFCOMMA(N)

//This creates a pointer to variable of the same type as argType holding onto a variable with the same value.
//This tempXArg will be used by ASSIGN_ARG_INFO to put each of these tempXArg pointers into the appropriate
//ArgInfo structs.
#define ALLOC_ARG(N,argType, argName) argType* C2(C2(temp, DIV2(N)),Arg) = micromock_malloc(sizeof(argType), #argType);\
	*C2(C2(temp, DIV2(N)), Arg) = argName;

//ainfo_x is a temporary name given to the ArgInfo that will be assigned the appropriate FunctionInfo*. It will
//take the correct tempXArg allocated above. ainfo_x is passed along to the ArgInfo array that belongs to 
//FunctionInfo for the corresponding function.
#define ASSIGN_ARG_INFO(N, aType, argName) ArgInfo* C2(ainfo_, DIV2(N)) = micromock_malloc(sizeof(*C2(ainfo_, DIV2(N))), "ArgInfo");\
	(*_Info)->args[(*_Info)->argc - DIV2(N)] = C2(ainfo_, DIV2(N));	\
	(*_Info)->args[(*_Info)->argc - DIV2(N)]->ignored = false;		\
	(*_Info)->args[(*_Info)->argc - DIV2(N)]->argValue = clone_arg(#aType, argName);\
	(*_Info)->args[(*_Info)->argc - DIV2(N)]->argType = #aType;

//Had to include the following two macros because: IF(ISZERO(COUNT_ARG(__VA_ARGS__)), FOR_EACH_2_COUNTED(MAKE_PARAM, __VA_ARGS__), void) does not work in linux.
//So I replaced it with: IF(ISZERO(COUNT_ARG(__VA_ARGS__)), REMOVE_PAREN, void REMOVE_ARGS) (FOR_EACH_2_COUNTED(MAKE_PARAM, __VA_ARGS__))
#define REMOVE_ARGS(...)

#define REMOVE_PAREN(...) __VA_ARGS__

#define MOCK_FUNCTION(returnType, name, ...)						\
	FunctionInfo* Expected_##name(IF(ISZERO(COUNT_ARG(__VA_ARGS__)), REMOVE_PAREN, void REMOVE_ARGS) (FOR_EACH_2_COUNTED(MAKE_PARAM, __VA_ARGS__))) \
	{																\
		FunctionInfo* name##_Info = micromock_malloc(sizeof(*name##_Info), "FunctionInfo");	\
		POPULATE_FUNCTION_INFO(name##_Info, name, __VA_ARGS__)		\
		RECORD_EXPECTED_MOCK_CALL(name##_Info)						\
		return name##_Info;											\
	}																\
	returnType name(IF(ISZERO(COUNT_ARG(__VA_ARGS__)), REMOVE_PAREN, void REMOVE_ARGS) (FOR_EACH_2_COUNTED(MAKE_PARAM, __VA_ARGS__)))	\
	{																\
		FunctionInfo* name##_Info = micromock_malloc(sizeof(*name##_Info), "FunctionInfo");	\
		POPULATE_FUNCTION_INFO(name##_Info, name, __VA_ARGS__)		\
		RECORD_ACTUAL_MOCK_CALL(name##_Info)						\

#define POPULATE_FUNCTION_INFO(info, name, ...)						\
	name##_Info->fName = #name;										\
	name##_Info->retValuePtr = NULL;								\
	name##_Info->argc = DIV2(COUNT_ARG(__VA_ARGS__));				\
	name##_Info->args = micromock_malloc(sizeof(ArgInfo*)* name##_Info->argc, "ArgInfo*");\
	FunctionInfo** _Info = &name##_Info;							\
	FOR_EACH_2_COUNTED(ASSIGN_ARG_INFO, __VA_ARGS__)				\

#define MOCK_FUNCTION_END(returnType, ...)							\
		IF(ISZERO(COUNT_ARG(__VA_ARGS__)), return EXTRACT_FIRST(__VA_ARGS__);, )\
	}

#define CAST(type, ptr)												\
	(type*)ptr;

#define IGNORE_ARG(N)												\
	if(N >= _f_ptr_a->argc) fprintf(stderr, "WARNING: Argument #%d is out of bounds. Ignoring...\n", N);\
	else _f_ptr_a->args[DEC(N)]->ignored = true;

#define IGNORE_ARGS(func_info_ptr, ...)								\
	FunctionInfo* _f_ptr_a = func_info_ptr;							\
	FOR_EACH_1(IGNORE_ARG, __VA_ARGS__)

#define IGNORE_ALL(func_info_ptr)									\
	FunctionInfo* _f_ptr_b = func_info_ptr;							\
	for(size_t arg=0; arg<_f_ptr_b->argc; arg++)					\
	{																\
		_f_ptr_b->args[arg]->ignored = true;						\
	}																\

#define EXPECTED_CALL(name)											\
	IGNORE_ALL(STRICT_EXPECTED_CALL(name))

#define STRICT_EXPECTED_CALL(name)									\
	Expected_##name													\

#define RECORD_EXPECTED_MOCK_CALL(name)								\
	if(lastExpectedCallIndex>=CALL_SIZE){							\
		fprintf(stderr, "Expected call count has exceeded the maximum call size of %d.\
			Increase the call size by redefining CALL_SIZE.\nExiting...", CALL_SIZE);\
			exit(EXIT_FAILURE);										\
	}																\
	expectedCalls[lastExpectedCallIndex] = name;					\
	lastExpectedCallIndex++;

#define RECORD_ACTUAL_MOCK_CALL(name)								\
	if(lastActualCallIndex>=CALL_SIZE){								\
		fprintf(stderr, "Actual call count has exceeded the maximum call size of %d.\
			Increase the call size by redefining CALL_SIZE.\nExiting...", CALL_SIZE);\
			exit(EXIT_FAILURE);										\
	}																\
	actualCalls[lastActualCallIndex] = name;						\
	lastActualCallIndex++;

/*
Set the global result variable by making a call to compare the expected and actual calls
Use a CTest assertion on this result to compare the expected and actual call strings.
Free the result pointer afterwards to prepare for the next comparison
*/
#define COMPARE_EXPECTED_AND_ACTUAL_CALLS(ordered)					\
	result = CompareExpectedAndActualCalls(ordered);				\
	CTEST_ASSERT_ARE_EQUAL(char_ptr, result->expected, result->actual, result->message); \
	free(result->actual);											\
	free(result->expected);											\
	free(result);

/*
This macro will clear the expected and actual call arrays and free all of the memory that is
no longer needed. (For now) this should always be used after a test is completed.
*/
#define RESET_EXPECTED_AND_ACTUAL_CALLS	reset_expected_and_actual_calls(expectedCalls, &lastExpectedCallIndex, actualCalls, &lastActualCallIndex);

/*
This generates a function to compare all of the expected and actual calls and returns
a result for the CTest assert to use.
*/
//NOTE: may be faster to sort 1 array and then search it with the contents of the other
#define GENERATE_COMPARE_EXPECTED_AND_ACTUAL_CALLS					\
MicroMock_C_Result* CompareExpectedAndActualCalls(comparisonType type)\
{																	\
	/*Allocate a result struct to put the result of the comparison*/\
	MicroMock_C_Result* results;									\
	results = micromock_malloc(sizeof(*results), "MicroMock_C_Result");\
	results->result = true;											\
	results->expected = micromock_malloc(CALL_STRING_SIZE, "char");	\
	results->actual = micromock_malloc(CALL_STRING_SIZE, "char");	\
	strcpy_s(results->expected, CALL_STRING_SIZE, "");				\
	strcpy_s(results->actual, CALL_STRING_SIZE, "");				\
	results->message = "The expected and actual calls match.";		\
	FunctionInfo** comp_expectedCalls = expectedCalls;				\
	FunctionInfo** comp_actualCalls = actualCalls;					\
																	\
	/*If this is an unordered comparison, copy and then sort the two\
	arrays, then continue as normal*/								\
	if (type == UnorderedComparison){								\
		FunctionInfo* temp_expectedCalls[CALL_SIZE];				\
		FunctionInfo* temp_actualCalls[CALL_SIZE];					\
		memcpy(temp_expectedCalls, expectedCalls, sizeof(temp_expectedCalls)); \
		memcpy(temp_actualCalls, actualCalls, sizeof(temp_actualCalls)); \
		qsort(temp_expectedCalls, lastExpectedCallIndex, sizeof(FunctionInfo*), function_info_name_cmpr);\
		qsort(temp_actualCalls, lastActualCallIndex, sizeof(FunctionInfo*), function_info_name_cmpr); \
		comp_expectedCalls = temp_expectedCalls;					\
		comp_actualCalls = temp_actualCalls;						\
	}																\
	int largerIndex = lastActualCallIndex;							\
	if (lastExpectedCallIndex>lastActualCallIndex)					\
		largerIndex = lastExpectedCallIndex;						\
	for (int index = 0; index < largerIndex; index++)				\
	{																\
		if(function_info_cmpr(comp_expectedCalls[index], comp_actualCalls[index])!=0){\
			results->result = false;								\
			make_function_call_string(comp_expectedCalls[index], results->expected, CALL_STRING_SIZE);\
			make_function_call_string(comp_actualCalls[index], results->actual, CALL_STRING_SIZE); \
			results->message = "The expected and actual calls do not match.";\
			return results;											\
		}															\
	}																\
	return results;													\
}																	

/*Use of this macro is necessary. Only include arguments if you are using your own comparison. 
The arguments should be formatted as follows: COMPARISONS(typeA, typeA_comparison_func_name, ...).
This function will be called if none of the standard types apply.
*/
#define COMPARISONS(...)											\
int custom_compare_args(void* arg1, void* arg2, const char* type)	\
{																	\
	FOR_EACH_2(MAKE_COMPARE_IF, __VA_ARGS__)						\
																	\
	return void_ptr_Compare(arg1, arg2);							\
}

/*Use of this macro is necessary. Only include arguments if you are using your own to strings.
The arguments should be formatted as follows: TO_STRINGS(typeA, typeA_to_string_func_name, ...).
This function will be called if none of the standard types apply.
*/
#define TO_STRINGS(...)												\
int custom_to_string(void* arg, const char* type, char** dest, size_t buffSize)\
{																	\
	FOR_EACH_2(MAKE_STRING_IF, __VA_ARGS__)							\
																	\
	size_t strlength = strlen(*dest);								\
	if(strlength >= buffSize)										\
	{																\
		fprintf(stderr, "Buffer size too small.\n");				\
		exit(EXIT_FAILURE);											\
	}																\
	size_t maxLen = buffSize - strlen(*dest);						\
	char* str = micromock_malloc(maxLen, "char*");					\
	void_ptr_ToString(str, maxLen, arg);							\
	int ret = strcat_s(*dest, buffSize, str);						\
	free(str);														\
	return ret;														\
}

/*Use of this macro is necessary. Only include arguments if you are using your own clones.
The arguments should be formatted as follows: CLONES(typeA, typeA_clone_func_name, ...).
This function will be called if none of the standard types apply.
*/
#define CLONES(...)													\
void* custom_clone_arg(const char* type, va_list* args)				\
{																	\
	FOR_EACH_2(MAKE_CLONE_IF, __VA_ARGS__)							\
	void* arg = va_arg(*args, void*);								\
	va_end(*args);													\
	return default_clone(arg);										\
}

#define MAKE_COMPARE_IF(arg_type, func_name)						\
	if (strcmp(type, #arg_type) == 0)								\
	{																\
		return func_name(arg1, arg2);								\
	}

#define MAKE_STRING_IF(arg_type, func_name)							\
	if (strcmp(type, #arg_type) == 0)								\
	{																\
		return func_name(*dest, buffSize, arg);						\
	}

#define MAKE_CLONE_IF(arg_type, func_name)							\
	if (strcmp(type, #arg_type) == 0)								\
	{																\
		arg_type arg = va_arg(*args, arg_type);						\
		va_end(*args);												\
		return func_name(arg);										\
	}																\

#endif