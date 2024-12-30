import utime
import urandom

def not_recursive_levenstein(string_one: str, string_two: str) -> int:
    rows = len(string_one) + 1
    cols = len(string_two) + 1
    matrix = [[0] * cols for i in range(rows)]
    for i in range(1, cols):
        matrix[0][i] = i
    for i in range(1, rows):
        matrix[i][0] = i

    for i in range(1, rows):
        for j in range(1, cols):
                matrix[i][j] = min(
                    matrix[i][j - 1] + 1, 
                    matrix[i - 1][j] + 1, 
                    matrix[i - 1][j - 1]  + (0 if string_one[i - 1] == string_two[j - 1] else 1)
                )

    return matrix[-1][-1]

def recursive_levenstein(string_one: str, string_two: str) -> int:
    if len(string_one) == 0:
        return len(string_two)
    if len(string_two) == 0:
        return len(string_one)
    if string_one[0] == string_two[0]:
        return recursive_levenstein(string_one[1:], string_two[1:])
    return 1 + min(
        recursive_levenstein(string_one[1:], string_two),
        recursive_levenstein(string_one[1:], string_two[1:]),
        recursive_levenstein(string_one, string_two[1:])
    )

def damerau_levenstein(string_one: str, string_two: str) -> int:
    rows = len(string_one) + 1
    cols = len(string_two) + 1
    matrix = [[0] * cols for i in range(rows)]
    for i in range(1, cols):
        matrix[0][i] = i
    for i in range(1, rows):
        matrix[i][0] = i

    for i in range(1, rows):
        for j in range(1, cols):
                if i >= 2 and j >= 2 and string_one[i - 1] == string_two[j - 2] and string_one[i - 2] == string_two[j - 1]:
                    matrix[i][j] = min(
                        matrix[i - 1][j] + 1,
                        matrix[i - 1][j - 1] + (0 if string_one[i - 1] == string_two[j - 1] else 1),
                        matrix[i - 2][j - 2] + 1,
                        matrix[i][j - 1] + 1,
                    )
                else:
                    matrix[i][j] = min(
                        matrix[i][j - 1] + 1, 
                        matrix[i - 1][j] + 1, 
                        matrix[i - 1][j - 1]  + (0 if string_one[i - 1] == string_two[j - 1] else 1)
                    )

    return matrix[-1][-1]

minLength = 2
maxLength = 8
testCount = 100

urandom.seed(utime.time())

recursiveTests = {}
cacheTests = {}
damerauTests = {}

for i in range(minLength, maxLength + 1):
    s1 = ''
    for j in range(i):
        if urandom.randint(0, 1):
            s1 += chr(urandom.randint(ord('A'), ord('Z')))
        else:
            s1 += chr(urandom.randint(ord('a'), ord('z')))
    s2 = ''
    for j in range(i):
        if urandom.randint(0, 1):
            s2 += chr(urandom.randint(ord('A'), ord('Z')))
        else:
            s2 += chr(urandom.randint(ord('a'), ord('z')))

    start = utime.ticks_ms()
    for _ in range(testCount):
        _ = recursive_levenstein(s1, s2)
    end = utime.ticks_ms()
    recursiveTests[i] = (end - start) // testCount


    start = utime.ticks_ms()
    for _ in range(testCount):
        _ = not_recursive_levenstein(s1, s2)
    end = utime.ticks_ms()
    cacheTests[i] = (end - start) // testCount

    start = utime.ticks_ms()
    for _ in range(testCount):
        _ = damerau_levenstein(s1, s2)
    end = utime.ticks_ms()
    damerauTests[i] = (end - start) // testCount

for i in range(minLength, maxLength + 1):
    print(i, str(recursiveTests[i]) + "ms", str(cacheTests[i]) + "ms", str(damerauTests[i]) + "ms")
