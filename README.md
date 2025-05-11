# Simple Browser Simulator

A lightweight “browser” implementation in C that models basic tab management and navigation history using linked lists and stacks. This README explains the overall architecture, data structures, and each function in detail to help you understand, maintain, and extend the code.

## Table of Contents

- [Introduction](#introduction)  
- [Data Structures](#data-structures)  
- [Initialization Functions](#initialization-functions)  
- [Tab Management Functions](#tab-management-functions)  
- [Navigation Functions](#navigation-functions)  
- [I/O and Parsing](#io-and-parsing)  
- [Memory Management](#memory-management)  
- [Main Program Flow](#main-program-flow)  
- [Building and Running](#building-and-running)  

---

## Introduction

This project simulates a minimal browser environment. It supports:
- Multiple tabs in a circular doubly-linked list.
- Backward/forward page navigation in each tab using two stacks.
- Opening, closing, switching, and listing tabs.
- Loading pages by ID from an input file.
- Printing current tab status and full navigation history.

Designed for clarity and teaching of linked lists and stack-based history, it can be extended to support bookmarking, tab grouping, or persistent storage.

---

## Data Structures

```c
typedef struct page {
    int    ID;               // Unique page identifier (0 = implicit home page)
    char   url[50];          // URL string
    char  *description;      // Dynamically allocated description
} page;

typedef struct node {
    struct node *next;       // Next stack node
    page        *holded_page;// Pointer to page stored in this node
} node;

typedef struct stack {
    node *top;               // Top of the stack
    int   len;               // Number of elements
} stack;

typedef struct tab {
    int    ID;               // Tab identifier
    page  *currentPage;      // Currently displayed page
    stack *backwardStack;    // History backward stack
    stack *forwardStack;     // History forward stack
} tab;

typedef struct tab_node {
    struct tab_node *next;   // Next tab_node in circular list
    struct tab_node *prev;   // Previous tab_node in circular list
    tab            *holded_tab; // Pointer to the tab data
} tab_node;

typedef struct tabsList {
    tab_node *sentinel;      // Sentinel node for circular list
    int       len;           // Number of open tabs
} tabsList;

typedef struct browser {
    tab       *current;      // Currently active tab
    tabsList   list;         // All open tabs
} browser;

## Initialization Functions

### `DISPLAY_ERROR_MESSAGE(FILE *out_file)`
Prints a standard “403 Forbidden” message to the given output file when an invalid operation is attempted.

### `IS_EMPTY_TABLIST(tabsList *tab_list)`
Returns 1 if there are no tabs open (i.e., `len == 0`), or 0 otherwise.

### `CREATE_IMPLICIT_PAGE()`
Allocates and returns a new `page` struct for the implicit “home” page:
- **ID**: 0  
- **URL**: `"https://acs.pub.ro/"`  
- **description**: dynamically allocated string `"Computer Science"`

### `INIT_STACKS(tab *current_tab)`
Allocates and initializes two empty `stack` objects on the given `tab`:
- `backwardStack`  
- `forwardStack`  
Each stack’s `top` pointer is set to `NULL` and length to 0.

### `INIT_BROWSER(browser *Chrome_x)`
Sets up the browser’s initial state:
1. Creates the implicit home tab (`ID = 0`) with its page and stacks.  
2. Allocates the circular list’s sentinel node.  
3. Inserts the home tab into the list so that `sentinel->next` and `sentinel->prev` both point to it.  
4. Sets `Chrome_x->current` to the home tab, and `list.len = 1`.

---

## Tab Management Functions

### `NEW_TAB(browser *Chrome_x, int *max_tabs)`
Opens a new tab:
1. Increments `*max_tabs` and assigns the new tab its ID.  
2. Creates a fresh implicit page and empty stacks.  
3. Inserts a new `tab_node` just before the sentinel in the circular list.  
4. Sets `Chrome_x->current` to the newly created tab and updates `list.len`.

### `CLOSE(browser *Chrome_x, FILE *out_file)`
Closes the currently active tab:
- If the tab’s ID is 0, writes a “403 Forbidden” error and returns.  
- Otherwise, relinks the neighbors in the circular list to remove the current node.  
- Frees the tab’s stacks and implicit page (if any), then the tab struct.  
- Updates `Chrome_x->current` to the tab on the immediate left.

### `OPEN(browser *Chrome_x, char *operation, FILE *out_file)`
Switches to a specific tab by ID:
1. Parses the target ID from `operation`.  
2. If it matches `current->ID`, does nothing.  
3. Otherwise, searches the list; if found, updates `Chrome_x->current`.  
4. If not found, writes “403 Forbidden” to `out_file`.

### `NEXT(browser *Chrome_x)` and `PREV(browser *Chrome_x)`
Navigate among tabs in circular fashion:
- `NEXT` moves `current` to the tab on its right (wrapping around after the last).  
- `PREV` moves to the tab on its left (wrapping to the last if at the first).

### `PRINT(browser *Chrome_x, FILE *out_file)`
Outputs two lines:
1. **Tab IDs**: space-separated IDs starting from the current tab and wrapping around once.  
2. **Current Page Description**: the `description` string of `current->currentPage`.

---

## Navigation Functions

### `PUSH_STACK(stack *stack_push, page *page_push)`
Pushes the given `page` pointer onto the top of `stack_push`, updating its `len`.

### `POP_STACK(stack *stack_pop)`
Removes and returns the top `page` pointer from `stack_pop`. Returns `NULL` if the stack is empty.

### `DISPLAY_STACK(stack *stack_display, FILE *out_file)`
Iterates from the top node down, printing each stored page’s `url` on its own line.

### `IS_EMPTY_STACK(stack *check_stack)`
Returns 1 if `check_stack->top` is `NULL`, or 0 otherwise.

### `PAGE(browser *Chrome_x, char *operation, page *page_vector, int *page_count, FILE *out_file)`
Loads a page by ID into the current tab:
1. Parses the target page ID.  
2. Searches `page_vector` for a matching `ID`.  
3. If found:
   - Pushes the existing `currentPage` onto `backwardStack`.  
   - Clears `forwardStack`.  
   - Sets `currentPage` to the new page.  
4. If not found, writes “403 Forbidden.”

### `BACKWARD(browser *Chrome_x, FILE *out_file)` and `FORWARD(browser *Chrome_x, FILE *out_file)`
Move through the current tab’s history:
- **BACKWARD**: if `backwardStack` is non-empty, pushes `currentPage` onto `forwardStack`, pops from `backwardStack`, and makes it the new current. Otherwise, error.  
- **FORWARD**: similarly moves one step forward, swapping stacks.

### `PRINT_HISTORY(browser *Chrome_x, char *operation, FILE *out_file)`
Prints the full history for a given tab ID:
1. Parses the tab ID.  
2. Locates the matching `tab_node`.  
3. If found:
   - Prints forward-stack URLs (top to bottom).  
   - Prints the current page’s URL.  
   - Prints backward-stack URLs (top to bottom).  
4. Otherwise, writes “403 Forbidden.”

---

## I/O and Parsing

### `PARSE_INPUT(page *page_vector, char ***operation_vector, int *page_count, int *operation_count)`
Reads `tema1.in`:
1. Integer `page_count`.  
2. For each page: reads `ID`, `url`, and `description` line.  
3. Integer `operation_count`.  
4. Reads each operation line into a dynamically allocated string array.

### `PERFORM_OPERATIONS(char **operation_vector, browser *Chrome_x, int *operation_count, int *page_count, page *page_vector, FILE *out_file, int *max_tabs)`
Loops through each operation string:
1. Determines its type by matching against known keywords (`NEW_TAB`, `PAGE`, etc.).  
2. Calls the corresponding function with parsed arguments.  

---

## Memory Management

### `FREE_PAGE(page **page_erase)`
Frees the `description` string and the `page` struct itself.

### `FREE_STACK(stack **stack_erase, int absolute_free)`
Walks and frees all `node`s in the stack, freeing any implicit pages (ID 0). If `absolute_free` is 1, also frees the `stack` struct.

### `FREE_TAB(tab_node *tab_node_erase)`
Frees both stacks and any implicit current page, then the `tab` struct.

### `FREE_BROWSER(browser *Chrome_x)`
Iterates and frees every `tab_node` in the circular list (calls `FREE_TAB`), then frees the sentinel node and resets browser fields.

---

## Main Program Flow

``` c
int main() {
    browser Chrome;
    tabsList initList = { NULL, 0 };
    page page_vector[50];
    char **operation_vector;
    int page_count = 0, operation_count = 0, max_tabs = 0;
    FILE *out_file = fopen("tema1.out", "w");

    INIT_BROWSER(&Chrome);
    PARSE_INPUT(page_vector, &operation_vector, &page_count, &operation_count);
    PERFORM_OPERATIONS(operation_vector, &Chrome, &operation_count,
                       &page_count, page_vector, out_file, &max_tabs);
    fclose(out_file);

    FREE_BROWSER(&Chrome);
    for (int i = 0; i < operation_count; i++) {
        free(operation_vector[i]);
    }
    free(operation_vector);
    for (int i = 0; i < page_count; i++) {
        free(page_vector[i].description);
    }
    return 0;
}
```

## Building and Running

Use the following Makefile targets to build, clean, run, and test the simulator:

``` makefile
build:
	gcc ./main.c -o tema1 -Wall -Wextra -g

clean:
	rm -f ./tema1*

run:
	./tema1

test:
	./checker.sh

```
