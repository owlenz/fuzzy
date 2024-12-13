#include "include/fuzzy.h"
#include <stdio.h>

void clear_input_buffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void add_variable(system_t *my_system) {
  while (1) {
    char name[50];
    char type[4];
    int range[2];
    printf(
        "Enter the variable’s name, type (IN/OUT) and range ([lower, upper]): "
        "(Press x to finish)\n");

    scanf("%s", name);
    if (strcmp(name, "x") == 0) {
      return;
    }

    int err = scanf("%s [%d, %d]", type, &range[0], &range[1]);
    /*printf("%s %s [%d, %d]", name, type, range[0], range[1]);*/

    if (err != 3) {
      fprintf(stderr, "invalid input format\n");
      continue;
    }

    if (strcmp(type, "IN") != 0 && strcmp(type, "OUT") != 0) {
      fprintf(stderr, "wrong type (it's either IN or OUT)\n");
      continue;
    }
    variable_t *variable = malloc(sizeof(variable_t));
    if (variable == NULL) {
      perror("error allocating memory");
      return;
    }
    variable->name = malloc(sizeof(strlen(name)));
    if (variable->name == NULL) {
      perror("error allocating memory");
      free(variable);
      return;
    }
    strcpy(variable->name, name);
    variable->type = strcmp(type, "IN") == 0 ? IN : OUT;

    variable->range[0] = range[0];
    variable->range[1] = range[1];

    my_system->variables[my_system->var_num++] = variable;
  }
};

void add_fuzzy_set(variable_t *variable) {
  if (variable->fuzzy_sets == NULL) {
    variable->fuzzy_sets = malloc(sizeof(fuzzy_set_t) * 10);
    variable->mem_values = malloc(sizeof(float) * 10);
    memset(variable->mem_values, 0, sizeof(float) * 10);
    if (variable->fuzzy_sets == NULL) {
      perror("error allocating memory for fuzzy sets");
      return;
    }
    variable->count = 0;
  }
  while (1) {
    fuzzy_set_t *fuzzy_set = malloc(sizeof(fuzzy_set_t));
    printf("Enter the fuzzy set name, type (TRI/TRAP) and values: (Press x to "
           "finish)\n");
    fuzzy_set->name = malloc(50);
    char type[5];
    if (scanf("%49s", fuzzy_set->name) != 1) {
      fprintf(stderr, "invalid input\n");
      free(fuzzy_set->name);
      free(fuzzy_set);
      continue;
    }
    if (strcmp(fuzzy_set->name, "x") == 0) {
      free(fuzzy_set->name);
      free(fuzzy_set);
      return;
    }
    if (scanf("%s", type) != 1) {
      fprintf(stderr, "invalid input\n");
      free(fuzzy_set->name);
      free(fuzzy_set);
      continue;
    }
    if (strcmp(type, "TRI") != 0 && strcmp(type, "TRAP") != 0) {
      fprintf(stderr, "Invalid fuzzy set type. Must be TRI or TRAP.\n");
      free(fuzzy_set->name);
      free(fuzzy_set);
      return;
    }
    fuzzy_set->type = strcmp(type, "TRI") == 0 ? TRI : TRAP;
    if (fuzzy_set->type == TRI) {
      if (scanf("%d %d %d", &fuzzy_set->values[0], &fuzzy_set->values[1],
                &fuzzy_set->values[2]) != 3) {
        fprintf(stderr, "invalid input\n");
        continue;
      }
    } else {
      if (scanf("%d %d %d %d", &fuzzy_set->values[0], &fuzzy_set->values[1],
                &fuzzy_set->values[2], &fuzzy_set->values[3]) != 4) {
        fprintf(stderr, "invalid input\n");
        continue;
      }
    }

    variable->fuzzy_sets[variable->count++] = *fuzzy_set;
    /*return;*/
  }
}

void add_rules(system_t *my_system) {
  if (my_system->rules == NULL) {
    my_system->rules = malloc(10 * sizeof(rule_t));
    if (my_system->rules == NULL) {
      perror("error allocating memory");
      return;
    }
    my_system->rule_num = 0;
  }
  while (1) {
    printf("Enter the rules in this format: (Press x to finish)\nIN_variable "
           "set operator IN_variable set => OUT_variable set\n");
    rule_t *rule = malloc(sizeof(rule_t));
    if (rule == NULL) {
      perror("error allocating memory");
      return;
    }

    char *in_vars[2];
    in_vars[0] = malloc(20);
    scanf("%s", in_vars[0]);
    if (strcmp(in_vars[0], "x") == 0) {
      clear_input_buffer();
      free(in_vars[0]);
      free(rule);
      return;
    }

    in_vars[1] = malloc(20);
    char *in_vars_set[2];
    in_vars_set[0] = malloc(20);
    in_vars_set[1] = malloc(20);
    rule->op = malloc(20);
    char *out_var_set = malloc(20);
    char *out_var = malloc(20);
    scanf("%s %s %s %s => %s %s", in_vars_set[0], rule->op, in_vars[1],
          in_vars_set[1], out_var, out_var_set);

    for (int j = 0; j < my_system->var_num; j++) {
      if (strcmp(in_vars[0], my_system->variables[j]->name) == 0 &&
          my_system->variables[j]->type == IN) {
        rule->in_vars[0] = my_system->variables[j];
      }
      if (strcmp(in_vars[1], my_system->variables[j]->name) == 0 &&
          my_system->variables[j]->type == IN) {
        rule->in_vars[1] = my_system->variables[j];
      }
      if (strcmp(out_var, my_system->variables[j]->name) == 0 &&
          my_system->variables[j]->type == OUT) {
        rule->out_var = my_system->variables[j];
      }
    }
    if (rule->in_vars[0] == NULL || rule->in_vars[1] == NULL ||
        rule->out_var == NULL) {
      fprintf(stderr, "Invalid variable names");
      continue;
    }
    for (int j = 0; j < rule->in_vars[0]->count; j++) {
      if (strcmp(in_vars_set[0], rule->in_vars[0]->fuzzy_sets[j].name) == 0) {
        rule->in_vars_set[0] = j;
        break;
      } else {
        rule->in_vars_set[0] = -1;
      }
    }

    for (int j = 0; j < rule->in_vars[1]->count; j++) {
      if (strcmp(in_vars_set[1], rule->in_vars[1]->fuzzy_sets[j].name) == 0) {
        rule->in_vars_set[1] = j;
        break;
      } else {
        rule->in_vars_set[1] = -1;
      }
    }

    for (int j = 0; j < rule->out_var->count; j++) {
      if (strcmp(out_var_set, rule->out_var->fuzzy_sets[j].name) == 0) {
        rule->out_var_set = j;
        break;
      } else {
        rule->out_var_set = -1;
      }
    }
    if (rule->in_vars_set[0] == -1 || rule->in_vars_set[1] == -1 ||
        rule->out_var_set == -1) {
      fprintf(stderr, "Invalid variable set names");
      continue;
    }

    clear_input_buffer();
    my_system->rules[my_system->rule_num++] = rule;
  }
}

float tri_memb(int crisp, fuzzy_set_t fuzzy_set) {
  if (crisp <= fuzzy_set.values[0] || crisp >= fuzzy_set.values[2]) {
    return 0.0;
  } else if (crisp == fuzzy_set.values[1]) {
    return 1.0;
  } else if (crisp < fuzzy_set.values[1]) {
    return (float)(crisp - fuzzy_set.values[0]) /
           (fuzzy_set.values[1] - fuzzy_set.values[0]);
  } else {
    return (float)(fuzzy_set.values[2] - crisp) /
           (fuzzy_set.values[2] - fuzzy_set.values[1]);
  }
}

float trap_memb(int crisp, fuzzy_set_t fuzzy_set) {
  if (crisp <= fuzzy_set.values[0] || crisp >= fuzzy_set.values[3]) {
    return 0.0;
  } else if (crisp >= fuzzy_set.values[1] && crisp <= fuzzy_set.values[2]) {
    return 1.0;
  } else if (crisp < fuzzy_set.values[1]) {
    float denominator = (fuzzy_set.values[1] - fuzzy_set.values[0]);
    return (denominator != 0)
               ? (float)(crisp - fuzzy_set.values[0]) / denominator
               : 0.0;
  } else {
    float denominator = (fuzzy_set.values[3] - fuzzy_set.values[2]);
    return (denominator != 0)
               ? (float)(fuzzy_set.values[3] - crisp) / denominator
               : 0.0;
  }
}

void fuzzification(system_t *my_system) {
  int crisp_value[my_system->var_num];
  printf("Enter the crisp values: \n\n");
  for (int j = 0; j < my_system->var_num; j++) {
    if (my_system->variables[j]->type == OUT)
      continue;
    printf("%s: ", my_system->variables[j]->name);
    scanf("%d", &crisp_value[j]);
    printf("\n");
  }
  for (int j = 0; j < my_system->var_num; j++) {
    if (my_system->variables[j]->type == OUT)
      continue;
    for (int i = 0; i < my_system->variables[j]->count; i++) {
      fuzzy_set_t fuzzy_set = my_system->variables[j]->fuzzy_sets[i];
      if (fuzzy_set.type == TRI) {
        my_system->variables[j]->mem_values[i] =
            tri_memb(crisp_value[j], fuzzy_set);
      } else {
        my_system->variables[j]->mem_values[i] =
            trap_memb(crisp_value[j], fuzzy_set);
      }
      printf("Variable %d, Set %d: Membership Value = %.2f\n", j, i,
             my_system->variables[j]->mem_values[i]);
    }
  }
}

void inference(system_t *my_system) {

  for (int i = 0; i < my_system->rule_num; i++) {
    rule_t *rule = my_system->rules[i];
    float in_mem_val1 = rule->in_vars[0]->mem_values[rule->in_vars_set[0]];
    float in_mem_val2 = rule->in_vars[1]->mem_values[rule->in_vars_set[1]];
    float *out_mem_val = &rule->out_var->mem_values[rule->out_var_set];
    if (strcmp(rule->op, "and") == 0) {
      *out_mem_val = fmax(fmin(in_mem_val1, in_mem_val2), *out_mem_val);
    } else if (strcmp(rule->op, "or") == 0) {
      *out_mem_val = fmax(fmax(in_mem_val1, in_mem_val2), *out_mem_val);
    } else if (strcmp(rule->op, "and_not") == 0) {
      *out_mem_val = fmax(fmin(in_mem_val1, 1.0f - in_mem_val2), *out_mem_val);
    } else if (strcmp(rule->op, "or_not") == 0) {
      *out_mem_val = fmax(fmax(in_mem_val1, 1.0f - in_mem_val2), *out_mem_val);
    }
  }
}

void defuzzification(system_t my_system) {
  variable_t *out = my_system.rules[0]->out_var;
  float centroids[out->count];
  for (int i = 0; i < out->count; i++) {
    if (out->fuzzy_sets->type == TRI) {
      centroids[i] =
          (out->fuzzy_sets[i].values[0] + out->fuzzy_sets[i].values[1] +
           out->fuzzy_sets[i].values[2]) /
          3.0;
    } else {
      centroids[i] =
          (out->fuzzy_sets[i].values[0] + out->fuzzy_sets[i].values[1] +
           out->fuzzy_sets[i].values[2] + out->fuzzy_sets[i].values[3]) /
          4.0;
    }
  };

  float denominator = 0, numerator = 0;
  for (int i = 0; i < out->count; i++) {
    numerator += (out->mem_values[i] * centroids[i]);
    denominator += out->mem_values[i];
  }
  float crisp_value = numerator / denominator;
  printf("crisp value (answer): %f\n", crisp_value);
}

void run() {
  while (1) {
    printf("Fuzzy Logic Toolbox\n"
           "===================\n"
           "1- Create new fuzzy system\n"
           "2- Quit\n");
    int input;
    scanf("%d", &input);
    getchar();
    if (input == 1) {

    } else if (input == 2) {
      return;
    } else {
      printf("wrong input");
      continue;
    }
    system_t *my_system = malloc(sizeof(system_t));
    if (my_system == NULL) {
      perror("error allocating memory");
      return;
    }
    my_system->name = malloc(50);
    my_system->desc = malloc(200);
    my_system->var_num = 0;
    my_system->variables = malloc(10 * sizeof(variable_t *));
    if (!my_system->name || !my_system->desc || !my_system->variables) {
      perror("Error allocating memory");
      free(my_system);
      return;
    }
    while (1) {
      printf("Enter the system’s name:\n");
      if (fgets(my_system->name, 50, stdin) != NULL) {
        my_system->name[strcspn(my_system->name, "\n")] = 0;
        break;
      } else {
        fprintf(stderr, "Invalid input format. Please try again.\n");
        while (getchar() != '\n')
          ;
      }
    }
    while (1) {
      printf("Enter a brief description:\n");
      if (fgets(my_system->desc, 200, stdin) != NULL) {
        my_system->desc[strcspn(my_system->desc, "\n")] = 0;
        break;
      } else {
        fprintf(stderr, "Invalid input format. Please try again.\n");
        while (getchar() != '\n')
          ;
      }
    }

    int cond = 1;
    while (cond) {
      /*test(*my_system);*/
      printf("Main Menu:\n"
             "==========\n"
             "1- Add variables.\n"
             "2- Add fuzzy sets to an existing variable.\n"
             "3- Add rules.\n"
             "4- Run the simulation on crisp values.\n"
             "5- Print variables and rules.\n"
             "6- Quit.\n");
      if (scanf("%d", &input) != 1) {
        input = 10;
      }
      clear_input_buffer();
      switch (input) {
      case 1: {
        add_variable(my_system);
        break;
      }
      case 2: {
        char *name = malloc(50);
        scanf("%s", name);
        int i;
        for (i = 0; i < my_system->var_num; i++) {
          if (strcmp(my_system->variables[i]->name, name) == 0)
            break;
        }
        add_fuzzy_set(my_system->variables[i]);
        break;
      }
      case 3: {
        add_rules(my_system);
        break;
      }
      case 4: {
        if (my_system->var_num == 0 || my_system->rule_num == 0) {
          fprintf(stderr, "add some variables and rules first");
          continue;
        }
        fuzzification(my_system);
        inference(my_system);
        defuzzification(*my_system);
        break;
      }
      case 5: {
        test(*my_system);
        break;
      }
      case 6:
        cond = 0;
        break;
      default:
        fprintf(stderr, "Wrong input\n");
        continue;
      }
    }
  }
}

int main() { run(); }
