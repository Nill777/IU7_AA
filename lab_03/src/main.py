import matplotlib.pyplot as plt 
from dictionary import Dictionary
import search as search

MENU = "\nМеню:\n\
1) Линейный поиск\n\
2) Бинарный поиск\n\
3) Построить график линейного поиска с сортировкой по возрастанию позиции\n\
4) Построить график бинарного поиска с сортировкой по возрастанию позиции\n\
5) Построить график бинарного поиска с сортировкой по возрастанию количества сравнений\n\
0) Выход\n>"
EXIT = 0
FULL_SEARCH = 1
BIN_SEARCH = 2
GRAPH_FULL = 3
GRAPH_BIN = 4
GRAPH_BIN_CMP = 5
STEP = 1

def data_sort_pos(dict_a: Dictionary, func_search, ):
    x = [-1]
    y = [func_search(dict_a, "EMPTY_VIN")[search.CMP]]
    pos = 0
    for key in list(dict_a.data.keys())[::STEP]:
        record = func_search(dict_a, key)
        x.append(pos)
        y.append(record[search.CMP])
        pos += STEP
    return [x, y]

def graph_full_sort_pos(dict_a: Dictionary):
    data = data_sort_pos(dict_a, search.full_search)
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.bar(data[0], data[1], color="blue", edgecolor="blue", linewidth=1)
    ax.set_xlabel('Позиция искомого элемента')
    ax.set_ylabel('Количество сравнений')
    ax.set_title('Линейный поиск')
    ax.grid(True)
    plt.savefig('./image/full_sort_pos.svg', format='svg', dpi=300, bbox_inches='tight')
    plt.show()

def graph_bin_sort_pos(dict_a: Dictionary):
    data = data_sort_pos(dict_a, search.bin_search)
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.bar(data[0], data[1], color="green", edgecolor="green", linewidth=1)
    ax.set_xlabel('Позиция искомого элемента')
    ax.set_ylabel('Количество сравнений')
    ax.set_title('Бинарный поиск')
    ax.grid(True)
    plt.savefig('./image/bin_sort_pos.svg', format='svg', dpi=300, bbox_inches='tight')
    plt.show()

def graph_bin_sort_cmp(dict_a: Dictionary):
    data = data_sort_pos(dict_a, search.bin_search)
    y = []
    for j in range(len(data[0])): 
        data[0][j] += 1
        y.append(data[1][j])
    y.sort()
    fig, ax = plt.subplots()
    ax.bar(data[0], y, color="red", edgecolor="red", linewidth=1)
    ax.set_ylabel('Количество сравнений')
    ax.set_title('Бинарный поиск с сортировкой по возрастанию количества сравнений')
    ax.set_xticks([])
    ax.grid(True)
    plt.savefig('./image/bin_sort_cmp.svg', format='svg', dpi=300, bbox_inches='tight')
    plt.show()

def main():
    option = -1
    dict_a = Dictionary("/home/andrey/SEM_05/AA/lab_03/src/data/transport.csv")
    while (option != EXIT):
        option = int(input(MENU))
        if (option == FULL_SEARCH):
            key = input("Введите VIN номер: ")
            record = search.full_search(dict_a, key)
            if (record[0] != search.NOT_FOUND):
                dict_a.print_record(key)
                print(f"Количество сравнений: {record[1]}")
            else:
                print(f"Данные по VIN номеру: {key} не найдены")
        elif (option == BIN_SEARCH):
            key = input("Введите VIN номер: ")
            record = search.bin_search(dict_a, key)
            if (record[0] != search.NOT_FOUND):
                dict_a.print_record(key)
                print(f"Количество сравнений: {record[1]}")
            else:
                print(f"Данные по VIN номеру: {key} не найдены")
        elif (option == GRAPH_FULL):
            graph_full_sort_pos(dict_a)
        elif (option == GRAPH_BIN):
            graph_bin_sort_pos(dict_a)
        elif (option == GRAPH_BIN_CMP):
            graph_bin_sort_cmp(dict_a)
        elif (option == EXIT):
            return 0
        else:
            print("Ошибка, некорректная команда\n")

if __name__ == "__main__":
    main()
