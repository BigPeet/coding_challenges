use std::cmp::Reverse;
use std::collections::BinaryHeap;

type Position = (usize, usize);
type Risk = u32;

pub struct StraightNeighbourIterator {
    center: Position,
    index: usize,
    size: usize,
}

impl StraightNeighbourIterator {
    const LEN: usize = 4;
    const OFFSET: [(i32, i32); Self::LEN] = [(-1, 0), (0, -1), (0, 1), (1, 0)];

    fn new(center: Position, size: usize) -> StraightNeighbourIterator {
        StraightNeighbourIterator {
            center,
            index: 0,
            size,
        }
    }
}

impl Iterator for StraightNeighbourIterator {
    type Item = Position;

    fn next(&mut self) -> Option<Self::Item> {
        while self.index < Self::LEN {
            let offset = Self::OFFSET[self.index];
            self.index += 1;
            let dx = self.center.0 as i32 + offset.0;
            let dy = self.center.1 as i32 + offset.1;
            if dx >= 0 && (dx as usize) < self.size && dy >= 0 && (dy as usize) < self.size {
                return Some((dx as usize, dy as usize));
            }
        }
        None
    }
}

fn to_index(p: Position, l: usize) -> usize {
    p.1 * l + p.0
}

fn inc_risk(r: Risk) -> Risk {
    if r == 9 {
        1
    } else {
        r + 1
    }
}

pub fn extend_graph(graph: Vec<Vec<Risk>>) -> Vec<Vec<Risk>> {
    let size = graph.len();
    let mut extended = vec![vec![0; size * 5]; size * 5];
    for roff in 0..5 {
        for (r, row) in graph.iter().enumerate() {
            for coff in 0..5 {
                for (c, &val) in row.iter().enumerate() {
                    let mut new_val = val;
                    for _ in 0..(coff + roff) {
                        new_val = inc_risk(new_val);
                    }
                    extended[roff * size + r][coff * size + c] = new_val;
                }
            }
        }
    }
    extended
}

// Preconditions:
// graph is a square plane of risk levels.
// start and end are positions inside the graph.
pub fn dijkstra_path(graph: &[Vec<Risk>], start: Position, end: Position) -> (Vec<Position>, Risk) {
    let size = graph.len();

    // Initialize
    let mut distance = vec![None; size * size];
    let mut predecessors = vec![None; size * size];
    distance[to_index(start, size)] = Some(0);

    let mut heap = BinaryHeap::new();
    heap.push(Reverse((0, start)));

    // Always get the node with the smallest distance.
    while let Some(Reverse((dist, pos))) = heap.pop() {
        let iter = StraightNeighbourIterator::new(pos, size);
        for neighbour in iter {
            let nidx = to_index(neighbour, size);
            // Update distance
            let new_dist = dist + graph[neighbour.1][neighbour.0];
            if distance[nidx].map_or(true, |old_dist| new_dist < old_dist) {
                heap.push(Reverse((new_dist, neighbour)));
                distance[nidx] = Some(new_dist);
                predecessors[nidx] = Some(pos);
            }
        }
    }

    // Build path (optional for the task)
    let mut path = vec![end];
    let mut pos = end;
    while let Some(pred) = predecessors[to_index(pos, size)] {
        pos = pred;
        path.push(pos);
    }
    path.reverse();
    (path, distance[to_index(end, size)].unwrap_or(Risk::MAX))
}
