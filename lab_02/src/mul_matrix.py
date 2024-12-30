from matrix import Matrix, Gen

ERROR_DIFFERENT_SIZE = 1

def mul_std(a: Matrix, b: Matrix):
    if (a.size != b.size):
        return ERROR_DIFFERENT_SIZE
    size = a.size
    res = Matrix(size, Gen.ZERO_MATRIX)
    for i in range(size):
        for j in range(size):
            for k in range(size):
                res.data[i][j] += a.data[i][k] * b.data[k][j]
    return res


def mul_grape(a: Matrix, b: Matrix):
    if (a.size != b.size):
        return ERROR_DIFFERENT_SIZE
    
    size = a.size
    row_a = size
    col_a = size
    col_b = size
    tmp_row = [0] * row_a
    tmp_col = [0] * col_b
    
    for i in range(row_a):
        for j in range(0, col_a // 2):
            tmp_row[i] = tmp_row[i] + a.data[i][2 * j] * a.data[i][2 * j + 1]

    for i in range(col_b):
        for j in range(0, col_a // 2):
            tmp_col[i] = tmp_col[i] + b.data[2 * j][i] * b.data[2 * j + 1][i] 

    res = Matrix(size, Gen.ZERO_MATRIX)
    for i in range(row_a):
        for j in range(col_b):
            res.data[i][j] = -tmp_row[i] - tmp_col[i]   
            for k in range(0, col_a // 2):
                res.data[i][j] = res.data[i][j] + (a.data[i][2 * k + 1] + b.data[2 * k][j]) * (a.data[i][2 * k] + b.data[2 * k + 1][j])

    if (col_a % 2 == 1):
        for i in range(row_a):
            for j in range(col_b):
                res.data[i][j] = res.data[i][j] + a.data[i][col_a - 1] * b.data[col_a - 1][j]

    return res

def mul_grape_opt(a: Matrix, b: Matrix):
    if (a.size != b.size):
        return ERROR_DIFFERENT_SIZE
    
    size = a.size
    row_a = size
    col_a = size
    col_b = size
    tmp_row = [0] * row_a
    tmp_col = [0] * col_b
    
    for i in range(row_a):
        for j in range(0, col_a // 2):
            tmp_row[i] = tmp_row[i] + a.data[i][2 * j] * a.data[i][2 * j + 1]

    for i in range(col_b):
        for j in range(0, col_a // 2):
            tmp_col[i] = tmp_col[i] + b.data[2 * j][i] * b.data[2 * j + 1][i] 

    flag = col_a % 2
    res = Matrix(size, Gen.ZERO_MATRIX)
    for i in range(row_a):
        for j in range(col_b):
            res.data[i][j] -= tmp_row[i] + tmp_col[i]
            for k in range(1, col_a, 2):
                res.data[i][j] += (a.data[i][k - 1] + b.data[k][j]) * (a.data[i][k] + b.data[k - 1][j])
            if (flag):
                res.data[i][j] += a.data[i][col_a - 1] * b.data[col_a - 1][j]

    return res

