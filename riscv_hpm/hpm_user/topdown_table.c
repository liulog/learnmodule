#include <stdio.h>

#define ROWS 15

typedef struct {
    const char* level1;
    const char* level2;
    const char* level3;
    float value;
} TopdownMetric;

int main() {
    TopdownMetric table[ROWS] = {
        {"Retiring", NULL, NULL, 0.00},
        {"FrontEnd Bound", NULL, NULL, 0.00},
        {NULL, "Fetch Latency Bound", NULL, 0.00},
        {NULL, "Fetch Bandwidth Bound", NULL, 0.00},
        {"Bad Speculation", NULL, NULL, 0.00},
        {NULL, "Branch Misspredict", NULL, 0.00},
        {NULL, "Machine Clears", NULL, 0.00},
        {"BackEnd Bound", NULL, NULL, 0.00},
        {NULL, "Core Bound", NULL, 0.00},
        {NULL, "Memory Bound", NULL, 0.00},
        {NULL, NULL, "L1 Bound", 0.00},
        {NULL, NULL, "L2 Bound", 0.00},
        {NULL, NULL, "L3 Bound", 0.00},
        {NULL, NULL, "Mem Bound", 0.00},
        {NULL, NULL, "Store Bound", 0.00}
    };

    // 打印表头
    printf("+------------------+------------------------+------------------+--------+\n");
    printf("| %-16s | %-22s | %-16s | %-6s |\n", "Level 1", "Level 2", "Level 3", "结果");
    printf("+------------------+------------------------+------------------+--------+\n");

    // 打印每行数据
    for (int i = 0; i < ROWS; ++i) {
        printf("| %-16s | %-22s | %-16s | %6.2f |\n",
            table[i].level1 ? table[i].level1 : "-",
            table[i].level2 ? table[i].level2 : "-",
            table[i].level3 ? table[i].level3 : "-",
            table[i].value);
    }

    printf("+------------------+------------------------+------------------+--------+\n");

    return 0;
}
