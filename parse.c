#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DELIMITER ','
#define QUOTATION '"'

// This function pulls decimal data from a specified column of the csv. It has basic error handling
//      and ignores white space. Skips first/header line.
//
// Undefined behavior if non-double characters are present in the data.
//
// Make sure to free the allocated space later.
//
// @param path to file as char*
// @param column to get
// @param length of data
// @return a double pointer to the column data on the heap
//
// Errors on failure to allocate space, open file
double *get_column_of_doubles(char *file, int col, int length)
{
    FILE *inputFile;

    double *ret = (double *)calloc(length, sizeof(double));
    if (ret == NULL)
    {
        perror("Failed to allocate memory");
        return NULL;
    }

    inputFile = fopen(file, "r");
    if (inputFile == NULL)
    {
        perror("failed to open file");
        free(ret);
        return NULL;
    }

    int c;

    int count = 0;  // The number of doubles stores so far
    char item[256]; // Up to 256 character precision for items

    int current_len = 0; // Length of current string
    int current_col = 0; // Current column being parsed

    bool valid = false; // Skips first line

    while ((c = fgetc(inputFile)) != EOF)
    {
        if (count == length)
        {
            fclose(inputFile);
            return ret;
        }

        switch (c)
        {
        case '\n':
            if (valid == true)
            {
                item[current_len] = 0;               // Completes the string
                *(ret + count) = strtod(item, NULL); // Stores the converted double

                count++;
            }
            valid = true;
            current_col = 0;
            current_len = 0;
            break;

        case DELIMITER:
            current_col++;
            break;

        default:
            if (current_col == col && c != '"' && current_len < 256)
            {
                item[current_len] = c;
                current_len++;
            }
        }
    }

    // In case last valid line wasn't parsed
    if (valid == true && count < length)
    {
        item[current_len] = 0;               // Completes the string
        *(ret + count) = strtod(item, NULL); // Stores the converted double
    }

    fclose(inputFile);
    return ret;
}

// Simple integer comparison function
int comp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// This function pulls decimal data from multiple columns of the csv. It has basic error handling
//      and ignores white space. Skips first/header line.
//
// Undefined behavior if non-double characters are present in the data.
//
// Make sure to free the allocated space later.
//
// @param path to file as char*
// @param array of column numbers (no duplicates)
// @param size of column array
// @param length of data
// @return an array of double pointers to the column data on the heap
//
// Errors on failure to allocate space, open file
double **get_columns_of_doubles(char *file, int col[], int col_size, int data_length)
{
    FILE *inputFile;

    // Open file and handle for issues
    inputFile = fopen(file, "r");
    if (inputFile == NULL)
    {
        perror("failed to open file");
        return NULL;
    }

    // create a sorted copy to use two-pointer approach
    int sorted[col_size];
    for (int i = 0; i < col_size; i++)
    {
        sorted[i] = col[i];
    }
    qsort(sorted, col_size, sizeof(int), comp);

    // Allocate the required memory for array of pointers
    double **data = (double **)malloc(col_size * sizeof(*data));
    if (data == NULL)
    {
        perror("failed to allocate memory");
        return NULL;
    }

    // Allocate the required memory for data
    for (int i = 0; i < col_size; i++)
    {
        data[i] = (double *)malloc(data_length * sizeof(double));

        // In case of allocation issue, free everything
        if (data[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(data[j]);
            }
            free(data);
            perror("Failed to allocate memory");
            return NULL;
        }
    }

    // Allocate necessary variables

    int c;              // Current character as int
    long int line = -1; // Current line, starts at -1 to account for header

    char temp[256];   // Temp string storage, up to 256 character precision for strings
    int temp_len = 0; // Length of current string

    int current_col = 0; // Current column being parsed

    int col_ptr = 0;
    bool valid = false; // Skips first line
    bool quote = false; // Check for open quotations

    // Loop through file
    while ((c = fgetc(inputFile)) != EOF)
    {
        if (line == data_length)
        {
            fclose(inputFile);
            break;
        }

        switch (c)
        {

        case '\n':
            if (!quote && valid)
            {
                current_col++;

                if (current_col == sorted[col_ptr])
                {
                    temp[temp_len] = 0;                           // End of String
                    *(data[col_ptr] + line) = strtod(temp, NULL); // Saves current temp
                    col_ptr++;
                }
                temp_len = 0; // Flush current temp
            }
            line++;
            current_col = 0;
            col_ptr = 0;
            valid = true;
            break;

        case DELIMITER:
            if (!quote && valid)
            {
                current_col++;

                if (current_col == sorted[col_ptr])
                {
                    temp[temp_len] = 0;                           // End of String
                    *(data[col_ptr] + line) = strtod(temp, NULL); // Saves current temp
                    col_ptr++;
                }
                temp_len = 0; // Flush current temp
            }
            break;

        case QUOTATION:
            quote = !quote;
            break;

        default:
            if (temp_len < (sizeof(temp) - 1))
            {
                temp[temp_len] = c;
                temp_len++;
            }
            break;
        }
    }

    // Process final item
    if (temp_len > 0 && line < data_length)
    {
        current_col++;
        if (current_col == sorted[col_ptr])
        {
            temp[temp_len] = 0;
            *(data[col_ptr] + line) = strtod(temp, NULL);
        }
    }

    // Allocate memory for final return
    double **unsorted = (double **)malloc(col_size * sizeof(*unsorted));
    if (unsorted == NULL)
    {
        free_data(data, col_size);
        fclose(inputFile);
        perror("Error in allocating memory for final return");
        return NULL;
    }

    // Unsort the pointers in data
    for (int i = 0; i < col_size; i++)
    {
        int original_col = col[i];
        for (int j = 0; j < col_size; j++)
        {
            if (sorted[j] == original_col)
            {
                unsorted[i] = data[j];
                break;
            }
        }
    }
    free(data);

    fclose(inputFile);
    return unsorted;
}

// Frees a set of double pointers to data (from get columns of doubles)
void free_data(double **col, int col_size)
{
    for (int i = 0; i < col_size; i++)
    {
        free(col[i]);
    }
    free(col);
}