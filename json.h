#ifndef __JSON_H__
#define __JSON_H__
#include <stdbool.h>
#define BONUS_JSON_ESCAPE_SEQUENCES
#define BONUS_JSON_CONSTANTS

typedef struct {
	enum  { ELEMENT_INT, ELEMENT_STRING, ELEMENT_LIST, ELEMENT_OBJECT, ELEMENT_NULL, ELEMENT_BOOL} type;
	union {
		int    as_int;
		char*  as_string;
		struct _Node* as_list;
		struct _BSTNode* as_object;
		void* as_null;
		bool as_bool;
	}; // ANONYMOUS union (C11).
} Element;


typedef struct _Node {
	struct _Node* next;
	Element element;
} Node;

typedef struct _BSTNode {
	struct _BSTNode* left;
	struct _BSTNode* right;
	Element element;
	char* key;
} BSTNode;

bool parse_int(int* a_value, char** a_pos);
bool parse_string(char** a_s, char** a_pos);
bool parse_element(Element* a_element, char** a_pos);
bool parse_list(Node** a_head, char** a_pos);
bool parse_object(BSTNode** a_root, char** a_pos);
void print_element(Element element);
void free_element(Element element);

#endif

#define JSON_H_VERSION_1
/* vim: set tabstop=4 shiftwidth=4 fileencoding=utf-8 noexpandtab: */
