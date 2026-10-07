#include <stdio.h>

/* Sequential version: computes sum, average, max and min of the same
   16-element dataset using a single process (used as baseline). */
int main(void)
{
    int data[16] = {
        10, 20, 30, 40,
        50, 60, 70, 80,
        90, 100, 110, 120,
        130, 140, 150, 160
    };

    int sum = 0;
    int max = data[0];
    int min = data[0];

    for (int i = 0; i < 16; i++)
    {
        sum += data[i];
        if (data[i] > max) max = data[i];
        if (data[i] < min) min = data[i];
    }

    printf("\n===== Sequential Dataset Statistics =====\n");
    printf("Dataset Size : 16\n");
    printf("Sum          : %d\n", sum);
    printf("Average      : %.2f\n", (double)sum / 16);
    printf("Maximum      : %d\n", max);
    printf("Minimum      : %d\n", min);
    printf("=========================================\n");
    return 0;
}
