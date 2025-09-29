#include <stdio.h>
#include <stdlib.h>

int main()
{

    FILE *fp;
    char row[MAXCHAR];

    fp = fopen("Data\\training_data.csv","r");

    ;

    while (feof(fp) != true)
    {
        fgets(row, MAXCHAR, fp);
        printf("Row: %s", row);
    }
    

    return 0;
}