use std::io::{self, BufRead};

fn adjacency_list_to_matrix(adj_list: &Vec<Vec<usize>>, n: usize) -> Vec<Vec<u8>> {
    let mut adj_matrix = vec![vec![0; n]; n];
    for i in 0..adj_list.len() {
        for &neighbor in &adj_list[i] {
            adj_matrix[i][neighbor - 1] = 1;
        }
    }
    adj_matrix
}

fn find_first_c3_naive(adj_matrix: &Vec<Vec<u8>>) {
    // Sprawdza, czy graf zawiera cykl C3 i wypisuje pierwszy znaleziony.
    let n = adj_matrix.len();
    for i in 0..n {
        for j in (i + 1)..n {
            for k in (j + 1)..n {
                if adj_matrix[i][j] == 1 && adj_matrix[j][k] == 1 && adj_matrix[k][i] == 1 {
                    println!("Znaleziono cykl C3: {}-{}-{}-{}", i, j, k, i);
                    return;
                }
            }
        }
    }
    println!("Nie znaleziono cyklu C3");
}

fn has_c3_matrix_multiplication(adj_matrix: &Vec<Vec<u8>>) -> &'static str {
    // Sprawdza, czy graf zawiera cykl C3, używając mnożenia macierzy.
    let n = adj_matrix.len();
    let mut a_cubed = vec![vec![0u32; n]; n];

    // Mnożenie macierzy: A * A * A
    let mut a_squared = vec![vec![0u32; n]; n];
    for i in 0..n {
        for j in 0..n {
            for k in 0..n {
                a_squared[i][j] += (adj_matrix[i][k] as u32) * (adj_matrix[k][j] as u32);
            }
        }
    }
    for i in 0..n {
        for j in 0..n {
            for k in 0..n {
                a_cubed[i][j] += a_squared[i][k] * (adj_matrix[k][j] as u32);
            }
        }
    }

    // Obliczenie śladu A^3
    let trace: u32 = (0..n).map(|i| a_cubed[i][i]).sum();

    if trace > 0 {
        "TAK"
    } else {
        "NIE"
    }
}

fn main() {
    let stdin = io::stdin();
    let mut adj_list = Vec::new();

    println!("Podaj listę sąsiedztwa (wpisz pustą linię, aby zakończyć):");

    for line in stdin.lock().lines() {
        let line = line.unwrap();
        if line.trim().is_empty() {
            break;
        }

        let nums: Vec<usize> = line
            .trim()
            .split_whitespace()
            .map(|s| s.parse().unwrap())
            .collect();

        if nums.is_empty() {
            continue;
        }

        // Pierwsza liczba to numer wierzchołka
        let vertex = nums[0] - 1;

        // Upewnij się, że lista jest wystarczająco duża
        if adj_list.len() <= vertex {
            adj_list.resize(vertex + 1, Vec::new());
        }

        // Reszta to sąsiedzi
        let neighbors = nums[1..].to_vec();
        adj_list[vertex] = neighbors;
    }

    let n = adj_list.len();
    let adj_matrix = adjacency_list_to_matrix(&adj_list, n);

    println!("Wersja naiwna dla grafu:");
    find_first_c3_naive(&adj_matrix);

    println!("Wersja z mnożeniem macierzy dla grafu:");
    println!("{}", has_c3_matrix_multiplication(&adj_matrix));
}

//Graf z cyklu C3:
/*
1 2 3
2 1 3 4
3 1 2 4
4 2 3 5
5 4 6
6 5
*/
//Graf bez cyklu C3:
/*
1 2
2 1 3
3 2 4
4 3 5
5 4 6
6 5 7
7 6 8
8 7
*/
