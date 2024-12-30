from random import randint

ERROR_INPUT_ROW = 1

class Gen:
    ZERO_MATRIX = 0
    SINGLE_MATRIX = 1
    RANDOM_MATRIX = 2

class Matrix(object):
    def __init__(self, size, flag_gen=None):
        data = []
        size: int
        if (flag_gen == Gen.RANDOM_MATRIX):
            self.gen_random_matrix(size)
        elif (flag_gen == Gen.ZERO_MATRIX):
            self.gen_matrix(size, 0)
        elif (flag_gen == Gen.SINGLE_MATRIX):
            self.gen_matrix(size, 1)
        else:
            self.data = []
            self.size = size
        return
    
    def input_matrix(size):
        matrix = Matrix(size)
        for i in range(size):
            row = list(int(j) for j in input().split())
            if (len(row) != size):
                return ERROR_INPUT_ROW
            matrix.data.append(row)
        return matrix

    def output_matrix(self):
        for i in range(self.size):
            row = list(self.data[i][j] for j in range(self.size))
            for k in range(len(row)):
                print("{:4d}".format(row[k]), end="")
            print()
        
    def gen_matrix(self, size, value):
        self.data = []
        self.size = size
        for i in range(size):
            row = list(value for j in range(size))
            self.data.append(row)

    def gen_random_matrix(self, size):
        self.data = []
        self.size = size
        for i in range(size):
            row = list(randint(1, 100) for j in range(size))
            self.data.append(row)

