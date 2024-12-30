import numpy as np
import itertools as it
from random import random

INIT_PHEROMONE = 1
MIN_PHEROMONE = 0.01

def brute_force_voyager(matrix, size):
    cities = list(range(size))
    permutations = it.permutations(cities)

    min_len = float("inf")
    best_route = None
    for perm in permutations:
        cur_route = list(perm)
        cur_len = 0

        for i in range(len(cur_route) - 1):
            cur_len += matrix[cur_route[i]][cur_route[i + 1]]

        if cur_len < min_len:
            min_len = cur_len
            best_route = cur_route

    return min_len, best_route

def daily_quota_Q(matrix, size):
    matrix = np.array(matrix)
    total_sum = np.sum(matrix) - np.sum(np.diag(matrix))
    count = size * (size - 1)
    return total_sum / count

def calc_len_route(matrix, route):
    length = sum(matrix[route[i], route[i + 1]] for i in range(len(route) - 1))
    return length

def update_pheromones(matrix, size, visited, pheromones, daily_quota, k_evaporation):
    for i in range(size):
        for j in range(size):
            delta_pheromones = 0
            for ant in range(size):
                length = calc_len_route(matrix, visited[ant])
                delta_pheromones += daily_quota / length

            pheromones[i][j] = pheromones[i][j] * (1 - k_evaporation) + delta_pheromones
            pheromones[i][j] = max(pheromones[i][j], MIN_PHEROMONE)

    return pheromones

def get_matrix_pheromones(size):
    return [[INIT_PHEROMONE] * size for _ in range(size)]

def get_matrix_visibility(matrix, size):
    visibility_matrix = []
    for i in range(size):
        row = []
        for j in range(size):
            if i != j:
                row.append(1.0 / matrix[i][j])
            else:
                row.append(0)
        visibility_matrix.append(row)

    return visibility_matrix

def get_matrix_visited(route, ants):
    visited = [[] for _ in range(ants)]
    for ant in range(ants):
        visited[ant].append(int(route[ant]))

    return visited

def choose_next_city(pheromones, ant, alpha, beta, visibility, visited, cities):
    probabilities = [0] * cities

    # Вероятности для каждого города
    for city in range(cities):
        if city not in visited[ant]:
            ant_city = visited[ant][-1]
            probabilities[city] = (pheromones[ant_city][city] ** alpha) * (visibility[ant_city][city] ** beta)
        else:
            probabilities[city] = 0

    sum_probabilities = sum(probabilities)
    if sum_probabilities > 0:
        probabilities = [p / sum_probabilities for p in probabilities]
    else:
        return None  # нет доступных мест

    posibility = random()
    # np.cumsum([0.1, 0.3, 0.2, 0.4]) Результат: [0.1, 0.4, 0.6, 1.0]
    cumulative_p = np.cumsum(probabilities)
    chosen_city = np.searchsorted(cumulative_p, posibility)
    
    return int(chosen_city)

def ant_alg(matrix, cities, alpha, beta, days, k_evaporation):
    best_route = []
    min_len = float("inf")
    daily_quota = daily_quota_Q(matrix, cities)
    pheromones = get_matrix_pheromones(cities)
    visibility = get_matrix_visibility(matrix, cities)
    ants = cities

    for day in range(days):
        route = np.arange(cities)
        visited = get_matrix_visited(route, ants)   # Каждого муравья в свой город
        for ant in range(ants):
            while (len(visited[ant]) != ants):
                chosen_city = choose_next_city(pheromones, ant, alpha, beta, visibility, visited, cities)
                visited[ant].append(chosen_city)    # -1

            cur_len = calc_len_route(matrix, visited[ant]) 
            if (cur_len < min_len):
                min_len = cur_len
                best_route = visited[ant]

        pheromones = update_pheromones(matrix, cities, visited, pheromones, daily_quota, k_evaporation)

    return min_len, best_route
