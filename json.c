#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "clog.h"
#include "json.h"

void _free_list(Node* head);
void _escape_sequences(char** a_pos, int* size, char** a_string, int* idx, bool* has_comp_escape, int* escape_counter);
bool _parse_null(void** a_null, char** a_pos);
bool _parse_bool(bool* a_bool, char** a_pos);
void _insert_node_bst(BSTNode** a_root, char* key, Element element);
void _print_bst(BSTNode const* root);
void _free_bst(BSTNode** a_root);

bool parse_int(int* a_value, char** a_pos) {
	bool is_int = false;
	bool is_negative = false;
	if(**a_pos == '-') {
		(*a_pos)++;
		is_negative = true;
		if(isdigit(**a_pos)) {
			is_int = true;
		}
	}
	if(isdigit(**a_pos)) {
		is_int = true;
	}
	int num = 0;
	while(isdigit(**a_pos)) {
		int dig = **a_pos - '0';
		num = dig + 10 * num;
		(*a_pos)++;
	}
	*a_value = num;
	if(is_negative) {
		*a_value = -1 * *a_value;
	}
	return is_int;
}

bool parse_string(char** a_string, char** a_pos) {
	bool is_string = false;
	int str_counter = 1;
	bool has_complex_escape = false;
	int escape_counter = 1;
		
	if(**a_pos == '\"') {
		is_string = true;
		(*a_pos)++;
	}
	while(**a_pos != '\"') {
		if(**a_pos == '\n' || **a_pos == '\0') {
			is_string = false;
			break;
		}
		if(**a_pos == '\\' && *(*a_pos + 2) != '\0') {
			_escape_sequences(a_pos, &str_counter, NULL, NULL, &has_complex_escape, &escape_counter);
		}
		else {
			str_counter++;
		}
		(*a_pos)++;
	}
	int no_str_counter = 0;
	int no_esc_counter = 0;
	if(is_string) {
		*a_string = malloc(sizeof(**a_string) * str_counter);
		(*a_pos) -= str_counter - 1;
		if(has_complex_escape) {
			(*a_pos) -= escape_counter - 1;
		}
		for(int idx = 0; idx < str_counter - 1; idx++) {
			if(**a_pos == '\\' && *(*a_pos + 2) != '\0') {
				_escape_sequences(a_pos, &no_str_counter, a_string, &idx, &has_complex_escape, &no_esc_counter);
			}
			else {
				(*a_string)[idx] = **a_pos;
			}
			(*a_pos)++;
		}
		*(*a_string + str_counter - 1) = '\0';
	}
	(*a_pos)++;
	return is_string;
}

void _escape_sequences(char** a_pos, int* size, char** a_string, int* idx, bool* has_comp_escape, int* escape_counter) {
	if(*(*a_pos + 1) == '\"') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\"';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == '\'') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\'';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 'b') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\b';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 'f') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\f';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 't') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\t';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 'n') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\n';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 'r') {
		*has_comp_escape = true;
		(*escape_counter)++;
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\r';
		}
		(*a_pos)++;
	}
	else if(*(*a_pos + 1) == 'u' && *(*a_pos + 2) == '0' && *(*a_pos + 3) == '0') {
		*has_comp_escape = true;
		(*escape_counter) += 5;
		if(a_string == NULL) {
			(*size)++;
			(*a_pos) += 4;
		}
		else {
			(*a_pos) += 4;
			int first_digit = (*(*a_pos)++ - '0') * 16;
			int second_digit = **a_pos - '0';
			char unicode_char = first_digit + second_digit;
			(*a_string)[*idx] = unicode_char;
			(*a_pos)--;
		}
		(*a_pos)++;
	}
	else {
		if(a_string == NULL) {
			(*size)++;
		}
		else {
			(*a_string)[*idx] = '\\';
		}
	}
}

bool parse_list(Node** a_head, char** a_pos) {
	bool is_list = false;
	if(**a_pos == '[') {
		is_list = true;
		(*a_pos)++;
		Node* tail = NULL;
		Node* new_head = NULL;
		while(**a_pos != ']') {	
			if(**a_pos == ',') {
				(*a_pos)++;
				if(**a_pos == ',') {
					_free_list(new_head);
					is_list = false;
					break;
				}
			}
			Node* add_node = malloc(sizeof(*add_node));
			add_node -> next = NULL;
			bool is_success = parse_element(&(add_node -> element), a_pos);
			if(!is_success) {
				_free_list(new_head);
				free(add_node);
				is_list = false;
				break;
			}
			if(new_head == NULL) {
				new_head = add_node;
				tail = add_node;
			}
			else {
				tail -> next = add_node;
				tail = add_node;
			}

			while(isspace(**a_pos)) {
				(*a_pos)++;
			}
			if(**a_pos != ',' && **a_pos != ']') {
				_free_list(new_head);
				is_list = false;
				break;
			}
		}
		*a_head = new_head;
		(*a_pos)++;
	}
	return is_list;
}

void _insert_node_bst(BSTNode** a_root, char* key, Element element) {
	if(*a_root == NULL) {
		BSTNode* add_node = malloc(sizeof(*add_node));
		add_node -> key = key;
		add_node -> element = element;
		add_node -> left = NULL;
		add_node -> right = NULL;
		*a_root = add_node;
	}
	else if(strcmp(key, (*a_root) -> key) < 0) {
		_insert_node_bst(&((*a_root) -> left), key, element);
	}
	else {
		_insert_node_bst(&((*a_root) -> right), key, element);
	}
}

bool parse_object(BSTNode** a_root, char** a_pos) {
	bool is_object = false;
	if(**a_pos == '{') {
		is_object = true;
		(*a_pos)++;
		BSTNode* new_root = NULL;
		while(**a_pos != '}') {
			while(isspace(**a_pos)) {
				(*a_pos)++;
			}
			char* key;
			bool is_key = parse_string(&key, a_pos);
			if(!is_key) {
				is_object = false;
				_free_bst(&new_root);
				break;
			}
			while(isspace(**a_pos)) {
				(*a_pos)++;
			}
			if(**a_pos != ':') {
				is_object = false;
				free(key);
				_free_bst(&new_root);
				break;
			}
			(*a_pos)++;
			Element element;
			bool is_element = parse_element(&element, a_pos);
			if(!is_element) {
				free(key);
				_free_bst(&new_root);
				is_object = false;
				break;
			}
			_insert_node_bst(&new_root, key, element);
			while(isspace(**a_pos)) {
				(*a_pos)++;
			}
			if(**a_pos != ',' && **a_pos != '}') {
				_free_bst(&new_root);
				is_object = false;
				break;
			}
			if(**a_pos != '}') {
				(*a_pos)++;
			}
		}
		*a_root = new_root;
		(*a_pos)++;
	}
	return is_object;
}


bool _parse_null(void** a_null, char** a_pos) {
	bool is_null = false;
	if(**a_pos == 'n' && *(*a_pos + 1) == 'u' && *(*a_pos + 2) == 'l' && *(*a_pos + 3) == 'l') {
		is_null = true;
		*a_pos = (*a_pos) + 4;
		*a_null = NULL;
	}
	return is_null;
}

bool _parse_bool(bool* a_bool, char** a_pos) {
	bool is_bool = false;
	if(**a_pos == 't' && *(*a_pos + 1) == 'r' && *(*a_pos + 2) == 'u' && *(*a_pos + 3) == 'e') {
		is_bool = true;
		*a_pos = (*a_pos) + 4;
		*a_bool = true;
	}
	else if(**a_pos == 'f' && *(*a_pos + 1) == 'a' && *(*a_pos + 2) == 'l' && *(*a_pos + 3) == 's' && *(*a_pos + 4) == 'e') {
		is_bool = true;
		*a_pos = (*a_pos) + 5;
		*a_bool = false;
	}
	return is_bool;
}
bool parse_element(Element *a_element, char** a_pos) {
	bool is_element = false;
	while(isspace(**a_pos)) {
		(*a_pos)++;
	}
	if(isdigit(**a_pos) || (**a_pos) == '-') {
		a_element -> type = ELEMENT_INT;
		is_element = parse_int(&(a_element -> as_int), a_pos);
	}
	else if(**a_pos == '\"') {
		a_element -> type = ELEMENT_STRING;
		is_element = parse_string(&(a_element -> as_string), a_pos);
	}
	else if(**a_pos == '[') {
		a_element -> type = ELEMENT_LIST;
		is_element = parse_list(&(a_element -> as_list), a_pos);
	}
	else if(**a_pos == 'n') {
		a_element -> type = ELEMENT_NULL;
		is_element = _parse_null(&(a_element -> as_null), a_pos);
	}
	else if(**a_pos == 't' || **a_pos == 'f') {
		a_element -> type = ELEMENT_BOOL;
		is_element = _parse_bool(&(a_element -> as_bool), a_pos);
	}
	else if(**a_pos == '{') {
		a_element -> type = ELEMENT_OBJECT;
		is_element = parse_object(&(a_element -> as_object), a_pos);
	}
	return is_element;
}

void print_element(Element element) {	
	if(element.type == ELEMENT_INT) {
		printf("%d", element.as_int);
	}
	else if(element.type == ELEMENT_STRING) {
		printf("\"%s\"", element.as_string);
	}
	else if(element.type == ELEMENT_LIST) {
		printf("[");
		for(Node* curr = element.as_list; curr != NULL; curr = curr -> next) {
			print_element(curr -> element);
			if(curr -> next != NULL) {
				printf(",");
			}
		}
		printf("]");
	}
	else if(element.type == ELEMENT_NULL) {
		printf("null");
	}
	else if(element.type == ELEMENT_BOOL) {
		if(element.as_bool) {
			printf("true");
		}
		else {
			printf("false");
		}
	}
	else if(element.type == ELEMENT_OBJECT) {
		printf("{");
		_print_bst(element.as_object);
		printf("}");
	}
}

void _print_bst(BSTNode const* root) {
	if(root != NULL) {
		if(root -> left != NULL) {
			_print_bst(root -> left);
			printf(", ");
		}
		printf("\"%s\":", root -> key);
		print_element(root -> element);
		if(root -> right != NULL) {
			printf(", ");
			_print_bst(root -> right);
		}
	}
}

void free_element(Element element) {
	if(element.type == ELEMENT_STRING) {
		free(element.as_string);
	}
	else if(element.type == ELEMENT_LIST) {
		Node* head = element.as_list;
		_free_list(head);
	}
	else if(element.type == ELEMENT_OBJECT) {
		BSTNode* root = element.as_object;
		_free_bst(&root);
	}
}

void _free_bst(BSTNode** a_root) {
	if(*a_root != NULL) {
		_free_bst(&((*a_root) -> left));
		_free_bst(&((*a_root) -> right));
		free_element((*a_root) -> element);
		free((*a_root) -> key);
		free(*a_root);
		*a_root = NULL;
	}
}

void _free_list(Node* head) {	
	while(head != NULL) {
		Node* temp = head;
		head = head -> next;
		free_element(temp -> element);	
		free(temp);
	}
}
