VIN = 0
BRAND = 1
YEAR = 2
COST = 3
class Dictionary(object):
    data = dict()

    def __init__(self, file_name=None):
        if (file_name):
            self.load_csv(file_name)
    
    def load_csv(self, file_name):
        data = []
        with open(file_name, "r") as file:
            for line in file.readlines()[1:]:
                data.append(list(line.split(",")))
        file.close()

        for i in range(len(data)):
            key = data[i][VIN]
            val = data[i][BRAND:]
            self.data[key] = val
        return 0
    
    def sort(self):
        sort_dict = dict()
        keys = list(self.data.keys())
        keys.sort()
        for key in keys:
            sort_dict[key] = self.data[key]
        return sort_dict

    def print_record(self, key):
        print(f"\nVIN: {key}\n"\
            f"Brand: {self.data[key][BRAND - 1]}\n"\
            f"Year: {self.data[key][YEAR - 1]}\n"\
            f"Cost: {self.data[key][COST - 1]}")   
             