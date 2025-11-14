#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096
#define MAX_FIELDS 100

int main() 
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *token;
    char *fields[MAX_FIELDS];
    int field_count;
    
    // Open the CSV file
    file = fopen("Data\\training_data.csv", "r");
    if (file == NULL) 
    {
        perror("Error opening file");
        return 1;
    }
    
    // Read line by line
    while (fgets(line, sizeof(line), file)) 
    {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;
        
        field_count = 0;
        
        // Split line by comma delimiter
        token = strtok(line, ",");
        while (token != NULL && field_count < MAX_FIELDS) 
        {
            fields[field_count++] = token;
            token = strtok(NULL, ",");
        }
        
        // Print the fields separated by commas
        for (int i = 0; i < field_count; i++) 
        {
            printf("%s", fields[i]);
            if (i < field_count - 1) 
            {
                printf(",");
            }
        }
        printf("\n");
    }
    
    fclose(file);

    system("pause");
    return 0;
}