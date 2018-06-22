#pragma once

#define ONE_BIT(v) (!((v) & ((v)-1)))

int optimal_selection(int *h, double *p);
int optimal_selection2(int *h, double *p);
