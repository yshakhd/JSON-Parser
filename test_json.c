#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "clog.h"
#include "miniunit.h"
#include "json.h"

typedef struct {
	bool is_success;
	union {
		Element element;
		long int error_idx;
	};
} ParseResult;

ParseResult _parse_json(char* s) {
	Element element; 
	char* pos = s;
	bool is_success = parse_element(&element, &pos);
	if(is_success) {
		return (ParseResult) { .is_success = is_success, .element = element };
	}
	else {
		return (ParseResult) { .is_success = is_success, .error_idx = pos - s };
	}
}

int _test_int() {
	mu_start();

	ParseResult result1 = _parse_json("0");
	mu_check(result1.is_success);
	if(result1.is_success) {
		mu_check(result1.element.type == ELEMENT_INT);
		print_element(result1.element);
		fputc('\n',stdout);
		mu_check(result1.element.as_int == 0);
		free_element(result1.element);
	}

	ParseResult result2 = _parse_json("9");
	mu_check(result2.is_success);
	if(result2.is_success) {
		mu_check(result2.element.type == ELEMENT_INT);
		print_element(result2.element);
		fputc('\n',stdout);
		mu_check(result2.element.as_int == 9);
		free_element(result2.element);
	}

	ParseResult result3 = _parse_json("15");
	mu_check(result3.is_success);
	if(result3.is_success) {
		mu_check(result3.element.type == ELEMENT_INT);
		print_element(result3.element);
		fputc('\n',stdout);
		mu_check(result3.element.as_int == 15);
		free_element(result3.element);
	}

	ParseResult result4 = _parse_json("-15");
	mu_check(result4.is_success);
	if(result4.is_success) {
		mu_check(result4.element.type == ELEMENT_INT);
		print_element(result4.element);
		fputc('\n',stdout);
		mu_check(result4.element.as_int == -15);
		free_element(result4.element);
	}

	ParseResult result5 = _parse_json("123ABC");
	mu_check(result5.is_success);
	if(result5.is_success) {
		mu_check(result5.element.type == ELEMENT_INT);
		print_element(result5.element);
		fputc('\n',stdout);
		mu_check(result5.element.as_int == 123);
		free_element(result5.element);
	}

	ParseResult result6 = _parse_json("-123ABC");
	mu_check(result6.is_success);
	if(result6.is_success) {
		mu_check(result6.element.type == ELEMENT_INT);
		print_element(result6.element);
		fputc('\n',stdout);
		mu_check(result6.element.as_int == -123);
		free_element(result6.element);
	}

	ParseResult result7 = _parse_json("123ABC456");
	mu_check(result7.is_success);
	if(result7.is_success) {
		mu_check(result7.element.type == ELEMENT_INT);
		print_element(result7.element);
		fputc('\n',stdout);
		mu_check(result7.element.as_int == 123);
		free_element(result7.element);
	}

	ParseResult result8 = _parse_json("ABC123DEF");
	mu_check(!result8.is_success);
	
	ParseResult result9 = _parse_json("-ABC123DEF");
	mu_check(!result9.is_success);
	
	ParseResult result10 = _parse_json("1A2B3C");
	mu_check(result10.is_success);
	if(result10.is_success) {
		mu_check(result10.element.type == ELEMENT_INT);
		print_element(result10.element);
		fputc('\n',stdout);
		mu_check(result10.element.as_int == 1);
		free_element(result10.element);
	}

	mu_end();
}

int _test_string() {
	mu_start();

	ParseResult string1 = _parse_json("\"ab\\\"c\""); // ab"c
	mu_check(string1.is_success);
	if(string1.is_success) {
		mu_check(string1.element.type == ELEMENT_STRING);
		print_element(string1.element);
		fputc('\n', stdout);
		mu_check(strcmp(string1.element.as_string, "ab\"c") == 0);
		mu_check(strlen(string1.element.as_string) == 4);
		free_element(string1.element);
	}

	ParseResult string2 = _parse_json("\"\"");
	mu_check(string2.is_success);
	if(string2.is_success) {
		mu_check(string2.element.type == ELEMENT_STRING);
		print_element(string2.element);
		fputc('\n', stdout);
		mu_check(strcmp(string2.element.as_string, "") == 0);
		mu_check(strlen(string2.element.as_string) == 0);
		free_element(string2.element);
	}

	ParseResult string3 = _parse_json("    \"abc\"defghi");
	mu_check(string3.is_success);
	if(string3.is_success) {
		mu_check(string3.element.type == ELEMENT_STRING);
		print_element(string3.element);
		fputc('\n', stdout);
		mu_check(strcmp(string3.element.as_string, "abc") == 0);
		mu_check(strlen(string3.element.as_string) == 3);
		free_element(string3.element);
	}

	ParseResult string4 = _parse_json("  \" a  bc d\""); 
	mu_check(string4.is_success);
	if(string4.is_success) {
		mu_check(string4.element.type == ELEMENT_STRING);
		print_element(string4.element);
		fputc('\n', stdout);
		mu_check(strcmp(string4.element.as_string, " a  bc d") == 0);
		mu_check(strlen(string4.element.as_string) == 8);
		free_element(string4.element);
	}

	ParseResult string5 = _parse_json("hello \"there\"");
	mu_check(!string5.is_success);
	mu_check(string5.element.as_string == NULL);
	free_element(string5.element);
	
	ParseResult string6 = _parse_json("\"there is a new line \n\"");
	mu_check(!string6.is_success);
	mu_check(string6.element.as_string == NULL);
	free_element(string6.element);
	
	ParseResult string7 = _parse_json("\n\"hi\"");
	mu_check(string7.is_success);
	if(string7.is_success) {
		mu_check(string7.element.type == ELEMENT_STRING);
		print_element(string7.element);
		fputc('\n', stdout);
		mu_check(strcmp(string7.element.as_string,  "hi") == 0);
		mu_check(strlen(string7.element.as_string) == 2);
		free_element(string7.element);
	}

	ParseResult string8 = _parse_json("\"abc\\\"");
	mu_check(string8.is_success);
	if(string8.is_success) {
		mu_check(string8.element.type == ELEMENT_STRING);
		print_element(string8.element);
		fputc('\n', stdout);
		mu_check(strcmp(string8.element.as_string, "abc\\") == 0);
		mu_check(strlen(string8.element.as_string) == 4);
		free_element(string8.element);
	}

	ParseResult string9 = _parse_json("\"abc\\z\"");
	mu_check(string9.is_success);
	if(string9.is_success) {
		mu_check(string9.element.type == ELEMENT_STRING);
		print_element(string9.element);
		fputc('\n', stdout);
		mu_check(strcmp(string9.element.as_string, "abc\\z") == 0);
		mu_check(strlen(string9.element.as_string) == 5);
		free_element(string9.element);
	}

	ParseResult string10 = _parse_json("\"bro");
	mu_check(!string10.is_success);
	mu_check(string10.element.as_string == NULL);
	free_element(string10.element);

	ParseResult string11 = _parse_json("\"ab\\tc\"");
	mu_check(string11.is_success);
	if(string11.is_success) {
		mu_check(string11.element.type == ELEMENT_STRING);
		print_element(string11.element);
		fputc('\n', stdout);
		mu_check(strcmp(string11.element.as_string, "ab\tc") == 0);
		mu_check(strlen(string11.element.as_string) == 4);
		free_element(string11.element);
	}

	ParseResult string12 = _parse_json("\"ab\\nc\"");
	mu_check(string12.is_success);
	if(string12.is_success) {
		mu_check(string12.element.type == ELEMENT_STRING);
		print_element(string12.element);
		fputc('\n', stdout);
		mu_check(strcmp(string12.element.as_string, "ab\nc") == 0);
		mu_check(strlen(string12.element.as_string) == 4);
		free_element(string12.element);
	}

	ParseResult string13 = _parse_json("\"ab\\bc\"");
	mu_check(string13.is_success);
	if(string13.is_success) {
		mu_check(string13.element.type == ELEMENT_STRING);
		print_element(string13.element);
		fputc('\n', stdout);
		mu_check(strcmp(string13.element.as_string, "ab\bc") == 0);
		mu_check(strlen(string13.element.as_string) == 4);
		free_element(string13.element);
	}

	ParseResult string14 = _parse_json("\"ab\\fc\"");
	mu_check(string14.is_success);
	if(string14.is_success) {
		mu_check(string14.element.type == ELEMENT_STRING);
		print_element(string14.element);
		fputc('\n', stdout);
		mu_check(strcmp(string14.element.as_string, "ab\fc") == 0);
		mu_check(strlen(string14.element.as_string) == 4);
		free_element(string14.element);
	}

	ParseResult string15 = _parse_json("\"ab\\rc\"");
	mu_check(string15.is_success);
	if(string15.is_success) {
		mu_check(string15.element.type == ELEMENT_STRING);
		print_element(string15.element);
		fputc('\n', stdout);
		mu_check(strcmp(string15.element.as_string, "ab\rc") == 0);
		mu_check(strlen(string15.element.as_string) == 4);
		free_element(string15.element);
	}

	ParseResult string16 = _parse_json("\"ab\\u0021\"");
	mu_check(string16.is_success);
	if(string16.is_success) {
		mu_check(string16.element.type == ELEMENT_STRING);
		print_element(string16.element);
		fputc('\n', stdout);
		mu_check(strcmp(string16.element.as_string, "ab!") == 0);
		mu_check(strlen(string16.element.as_string) == 3);
		free_element(string16.element);
	}

	ParseResult string17 = _parse_json("\"ab\\b and \\nnice\"");
	mu_check(string17.is_success);
	if(string17.is_success) {
		mu_check(string17.element.type == ELEMENT_STRING);
		print_element(string17.element);
		fputc('\n', stdout);
		mu_check(strcmp(string17.element.as_string, "ab\b and \nnice") == 0);
		mu_check(strlen(string17.element.as_string) == 13);
		free_element(string17.element);
	}

	ParseResult string18 = _parse_json("\"ab\\\'c\"");
	mu_check(string18.is_success);
	if(string18.is_success) {
		mu_check(string18.element.type == ELEMENT_STRING);
		print_element(string18.element);
		fputc('\n', stdout);
		mu_check(strcmp(string18.element.as_string, "ab\'c") == 0);
		mu_check(strlen(string18.element.as_string) == 4);
		free_element(string18.element);
	}
	
	ParseResult string19 = _parse_json("\"ab\\\\\"");
	mu_check(string19.is_success);
	if(string19.is_success) {
		mu_check(string19.element.type == ELEMENT_STRING);
		print_element(string19.element);
		fputc('\n', stdout);
		mu_check(strcmp(string19.element.as_string, "ab\\\\") == 0);
		mu_check(strlen(string19.element.as_string) == 4);
		free_element(string19.element);
	}

	ParseResult string20 = _parse_json("\"ab\\\"abc");
	mu_check(!string20.is_success);
	mu_check(string20.element.as_string == NULL);
	free_element(string20.element);
	
	ParseResult string21 = _parse_json("\"ab\\\"\\\'\\b\\n\"");	
	mu_check(string21.is_success);
	if(string21.is_success) {
		mu_check(string21.element.type == ELEMENT_STRING);
		print_element(string21.element);
		fputc('\n', stdout);
		mu_check(strcmp(string21.element.as_string, "ab\"\'\b\n") == 0);
		mu_check(strlen(string21.element.as_string) == 6);
		free_element(string21.element);
	}

	mu_end();
}

int _test_list() {
	mu_start();
	
	ParseResult list1 = _parse_json("[]");
	mu_check(list1.is_success);
	print_element(list1.element);
	fputc('\n', stdout);
	free_element(list1.element);
	
	ParseResult list2 = _parse_json("[0]");
	mu_check(list2.is_success);
	print_element(list2.element);
	fputc('\n', stdout);
	free_element(list2.element);

	ParseResult list3 = _parse_json("[\"abc\"]");
	mu_check(list3.is_success);
	print_element(list3.element);
	fputc('\n', stdout);
	free_element(list3.element);

	ParseResult list4 = _parse_json("[1,[2, [2, [2]]], [1, 2]]AAA");
	mu_check(list4.is_success);
	print_element(list4.element);
	fputc('\n', stdout);
	free_element(list4.element);
	
	ParseResult list5 = _parse_json("  [1,   \"asb\", 3, [1, \"asb\", 3]]");
	mu_check(list5.is_success);
	print_element(list5.element);
	fputc('\n', stdout);
	free_element(list5.element);

	ParseResult list6 = _parse_json("[1, 2 ]");
	mu_check(list6.is_success);
	print_element(list6.element);
	fputc('\n',stdout);
	free_element(list6.element);
	
	ParseResult list7 = _parse_json("[1, \"a\"");
	mu_check(!list7.is_success);
	free_element(list7.element);

	ParseResult list8 = _parse_json("A[1, 2");
	mu_check(!list8.is_success);
	free_element(list8.element);

	ParseResult list9 = _parse_json(",\"a\"]");
	mu_check(!list9.is_success);
	free_element(list9.element);

	ParseResult list10 = _parse_json("[2,,\"s\"]");
	mu_check(!list10.is_success);
	free_element(list10.element);

	ParseResult list11 = _parse_json("[\"]\"]");
	mu_check(list11.is_success);
	print_element(list11.element);
	fputc('\n', stdout);
	free_element(list11.element);

	ParseResult list12 = _parse_json("[[[]]]");
	mu_check(list12.is_success);
	print_element(list12.element);
	fputc('\n', stdout);
	free_element(list12.element);

	ParseResult list13 = _parse_json("[[");
	mu_check(!list13.is_success);
	free_element(list13.element);
	
	ParseResult list14 = _parse_json("[null, [true, false], [1, [\"a\", \"b\"]], \"yeah\"]");
	mu_check(list14.is_success);
	print_element(list14.element);
	fputc('\n', stdout);
	free_element(list14.element);

	ParseResult list15 = _parse_json("]");
	mu_check(!list15.is_success);
	free_element(list15.element);

	ParseResult list16 = _parse_json("[\"A\"[]\"B\"]");
	mu_check(!list16.is_success);
	free_element(list16.element);

	ParseResult list17 = _parse_json("[5,]");
	mu_check(!list17.is_success);
	free_element(list17.element);

	mu_end();
}

int _test_null() {
	mu_start();
	
	ParseResult null1 = _parse_json("null");
	mu_check(null1.is_success);
	print_element(null1.element);
	fputc('\n', stdout);

	ParseResult null2 = _parse_json("   nullasdf");
	mu_check(null2.is_success);
	print_element(null2.element);
	fputc('\n', stdout);

	ParseResult null3 = _parse_json("nsull");
	mu_check(!null3.is_success);

	ParseResult null4 = _parse_json("nu ll");
	mu_check(!null4.is_success);
	
	ParseResult null5 = _parse_json("NULL");
	mu_check(!null5.is_success);

	mu_end();
}

int _test_bool() {
	mu_start();
	
	ParseResult bool1 = _parse_json("true");
	mu_check(bool1.is_success);
	print_element(bool1.element);
	fputc('\n', stdout);

	ParseResult bool2 = _parse_json("false");
	mu_check(bool2.is_success);
	print_element(bool2.element);
	fputc('\n', stdout);

	ParseResult bool3 = _parse_json("truefalse");
	mu_check(bool3.is_success);
	print_element(bool3.element);
	fputc('\n', stdout);

	ParseResult bool4 = _parse_json(" fa lse");
	mu_check(!bool4.is_success);

	ParseResult bool5 = _parse_json("TRUE");
	mu_check(!bool5.is_success);

	mu_end();
}

int _test_object() {
	mu_start();

	ParseResult object1 = _parse_json("{\"key\": [1, 2], \"apple\": 2}");
	mu_check(object1.is_success);
	print_element(object1.element);
	fputc('\n', stdout);
	free_element(object1.element);

	ParseResult object2 = _parse_json("{}");
	mu_check(object2.is_success);
	print_element(object2.element);
	fputc('\n', stdout);
	free_element(object2.element);

	ParseResult object3 = _parse_json("{\"apple\": [\"boys\", \"girls\"], \"bananas\": 8, \"orange\": \"not a boy\"}");
	mu_check(object3.is_success);
	print_element(object3.element);
	fputc('\n', stdout);
	free_element(object3.element);
	
	ParseResult object4 = _parse_json("{\"apple\": [\"boys\", \"girls\"], \"banana\": 8, \"orange\": \"not boy\",       \"oh\": [12,25]}");
	mu_check(object4.is_success);
	print_element(object4.element);
	fputc('\n', stdout);
	free_element(object4.element);

	ParseResult object5 = _parse_json("{\"apple\": [\"boys\", \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object5.is_success);
	free_element(object5.element);

	ParseResult object6 = _parse_json("{\"apple\": [\"boys\", \"girls\", {\"key\": 123}], \"banana\": 8, \"orange\": \"not boy\",       \"oh\": [12,25]}");
	mu_check(object6.is_success);
	print_element(object6.element);
	fputc('\n', stdout);
	free_element(object6.element);

	ParseResult object7 = _parse_json("{apple\": [\"boys\", \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object7.is_success);
	free_element(object7.element);

	ParseResult object8 = _parse_json("{\"apple\" [\"boys\", \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object8.is_success);
	free_element(object8.element);

	ParseResult object9 = _parse_json("{\"apple\": [\"boys\" \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object9.is_success);
	free_element(object9.element);
	
	ParseResult object10 = _parse_json("{\"apple\": [\"boys\", \"girls\"] \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object10.is_success);
	free_element(object10.element);

	ParseResult object11 = _parse_json("{\"apple\": [\"boys\", \"girls\"],, \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object11.is_success);
	free_element(object11.element);
	
	ParseResult object12 = _parse_json("{{\"apple\": [\"boys\", \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}");
	mu_check(!object12.is_success);
	free_element(object12.element);

	ParseResult object13 = _parse_json("{\"apple\": [\"boys\", \"girls\"], \"banana\": 8, \"orange\": not boy\",      \"oh\": [12,25]}}");
	mu_check(!object13.is_success);
	free_element(object13.element);
	
	ParseResult object14 = _parse_json("{\"key\": {}}");
	mu_check(object14.is_success);
	print_element(object14.element);
	fputc('\n', stdout);
	free_element(object14.element);
	
	ParseResult object15 = _parse_json("{\"key\": }");
	mu_check(!object15.is_success);
	free_element(object15.element);

	ParseResult object16 = _parse_json("{\"key\": [1, 2], \"apple\": 2, \"zebra\": \"animal\", \"grapes\": [400,800]}");
	mu_check(object16.is_success);
	print_element(object16.element);
	fputc('\n', stdout);
	free_element(object16.element);

	mu_end();
}

int main() {
	mu_run(_test_int);
	mu_run(_test_string);
	mu_run(_test_list);
	mu_run(_test_null);
	mu_run(_test_bool);
	mu_run(_test_object);

	return EXIT_SUCCESS;
}
