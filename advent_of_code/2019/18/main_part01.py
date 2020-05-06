#!/usr/bin/env python3

import sys
import queue
import heapq

def parse_input(input):
    keys = set()
    height = 0
    width = 0
    start = None

    area = input.split()
    height = len(area)
    width = len(area[0])
    for y, row in enumerate(area):
        for x, field in enumerate(row):
            if field.islower():
                keys.add((field, (x, y)))
            elif field == "@":
                start = (x, y)
    return area, (width, height), start, keys

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


def dijkstra(keys, distances):
    q = [(0, "@", frozenset([label for label, pos in keys]))]
    dist = dict()
    while len(q) > 0:
        steps, label, remaining_keys = heapq.heappop(q)
        if (label, remaining_keys) in dist:
            continue
        dist[(label, remaining_keys)] = steps

        if len(remaining_keys) == 0:
            return steps

        for n_label, (n_steps, n_doors) in distances[label].items():
            if n_doors.isdisjoint(remaining_keys) and n_label in remaining_keys:
                n_keys = set(remaining_keys)
                n_keys.remove(n_label)
                heapq.heappush(q, (n_steps + steps, n_label, frozenset(n_keys)))
    return -1


if __name__ == "__main__":
    content = ""
    with open(sys.argv[1], "r") as f:
        content = f.read().strip()
    print(content)
    area, size, start, keys = parse_input(content)

    if start is None:
        print("Error in parsing input: No start defined.")
    else:
        distances = dict()
        distances["@"] = bfs(area, size, start)
        for key, pos in keys:
            distances[key] = bfs(area, size, pos)

        steps = dijkstra(keys, distances)
        print("Minimal path length:", steps)

