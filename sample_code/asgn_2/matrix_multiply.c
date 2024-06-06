#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

// 矩阵结构体
typedef struct {
    int rows;
    int cols;
    double *data;
} Matrix;

// 线程参数结构体
typedef struct {
    Matrix *a;
    Matrix *b;
    Matrix *result;
    int start_row;
    int end_row;
} ThreadData;

// 读取矩阵文件，忽略注释行
Matrix read_matrix(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    Matrix matrix;
    fscanf(file, "%d %d", &matrix.rows, &matrix.cols);
    matrix.data = (double *)malloc(matrix.rows * matrix.cols * sizeof(double));
    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            continue;  // 忽略注释行
        }
        sscanf(line, "%lf", &matrix.data[count++]);
    }

    fclose(file);
    return matrix;
}

// 释放矩阵内存
void free_matrix(Matrix matrix) {
    free(matrix.data);
}

// 矩阵乘法线程函数
void *matrix_multiply_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    Matrix *a = data->a;
    Matrix *b = data->b;
    Matrix *result = data->result;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < b->cols; j++) {
            result->data[i * result->cols + j] = 0;
            for (int k = 0; k < a->cols; k++) {
                result->data[i * result->cols + j] += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
        }
    }

    return NULL;
}

// 将矩阵写入文件
void write_matrix_to_file(Matrix matrix, const char *file_name) {
    FILE *file = fopen(file_name, "w");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d %d\n", matrix.rows, matrix.cols);
    for (int i = 0; i < matrix.rows; i++) {
        fprintf(file, "# Row %d\n", i + 1);
        for (int j = 0; j < matrix.cols; j++) {
            fprintf(file, "%lf\n", matrix.data[i * matrix.cols + j]);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 9) {
        fprintf(stderr, "Usage: %s -a a_matrix_file.txt -b b_matrix_file.txt -t thread_count -o result_matrix.txt\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *a_file = NULL;
    char *b_file = NULL;
    char *output_file = NULL;
    int thread_count = 0;

    // 解析命令行参数
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-a") == 0) {
            a_file = argv[i + 1];
        } else if (strcmp(argv[i], "-b") == 0) {
            b_file = argv[i + 1];
        } else if (strcmp(argv[i], "-t") == 0) {
            thread_count = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-o") == 0) {
            output_file = argv[i + 1];
        } else {
            fprintf(stderr, "Unknown option %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }

    // 检查参数是否解析正确
    if (!a_file || !b_file || !output_file || thread_count <= 0) {
        fprintf(stderr, "Invalid arguments\n");
        fprintf(stderr, "Usage: %s -a a_matrix_file.txt -b b_matrix_file.txt -t thread_count -o result_matrix.txt\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    Matrix a = read_matrix(a_file);
    Matrix b = read_matrix(b_file);

    if (a.cols != b.rows) {
        fprintf(stderr, "Matrix dimensions do not match for multiplication\n");
        free_matrix(a);
        free_matrix(b);
        exit(EXIT_FAILURE);
    }

    Matrix result = { a.rows, b.cols, (double *)malloc(a.rows * b.cols * sizeof(double)) };
    pthread_t threads[thread_count];
    ThreadData thread_data[thread_count];

    int rows_per_thread = a.rows / thread_count;
    int extra_rows = a.rows % thread_count;

    clock_t start_time = clock();

    for (int i = 0; i < thread_count; i++) {
        int start_row = i * rows_per_thread;
        int end_row = (i + 1) * rows_per_thread;
        if (i < extra_rows) {
            start_row += i;
            end_row += (i + 1);
        } else {
            start_row += extra_rows;
            end_row += extra_rows;
        }

        thread_data[i] = (ThreadData){ &a, &b, &result, start_row, end_row };
        pthread_create(&threads[i], NULL, matrix_multiply_thread, &thread_data[i]);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);

    write_matrix_to_file(result, output_file);

    free_matrix(a);
    free_matrix(b);
    free_matrix(result);

    return 0;
}
