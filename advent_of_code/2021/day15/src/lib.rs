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

fn to_pos(idx: usize, l: usize) -> Position {
    (idx.rem_euclid(l), idx / l)
}

// Preconditions:
// graph is a square plane of risk levels.
// start and end are positions inside the graph.
pub fn dijkstra_path(graph: &[Vec<Risk>], start: Position, end: Position) -> (Vec<Position>, Risk) {
    let size = graph.len();

    // Initialize
    let mut distance = vec![None; size * size];
    let mut predecessors = vec![None; size * size];
    let mut visited = vec![false; size * size];
    distance[to_index(start, size)] = Some(0);

    // Iterate until all nodes are visited.
    // Always get the node with the smallest distance.
    while let Some((next, dist)) = visited
        .iter()
        .zip(distance.iter())
        .enumerate()
        .filter_map(|(i, (&v, &dist))| {
            if !v && dist.is_some() {
                Some((i, dist.unwrap()))
            } else {
                None
            }
        })
        .min_by_key(|(_, dist)| *dist)
    {
        visited[next] = true;
        let pos = to_pos(next, size);
        let iter = StraightNeighbourIterator {
            center: pos,
            index: 0,
            size,
        };

        for neighbour in iter {
            let nidx = to_index(neighbour, size);
            if !visited[nidx] {
                // Update distance
                let new_dist = dist + graph[neighbour.1][neighbour.0];
                if distance[nidx].is_none() || new_dist < distance[nidx].unwrap() {
                    distance[nidx] = Some(new_dist);
                    predecessors[nidx] = Some(pos);
                }
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
