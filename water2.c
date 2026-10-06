#include <stdio.h>

int main()
{
    float v, k;
    float ua, temp, g, ec, ec25, tds;
    float a = 0.02;      /* temperature coefficient */
    float factor = 0.5;  /* TDS factor */

    printf("WATER QUALITY CHECKER\n");

    printf("Enter supply voltage (V): ");
    scanf("%f", &v);

    printf("Enter cell constant K (1/cm): ");
    scanf("%f", &k);

    /* Measure water samples */
    printf("\nMEASURE (enter 0 current to stop)\n");
    while (1)
    {
        printf("\nEnter meter current (uA): ");
        scanf("%f", &ua);
        if (ua <= 0)
            break;

        printf("Enter water temperature (C): ");
        scanf("%f", &temp);

        g = (ua / 1000000.0) / v;
        ec = g * k * 1000000.0;
        ec25 = ec / (1 + a * (temp - 25));
        tds = ec25 * factor;

        printf("Conductance = %.2f uS\n", g * 1000000.0);
        printf("EC at 25C   = %.1f uS/cm\n", ec25);
        printf("TDS         = %.0f ppm\n", tds);

        if (tds < 300)
            printf("Quality     = Excellent\n");
        else if (tds < 600)
            printf("Quality     = Good\n");
        else if (tds < 900)
            printf("Quality     = Fair\n");
        else if (tds < 1200)
            printf("Quality     = Poor\n");
        else
            printf("Quality     = Unacceptable\n");
    }

    printf("Done.\n");
    return 0;
}
