import os
import csv
from random import randint, choice

from config import faker_ru,\
                    faker_en,\
                    cur_dir,\
                    TRANSPORT,\
                    COUNT,\
                    VEHICLE_BRANDS
SUCCESS = 0
ERROR_WRITE = 1

try:
    os.mkdir(cur_dir)
except:
    print("Directory /data exist")

def create_transport(count):
    transports = []
    vins = []
    for i in range(count):
        vin = faker_en.vin()
        while (vin in vins):
            vin = faker_en.vin()
        vins.append(vin)
        transport = [vin,
                choice(VEHICLE_BRANDS),
                faker_en.year(),
                randint(100, 500)]
        transports.append(transport)
    return transports


def write_cvs(file_name, data, coloumn_names):
    try:
        with open(str(cur_dir + file_name), "w", encoding='utf-8') as file:
            writer = csv.writer(file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
            writer.writerow(coloumn_names)
            for i in range(len(data)):
                writer.writerow(data[i])
        res = SUCCESS
    except:
        res = ERROR_WRITE
    return res


def gen_transport(file_name, count):
    return write_cvs(file_name, create_transport(count), ["vin", "brand", "year", "cost"])


def is_exist(file_name):
    flag_exist = True
    if (os.path.exists(cur_dir + file_name)):
        ans = input("Rewrite " + file_name + " Y/n: ")
        if (ans not in ["Y", "y", ""]):
            flag_exist = False
    return flag_exist


if (__name__ == "__main__"):
    if (is_exist(TRANSPORT)):
        print("Transports generating")
        if (not gen_transport(TRANSPORT, COUNT)):
            print("Success")
        else:    print("Fail")
