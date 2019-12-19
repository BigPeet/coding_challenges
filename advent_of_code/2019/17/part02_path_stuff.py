#!/usr/bin/python3

def get_sequence_size(seq_tokens):
    length = 0

    # The length of the symbols
    for t in seq_tokens:
        length += len(t)

    # Extra length due to ,
    return length + len(seq_tokens) - 1

def get_ngrams(tokens, max_size):
    stop = False
    ngrams = []

    n = 1
    while (not stop):
        stop = True
        for i in range(len(tokens) + 1 - n):
            ngram = tokens[i:i+n]
            if (ngram not in ngrams) and (get_sequence_size(ngram) <= max_size):
                ngrams.append(ngram)
                stop = False
        n += 1
    return ngrams

def check_combination(tokens, A, B, C, index, order):

    if (index == len(tokens)):
        return True, order

    if (index > len(tokens)):
        return False, order

    fits = False
    if (index+len(A) <= len(tokens)) and (A == tokens[index:index+len(A)]):
        fits, order = check_combination(tokens, A, B, C, index + len(A), order)
        if fits:
            order.insert(0, "A")

    if (not fits) and (index+len(B) <= len(tokens)) and (B == tokens[index:index+len(B)]):
        fits, order = check_combination(tokens, A, B, C, index + len(B), order)
        if fits:
            order.insert(0, "B")

    if (not fits) and (index+len(C) <= len(tokens)) and (C == tokens[index:index+len(C)]):
        fits, order = check_combination(tokens, A, B, C, index + len(C), order)
        if fits:
            order.insert(0, "C")
    return fits, order


def find_combination(tokens, ngrams, max_size):

    length = len(ngrams)

    # Could be sped up by separating the search to multiple threads for the outer loop
    for i in range(length):
        for j in range(1, length):
            for k in range(2, length):
                if i == j or i == k or k == j:
                    # Assuming we want to use 3 different sub routines
                    continue

                A = ngrams[i]
                B = ngrams[j]
                C = ngrams[k]

                fits, order = check_combination(tokens, A, B, C, 0, [])
                if fits and get_sequence_size(order) <= max_size:
                    return order, A, B, C

    return None, None, None, None


def find_subroutines(path, max_size):
    print("Path: ", path)

    path_tokens = path.split(",")
    ngrams = get_ngrams(path_tokens, max_size)
    print("Ngrams: ", len(ngrams))

    solution, A, B, C = find_combination(path_tokens, ngrams, max_size)

    print("Solution: ", solution)
    print("A: ", A)
    print("B: ", B)
    print("C: ", C)


if __name__ == "__main__":

    # Example path from task.md
    example_path = "R,8,R,8,R,4,R,4,R,8,L,6,L,2,R,4,R,4,R,8,R,8,R,8,L,6,L,2"
    find_subroutines(example_path, 20)

    # Path for my input
    # "Generated" by doing the following:
    # Follow current direction for as long as possible (until you would fall off).
    # Then turn either left or right (whichever is possible)
    # If you hit a dead end, you're done (at least with my input, but I assume
    # all the inputs only have two dead ends, with one at your start.)
    solution_path = "L,12,L,8,L,8,L,12,R,4,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,R,4,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,R,4,L,12,R,6"
    find_subroutines(solution_path, 20)

