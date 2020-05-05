#!/usr/bin/env python3

import sys
import queue

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
            if field.isalpha():
                key = field.lower()
                keys.add(key)
            elif field == "@":
                start = (x, y)
    return area, (width, height), start, keys

def bfs(area, size, start, keys):
    q = queue.Queue()
    remaining_keys = "".join(sorted([k for k in keys]))
    visited = set()

    visited.add((start, remaining_keys))
    q.put((start, remaining_keys, 0))

    while not q.empty():
        cur = q.get()
        cur_pos = cur[0]
        cur_keys = set(cur[1])
        cur_steps = cur[2]

        if len(cur_keys) == 0:
            return cur_steps

        for ds in [(-1, 0), (0, 1), (1, 0), (0, -1)]:
            new_pos = (cur_pos[0] + ds[0], cur_pos[1] + ds[1])
            if (new_pos[0] < 0) or (new_pos[0] >= size[0]) or (new_pos[1] < 0)\
            or (new_pos[1] >= size[1]):
                # out of bounds
                continue

            val = area[new_pos[1]][new_pos[0]]
            if val == "#":
                # hit wall
                continue


            if val.isalpha() and val.isupper() and val.lower() in cur_keys:
                # locked door
                continue

            if (new_pos, "".join(sorted([k for k in cur_keys]))) in visited:
                # already visited
                continue


            if val.isalpha() and val.islower() and val in cur_keys:
                # remove key from set
                cur_keys.remove(val)

            # Add to queue
            hashed_keys = "".join(sorted([k for k in cur_keys]))
            q.put((new_pos, hashed_keys, cur_steps + 1))
            visited.add((new_pos, hashed_keys))

            if val.isalpha() and val.islower():
                # add key again for remainder of this exploration step
                cur_keys.add(val)

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
        steps = bfs(area, size, start, keys)
        print("Minimal path length:", steps)

