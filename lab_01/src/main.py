import time
import random
import string
import tracemalloc
import matplotlib.pyplot as plt 
import algorithms as alg

class Define:
    LEV_MATRIX = 1
    LEV_RECURSIVE = 2
    DAM_LEV_MATRIX = 3
    FILE_GRAPH_TIME = "graph_time.svg"
    FILE_GRAPH_MEMORY = "graph_memory.svg"
    NUM_TESTS = 100
    MILLISECONDS = 1000
    MAX_LEN_STRING = 10

def print_menu():
    print("Меню:\n"
        "1. Расстояние Левенштейна (матричный)\n"
        "2. Расстояние Левенштейна (рекурсивный)\n"
        "3. Расстояние Дамерау-Левенштейна (матричный)\n"
        "4. Замеры времени и памяти\n"
        "0. Выход")

def input_str(msg : str):
    try:
        tmp = input(msg)
    except Exception:
        tmp = "invalid_str"
    return tmp

def get_random_string(size):
    letters = string.ascii_lowercase
    return "".join(random.choice(letters) for _ in range(size))

def get_time(func, size):
    time_res = 0
    for _ in range(Define.NUM_TESTS):
        str1 = get_random_string(size)
        str2 = get_random_string(size)

        time_start = time.process_time()
        func(str1, str2)
        time_end = time.process_time()

        time_res += (time_end - time_start)

    return time_res / Define.NUM_TESTS

def get_memory(func, size):
    memory_res = 0
    for _ in range(Define.NUM_TESTS):
        str1 = get_random_string(size)
        str2 = get_random_string(size)

        tracemalloc.start()
        func(str1, str2)
        current, peak = tracemalloc.get_traced_memory()
        tracemalloc.stop()

        memory_res += peak

    return memory_res / Define.NUM_TESTS 

def graph_time(time_lm, time_lr, time_dlm):
    lens = [i for i in range(Define.MAX_LEN_STRING)]
    plt.figure(figsize=(10, 5))
    plt.plot(lens, time_lm, label='Алгоритм поиска расстояния Левинштейна(матричный)', marker='+')
    plt.plot(lens, time_lr, label='Алгоритм поиска расстояния Левинштейна(рекурсивный)', marker='s')
    plt.plot(lens, time_dlm, label='Алгоритм поиска расстояния Дамерау-Левинштейна(матричный)', marker='x')
    plt.xlabel('Длина строки(символ)')
    plt.ylabel('Время работы(мс)')
    plt.legend()
    plt.grid()
    plt.savefig(Define.FILE_GRAPH_TIME, format='svg')
    plt.show()

def graph_memory(mem_lm, mem_lr, mem_dlm):
    lens = [i for i in range(Define.MAX_LEN_STRING)]
    plt.figure(figsize=(10, 5))
    plt.plot(lens, mem_lm, label='Алгоритм поиска расстояния Левинштейна(матричный)', marker='+')
    plt.plot(lens, mem_lr, label='Алгоритм поиска расстояния Левинштейна(рекурсивный)', marker='s')
    plt.plot(lens, mem_dlm, label='Алгоритм поиска расстояния Дамерау-Левинштейна(матричный)', marker='x')
    plt.xlabel('Длина строки(символ)')
    plt.ylabel('Память(байт)')
    plt.legend()
    plt.grid()
    plt.savefig(Define.FILE_GRAPH_MEMORY, format='svg')
    plt.show()

def test():
    time_lm = []
    time_lr = []
    time_dlm = []
    mem_lm = []
    mem_lr = []
    mem_dlm = []

    for i in range(Define.MAX_LEN_STRING):
        time_lm.append(get_time(alg.levenstein_matrix, i))
        time_lr.append(get_time(alg.levenstein_recursive, i))
        time_dlm.append(get_time(alg.damerau_levenstein_matrix, i))

        mem_lm.append(get_memory(alg.levenstein_matrix, i))
        mem_lr.append(get_memory(alg.levenstein_recursive, i))
        mem_dlm.append(get_memory(alg.damerau_levenstein_matrix, i))

    print("TIME")
    print("№    LM    LR    DLM")

    for i in range(Define.MAX_LEN_STRING):
        print("%2d & %.4f & %.4f & %.4f \\\\\\hline" %(i+1, \
            time_lm[i] * Define.MILLISECONDS, \
            time_lr[i] * Define.MILLISECONDS, \
            time_dlm[i] * Define.MILLISECONDS))
        
    print("MEMORY")
    print("№    LM    LR    DLM")

    for i in range(Define.MAX_LEN_STRING):
        print("%2d & %d & %d & %d \\\\\\hline" % (i + 1, \
            mem_lm[i], \
            mem_lr[i], \
            mem_dlm[i]))

    graph_time(time_lm=time_lm, time_lr=time_lr, time_dlm=time_dlm)
    graph_memory(mem_lm=mem_lm, mem_lr=mem_lr, mem_dlm=mem_dlm)

def main():
    while True:
        print_menu()
        choice = input("> ")

        if choice == '1':
            str1 = input_str("Введите строку 1: ")
            str2 = input_str("Введите строку 2: ")
            print("Расстояние: ", alg.levenstein_matrix(str1, str2))
        elif choice == '2':
            str1 = input_str("Введите строку 1: ")
            str2 = input_str("Введите строку 2: ")
            print("Расстояние: ", alg.levenstein_recursive(str1, str2))
        elif choice == '3':
            str1 = input_str("Введите строку 1: ")
            str2 = input_str("Введите строку 2: ")
            print("Расстояние: ", alg.damerau_levenstein_matrix(str1, str2))
        elif choice == '4':
            test()
        elif choice == '0':
            break
        else:
            print("Некорректное действие. Попробуйте снова.")

if __name__ == "__main__":
    main()