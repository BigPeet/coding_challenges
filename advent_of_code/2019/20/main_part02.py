#!/usr/bin/env python3

import sys
import queue
import heapq

INNER = 0
OUTER = 1

def add_portal(portals, label, position, outer):
    print(label, position, outer)
    if label not in portals.keys():
        portals[label] = [None, None]
    if outer:
        portals[label][OUTER] = position
    else:
        portals[label][INNER] = position

def is_outer(x, y, width, height):
    return (x == 0) or (y == 0) or \
            (x == width - 1) or (y == height - 1)

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
                        p_x = x - 3
                        p_y = y - 2
                        add_portal(portals, a + b, (p_x, p_y), is_outer(p_x, \
                            p_y, len(area[0]), len(area)))
                    elif lines[y][x + 2] == ".": # assumed to exist in this case
                        p_y = y - 2
                        add_portal(portals, a + b, (x, p_y), is_outer(x, \
                            p_y, len(area[0]), len(area)))

                # below
                else:
                    b = lines[y + 1][x]
                    if b.isupper():
                        # find linked field (either above a or below b)
                        if y > 0 and lines[y - 1][x] == ".":
                            p_x = x - 2
                            p_y = y - 3
                            add_portal(portals, a + b, (p_x, p_y), \
                                    is_outer(p_x, p_y, len(area[0]), len(area)))
                        elif lines[y + 2][x] == ".": # assumed to exist in this case
                            p_x = x - 2
                            add_portal(portals, a + b, (p_x, y), \
                                is_outer(p_x, y, len(area[0]), len(area)))


    return area, portals

def get_portal_at(portals, given_pos):
    for label, positions in portals.items():
        for i, pos in enumerate(positions):
            if given_pos == pos:
                return label, pos, i
    return None

def get_goal_portal_from(portals, given_pos):
    for label, positions in portals.items():
        if len(positions) == 2:
            for i, pos in enumerate(positions):
                if given_pos == pos:
                    return label, positions[(i + 1) % 2]
    return None



def bfs(area, portals, start, goal):
    q = queue.Queue()
    visited = set()
    distances = dict()
    q.put((start, 0, 0))

    while not q.empty():
        pos, level, steps = q.get()

        if (pos, level) in visited:
            continue
        visited.add((pos, level))


        portal = get_portal_at(portals, pos)
        if pos != start and portal:
            # found a portal, note down distance to start
            distances[portal[0] + str(portal[2])] = steps

            if pos == goal and level == 0:
                # found goal
                return distances

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
                        if portal[2] == OUTER:
                            new_level = level - 1
                        else:
                            new_level = level + 1
                        q.put((goal[1], steps + 1, new_level))
                continue


            # hit a "."
            if level > 0 and (new_pos == portals["AA"][OUTER] or \
                    new_pos == portals["ZZ"][OUTER]):
                # treat AA and ZZ as wall on inner levels
                continue
            if level == 0:
                new_portal = get_portal_at(portals, new_pos)
                if new_portal:
                    if new_portal[2] == OUTER and new_portal[0] != "AA" \
                            and new_portal[0] != "ZZ":
                        # treat every outer portal except AA and ZZ as wall on
                        # level 0
                        continue
            q.put((new_pos, steps + 1, level))

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
        content = f.read()
    area, portals = parse_input(content)

    print(content)

    print(portals)

    start = portals["AA"][OUTER]
    end = portals["ZZ"][OUTER]
    distances = bfs(area, portals, start, end)

    steps = distances[end]
    print("Minimal path length:", steps)
