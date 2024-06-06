import numpy as np

# 生成 1000x1000 的随机矩阵并保存到文件，带有注释行
def generate_matrix_file(file_name, rows, cols):
    matrix = np.random.rand(rows, cols)
    with open(file_name, 'w') as f:
        f.write(f"{rows} {cols}\n")
        for i in range(rows):
            f.write(f"# Row {i+1}\n")
            for j in range(cols):
                f.write(f"{matrix[i, j]}\n")

# 生成 a_matrix_file.txt 和 b_matrix_file.txt
generate_matrix_file("a_matrix_file.txt", 1000, 1000)
generate_matrix_file("b_matrix_file.txt", 1000, 1000)

print("Matrices generated and saved to a_matrix_file.txt and b_matrix_file.txt")
