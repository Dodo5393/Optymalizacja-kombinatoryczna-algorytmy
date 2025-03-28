# Instalacja i ładowanie pakietu igraph (odkomentuj, jeśli nie jest zainstalowany)
# install.packages("igraph")
library(igraph)

# Funkcja do wczytywania grafu z pliku tekstowego
wczytaj_graf <- function(sciezka) {
  if (file.exists(sciezka)) {
    edge_data <- as.matrix(read.table(sciezka, header = FALSE))
    edges <- as.vector(t(edge_data))
    unique_vertices <- sort(unique(c(edge_data[, 1], edge_data[, 2])))
    g <- graph(edges, n = length(unique_vertices), directed = FALSE)
    V(g)$name <- as.character(unique_vertices)
    return(g)
  } else {
    warning("Plik nie został znaleziony! Używam domyślnego grafu.")
    g <- make_graph(c(1, 2, 2, 3, 3, 4), directed = FALSE)
    V(g)$name <- as.character(1:4)
    return(g)
  }
}

# Określenie ścieżki do pliku
if (requireNamespace("rstudioapi", quietly = TRUE)) {
  skrypt_folder <- dirname(rstudioapi::getSourceEditorContext()$path)
  plik <- file.path(skrypt_folder, "graf.txt")
} else {
  plik <- "graf.txt"  # Zakłada, że plik jest w bieżącym katalogu roboczym
}

# Wczytaj graf
g <- wczytaj_graf(plik)

# Początkowa wizualizacja z grubszymi krawędziami
par(mfrow = c(1, 2))  # Ustawienie layoutu: 1 wiersz, 2 kolumny
plot(g, vertex.label = V(g)$name, vertex.color = "lightblue", vertex.size = 30,
     edge.width = 4, edge.color = "black", main = "Początkowy graf")
plot(g, vertex.label = V(g)$name, vertex.color = "lightblue", vertex.size = 30,
     edge.width = 4, edge.color = "black", main = "Aktualny graf (g_current)")

# Algorytm 2-przybliżony dla problemu vertex cover
vertex_cover <- c()  # Pusty zbiór pokrycia
g_current <- g       # Kopia grafu do modyfikacji
# Inicjalizacja licznika dla nazw plików
iteration <- 1  

while (ecount(g_current) > 0) {
  edge <- E(g_current)[1]
  endpoints <- ends(g_current, edge)
  vertex_cover <- unique(c(vertex_cover, endpoints))
  
  incident_edges_1 <- incident(g_current, V(g_current)[V(g_current)$name == endpoints[1]])
  incident_edges_2 <- incident(g_current, V(g_current)[V(g_current)$name == endpoints[2]])
  incident_edges <- unique(c(incident_edges_1, incident_edges_2))
  
  g_current <- delete_edges(g_current, incident_edges)
  V(g)$color <- ifelse(V(g)$name %in% vertex_cover, "red", "lightblue")
  
  # Zapis wykresów do plików PNG
  png(filename = sprintf("iteracja_%02d_oryginalny.png", iteration), width = 800, height = 600)
  plot(g, vertex.color = V(g)$color, vertex.size = 20, edge.width = 4, edge.color = "black",
       main = sprintf("Iteracja %d - Oryginalny graf", iteration))
  dev.off()
  
  png(filename = sprintf("iteracja_%02d_aktualny.png", iteration), width = 800, height = 600)
  plot(g_current, vertex.label = V(g_current)$name, vertex.color = "lightblue", vertex.size = 20,
       edge.width = 4, edge.color = "black", main = sprintf("Iteracja %d - Aktualny graf", iteration))
  dev.off()
  
  Sys.sleep(1)
  iteration <- iteration + 1  # Zwiększenie licznika
}

# Zapis finalnego wyniku
png(filename = "finalny_vertex_cover.png", width = 800, height = 600)
plot(g, vertex.color = V(g)$color, vertex.size = 20, edge.width = 4, edge.color = "black",
     main = "Finalny zbiór vertex cover")
dev.off()


# Wyświetlenie wyniku
cat("Finalny zbiór vertex cover:", paste(vertex_cover, collapse = ", "), "\n")

# Końcowa wizualizacja z grubszymi krawędziami
par(mfrow = c(1, 1))  # Powrót do pojedynczego wykresu
plot(g, vertex.color = V(g)$color, vertex.size = 20, edge.width = 4, edge.color = "black",
     main = "Finalny zbiór vertex cover")
