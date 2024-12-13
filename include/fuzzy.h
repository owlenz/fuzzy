#ifndef _FUZZY_H
#define _FUZZY_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum var_type { IN = 0, OUT = 1 };
enum fuzzy_type { TRI = 0, TRAP = 1 };

typedef struct {
  char *name;
  enum fuzzy_type type;
  int values[4];
} fuzzy_set_t;

typedef struct {
  char *name;
  enum var_type type;
  int range[2];
  int count;
  float *mem_values;
  fuzzy_set_t *fuzzy_sets;
} variable_t;

typedef struct {
  variable_t *in_vars[2];
  int in_vars_set[2];
  char *op;
  variable_t *out_var;
  int out_var_set;
} rule_t;

typedef struct {
  variable_t **variables;
  rule_t **rules;
  int var_num;
  int rule_num;
  char *name;
  char *desc;
} system_t;

void test(system_t);
void clear_input_buffer();
void add_variable(system_t *my_system);
void add_fuzzy_set(variable_t *variable);
void add_rules(system_t *my_system);
float tri_memb(int crisp, fuzzy_set_t fuzzy_set);
float trap_memb(int crisp, fuzzy_set_t fuzzy_set);
void fuzzification(system_t *my_system);
void inference(system_t *my_system);
void defuzzification(system_t my_system);

#endif
