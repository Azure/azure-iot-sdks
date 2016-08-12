# JSON Decoder

## References: JSON objects

JSON decoder is a module that exposes the following API:
```c
typedef enum JSON_DECODER_RESULT_TAG
{
    JSON_DECODER_OK,
    JSON_DECODER_INVALID_ARG,
    JSON_DECODER_PARSE_ERROR,
    JSON_DECODER_MULTITREE_FAILED
} JSON_DECODER_RESULT;

JSON_DECODER_RESULT JSONDecoder_JSON_To_MultiTree(char* json,
MULTITREE_HANDLE* multiTreeHandle);
```

**SRS_JSON_DECODER_99_008: [**  JSONDecoder_JSON_To_MultiTree shall create a multi tree based on the json string argument. **]**

**SRS_JSON_DECODER_99_009: [**  On success, JSONDecoder_JSON_To_MultiTree shall return a handle to the multi tree it created in the multiTreeHandle argument and it shall return JSON_DECODER_OK. **]**

Example of json argument:
JSON object:
```json
{
	"a":23
	"b":{
		"B1":"class"
		"B2":"mountain"
	}
}
```

**SRS_JSON_DECODER_99_001: [**  If any of the parameters passed to the JSONDecoder_JSON_To_MultiTree function is NULL then the function call shall return JSON_DECODER_INVALID_ARG. **]**

**SRS_JSON_DECODER_99_002: [**  JSONDecoder_JSON_To_MultiTree shall use the MultiTree APIs to create the multi tree and add leafs to the multi tree. **]**

**SRS_JSON_DECODER_99_038: [**  If any MultiTree API fails, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_MULTITREE_FAILED. **]**

**SRS_JSON_DECODER_99_003: [**  When a JSON element is decoded from the JSON object then a leaf shall be added to the MultiTree. **]**

**SRS_JSON_DECODER_99_004: [**  The leaf node name in the multi tree shall be the JSON element name. **]**

**SRS_JSON_DECODER_99_039: [**  For array elements the multi tree node name shall be the string representation of the array index. **]**

**SRS_JSON_DECODER_99_005: [**  The leaf node added in the multi tree shall have the value the string value of the JSON element as parsed from the JSON object. **]**

**SRS_JSON_DECODER_99_007: [**  If parsing the JSON fails due to the JSON string being malformed, JSONDecoder_JSON_To_MultiTree shall return JSON_DECODER_PARSE_ERROR. **]**

**SRS_JSON_DECODER_99_049: [**  JSONDecoder shall not allocate new string values for the leafs, but rather point to strings in the original JSON. **]**


Here are the relevant portions of the RFC4627:


2.  JSON Grammar

   A JSON text is a sequence of tokens. The set of tokens includes six
   structural characters, strings, numbers, and three literal names.


**SRS_JSON_DECODER_99_012: [**  A JSON text is a serialized object or array. **]**


      JSON-text = object / array

**SRS_JSON_DECODER_99_037:[ **    These are the six structural characters: **]**

      begin-array     = ws %x5B ws  ; [ left square bracket

      begin-object    = ws %x7B ws  ; { left curly bracket

      end-array       = ws %x5D ws  ; ] right square bracket

      end-object      = ws %x7D ws  ; } right curly bracket

      name-separator  = ws %x3A ws  ; : colon

      value-separator = ws %x2C ws  ; , comma]

**SRS_JSON_DECODER_99_016: [**     Insignificant whitespace is allowed before or after any of the six structural characters. **]**

       
**SRS_JSON_DECODER_99_017: [**  ws = *( %x20 /              ; Space  **]**

**SRS_JSON_DECODER_99_040: [**  %x09 /              ; Horizontal tab **]**

**SRS_JSON_DECODER_99_041: [**  %x0A /              ; Line feed or New line **]**

**SRS_JSON_DECODER_99_042: [**  %x0D                ; Carriage return 
)**]**

            

2.1.  Values

**SRS_JSON_DECODER_99_018:[** A JSON value MUST be an object, array, number, or string, or one of the following three literal names: false null true**]**
	
**SRS_JSON_DECODER_99_019: [**  The literal names MUST be lowercase. **]**

****SRS_JSON_DECODER_99_020: [** **  No other literal names are allowed. ** **]**


         value = false / null / true / object / array / number / string

         false = %x66.61.6c.73.65   ; false

         null  = %x6e.75.6c.6c      ; null

         true  = %x74.72.75.65      ; true

2.2.  Objects

**SRS_JSON_DECODER_99_021: [** An object structure is represented as a pair of curly brackets surrounding zero or more name/value pairs (or members). **]**

**SRS_JSON_DECODER_99_022: [**  A name is a string. **]**

**SRS_JSON_DECODER_99_023: [**   A single colon comes after each name, separating the name from the value. **]**

**SRS_JSON_DECODER_99_024: [**  A single comma separates a value from a following name. **]**

**SRS_JSON_DECODER_99_025: [**  The names within an object SHOULD be unique. **]**

      object = begin-object [ member *( value-separator member ) ]
      end-object

      member = string name-separator value

2.3.  Arrays

**SRS_JSON_DECODER_99_026: [**  An array structure is represented as square brackets surrounding zero or more values (or elements). **]**

**SRS_JSON_DECODER_99_027: [**  Elements are separated by commas. **]**

      array = begin-array [ value *( value-separator value ) ] end-array

2.4.  Numbers

The representation of numbers is similar to that used in most programming languages.
**SRS_JSON_DECODER_99_043: [**  A number contains an integer component that may be prefixed with an optional minus sign, which may be followed by a fraction part and/or an exponent part. **]**

**SRS_JSON_DECODER_99_044: [**  Octal and hex forms are not allowed. **]**

**SRS_JSON_DECODER_99_045: [** Leading zeros are not allowed. **]**

**SRS_JSON_DECODER_99_046: [**  A fraction part is a decimal point followed by one or more digits. **]**

**SRS_JSON_DECODER_99_047: [**  An exponent part begins with the letter E in upper or lowercase, which may be followed by a plus or minus sign. **]**

**SRS_JSON_DECODER_99_048: [**  The E and optional sign are followed by one or more digits. **]**

   Numeric values that cannot be represented as sequences of digits
   (such as Infinity and NaN) are not permitted.


         number = [ minus ] int [ frac ] [ exp ]

         decimal-point = %x2E       ; .

         digit1-9 = %x31-39         ; 1-9

         e = %x65 / %x45            ; e E

         exp = e [ minus / plus ] 1*DIGIT

         frac = decimal-point 1*DIGIT

         int = zero / ( digit1-9 *DIGIT )

         minus = %x2D               ; -

         plus = %x2B                ; +

         zero = %x30                ; 0


2.5.  Strings

   The representation of strings is similar to conventions used in the C
   family of programming languages.  
**SRS_JSON_DECODER_99_028: [**  A string begins and ends with quotation marks. **]**

   All Unicode characters may be placed within the
   quotation marks except for the characters that must be escaped:
   quotation mark, reverse solidus, and the control characters (U+0000
   through U+001F).

**SRS_JSON_DECODER_99_030: [** Any character may be escaped. **]**

**SRS_JSON_DECODER_99_031: [**  If the character is in the Basic Multilingual Plane (U+0000 through U+FFFF), then it may be represented as a six-character sequence: a reverse solidus, followed by the lowercase letter u, followed by four hexadecimal digits that encode the character's code point. **]**

**SRS_JSON_DECODER_99_032: [**    The hexadecimal letters A though F can be upper or lowercase. **]**

So, for example, a string containing
   only a single reverse solidus character may be represented as
   "\u005C".

**SRS_JSON_DECODER_99_033: [** Alternatively, there are two-character sequence escape  representations of some popular characters.  So, for example, a   string containing only a single reverse solidus character may be   represented more compactly as "\\". **]**

   To escape an extended character that is not in the Basic Multilingual Plane, the character is represented as a twelve-character sequence, encoding the UTF-16 surrogate pair.
  So, for example, a string
   containing only the G clef character (U+1D11E) may be represented as
   "\uD834\uDD1E".

         string = quotation-mark *char quotation-mark

         char = unescaped /
                escape (
**SRS_JSON_DECODER_99_050: [**  %x22 /          ; "    quotation mark  U+0022 **]**


**SRS_JSON_DECODER_99_051: [**  %x5C /          ; \    reverse solidus U+005C **]**


**SRS_JSON_DECODER_99_052: [**  %x2F /          ; /    solidus         U+002F **]**


**SRS_JSON_DECODER_99_053: [**  %x62 /          ; b    backspace       U+0008 **]**


**SRS_JSON_DECODER_99_054: [**  %x66 /          ; f    form feed       U+000C **]**


**SRS_JSON_DECODER_99_055: [**  %x6E /          ; n    line feed       U+000A **]**


**SRS_JSON_DECODER_99_056: [**  %x72 /          ; r    carriage return U+000D **]**


**SRS_JSON_DECODER_99_057: [**  %x74 /          ; t    tab             U+0009 **]**


**SRS_JSON_DECODER_99_058: [**  %x75 4HEXDIG )  ; uXXXX                U+XXXX **]**



         escape = %x5C              ; \

         quotation-mark = %x22      ; "

         unescaped = %x20-21 / %x23-5B / %x5D-10FFFF

