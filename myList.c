
#include "myList.h"

static void appendToGen_list(gen_list* list, gen_node* new);
static void prependToGen_list(gen_list* list, gen_node* new);
static void removeFromGen_list(gen_list* list, gen_node* node);

static void appendToGen_list(gen_list* list, gen_node* new) {
  if (list->start == NULL) {
    list->start = new;
  }
  if (list->end != NULL) {
    list->end->next = new;
    new->prev = list->end;
  }
  else {
    new->prev = NULL;
    list->start = new;
  }
  list->end = new;
  new->next = NULL;
}

static void prependToGen_list(gen_list* list, gen_node* new) {
  if (list->end == NULL) {
    list->end = new;
  }
  if (list->start != NULL) {
    list->start->prev = new;
    new->next = list->start;
  }
  else {
    new->next = NULL;
    list->end = new;
  }
  list->start = new;
  new->prev = NULL;
}

static void removeFromGen_list(gen_list* list, gen_node* node) {
  if (node != NULL) {
    if (node->prev == NULL && node->next == NULL) {
      list->start = NULL;
      list->end = NULL;
    }
    else {
      if (node->prev == NULL) {
	list->start = node->next;
	node->next->prev = NULL;
      }
      else if (node->next == NULL ){
	list->end = node->prev;
	node->prev->next = NULL;
      }
      else {
	node->prev->next = node->next;
	node->next->prev = node->prev;
      }
    }
    node->next = NULL;
    node->prev = NULL;
  }
}

gen_list* createNewGen_list() {
  gen_list* new = malloc(sizeof(gen_list));
  new->start = NULL;
  new->end = NULL;
  return new;
}

gen_node* createNewGen_node(void* data) {
  gen_node* new = malloc(sizeof(gen_node));
  new->next = NULL;
  new->prev = NULL;
  new->stored = data;
  return new;
}

void freeGen_node(gen_node* old) {
  free(old->stored);
  free(old);
}

void freeGen_list(gen_list* old) {
  gen_node* temp;
  gen_node* current;
  current = old->start;
  while(current != NULL) {
    temp = current;
    current = temp->next;
    freeGen_node(temp);
  }
  free(old);
}





