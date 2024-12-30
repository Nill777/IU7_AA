from time import process_time
import numpy as np
import matplotlib.pyplot as plt
import voyager as vr
from gen_matrix import generate_matrix

MENU = "\nМеню:\n\
1) Полный перебор\n\
2) Муравьиный алгоритм\n\
3) Оба алгоритма\n\
4) Замер времени\n\
5) Параметризация\n\
0) Выход\n>"

EXIT = 0
BRUTE_FORCE_ALG = 1
ANT_ALG = 2
ALL_ALG = 3
TIME_TEST = 4
PARAMETRIZATION = 5

FILE_MATRIX = "matrix.csv"
NUM_TESTS = 15
LATEX = 0

TIME_TESTS = 20
MIN_SIZE_MATRIX = 2
MAX_SIZE_MAYRIX = 12
FILE_GRAPH = "graph.svg"

def print_res(res):
    print("Минимальная длина пути: {}\nПуть: {}".format(res[0], res[1]))

def read_koefs():
    alpha = float(input("Введите коэффициент alpha: "))
    beta = 1 - alpha
    k_evaporation = float(input("Введите коэффициент evaporation: "))
    days = int(input("Введите количество дней: "))
    
    return alpha, beta, k_evaporation, days

def read_matrix(file_name):
    with open("data/" + file_name, "r") as file:
        size = len(file.readline().split())
        file.seek(0)
        matrix = np.zeros((size, size), dtype=int)

        for i, line in enumerate(file):
            matrix[i] = np.array(line.split(), dtype=int)

    return matrix

def print_matrix(matrix):
    for i in range(len(matrix)):
        for j in range(len(matrix[0])):
            print("{4d}".format(matrix[i][j]), end = "")
        print()

def parametrization(type):
    alphas = [0.1, 0.25, 0.5, 0.75, 0.9]
    k_evaporations = [0.1, 0.25, 0.5, 0.75, 0.9]
    days_lst = [100, 200, 500, 1000, 2000]
    size = 10
    if (type == LATEX):
        sep = " & "
        ender = " \\\\ \\hline"
    else:
        sep = " " 
        ender = ""

    matrix1 = read_matrix("matrix_1.csv")
    matrix2 = read_matrix("matrix_2.csv")
    matrix3 = read_matrix("matrix_3.csv")

    standard1 = vr.brute_force_voyager(matrix1, size)
    standard2 = vr.brute_force_voyager(matrix2, size)
    standard3 = vr.brute_force_voyager(matrix3, size)

    file1 = open("res/class1.csv", "w")
    file2 = open("res/class2.csv", "w")
    file3 = open("res/class3.csv", "w")

    count = 0
    count_all = len(alphas) * len(k_evaporations) * len(days_lst)

    for alpha in alphas:
        beta = 1 - alpha
        for k_eva in k_evaporations:
            for days in days_lst:
                max_deviation1 = 0
                max_deviation2 = 0
                max_deviation3 = 0

                median_deviation1 = []
                median_deviation2 = []
                median_deviation3 = []

                arithmetic_avg_deviation1 = 0
                arithmetic_avg_deviation2 = 0
                arithmetic_avg_deviation3 = 0

                for _ in range(NUM_TESTS):
                    res1 = vr.ant_alg(matrix1, size, alpha, beta, days, k_eva)
                    res2 = vr.ant_alg(matrix2, size, alpha, beta, days, k_eva)
                    res3 = vr.ant_alg(matrix3, size, alpha, beta, days, k_eva)

                    if (res1[0] - standard1[0] > max_deviation1):
                        max_deviation1 = res1[0] - standard1[0]
                    if (res2[0] - standard2[0] > max_deviation2):
                        max_deviation2 = res2[0] - standard2[0]
                    if (res3[0] - standard3[0] > max_deviation3):
                        max_deviation3 = res3[0] - standard3[0]

                    median_deviation1.append(res1[0] - standard1[0])
                    median_deviation2.append(res2[0] - standard2[0])
                    median_deviation3.append(res3[0] - standard3[0])

                    arithmetic_avg_deviation1 += res1[0] - standard1[0]
                    arithmetic_avg_deviation2 += res2[0] - standard2[0]
                    arithmetic_avg_deviation3 += res3[0] - standard3[0]
                    
                median_deviation1 = np.median(median_deviation1)
                median_deviation2 = np.median(median_deviation2)
                median_deviation3 = np.median(median_deviation3)

                arithmetic_avg_deviation1 /= float(NUM_TESTS)
                arithmetic_avg_deviation2 /= float(NUM_TESTS)
                arithmetic_avg_deviation3 /= float(NUM_TESTS)

                str1 = "%4d%s%3.2f%s%3.2f%s%5d%s%4d%s%4d%s%5.2f%s\n" \
                    % (count + 1, sep, alpha, sep, k_eva, sep, days, sep, max_deviation1, sep, median_deviation1, sep, arithmetic_avg_deviation1, ender)
                str2 = "%4d%s%3.2f%s%3.2f%s%5d%s%4d%s%4d%s%5.2f%s\n" \
                    % (count + 1, sep, alpha, sep, k_eva, sep, days, sep, max_deviation2, sep, median_deviation2, sep, arithmetic_avg_deviation2, ender)
                str3 = "%4d%s%3.2f%s%3.2f%s%5d%s%4d%s%4d%s%5.2f%s\n" \
                    % (count + 1, sep, alpha, sep, k_eva, sep, days, sep, max_deviation3, sep, median_deviation3, sep, arithmetic_avg_deviation3, ender)
                
                file1.write(str1)
                file2.write(str2)
                file3.write(str3)

                count += 1
                print("{}/{}".format(count, count_all))

    file1.close()
    file2.close()
    file3.close()

def plot_graph(sizes, brute_force, ant):
    plt.plot(sizes, brute_force, color="red", label="Алгоритм полного перебора")
    plt.plot(sizes, ant, color="green", label="Муравьиный алгоритм")
    plt.xlabel("Размерность матрицы")
    plt.ylabel("Процессорное время (секунды)")
    plt.legend()
    plt.grid(True)
    plt.savefig(FILE_GRAPH, format="svg", dpi=300, bbox_inches="tight")
    plt.show()

def time_tests():
    sizes = []
    brute_force = []
    ant = []
    for size in range(MIN_SIZE_MATRIX, MAX_SIZE_MAYRIX):
        sizes.append(size)
        sum_brute = 0
        sum_ant = 0
        for i in range(NUM_TESTS):
            matrix = generate_matrix(size, 1, 100)

            start_b = process_time()
            vr.brute_force_voyager(matrix, size)
            sum_brute += process_time() - start_b

            start_a = process_time()
            vr.ant_alg(matrix, size, 0.1, 0.9, 100, 0.1)
            sum_ant += process_time() - start_a

        brute_force.append(sum_brute / NUM_TESTS)
        ant.append(sum_ant / NUM_TESTS)

    plot_graph(sizes, brute_force, ant)

def main():
    option = -1
    while (option != EXIT):
        option = int(input(MENU))

        if (option == BRUTE_FORCE_ALG):
            matrix = read_matrix(FILE_MATRIX)
            size = len(matrix)
            start = process_time()
            res = vr.brute_force_voyager(matrix, size)
            end = process_time()
            print_res(res)
            print("Затраченное время: {}".format(end - start))
        elif (option == ANT_ALG):
            matrix = read_matrix(FILE_MATRIX)
            size = len(matrix)
            alpha, beta, k_evaporation, days = read_koefs()
            start = process_time()
            res = vr.ant_alg(matrix, size, alpha, beta, days, k_evaporation)
            end = process_time()
            print_res(res)
            print("Затраченное время: {}".format(end - start))
        elif (option == ALL_ALG):
            matrix = read_matrix(FILE_MATRIX)
            size = len(matrix)
            alpha, beta, k_evaporation, days = read_koefs()

            start = process_time()
            res = vr.brute_force_voyager(matrix, size)
            end = process_time()
            print("\n---Алгоритм полного перебора---")
            print_res(res)
            print("Затраченное время: {}".format(end - start))

            start = process_time()
            res = vr.ant_alg(matrix, size, alpha, beta, days, k_evaporation)
            end = process_time()
            print("\n---Муравьиный алгоритм---")
            print_res(res)
            print("Затраченное время: {}".format(end - start))
        elif (option == TIME_TEST):
            time_tests()
        elif (option == PARAMETRIZATION):
            parametrization(LATEX)
        elif (option == EXIT):
            return 0
        else:
            print("Ошибка, некорректная команда\n")

if __name__ == "__main__":
    main()
