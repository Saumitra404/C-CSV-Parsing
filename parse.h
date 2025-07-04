#ifndef CSV_PARSER_H
#define CSV_PARSER_H

/**
 * @file csv_parser.h
 * @brief A library for parsing numerical data from columns in a CSV file.
 */

/**
 * @brief This function pulls decimal data from multiple columns of the csv. It has basic error handling
 * and ignores white space. Skips first/header line.
 *
 * Undefined behavior if non-double characters are present in the data.
 *
 * Make sure to free the allocated space later by calling free_data().
 *
 * @param file Path to file as char*.
 * @param col Array of column numbers (no duplicates).
 * @param col_size Size of column array.
 * @param data_length Length of data.
 * @return An array of double pointers to the column data on the heap.
 *
 * Errors on failure to allocate space, open file.
 */
double **get_columns_of_doubles(char *file, int col[], int col_size, int data_length);

/**
 * @brief Extracts data from a single specified column of a CSV file.
 *
 * This is a convenience function that reads a CSV file and extracts numerical data
 * from a single column. It allocates memory on the heap which must be freed later
 * using the standard `free()` function.
 *
 * @param file A string representing the path to the CSV file.
 * @param col_num The 0-based index of the column to extract.
 * @param data_length The number of data rows to read from the file.
 * @return On success, returns a pointer to an array of doubles containing the
 * column's data. On failure, returns NULL.
 */
double *get_single_column_of_doubles(char *file, int col_num, int data_length);

/**
 * @brief Frees a set of double pointers to data (from get_columns_of_doubles).
 *
 * This function safely deallocates the nested array structure created by the
 * get_columns_of_doubles function to prevent memory leaks.
 *
 * @param data The array of double pointers to be freed.
 * @param col_size The number of columns that were allocated (the first dimension
 * of the 'data' array).
 */
void free_data(double **data, int col_size);

#endif // CSV_PARSER_H
