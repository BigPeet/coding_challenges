#!/usr/bin/env python3

import sys
import queue
import heapq

def place_extra_robots(area):
    """
    replace ... with @#@
            .@.      ###
            ...      @#@
    """
    for y, row in enumerate(area):
        for x, field in enumerate(row):
            if field == "@":
                area[y][x] = "#"
                area[y + 1][x] = "#"
                area[y - 1][x] = "#"
                area[y][x + 1] = "#"
                area[y][x - 1] = "#"
                area[y - 1][x - 1] = "@"
                area[y - 1][x + 1] = "@"
                area[y + 1][x - 1] = "@"
                area[y + 1][x + 1] = "@"
                return area

def parse_input(input):
    keys = set()
    height = 0
    width = 0
    starts = []

    area = []
    lines = input.split()
    for line in lines:
        area.append([c for c in line])

    area = place_extra_robots(area)
    height = len(area)
    width = len(area[0])

    for y, row in enumerate(area):
        for x, field in enumerate(row):
            if field.islower():
                keys.add((field, (x, y)))
            elif field == "@":
                starts.append((x, y))
    return area, (width, height), starts, keys

def bfs(area, size, start):
    q = queue.Queue()
    visited = set()
    distances = dict()
    q.put((start, 0, set()))

    while not q.empty():
        pos, steps, doors = q.get()

        if pos in visited:
            continue
        visited.add(pos)

        val = area[pos[1]][pos[0]]
        if val.islower():
            # found a key, note down distance of key to start and the doors in
            # the way
            distances[val] = (steps, doors)

        for ds in [(-1, 0), (0, 1), (1, 0), (0, -1)]:
            new_pos = (pos[0] + ds[0], pos[1] + ds[1])
            if (new_pos[0] < 0) or (new_pos[0] >= size[0]) or (new_pos[1] < 0)\
            or (new_pos[1] >= size[1]):
                # out of bounds
                continue

            val = area[new_pos[1]][new_pos[0]]
            if val == "#":
                # hit wall
                continue

            # check for door
            new_doors = doors.copy()
            if val.isupper():
                new_doors.add(val.lower())

            # Add to queue
            q.put((new_pos, steps + 1, new_doors))

    return distances


def dijkstra(keys, distances, starts):
    q = []
    start_labels = tuple("@" + str(i) for i in range(len(starts)))
    q.append((0, start_labels, frozenset([label for label, pos in keys])))
    dist = dict()
    while len(q) > 0:
        steps, labels, remaining_keys = heapq.heappop(q)
        if (labels, remaining_keys) in dist:
            continue
        dist[(labels, remaining_keys)] = steps

        if len(remaining_keys) == 0:
            return steps

        for i, label in enumerate(labels):
            for n_label, (n_steps, n_doors) in distances[label].items():
                if n_doors.isdisjoint(remaining_keys) and n_label in remaining_keys:
                    n_keys = set(remaining_keys)
                    n_keys.remove(n_label)
                    n_labels = tuple(labels[:i] + (n_label,) + labels[i+1:])
                    heapq.heappush(q, (n_steps + steps, n_labels, frozenset(n_keys)))
    return -1


if __name__ == "__main__":
    content = ""
    with open(sys.argv[1], "r") as f:
        content = f.read().strip()
    print(content)
    area, size, starts, keys = parse_input(content)
    for line in area:
        print("".join(line))


    if len(starts) != 4:
        print("Error in parsing input: Not the expected amount of starts defined.")
    else:
        distances = dict()
        for i, pos in enumerate(starts):
            distances["@" + str(i)] = bfs(area, size, pos)
        for key, pos in keys:
            distances[key] = bfs(area, size, pos)
        steps = dijkstra(keys, distances, starts)
        print("Minimal path length:", steps)

