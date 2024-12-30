from dictionary import Dictionary
NOT_FOUND = -1
RECORD = 0
CMP = 1

def full_search(dict_a: Dictionary, search_key):
    count = 0
    keys = dict_a.data.keys()
    for key in keys:
        count += 1
        if (key == search_key):
            return [dict_a.data[key], count]
    return [NOT_FOUND, count] 

def bin_search(dict_a: Dictionary, key):
    count = 0
    sort_dict = Dictionary()
    sort_dict.data = dict_a.sort()
    keys = list(sort_dict.data.keys())
    left = 0 
    right = len(keys)
    # 6
    while (left <= right):          # 1
        count += 1                  # 1
        mid = (left + right) // 2   # 1+1+2
        key_mid = keys[mid]         # 1+1
        if (key_mid == key):        # 1
            return [sort_dict.data[key_mid], count] # 1
        elif (key_mid < key):       # 1
            left = mid + 1          # 1+1
        else:
            right = mid - 1         # 1+1
        # 15
    return [NOT_FOUND, count]

# favg = 6 + 15 * 8 = 126
