def levenstein_matrix(str1: str, str2: str) -> int:
    rows = len(str1) + 1
    cols = len(str2) + 1
    matrix = [[0] * cols for i in range(rows)]
    for i in range(1, cols):
        matrix[0][i] = i
    for i in range(1, rows):
        matrix[i][0] = i

    for i in range(1, rows):
        for j in range(1, cols):
            if str1[i - 1] == str2[j - 1]: cost = 0
            else: cost = 1
            matrix[i][j] = min(
                matrix[i][j - 1] + 1, 
                matrix[i - 1][j] + 1, 
                matrix[i - 1][j - 1]  + cost
            )

    return matrix[-1][-1]

def levenstein_recursive(str1: str, str2: str) -> int:
    if (len(str1) == 0 or len(str2) == 0):
        return max(len(str1), len(str2))
    if str1[0] == str2[0]:
        return levenstein_recursive(str1[1:], str2[1:])
    return 1 + min(
        levenstein_recursive(str1[1:], str2),
        levenstein_recursive(str1[1:], str2[1:]),
        levenstein_recursive(str1, str2[1:])
    )

def damerau_levenstein_matrix(str1: str, str2: str) -> int:
    rows = len(str1) + 1
    cols = len(str2) + 1
    matrix = [[0] * cols for i in range(rows)]
    for i in range(1, cols):
        matrix[0][i] = i
    for i in range(1, rows):
        matrix[i][0] = i

    for i in range(1, rows):
        for j in range(1, cols):
            if i >= 2 and j >= 2 and str1[i - 1] == str2[j - 2] and str1[i - 2] == str2[j - 1]:
                matrix[i][j] = min(
                    matrix[i - 1][j] + 1,
                    matrix[i - 1][j - 1] + (0 if str1[i - 1] == str2[j - 1] else 1),
                    matrix[i - 2][j - 2] + 1,
                    matrix[i][j - 1] + 1,
                )
            else:
                matrix[i][j] = min(
                    matrix[i][j - 1] + 1, 
                    matrix[i - 1][j] + 1, 
                    matrix[i - 1][j - 1]  + (0 if str1[i - 1] == str2[j - 1] else 1)
                )

    return matrix[-1][-1]
