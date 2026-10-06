#include <stdio.h>

void line(void)
{
    printf("============================================================\n");
}

void header(void)
{
    printf("\n");
    line();
    printf("                 WATER QUALITY CHECKER\n");
    printf("              Electrical Conductivity System\n");
    line();
    printf("              Measure  |  Calculate  |  Analyze\n");
    printf("============================================================\n");
    printf("\n");
}

int main()
{
    float v, k;
    float ua, temp, g, ec, ec25, tds;
    float a = 0.02;      /* temperature coefficient */
    float factor = 0.5;  /* TDS factor */

    header();

    printf(" PROJECT SETUP\n");
    printf("------------------------------------------------------------\n");
    printf(" Enter supply voltage (V): ");
    scanf("%f", &v);

    printf(" Enter cell constant K (1/cm): ");
    scanf("%f", &k);

    printf("\n");
    line();
    printf("                     WATER ANALYSIS\n");
    line();
    printf(" Enter 0 for current to finish the program.\n");

    /* Measure water samples */
    while (1)
    {
        printf("\n");
        printf(" Meter current (uA): ");
        scanf("%f", &ua);

        if (ua <= 0)
            break;

        printf(" Water temperature (C): ");
        scanf("%f", &temp);

        g = (ua / 1000000.0) / v;
        ec = g * k * 1000000.0;
        ec25 = ec / (1 + a * (temp - 25));
        tds = ec25 * factor;

        printf("\n");
        printf(" +--------------------------------------------------------+\n");
        printf(" |                    MEASUREMENT RESULT                 |\n");
        printf(" +--------------------------------------------------------+\n");
        printf(" | Conductance        : %10.2f uS                       |\n", g * 1000000.0);
        printf(" | EC at 25 C         : %10.1f uS/cm                     |\n", ec25);
        printf(" | TDS                : %10.0f ppm                       |\n", tds);
        printf(" | Temperature        : %10.1f C                         |\n", temp);
        printf(" +--------------------------------------------------------+\n");

        printf(" | Water Quality      : ");

        if (tds < 300)
            printf("Excellent");
        else if (tds < 600)
            printf("Good");
        else if (tds < 900)
            printf("Fair");
        else if (tds < 1200)
            printf("Poor");
        else
            printf("Unacceptable");

        printf("\n");
        printf(" +--------------------------------------------------------+\n");
    }

    printf("\n");
    line();
    printf("                 ANALYSIS COMPLETED\n");
    printf("              Thank you for using the system!\n");
    line();
    printf("\n");

    return 0;
}
