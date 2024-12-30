from random import randint
import numpy as np
from os import makedirs

def generate_matrix(size, start, end):
    matrix = np.zeros((size, size), dtype = int)

    for i in range(size):
        for j in range(size):
            if (i == j):
                num = 0
            else:
                num = randint(start, end)
            matrix[i][j] = num
            matrix[j][i] = num

    return matrix

def generate_matrix_file(file_name, size, rand_start, rand_end):
    matrix = generate_matrix(size, rand_start, rand_end)
    makedirs("data", exist_ok=True)
    with open("data/" + file_name, "w") as file:
        for i in range(size):
            string = ""
            for j in range(size):
                string += str(matrix[i][j])
                string += " "
            string += "\n"
            file.write(string)

def main():
    print("Введите")
    file = input("Имя файла: ")
    size = int(input("Размер матрицы: "))
    start = int(input("Начальное число: "))
    end = int(input("Конечное число: "))
    generate_matrix_file(file + ".csv", size, start, end)
    print("Файл успешно сгенерирован")

if __name__ == "__main__":
    main()