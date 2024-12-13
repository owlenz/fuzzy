#include "include/fuzzy.h"

void test(system_t my_system) {
  for (int i = 0; i < my_system.var_num; i++) {
    variable_t var = *my_system.variables[i];
    printf("\nVariable output:\n");
    printf("%s, %d, %d, %d, %d \n", my_system.variables[i]->name, var.type,
           var.count, var.range[0], var.range[1]);
    for (int j = 0; j < var.count; j++) {
      printf("%s, %d, %d, %d, %d, %f\n", var.fuzzy_sets[j].name,
             var.fuzzy_sets[j].type, var.fuzzy_sets[j].values[0],
             var.fuzzy_sets[j].values[1], var.fuzzy_sets[j].values[2],
             var.mem_values[j]);
    }
  }

  for (int i = 0; i < my_system.rule_num; i++) {
    rule_t *rule = my_system.rules[i];

    // Print the input variables and their corresponding set index
    printf("Rule %d:\n", i + 1);

    printf("  Input Variables:\n");
    for (int j = 0; j < 2; j++) {
      if (rule->in_vars[j] != NULL) {
        printf("    - %s (Set Index: %d)\n", rule->in_vars[j]->name,
               rule->in_vars_set[j]);
      }
    }

    // Print the operator
    printf("  Operator: %s\n", rule->op);

    // Print the output variable and its set index
    if (rule->out_var != NULL) {
      printf("  Output Variable: %s (Set Index: %d)\n", rule->out_var->name,
             rule->out_var_set);
    }

    printf("\n");
  }
}
