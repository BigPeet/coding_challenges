#!/usr/bin/env python3

import sys
import queue
import heapq

def add_portal(portals, label, position):
    if label in portals.keys():
        portals[label] += (position, )
    else:
        portals[label] = (position, )

def parse_input(input):

    lines = input.split("\n")[:-1] #skip last empty line
    area = []

    for line in lines[2:-2]:
        w = []
        for c in line[2:-2]:
            val = c
            if c.isupper():
                val = " "
            w.append(val)
        area.append(w)

    portals = dict()

    for y in range(len(lines) - 1):
        for x in range(len(lines[y]) - 1):
            a = lines[y][x]
            if a.isupper():
                # found portal, look below or right for second char

                # right
                b = lines[y][x + 1]
                if b.isupper():
                    # find linked field (either left of a or right of b)
                    if x > 0 and lines[y][x - 1] == ".":
                        add_portal(portals, a + b, (x - 3, y - 2))
                    elif lines[y][x + 2] == ".": # assumed to exist in this case
                        add_portal(portals, a + b, (x, y - 2))

                # below
                else:
                    b = lines[y + 1][x]
                    if b.isupper():
                        # find linked field (either above a or below b)
                        if y > 0 and lines[y - 1][x] == ".":
                            add_portal(portals, a + b, (x - 2, y - 3))
                        elif lines[y + 2][x] == ".": # assumed to exist in this case
                            add_portal(portals, a + b, (x - 2, y))


    return area, portals

def get_portal_at(portals, given_pos):
    for label, positions in portals.items():
        for pos in positions:
            if given_pos == pos:
                return label, pos
    return None

def get_goal_portal_from(portals, given_pos):
    for label, positions in portals.items():
        if len(positions) == 2:
            for i, pos in enumerate(positions):
                if given_pos == pos:
                    return label, positions[(i + 1) % 2]
    return None



def bfs(area, portals, start):
    q = queue.Queue()
    visited = set()
    distances = dict()
    q.put((start, 0))

    while not q.empty():
        pos, steps = q.get()

        if pos in visited:
            continue
        visited.add(pos)

        portal = get_portal_at(portals, pos)
        if pos != start and portal:
            # found a portal, note down distance to start
            distances[portal[0] + str(portal[1])] = steps

        for ds in [(-1, 0), (0, 1), (1, 0), (0, -1)]:
            new_pos = (pos[0] + ds[0], pos[1] + ds[1])
            if (new_pos[0] < 0) or (new_pos[0] >= len(area[0])) or (new_pos[1] < 0)\
            or (new_pos[1] >= len(area)):
                # out of bounds: replace with " "
                val = " "
            else:
                val = area[new_pos[1]][new_pos[0]]

            if val == "#":
                # hit wall
                continue

            if val == " ":
                if portal:
                    # hit an empty field, while standing on a portal: go through
                    # portal
                    goal = get_goal_portal_from(portals, portal[1])
                    if goal:
                        q.put((goal[1], steps + 1))
                continue


            # hit a "."
            q.put((new_pos, steps + 1))

    return distances

if __name__ == "__main__":
    content = ""
    with open(sys.argv[1], "r") as f:
        content = f.read()
    area, portals = parse_input(content)

    start = portals["AA"][0]
    end = "ZZ" + str(portals["ZZ"][0])
    distances = bfs(area, portals, start)

    steps = distances[end]
    print("Minimal path length:", steps)
