// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "micromock_c.h"

int* int_Clone(int val);
char* char_Clone(char val);
short* short_Clone(short val);
long* long_Clone(long val);
size_t* size_t_Clone(size_t val);
float* float_Clone(float val);
double* double_Clone(double val);
char** char_ptr_Clone(char* val);
void** void_ptr_Clone(void* val);
unsigned long* unsigned_long_Clone(unsigned long val);


/*
Creates a char* representation of the function call
*/
char* make_function_call_string(const FunctionInfo *info, char* dest, size_t buffsize)
{
	if (info != NULL){
		if (strcpy_s(dest, buffsize, info->fName) != 0)
		{
			fprintf(stderr, "Could not copy %s into the destination with a buffer size of %d. Try increasing CALL_STRING_SIZE.\nExiting...\n", info->fName, (int)buffsize);
			exit(EXIT_FAILURE);
		}

		if (strcat_s(dest, buffsize, "(") != 0)
		{
			fprintf(stderr, "Could not add onto the destination with a buffer size of %d. Try increasing CALL_STRING_SIZE.\nExiting...\n", (int)buffsize);
			exit(EXIT_FAILURE);
		}

		for (size_t i = 0; i < info->argc; i++)
		{

			if (to_string_args(info, i, dest, buffsize) != 0)
			{
				fprintf(stderr, "Could not create string representation of this function beyond %s with a buffer size of %d. Try increasing CALL_STRING_SIZE.\nExiting...\n", dest, (int)buffsize);
				exit(EXIT_FAILURE);
			}
			if (i != info->argc - 1){
				if(strcat_s(dest, buffsize, ", ") != 0)
				{
					fprintf(stderr, "Could not add onto the destination with a buffer size of %d. Try increasing CALL_STRING_SIZE.\nExiting...\n", (int)buffsize);
					exit(EXIT_FAILURE);
				}
			}
			else {
				if(strcat_s(dest, buffsize, ")") != 0)
				{
					fprintf(stderr, "Could not add onto the destination with a buffer size of %d. Try increasing CALL_STRING_SIZE.\nExiting...\n", (int)buffsize);
					exit(EXIT_FAILURE);
				}
			}
		}
	}
	else {
		strcpy_s(dest, buffsize, "(No function call)");
	}
	return dest;
}

/*
This is used in the sorting function. It compares only the name of two FunctionInfo structs
*/
int function_info_name_cmpr(const void* a, const void* b)
{
	const char* aMName = (*(FunctionInfo**)a)->fName;
	const char* bMName = (*(FunctionInfo**)b)->fName;
	return strcmp(aMName, bMName);
}

/*
Comparison of a FunctionInfo struct to another used in the CompareExpectedAndActualCalls function
*/
int function_info_cmpr(const FunctionInfo* info_a, const FunctionInfo* info_b)
{
	//NOTE: Not including comparing return value

	//If one of the FunctionInfo pointers is null (i.e. there was no function call)
	if (info_a == NULL || info_b == NULL)
		return -1;
	int strc, comp_result;
	if ((strc = strcmp(info_a->fName, info_b->fName)) == 0){
		if (info_a->argc == info_b->argc){
			for (size_t args = 0; args < info_a->argc; args++){
				comp_result = compare_args(info_a, info_b, args);
				if (comp_result != 0){
					return comp_result;
				}
			}
			return 0;
		}
		return -1;
	}
	return strc;
}

/*
Creates a string representation of a particular void* pointer from the FunctionInfo pointer and 
copies it into the destination
*/
//NOTE: Change to use the CTest to_string functions
int to_string_args(const FunctionInfo* info, int index, char* dest, size_t buffsize)
{
	const char* type = info->args[index]->argType;
	int maxVarLen = buffsize - strlen(dest);
	int ret;
	char* str = micromock_malloc(maxVarLen, "char*");

	if (strcmp(type, "int") == 0)
	{
		int *aptr = CAST(int, info->args[index]->argValue);
		int_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "char") == 0)
	{
		char *aptr = CAST(char, info->args[index]->argValue);
		char_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "short") == 0)
	{
		short *aptr = CAST(short, info->args[index]->argValue);
		short_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "long") == 0)
	{
		long *aptr = CAST(long, info->args[index]->argValue);
		long_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "size_t") == 0)
	{
		size_t *aptr = CAST(size_t, info->args[index]->argValue);
		size_t_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "float") == 0)
	{
		float *aptr = CAST(float, info->args[index]->argValue);
		float_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "double") == 0)
	{
		double *aptr = CAST(double, info->args[index]->argValue);
		double_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "char*") == 0)
	{
		char* *aptr = CAST(char*, info->args[index]->argValue);
		char_ptr_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "void*") == 0)
	{
		void* *aptr = CAST(void*, info->args[index]->argValue);
		void_ptr_ToString(str, maxVarLen, *aptr);
	}
	else if (strcmp(type, "unsigned long") == 0)
	{
		unsigned long *aptr = CAST(unsigned long, info->args[index]->argValue);
		unsigned_long_ToString(str, maxVarLen, *aptr);
	}
	else
	{
		free(str);
		return custom_to_string(info->args[index]->argValue, type, &dest, buffsize);
	}

	ret = strcat_s(dest, buffsize, str);
	free(str);
	return ret;
}

/*
Compares a particular argument to another. This function assumes that both arguments will be of the same type.
*/
int compare_args(const FunctionInfo* info_a, const FunctionInfo* info_b, int index)
{
	const char* type = info_a->args[index]->argType;

	if (info_a->args[index]->ignored == true || info_b->args[index]->ignored == true)
		return 0;
	if (strcmp(info_a->args[index]->argType, info_b->args[index]->argType) != 0)
		return -1;

	if (strcmp(type, "int") == 0)
	{
		int *aptr = CAST(int, info_a->args[index]->argValue);
		int *bptr = CAST(int, info_b->args[index]->argValue);
		return int_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "char") == 0)
	{
		char *aptr = CAST(char, info_a->args[index]->argValue);
		char *bptr = CAST(char, info_b->args[index]->argValue);
		return char_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "short") == 0)
	{
		short *aptr = CAST(short, info_a->args[index]->argValue);
		short *bptr = CAST(short, info_b->args[index]->argValue);
		return short_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "long") == 0)
	{
		long *aptr = CAST(long, info_a->args[index]->argValue);
		long *bptr = CAST(long, info_b->args[index]->argValue);
		return long_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "size_t") == 0)
	{
		size_t *aptr = CAST(size_t, info_a->args[index]->argValue);
		size_t *bptr = CAST(size_t, info_b->args[index]->argValue);
		return size_t_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "float") == 0)
	{
		float *aptr = CAST(float, info_a->args[index]->argValue);
		float *bptr = CAST(float, info_b->args[index]->argValue);
		return float_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "double") == 0)
	{
		double *aptr = CAST(double, info_a->args[index]->argValue);
		double *bptr = CAST(double, info_b->args[index]->argValue);
		return double_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "char*") == 0)
	{
		char* *aptr = CAST(char*, info_a->args[index]->argValue);
		char* *bptr = CAST(char*, info_b->args[index]->argValue);
		return char_ptr_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "void*") == 0)
	{
		void* *aptr = CAST(void*, info_a->args[index]->argValue);
		void* *bptr = CAST(void*, info_b->args[index]->argValue);
		return void_ptr_Compare(*(aptr), *(bptr));
	}
	else if (strcmp(type, "unsigned long") == 0)
	{
		unsigned long *aptr = CAST(unsigned long, info_a->args[index]->argValue);
		unsigned long *bptr = CAST(unsigned long, info_b->args[index]->argValue);
		return unsigned_long_Compare(*(aptr), *(bptr));
	}
	else
	{
		return custom_compare_args(info_a->args[index]->argValue, info_b->args[index]->argValue, type);
	}
}


void* clone_arg(const char* type, ...)
{
	va_list args;
	va_start(args, type);
	if (strcmp(type, "int") == 0)
	{
		int arg = va_arg(args, int);
		va_end(args);
		return int_Clone(arg);
	}
	else if (strcmp(type, "char") == 0)
	{
		char arg = (char)va_arg(args, int);
		va_end(args);
		return char_Clone(arg);
	}
	else if (strcmp(type, "short") == 0)
	{
		short arg = (short)va_arg(args, int);
		va_end(args);
		return short_Clone(arg);
	}
	else if (strcmp(type, "long") == 0)
	{
		long arg = va_arg(args, long);
		va_end(args);
		return long_Clone(arg);
	}
	else if (strcmp(type, "size_t") == 0)
	{
		size_t arg = va_arg(args, size_t);
		va_end(args);
		return size_t_Clone(arg);
	}
	else if (strcmp(type, "float") == 0)
	{
		float arg = (float)va_arg(args, double);
		va_end(args);
		return float_Clone(arg);
	}
	else if (strcmp(type, "double") == 0)
	{
		double arg = va_arg(args, double);
		va_end(args);
		return double_Clone(arg);
	}
	else if (strcmp(type, "char*") == 0)
	{
		char* arg = va_arg(args, char*);
		va_end(args);
		return char_ptr_Clone(arg);
	}
	else if (strcmp(type, "void*") == 0)
	{
		void* arg = va_arg(args, void*);
		va_end(args);
		return void_ptr_Clone(arg);
	}
	else if (strcmp(type, "unsigned long") == 0)
	{
		unsigned long arg = va_arg(args, unsigned long);
		va_end(args);
		return unsigned_long_Clone(arg);
	}
	else
	{
		return custom_clone_arg(type, &args);
	}
}

int* int_Clone(int val)
{
	int* clone = malloc(sizeof(int));
	*clone = val;
	return clone;
}

char* char_Clone(char val)
{
	char* clone = malloc(sizeof(char));
	*clone = val;
	return clone;
}

short* short_Clone(short val)
{
	short* clone = malloc(sizeof(short));
	*clone = val;
	return clone;
}

long* long_Clone(long val)
{
	long* clone = malloc(sizeof(long));
	*clone = val;
	return clone;
}

size_t* size_t_Clone(size_t val)
{
	size_t* clone = malloc(sizeof(size_t));
	*clone = val;
	return clone;
}

float* float_Clone(float val)
{
	float* clone = malloc(sizeof(float));
	*clone = val;
	return clone;
}

double* double_Clone(double val)
{
	double* clone = malloc(sizeof(double));
	*clone = val;
	return clone;
}

char** char_ptr_Clone(char* val)
{
	char** clone = malloc(sizeof(char*));
	*clone = val;
	return clone;
}

void** void_ptr_Clone(void* val)
{
	void** clone = malloc(sizeof(void*));
	*clone = val;
	return clone;
}

unsigned long* unsigned_long_Clone(unsigned long val)
{
	unsigned long* clone = malloc(sizeof(unsigned long));
	*clone = val;
	return clone;
}

void** default_clone(void* val)
{
	return void_ptr_Clone(val);
}

void reset_expected_and_actual_calls(FunctionInfo** expected, int* lastExpectedIndex, FunctionInfo** actual, int* lastActualIndex)
{
	int largerIndex = (*lastActualIndex)-1;						
	if(*lastExpectedIndex>*lastActualIndex)					
		largerIndex = (*lastExpectedIndex)-1;						
	for(largerIndex; largerIndex >= 0; largerIndex--)				
	{																
		if(largerIndex<*lastExpectedIndex)						
		{															
			for(size_t arg=0; arg<expected[largerIndex]->argc; arg++)
			{														
				free(expected[largerIndex]->args[arg]->argValue);
				free(expected[largerIndex]->args[arg]);	
			}													
			free(expected[largerIndex]->args);			
			free(expected[largerIndex]);					
		}														
		if (largerIndex < *lastActualIndex)					
		{														
			for (size_t arg = 0; arg<actual[largerIndex]->argc; arg++)
			{														
				free(actual[largerIndex]->args[arg]->argValue); 
				free(actual[largerIndex]->args[arg]);			
			}														
			free(actual[largerIndex]->args);					
			free(actual[largerIndex]);							
		}															
	}																
	memset(expected, 0, sizeof(expected));				
	memset(actual, 0, sizeof(actual));					
	*lastExpectedIndex = 0; *lastActualIndex = 0;				
}

/*
Wraps a malloc call and checks for failure. Gives an error message and exits on failure, proceeds normally on success.
*/
void* micromock_malloc(size_t _Size, char* _Type)
{
	void* ptr = malloc(_Size);
	if (ptr == NULL)
	{
		fprintf(stderr, "Could not allocate memory for a pointer of type %s\nExiting...\n", _Type);
		exit(EXIT_FAILURE);
	}
	return ptr;
}