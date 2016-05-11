#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>  // true, false
#include <unistd.h>   // getopt

#define MAXTOKENS 100
#define MAXTOKENLEN 64
#define NBCLASSIFIER 15

/*
 * a first attempt to deal with c declaration
 *
 * this version ignore function arguments
 *
 * TODO:
 * (1) replace sprintf with snprintf
 # (2) replace calloc, snprintf, strncat with asprintf(&cmd, "strings %s", in);
 */

static bool VERBOSE = false;

typedef enum typetag_ { IDENTIFIER, QUALIFIER, TYPE} typetag_t;

typedef struct token_ {
  char type;
  char str[MAXTOKENLEN];
} token_t;

typedef struct stack_ {
  int top;
  token_t cell[MAXTOKENS];
} stack_t;

typedef struct buffer_ {
  const char *cont;
  unsigned int pos;
} buffer_t;

typedef struct classifier_ {
  char *name;
  typetag_t type;
} classifier_t;

classifier_t g_CLSTR[NBCLASSIFIER] = {           // update this above if yiu add a line here !!!!
  { .name = "const", .type = QUALIFIER },
  { .name = "volatile", .type = QUALIFIER },
  { .name = "void", .type = TYPE },
  { .name = "char", .type = TYPE },
  { .name = "signed", .type = TYPE },
  { .name = "unsigned", .type = TYPE },
  { .name = "short", .type = TYPE },
  { .name = "int", .type = TYPE },
  { .name = "long", .type = TYPE },
  { .name = "float", .type = TYPE },
  { .name = "double", .type = TYPE },
  { .name = "bool", .type = TYPE },
  { .name = "struct", .type = TYPE },
  { .name = "union", .type = TYPE },
  { .name = "enum", .type = TYPE },
};



/*
 * usual stack operations
 */

static token_t pop(stack_t *stack) {
  if (stack->top > -1) {
    token_t t = stack->cell[stack->top];
    stack->top--;
    return t;
  }
  else {
    perror("[!] stack is empty");
    exit(EXIT_FAILURE);
  }         
}

static void push(stack_t *stack, token_t t) {
  stack->top++;
  if (stack->top > MAXTOKENS) {
    perror("[!] stack is full");
    exit(EXIT_FAILURE);
  }
  stack->cell[stack->top] = t;
}


/*
 * utilities
 */
static int usage(const char *fname) {
  printf("\nusage:\n\t%s -h |\n", fname);
  printf("\t%s [-v] '<c declaration to analyze>;\n", fname);
  printf("\nexample:\n\t%s 'char* const *(*next)();'\n", fname);
  exit(EXIT_SUCCESS);
}

static unsigned long max_of(unsigned long s1, unsigned long s2) {
  return (s1 > s2) ? s1 : s2;
}

static void *_realloc(void *ptr, size_t sz) {
  char *s = realloc(ptr, sz);
  if (s != NULL) return s;
  else {
    perror("[!] realloc error - could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

static void *_calloc(size_t n, size_t size) {
  char *str = (char *) calloc(n, size);   // cast not required as calloc return a void *
  if (str != NULL) return str;            // this return is valid as we are using the heap
  else {
    perror("[!] calloc error - could not allocate memory");
    exit(EXIT_FAILURE);
  }
}

static void *str_append(char **str_res, const char *str) {
  unsigned long n  = strlen(str);
  unsigned long nv = strlen(*str_res);
  *str_res = (char *) _realloc(*str_res, sizeof(char) * (n + nv));
  char *s = strncat(*str_res, str, n);
  if (s == NULL) {
    perror("[!] strncat error - could not allocate memory");
    exit(EXIT_FAILURE);
  }
  else 
    return s;
}

typetag_t classify_string(const char *str) { 
  for (unsigned short i = 0; i < NBCLASSIFIER; i++) {
    const char *item = g_CLSTR[i].name;
    if (strncmp(str, item, strlen(item)) == 0) return g_CLSTR[i].type;
  }
  // otherwise:
  return IDENTIFIER;
}

int get_nchar(buffer_t *buff) {
  int pos = buff->pos;
  if (buff->pos < strlen(buff->cont)) {
    buff->pos++;
    return buff->cont[pos];
  }
  else {
    buff->pos--;
    return EOF;
  }
}

void unget_char(buffer_t *buff) {
  buff->pos--; // or eof if ... TODO ...
}

token_t get_token(buffer_t *buff) {
  //
  if (VERBOSE) printf("[get_token] starts.... pos is %d - buff[cont]: <%s> - buff[len]: %lu\n", buff->pos, buff->cont, strlen(buff->cont));
  unsigned int n = (unsigned int)strlen(buff->cont) - buff->pos; // in theory, possible truncation here - SHOULD NOT HAPPEN THOUGH
  char *p = (char *) _calloc(n, sizeof(char)); // need to make a copy of buff->cont[buf->pos .. n] 
  memcpy(p, &buff->cont[buff->pos], n); // or strncpy(p, &buff->cont[buff->pos], n);
  token_t this;
  while (get_nchar(buff) == ' ') { ++p; }
  char *s = p;  // alias on p
  if (VERBOSE) printf("[get_token] p is now [%s]\n", p);
  if (isalnum(*p)) { // starts with a-z, A-Z or 0-9 => id ?
    while (isalnum(*++p = get_nchar(buff))); // until we reach a non alpha character
    unget_char(buff); 
    *p = '\0';                               // terminate this tring
    memcpy(this.str, s, strlen(s) + 1);      // strncpy(this.str, s, strlen(s) + 1);
    this.type = classify_string(s);          // classify 
  }
  else if (*p == '*') {
    char *_s = "pointer to ";              
    memcpy(this.str, _s, strlen(_s));        // strncpy(this.str, _s, strlen(_s));
    this.type = '*';
  }
  else {
    memcpy(this.str, p, 1);                  // strncpy(this.str, p, 1); 
    this.str[1] = '\0';
    this.type = p[0];                   
  }
  free(p);
  if (VERBOSE) printf("[get_token] - this.str = [%s] - type [%c]\n\n", this.str, this.type);
  return this;
}

token_t read_to_first_id(buffer_t *buff, stack_t *stack, char **str_res) {
  //
  if (VERBOSE) printf("[read_to_first_id] starts...\n");
  token_t this = get_token(buff);  
  while (this.type != IDENTIFIER) {
    push(stack, this); 
    if (VERBOSE) printf("[read_to_first_id] push <<%s>> onto stack\n", this.str);
    this = get_token(buff);
  }

  printf("++ size of str: [%s] is %lu\n", this.str, strlen(this.str));
  unsigned len = strlen(this.str) + 5; // we need room for "%s is " which is '\0' terminated
  //                                   // = strlen(%s) + 4 + 1 for '\0'
  char *str = (char *) _calloc(len, sizeof(char));
  //sprintf(str, "%s is ", this.str); // 
  snprintf(str, len, "%s is ", this.str); // problem (!) heer if I remove printf ...
  printf("++ size of str: [%s] is %lu\n", str, strlen(str));

  *str_res = (char *) _calloc(strlen(str), sizeof(char));
  memcpy(*str_res, str, strlen(str));        // strncpy(*str_res, str, strlen(str)); // copy of s here
  free(str);
  str = NULL;
  if (VERBOSE) printf("[read_to_first_id] ====> FOUND ID: - str_res: [%s] - this.str = [%s]\n", *str_res, this.str);
  return get_token(buff);
}
 
/*
 * parsing
 */
token_t deal_with_fun_args(token_t this, buffer_t *buff, char **str_res) {
  if (VERBOSE) printf("[deal_with_fun_args] starts\n");
  while (this.type != ')') this = get_token(buff);
  this = get_token(buff);
  char *str = "function returning ";
  str_append(str_res, str);
  return this;
}

token_t deal_with_arrays(token_t this, buffer_t *buff, char **str_res) {
  if (VERBOSE) printf("[deal_with_arrays] starts\n");
  while (this.type == '[') {
    str_append(str_res, "array ");    
    this = get_token(buff);
    if (isdigit(this.str[0])) {
      int n   = atoi(this.str) - 1;
      char *str = (char *)_calloc(n, sizeof(char));
      unsigned len = strlen(this.str) + 6; // +7 for "[0..%d] "
      snprintf(str, len, "[0..%d] ", n); //sprintf(str, "[0..%d] ", n);
      str_append(str_res, str);  // copy of str here 
      this = get_token(buff);    // read the ']'
      free(str);
      str = NULL;
    }
    this = get_token(buff);
    str_append(str_res, "of ");
  }
  return this;
}

void deal_with_pointers(stack_t *stack, char **str_res) {
  if (VERBOSE) printf("[deal_with_pointers] starts\n");
  while (stack->cell[stack->top].type == '*') {
    token_t this = pop(stack);
    /*
    char *str = (char *) _calloc(strlen(this.str), sizeof(char *));
    sprintf(str, "%s ", this.str);
    */
    unsigned n = strlen(this.str) + 2;
    char *str = (char *) _calloc(n, sizeof(char *));
    snprintf(str, n, "%s ", this.str);
    str_append(str_res, str);   // copy of str here
    free(str);                  // we do not need str anymore
    str = NULL;
  }
}

void deal_with_declarators(token_t this, buffer_t *buff, stack_t *stack, char **str_res) {
  // deal with possible array/function following the identifier
  if (VERBOSE) {
    printf("[deal_with_declarators] deal with poss. array/function following the id.\n");
    printf("[deal_with_declarators] so far str_res is [%s]\n", *str_res);
  }
  switch (this.type) {
  case '[' : 
    this = deal_with_arrays(this, buff, str_res);
    break;
    
  case '(':
    this = deal_with_fun_args(this, buff, str_res);
    
  }
  deal_with_pointers(stack, str_res);
  // process tokens stacked while reading identifier
  while (stack->top >= 0) {
    if (VERBOSE) printf("[deal_with_declarators] stack top: [%s]\n", stack->cell[stack->top].str);
    if (strncmp(stack->cell[stack->top].str, "(", 1) == 0) {
      (void) pop(stack);
      token_t t = get_token(buff);
      deal_with_declarators(t, buff, stack, str_res);
    }
    else {
      token_t t = pop(stack);
      char *_s  = "read-only ";
      char *str = (char *) _calloc(max_of(strlen(_s), strlen(t.str)), sizeof(char *));
      if (strncmp(t.str, "const", 5) == 0) { // mapping const --> read-only
        strcpy(str, "read-only ");
      }
      else {
        unsigned n = strlen(t.str) + 2;
        snprintf(str, n, "%s ", t.str);  // sprintf(str, "%s ", t.str); 
      }
      str_append(str_res, str);  // copy of str
      free(str);
      str = NULL;
    }
  }
}


/*
 * main
 */
int main(const int argc, const char **argv) {
  if (argc == 1 || argc > 2) {
    usage(argv[0]);
  } 
  if (strlen(argv[1]) > MAXTOKENS * MAXTOKENLEN) {
    printf("[!] argument received too long. Giving up...\n");
    exit(EXIT_FAILURE);
  }
  // 
  buffer_t buffer = {
    .cont = argv[1],    // argv[1] contains the string to analyze
    .pos = 0
  };
  char *str_res = NULL;
  stack_t stack;
  stack.top = -1;
  //
  token_t t = read_to_first_id(&buffer, &stack, &str_res);
  deal_with_declarators(t, &buffer, &stack, &str_res);
  //
  printf("\n\toriginal declaration to parse: %s\n", argv[1]);
  printf("\tresult: %s\n", str_res);
  return 0;  // not required
}

/*
 * examples:
 *  ./bin/cdecl 'char* const *(*next)();'
 *  => next is pointer to function returning pointer to read-only pointer to char
 *
 *  ./bin/cdecl 'const char *ptr;'
 *
 *  ./bin/cdecl 'char *(*c[10])(int **p);'
 *  => c is array [0..9] of pointer to function returning pointer to char
 */

