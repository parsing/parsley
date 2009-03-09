// typedef struct __pxpath_node {
// 	int type;
// 	char* value;
//  	__pxpath_node * next;
//  	__pxpath_node * child;
// } pxpath_node;
// 
// typedef pxpath_node pxpathPtr;
// 
// enum {
//    PXPATH_FUNCTION,
// 	 PXPATH_PATH
// };

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "parsed_xpath.h"
#include "printbuf.h"

pxpathPtr pxpath_new(int type, char* value) {
	pxpathPtr ptr = (pxpathPtr) calloc(sizeof(pxpath_node), 1);
	ptr->value = strdup(value);
	ptr->type = type;
	ptr->next = NULL;
	ptr->child = NULL;
	return ptr;
}

// pxpathPtr pxpath_cat_paths(int n, va_list) {
// 	
// }
// pxpathPtr pxpath_new_paths(int n, va_list) {
// 	
// }

static void
_pxpath_to_string(pxpathPtr ptr, struct printbuf *buf) {	
	if(ptr->type == PXPATH_FUNCTION) {
		sprintbuf(buf, "%s(", ptr->value);
		pxpathPtr next = ptr->child;
		while(next != NULL) {
			char* comma = (next->next == NULL) ? "" : ", ";
			_pxpath_to_string(next, buf);
			sprintbuf(buf, "%s", comma);
			next = next->next;
		}
		sprintbuf(buf, ")");
	} else {
		sprintbuf(buf, "%s", ptr->value);
	}
}

static char * 
format_n(int n) {
  char * out = calloc(2 * n + 1, 1);
  for(int i =0; i < n; i++) {
    strcat(out, "%s");
  }
}

char* pxpath_to_string(pxpathPtr ptr) {
	struct printbuf *buf = printbuf_new();
	_pxpath_to_string(ptr, buf);
	char *out = strdup(buf->buf);
	free(buf);
	return out;
}

void pxpath_free(pxpathPtr ptr) {
	if(ptr == NULL) return;
	free(ptr->value);
	pxpath_free(ptr->next);
	pxpath_free(ptr->child);
	free(ptr);
}

pxpathPtr pxpath_new_func(char* value, pxpathPtr child) {
	pxpathPtr ptr = pxpath_new(PXPATH_FUNCTION, value);
	ptr->child = child;
	return ptr;
}

pxpathPtr pxpath_cat_paths(int n, ...) {
	struct printbuf *buf = printbuf_new();
  va_list va;
  va_start(va, n);
  pxpathPtr last;
  pxpathPtr first;
  for(int i = 0; i < n; i++) {
    pxpathPtr ptr = va_arg(va, pxpathPtr);
    sprintbuf(buf, "%s", pxpath_to_string(ptr));
    if(i == 0) first = ptr;
    if(i != 0) last->next = ptr;
    last = ptr;
  }
  pxpathPtr out = pxpath_new(PXPATH_PATH, buf->buf);
  printbuf_free(buf);
  out->child = first;
  return out;
}

pxpathPtr pxpath_new_path(int n, ...) {
  va_list va;
  char * value;
  va_start(va, n);
  vasprintf(&value, format_n(n), va);
  va_end(va);
  pxpathPtr ptr = pxpath_new(PXPATH_PATH, value);
  free(value);
  return ptr;
}

pxpathPtr pxpath_new_literal(int n, ...) {
  va_list va;
  char * value;
  va_start(va, n);
  vasprintf(&value, format_n(n), va);
  va_end(va);
  pxpathPtr ptr = pxpath_new(PXPATH_LITERAL, value);
  free(value);
  return ptr;
}