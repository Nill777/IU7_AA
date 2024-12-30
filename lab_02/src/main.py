import matplotlib.pyplot as plt 
from time import process_time
import csv
from mul_matrix import *

MENU = "\nМеню:\n\
1) Умножение матриц (стандартный алг.)\n\
2) Умножение матриц (алг. Винограда)\n\
3) Умножение матриц (оптимизированный алг. Винограда)\n\
4) Умножение матриц (все алгоритмы)\n\
5) Произвести замер времени работы алгоритмов\n\
0) Выход\n>"
EXIT = 0
ALG_STANDART = 1
ALG_GRAPE = 2
ALG_OPT_GRAPE = 3
ALL_MUL = 4
TEST_TIME = 5
MIN_SIZE = 1
MAX_SIZE = 100
STEP = 1
NUM_TIME_TESTS = 20
DIMENSION = 0
INVITATION_SIZE = "Введите размер матрицы "
INVITATION_MATRIX = "Введите матрицу "
RESULT_MATRIX = "Результирующая матрица:"
GRAPH_RANDOM = "random.svg"
GRAPH_BEST_GRAPE = "best_grape.svg"
GRAPH_WORST_GRAPE = "worst_grape.svg"
TIME_DEFAULT_FILE = "/home/andrey/SEM_05/AA/lab_02/src/time_default_data.csv"
TIME_BEST_FILE = "/home/andrey/SEM_05/AA/lab_02/src/time_best_data.csv"
TIME_WORST_FILE = "/home/andrey/SEM_05/AA/lab_02/src/time_worst_data.csv"
SUCCESS_WRITE = 0
ERROR_WRITE = 1
DEFAULT_CASE = 0
BEST_CASE = 1
WORST_CASE = 2

def save_time_scv(file_name, data, coloumn_names):
    try:
        with open(str(file_name), "w", encoding='utf-8') as file:
            writer = csv.writer(file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
            writer.writerow(coloumn_names)
            for i in range(len(data[DIMENSION])):
                writer.writerow(list(data[j][i] for j in range(data)))
        res = SUCCESS_WRITE
    except:
        res = ERROR_WRITE
    return res

def get_default_times(func):
    time = []
    for size in range(MIN_SIZE, MAX_SIZE, STEP):
        sum_time = 0
        for _ in range(NUM_TIME_TESTS):
            a = Matrix(size, Gen.RANDOM_MATRIX)
            b = Matrix(size, Gen.RANDOM_MATRIX)
            start = process_time()
            func(a, b)
            sum_time += process_time() - start
        time.append(sum_time / NUM_TIME_TESTS)
    return time

# четные размеры
def get_best_times(func):
    time = []
    for size in range(2, MAX_SIZE, 2):
        sum_time = 0
        for _ in range(NUM_TIME_TESTS):
            a = Matrix(size, Gen.RANDOM_MATRIX)
            b = Matrix(size, Gen.RANDOM_MATRIX)
            start = process_time()
            func(a, b)
            sum_time += process_time() - start
        time.append(sum_time / NUM_TIME_TESTS)
    return time

# нечетные размеры
def get_worst_times(func):
    time = []
    for size in range(1, MAX_SIZE, 2):
        sum_time = 0
        for _ in range(NUM_TIME_TESTS):
            a = Matrix(size, Gen.RANDOM_MATRIX)
            b = Matrix(size, Gen.RANDOM_MATRIX)
            start = process_time()
            func(a, b)
            sum_time += process_time() - start
        time.append(sum_time / NUM_TIME_TESTS)
    return time

def get_all_times(case):
    if (case == DEFAULT_CASE):
        return [[i for i in range(MIN_SIZE, MAX_SIZE, STEP)],
            get_default_times(mul_std),
            get_default_times(mul_grape),
            get_default_times(mul_grape_opt)]
    elif (case == BEST_CASE):
        return [[i for i in range(2, MAX_SIZE, 2)],
            get_best_times(mul_std),
            get_best_times(mul_grape),
            get_best_times(mul_grape_opt)]
    elif (case == WORST_CASE):
        return [[i for i in range(1, MAX_SIZE, 2)],
            get_worst_times(mul_std),
            get_worst_times(mul_grape),
            get_worst_times(mul_grape_opt)]

def plot_graph(dst_file, data, title=None):
    plt.plot(data[DIMENSION], data[ALG_STANDART], color="red", label="Стандартный алгоритм")
    plt.plot(data[DIMENSION], data[ALG_GRAPE], color="green", label="Алгоритм Винограда")
    plt.plot(data[DIMENSION], data[ALG_OPT_GRAPE], color="blue", label="Оптимизированный алгоритм Винограда")
    plt.xlabel("Размерность матрицы")
    plt.ylabel("Процессорное время (секунды)")
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.savefig(dst_file, format="svg", dpi=300, bbox_inches="tight")
    plt.show()

def action_menu_mul(func_mul):
    print(INVITATION_SIZE)
    size = int(input())
    print(INVITATION_MATRIX + "A:")
    a = Matrix.input_matrix(size)
    print(INVITATION_MATRIX + "B:")
    b = Matrix.input_matrix(size)
    res: Matrix = func_mul(a, b)
    print(RESULT_MATRIX)
    res.output_matrix()

def main():
    option = -1
    while (option != EXIT):
        option = int(input(MENU))
        if (option == ALG_STANDART):
            action_menu_mul(mul_std)
        elif (option == ALG_GRAPE):
            action_menu_mul(mul_grape)
        elif (option == ALG_OPT_GRAPE):
            action_menu_mul(mul_grape_opt)
        elif (option == ALL_MUL):
            print(INVITATION_SIZE)
            size = int(input())
            print(INVITATION_MATRIX + "A:")
            a = Matrix.input_matrix(size)
            print(INVITATION_MATRIX + "B:")
            b = Matrix.input_matrix(size)

            res_std = mul_std(a, b)
            res_grape = mul_grape(a, b)
            res_grape_opt = mul_grape_opt(a, b)

            print("Стандартный алгоритм\n" + RESULT_MATRIX)
            res_std.output_matrix()
            print("Алгоритм Винограда\n" + RESULT_MATRIX)
            res_grape.output_matrix()
            print("Оптимизированный алгоритм Винограда\n" + RESULT_MATRIX)
            res_grape_opt.output_matrix()
        elif (option == TEST_TIME):
            data = get_all_times(DEFAULT_CASE)
            plot_graph(GRAPH_RANDOM, data, "Умножение матриц, общий случай")
            
            data = get_all_times(BEST_CASE)          
            plot_graph(GRAPH_BEST_GRAPE, data, "Умножение матриц, размерность чётная")
            
            data = get_all_times(WORST_CASE)          
            plot_graph(GRAPH_WORST_GRAPE, data, "Умножение матриц, размерность нечётная")  
        elif (option == EXIT):
            return 0
        else:
            print("Ошибка, некорректная команда\n")

if __name__ == "__main__":
    main()
