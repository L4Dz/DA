/**
 * @file Graph.h
 * @brief Generic directed weighted graph with flow support and Edmonds-Karp algorithm.
 * @brief (PT) Grafo genérico dirigido e pesado com suporte a fluxo e algoritmo de Edmonds-Karp.
 *
 * @details
 * (EN) This header defines a templated graph structure composed of Vertex and Edge classes,
 * supporting max-flow computation via the Edmonds-Karp algorithm (BFS-based Ford-Fulkerson).
 * Additionally, vertices carry project metadata (type, title, domain) for use in
 * submission/reviewer assignment problems.
 *
 * (PT) Este ficheiro define uma estrutura de grafo genérica (template) composta pelas classes
 * Vertex e Edge, com suporte a cálculo de fluxo máximo através do algoritmo de Edmonds-Karp
 * (Ford-Fulkerson com BFS). Os vértices também contêm metadados de projeto (tipo, título,
 * domínio) para uso em problemas de atribuição de submissões e revisores.
 *
 * @author DA Project Group
 * @date 2026
 */

#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <string>

template <class T> class Edge;
template <class T> class Graph;

/**
 * @brief (EN) Represents positive infinity for distance/flow computations.
 * @brief (PT) Representa infinito positivo para cálculos de distância/fluxo.
 */
#define INF std::numeric_limits<double>::max()

/**
 * @enum VertexType
 * @brief (EN) Classifies the role of a vertex in the flow network.
 * @brief (PT) Classifica o papel de um vértice na rede de fluxo.
 *
 * @var VertexType::SOURCE
 * (EN) Super-source node that originates all flow. / (PT) Nó super-fonte que origina todo o fluxo.
 * @var VertexType::SINK
 * (EN) Super-sink node that absorbs all flow. / (PT) Nó super-sumidouro que absorve todo o fluxo.
 * @var VertexType::SUBMISSION
 * (EN) A paper/submission to be reviewed. / (PT) Um artigo/submissão a ser revisto.
 * @var VertexType::REVIEWER
 * (EN) A reviewer that evaluates submissions. / (PT) Um revisor que avalia submissões.
 */
enum class VertexType { SOURCE, SINK, SUBMISSION, REVIEWER };

/************************* Vertex **************************/

/**
 * @class Vertex
 * @brief (EN) Represents a node in the graph, carrying adjacency lists and traversal state.
 * @brief (PT) Representa um nó no grafo, com listas de adjacência e estado de travessia.
 *
 * @tparam T Type of the identifying information stored in the vertex.
 * @tparam T (PT) Tipo da informação identificadora armazenada no vértice.
 *
 * @details
 * (EN) Each vertex stores outgoing edges (adj), incoming edges (incoming), and fields
 * used by graph algorithms: visited, processing, dist, path, low, num.
 * Public metadata fields (type, title, primaryDomain, secondaryDomain) are used
 * to model the submission-reviewer assignment problem.
 *
 * (PT) Cada vértice armazena arestas de saída (adj), arestas de entrada (incoming), e campos
 * usados pelos algoritmos do grafo: visited, processing, dist, path, low, num.
 * Os campos públicos de metadados (type, title, primaryDomain, secondaryDomain) são usados
 * para modelar o problema de atribuição de submissões a revisores.
 */
template <class T>
class Vertex {
public:
    /**
     * @brief (EN) Constructs a vertex with the given info and default metadata.
     * @brief (PT) Constrói um vértice com a informação dada e metadados por defeito.
     * @param in Identifying value stored in this vertex. / (PT) Valor identificador armazenado.
     */
    Vertex(T in);

    /**
     * @brief (EN) Less-than comparison based on the vertex's distance (dist field).
     * @brief (PT) Comparação menor-que baseada na distância do vértice (campo dist).
     * @param vertex The vertex to compare against. / (PT) O vértice com o qual comparar.
     * @return true if this vertex has a smaller dist. / (PT) true se este vértice tiver dist menor.
     */
    bool operator<(Vertex<T> & vertex) const;

    // ---- Getters ----

    /** @brief (EN) Returns the info stored in this vertex. / (PT) Devolve a informação armazenada. */
    T getInfo() const;

    /** @brief (EN) Returns the list of outgoing edges. / (PT) Devolve a lista de arestas de saída. */
    std::vector<Edge<T> *> getAdj() const;

    /** @brief (EN) Returns whether this vertex has been visited. / (PT) Devolve se o vértice foi visitado. */
    bool isVisited() const;

    /** @brief (EN) Returns whether this vertex is currently being processed (e.g. in DFS stack). / (PT) Devolve se o vértice está a ser processado (ex.: na pilha DFS). */
    bool isProcessing() const;

    /** @brief (EN) Returns the in-degree of this vertex. / (PT) Devolve o grau de entrada do vértice. */
    unsigned int getIndegree() const;

    /** @brief (EN) Returns the current shortest/accumulated distance to this vertex. / (PT) Devolve a distância acumulada/mais curta até este vértice. */
    double getDist() const;

    /** @brief (EN) Returns the edge used to reach this vertex in a path. / (PT) Devolve a aresta usada para chegar a este vértice num caminho. */
    Edge<T> *getPath() const;

    /** @brief (EN) Returns the list of incoming edges. / (PT) Devolve a lista de arestas de entrada. */
    std::vector<Edge<T> *> getIncoming() const;

    /** @brief (EN) Returns the low value used in Tarjan's SCC algorithm. / (PT) Devolve o valor low usado no algoritmo de Tarjan para SCCs. */
    int getLow() const;

    /** @brief (EN) Returns the discovery number used in Tarjan's SCC algorithm. / (PT) Devolve o número de descoberta usado no algoritmo de Tarjan. */
    int getNum() const;

    // ---- Setters ----

    /** @brief (EN) Sets the info stored in this vertex. / (PT) Define a informação armazenada no vértice. @param info New value. / (PT) Novo valor. */
    void setInfo(T info);

    /** @brief (EN) Marks this vertex as visited or unvisited. / (PT) Marca o vértice como visitado ou não. @param visited New state. / (PT) Novo estado. */
    void setVisited(bool visited);

    /** @brief (EN) Marks this vertex as being processed. / (PT) Marca o vértice como em processamento. @param processing New state. / (PT) Novo estado. */
    void setProcessing(bool processing);

    /** @brief (EN) Sets the in-degree of this vertex. / (PT) Define o grau de entrada do vértice. @param indegree New in-degree. / (PT) Novo grau de entrada. */
    void setIndegree(unsigned int indegree);

    /** @brief (EN) Sets the accumulated distance to this vertex. / (PT) Define a distância acumulada até este vértice. @param dist New distance. / (PT) Nova distância. */
    void setDist(double dist);

    /** @brief (EN) Sets the edge that leads to this vertex in a path tree. / (PT) Define a aresta que leva a este vértice numa árvore de caminhos. @param path Pointer to the edge. / (PT) Ponteiro para a aresta. */
    void setPath(Edge<T> *path);

    /** @brief (EN) Sets the low value for Tarjan's algorithm. / (PT) Define o valor low para o algoritmo de Tarjan. @param value New low value. / (PT) Novo valor low. */
    void setLow(int value);

    /** @brief (EN) Sets the discovery number for Tarjan's algorithm. / (PT) Define o número de descoberta para o algoritmo de Tarjan. @param value New num value. / (PT) Novo valor num. */
    void setNum(int value);

    // ---- Edge operations ----

    /**
     * @brief (EN) Adds a directed edge from this vertex to the given destination with weight w.
     * @brief (PT) Adiciona uma aresta dirigida deste vértice para o destino dado, com peso w.
     * @param dest Pointer to the destination vertex. / (PT) Ponteiro para o vértice destino.
     * @param w Weight of the edge. / (PT) Peso da aresta.
     * @return Pointer to the newly created edge. / (PT) Ponteiro para a aresta criada.
     */
    Edge<T> * addEdge(Vertex<T> *dest, double w);

    /**
     * @brief (EN) Removes all outgoing edges to the vertex identified by `in`.
     * @brief (PT) Remove todas as arestas de saída para o vértice identificado por `in`.
     * @param in Info of the destination vertex to disconnect. / (PT) Info do vértice destino a desligar.
     * @return true if at least one edge was removed. / (PT) true se pelo menos uma aresta foi removida.
     */
    bool removeEdge(T in);

    /**
     * @brief (EN) Removes all outgoing edges from this vertex.
     * @brief (PT) Remove todas as arestas de saída deste vértice.
     */
    void removeOutgoingEdges();

    // ---- Public metadata (project-specific) ----

    /**
     * @brief (EN) Role of this vertex in the flow network (source, sink, submission, reviewer).
     * @brief (PT) Papel deste vértice na rede de fluxo (fonte, sumidouro, submissão, revisor).
     */
    VertexType type;

    /**
     * @brief (EN) Title of the submission or name of the reviewer.
     * @brief (PT) Título da submissão ou nome do revisor.
     */
    std::string title;

    /**
     * @brief (EN) Primary research domain (encoded as integer ID).
     * @brief (PT) Domínio de investigação primário (codificado como inteiro).
     */
    int primaryDomain;

    /**
     * @brief (EN) Secondary research domain (-1 if none assigned).
     * @brief (PT) Domínio de investigação secundário (-1 se não atribuído).
     */
    int secondaryDomain;

protected:
    /** @brief (EN) Identifying information of this vertex. / (PT) Informação identificadora do vértice. */
    T info;

    /** @brief (EN) List of outgoing edges. / (PT) Lista de arestas de saída. */
    std::vector<Edge<T> *> adj;

    /** @brief (EN) Whether this vertex was visited in the current traversal. / (PT) Se o vértice foi visitado na travessia atual. */
    bool visited = false;

    /** @brief (EN) Whether this vertex is on the current DFS processing stack. / (PT) Se o vértice está na pilha de processamento DFS atual. */
    bool processing = false;

    /** @brief (EN) Number of incoming edges (in-degree). / (PT) Número de arestas de entrada (grau de entrada). */
    unsigned int indegree;

    /** @brief (EN) Distance used by shortest-path algorithms. / (PT) Distância usada por algoritmos de caminho mais curto. */
    double dist = 0;

    /**
     * @brief (EN) Low and discovery-time values used in Tarjan's SCC algorithm.
     * @brief (PT) Valores low e de tempo de descoberta usados no algoritmo de Tarjan.
     */
    int low = -1, num = -1;

    /** @brief (EN) Edge used to reach this vertex in a BFS/DFS path tree. / (PT) Aresta usada para chegar a este vértice numa árvore BFS/DFS. */
    Edge<T> *path = nullptr;

    /** @brief (EN) List of incoming edges (reverse adjacency). / (PT) Lista de arestas de entrada (adjacência inversa). */
    std::vector<Edge<T> *> incoming;

    /**
     * @brief (EN) Deletes an edge and removes it from the destination's incoming list.
     * @brief (PT) Elimina uma aresta e remove-a da lista de entradas do destino.
     * @param edge Pointer to the edge to delete. / (PT) Ponteiro para a aresta a eliminar.
     */
    void deleteEdge(Edge<T> *edge);
};

/********************** Edge ****************************/

/**
 * @class Edge
 * @brief (EN) Represents a directed weighted edge with flow support between two vertices.
 * @brief (PT) Representa uma aresta dirigida e pesada com suporte a fluxo entre dois vértices.
 *
 * @tparam T Type of the vertex info. / (PT) Tipo da informação dos vértices.
 *
 * @details
 * (EN) Each edge stores its origin and destination vertices, a capacity (weight),
 * a current flow value, a selected flag (for path marking), and an optional
 * reverse edge pointer (used in residual graph construction for flow algorithms).
 *
 * (PT) Cada aresta armazena os vértices de origem e destino, uma capacidade (weight),
 * um valor de fluxo atual, um flag de seleção (para marcar caminhos), e um ponteiro
 * opcional para a aresta inversa (usado na construção do grafo residual para algoritmos de fluxo).
 */
template <class T>
class Edge {
public:
    /**
     * @brief (EN) Constructs an edge from orig to dest with capacity w.
     * @brief (PT) Constrói uma aresta de orig para dest com capacidade w.
     * @param orig Pointer to the origin vertex. / (PT) Ponteiro para o vértice de origem.
     * @param dest Pointer to the destination vertex. / (PT) Ponteiro para o vértice destino.
     * @param w Edge capacity/weight. / (PT) Capacidade/peso da aresta.
     */
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);

    /** @brief (EN) Returns the destination vertex. / (PT) Devolve o vértice destino. */
    Vertex<T> * getDest() const;

    /** @brief (EN) Returns the origin vertex. / (PT) Devolve o vértice de origem. */
    Vertex<T> * getOrig() const;

    /** @brief (EN) Returns the capacity (weight) of this edge. / (PT) Devolve a capacidade (peso) desta aresta. */
    double getWeight() const;

    /** @brief (EN) Returns the current flow through this edge. / (PT) Devolve o fluxo atual nesta aresta. */
    double getFlow() const;

    /** @brief (EN) Sets the flow through this edge. / (PT) Define o fluxo nesta aresta. @param flow New flow value. / (PT) Novo valor de fluxo. */
    void setFlow(double flow);

    /** @brief (EN) Returns the reverse edge in the residual graph (nullptr if unset). / (PT) Devolve a aresta inversa no grafo residual (nullptr se não definida). */
    Edge<T> *getReverse() const;

    /** @brief (EN) Returns whether this edge is selected (e.g., part of a highlighted path). / (PT) Devolve se esta aresta está selecionada (ex.: parte de um caminho destacado). */
    bool isSelected() const;

    /** @brief (EN) Marks or unmarks this edge as selected. / (PT) Marca ou desmarca esta aresta como selecionada. @param selected New state. / (PT) Novo estado. */
    void setSelected(bool selected);

    /** @brief (EN) Sets the reverse edge (for residual graph). / (PT) Define a aresta inversa (para o grafo residual). @param reverse Pointer to the reverse edge. / (PT) Ponteiro para a aresta inversa. */
    void setReverse(Edge<T> *reverse);

protected:
    /** @brief (EN) Origin vertex of this edge. / (PT) Vértice de origem desta aresta. */
    Vertex<T> * orig;

    /** @brief (EN) Destination vertex of this edge. / (PT) Vértice destino desta aresta. */
    Vertex<T> * dest;

    /** @brief (EN) Capacity (maximum flow) of this edge. / (PT) Capacidade (fluxo máximo) desta aresta. */
    double weight;

    /** @brief (EN) Current flow passing through this edge (0 initially). / (PT) Fluxo atual que passa nesta aresta (0 inicialmente). */
    double flow = 0;

    /** @brief (EN) Whether this edge is marked as selected. / (PT) Se esta aresta está marcada como selecionada. */
    bool selected = false;

    /** @brief (EN) Pointer to the reverse edge in the residual graph (nullptr if not set). / (PT) Ponteiro para a aresta inversa no grafo residual (nullptr se não definida). */
    Edge<T> *reverse = nullptr;
};

/********************** Graph ****************************/

/**
 * @class Graph
 * @brief (EN) Generic directed weighted graph with max-flow support.
 * @brief (PT) Grafo genérico dirigido e pesado com suporte a fluxo máximo.
 *
 * @tparam T Type of the vertex info. / (PT) Tipo da informação dos vértices.
 *
 * @details
 * (EN) Manages a set of vertices and provides methods to add/remove vertices and edges,
 * find vertices by value, and support auxiliary distance/path matrices for algorithms
 * such as Floyd-Warshall. The Edmonds-Karp max-flow algorithm is implemented as
 * free template functions that operate on a Graph pointer.
 *
 * (PT) Gere um conjunto de vértices e fornece métodos para adicionar/remover vértices e arestas,
 * encontrar vértices por valor, e suportar matrizes auxiliares de distância/caminho para algoritmos
 * como Floyd-Warshall. O algoritmo de fluxo máximo de Edmonds-Karp está implementado como
 * funções template livres que operam sobre um ponteiro para Graph.
 */
template <class T>
class Graph {
public:
    /**
     * @brief (EN) Destructor. Frees all vertices and auxiliary matrices.
     * @brief (PT) Destrutor. Liberta todos os vértices e matrizes auxiliares.
     */
    ~Graph();

    /**
     * @brief (EN) Finds and returns the vertex with the given info, or nullptr if not found.
     * @brief (PT) Encontra e devolve o vértice com a informação dada, ou nullptr se não encontrado.
     * @param in Info value to search for. / (PT) Valor de info a pesquisar.
     * @return Pointer to the vertex, or nullptr. / (PT) Ponteiro para o vértice, ou nullptr.
     */
    Vertex<T> *findVertex(const T &in) const;

    /**
     * @brief (EN) Returns the index of the vertex with the given info in the vertex set, or -1.
     * @brief (PT) Devolve o índice do vértice com a informação dada no conjunto de vértices, ou -1.
     * @param in Info value to search for. / (PT) Valor de info a pesquisar.
     * @return Index in vertexSet, or -1 if not found. / (PT) Índice em vertexSet, ou -1 se não encontrado.
     */
    int findVertexIdx(const T &in) const;

    /**
     * @brief (EN) Adds a new vertex with the given info. Does nothing if it already exists.
     * @brief (PT) Adiciona um novo vértice com a informação dada. Não faz nada se já existir.
     * @param in Info of the new vertex. / (PT) Info do novo vértice.
     * @return true if added successfully, false if already exists. / (PT) true se adicionado com sucesso, false se já existe.
     */
    bool addVertex(const T &in);

    /**
     * @brief (EN) Removes the vertex with the given info and all its associated edges.
     * @brief (PT) Remove o vértice com a informação dada e todas as suas arestas associadas.
     * @param in Info of the vertex to remove. / (PT) Info do vértice a remover.
     * @return true if removed, false if not found. / (PT) true se removido, false se não encontrado.
     */
    bool removeVertex(const T &in);

    /**
     * @brief (EN) Adds a directed edge from sourc to dest with weight w.
     * @brief (PT) Adiciona uma aresta dirigida de sourc para dest com peso w.
     * @param sourc Info of the source vertex. / (PT) Info do vértice fonte.
     * @param dest Info of the destination vertex. / (PT) Info do vértice destino.
     * @param w Weight/capacity of the edge. / (PT) Peso/capacidade da aresta.
     * @return true if added, false if either vertex does not exist. / (PT) true se adicionada, false se algum vértice não existe.
     */
    bool addEdge(const T &sourc, const T &dest, double w);

    /**
     * @brief (EN) Removes the directed edge from sourc to dest.
     * @brief (PT) Remove a aresta dirigida de sourc para dest.
     * @param sourc Info of the source vertex. / (PT) Info do vértice fonte.
     * @param dest Info of the destination vertex. / (PT) Info do vértice destino.
     * @return true if removed, false if not found. / (PT) true se removida, false se não encontrada.
     */
    bool removeEdge(const T &sourc, const T &dest);

    /**
     * @brief (EN) Adds a bidirectional edge between sourc and dest (with linked reverse pointers).
     * @brief (PT) Adiciona uma aresta bidirecional entre sourc e dest (com ponteiros inversos ligados).
     * @param sourc Info of one endpoint. / (PT) Info de um extremo.
     * @param dest Info of the other endpoint. / (PT) Info do outro extremo.
     * @param w Weight/capacity of both directions. / (PT) Peso/capacidade de ambas as direções.
     * @return true if added, false if either vertex does not exist. / (PT) true se adicionada, false se algum vértice não existe.
     */
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w);

    /**
     * @brief (EN) Returns the number of vertices in the graph.
     * @brief (PT) Devolve o número de vértices no grafo.
     * @return Number of vertices. / (PT) Número de vértices.
     */
    int getNumVertex() const;

    /**
     * @brief (EN) Returns the full list of vertices.
     * @brief (PT) Devolve a lista completa de vértices.
     * @return Vector of vertex pointers. / (PT) Vetor de ponteiros para vértices.
     */
    std::vector<Vertex<T> *> getVertexSet() const;

protected:
    /** @brief (EN) Set of all vertices in the graph. / (PT) Conjunto de todos os vértices no grafo. */
    std::vector<Vertex<T> *> vertexSet;

    /** @brief (EN) Auxiliary distance matrix (e.g., for Floyd-Warshall). / (PT) Matriz auxiliar de distâncias (ex.: para Floyd-Warshall). */
    double ** distMatrix = nullptr;

    /** @brief (EN) Auxiliary path matrix (e.g., for Floyd-Warshall). / (PT) Matriz auxiliar de caminhos (ex.: para Floyd-Warshall). */
    int **pathMatrix = nullptr;
};

/********************** Edmonds-Karp ****************************/

/**
 * @brief (EN) Helper for BFS in Edmonds-Karp: visits vertex w via edge e if there is residual capacity.
 * @brief (PT) Auxiliar para BFS no Edmonds-Karp: visita o vértice w pela aresta e se há capacidade residual.
 *
 * @details
 * (EN) A vertex is enqueued only if it has not been visited yet and the residual capacity is positive.
 * This is called for both forward edges (capacity - flow) and backward edges (flow).
 *
 * (PT) Um vértice só é colocado na fila se ainda não foi visitado e a capacidade residual é positiva.
 * É chamado tanto para arestas diretas (capacidade - fluxo) como inversas (fluxo).
 *
 * @note Time Complexity: $O(1)$ constant time execution.
 * @tparam T Vertex info type. / (PT) Tipo da informação dos vértices.
 * @param q BFS queue. / (PT) Fila BFS.
 * @param e The edge being explored. / (PT) A aresta a explorar.
 * @param w The candidate next vertex. / (PT) O próximo vértice candidato.
 * @param residual Available residual capacity on this edge. / (PT) Capacidade residual disponível nesta aresta.
 */
template <class T>
void testAndVisit(std::queue<Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    if (!w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

/**
 * @brief (EN) BFS-based search for an augmenting path from s to t in the residual graph.
 * @brief (PT) Procura BFS de um caminho aumentante de s até t no grafo residual.
 *
 * @details
 * (EN) Resets all visited flags, then runs BFS from s. For each vertex, explores
 * forward edges with remaining capacity (weight - flow > 0) and backward edges
 * with non-zero flow (allowing flow cancellation). Returns true if t is reached.
 *
 * (PT) Reinicia todos os flags de visita e executa BFS a partir de s. Para cada vértice,
 * explora arestas diretas com capacidade residual (weight - flow > 0) e arestas inversas
 * com fluxo não nulo (permitindo cancelamento de fluxo). Devolve true se t for alcançado.
 *
 * @note Time Complexity: $O(V + E)$ where $V$ is the number of vertices and $E$ is the number of edges.
 * @tparam T Vertex info type. / (PT) Tipo da informação dos vértices.
 * @param g Pointer to the graph. / (PT) Ponteiro para o grafo.
 * @param s Pointer to the source vertex. / (PT) Ponteiro para o vértice fonte.
 * @param t Pointer to the sink vertex. / (PT) Ponteiro para o vértice sumidouro.
 * @return true if an augmenting path exists. / (PT) true se existir um caminho aumentante.
 */
template <class T>
bool findAugmentingPath(Graph<T> *g, Vertex<T> *s, Vertex<T> *t) {
    for (auto v : g->getVertexSet()) v->setVisited(false);
    s->setVisited(true);
    std::queue<Vertex<T>*> q;
    q.push(s);

    while (!q.empty() && !t->isVisited()) {
        auto v = q.front(); q.pop();
        for (auto e : v->getAdj()) 
            testAndVisit(q, e, e->getDest(), e->getWeight() - e->getFlow());
        for (auto e : v->getIncoming()) 
            testAndVisit(q, e, e->getOrig(), e->getFlow());
    }
    return t->isVisited();
}

/**
 * @brief (EN) Computes the maximum flow from source to target using the Edmonds-Karp algorithm.
 * @brief (PT) Calcula o fluxo máximo de source para target usando o algoritmo de Edmonds-Karp.
 *
 * @details
 * (EN) Implements the Edmonds-Karp variant of Ford-Fulkerson, which uses BFS to find
 * shortest augmenting paths. This guarantees a polynomial time complexity.
 * Steps:
 * 1. Reset all edge flows to 0.
 * 2. Repeatedly find an augmenting path via BFS (findAugmentingPath).
 * 3. Determine the bottleneck (minimum residual capacity along the path).
 * 4. Update flows along the path (forward edges increase, backward edges decrease).
 * After this call, each edge's flow field reflects the optimal flow assignment.
 *
 * (PT) Implementa a variante Edmonds-Karp de Ford-Fulkerson, que usa BFS para encontrar
 * os caminhos aumentantes mais curtos, garantindo complexidade temporal polinomial.
 * Passos:
 * 1. Reinicia todos os fluxos das arestas para 0.
 * 2. Encontra repetidamente um caminho aumentante via BFS (findAugmentingPath).
 * 3. Determina o bottleneck (mínima capacidade residual ao longo do caminho).
 * 4. Atualiza os fluxos ao longo do caminho (arestas diretas aumentam, inversas diminuem).
 * Após esta chamada, o campo flow de cada aresta reflete a atribuição ótima de fluxo.
 *
 * @note Time Complexity: $O(V \cdot E^2)$ where $V$ is the number of vertices and $E$ is the number of edges.
 *
 * @tparam T Vertex info type. / (PT) Tipo da informação dos vértices.
 * @param g Pointer to the graph. / (PT) Ponteiro para o grafo.
 * @param source Info of the source vertex. / (PT) Info do vértice fonte.
 * @param target Info of the sink vertex. / (PT) Info do vértice sumidouro.
 */
template <class T>
void edmondsKarp(Graph<T> *g, T source, T target) {
    Vertex<T>* s = g->findVertex(source);
    Vertex<T>* t = g->findVertex(target);
    if (s == nullptr || t == nullptr || s == t) return;

    for (auto v : g->getVertexSet()) 
        for (auto e : v->getAdj()) e->setFlow(0);

    while (findAugmentingPath(g, s, t)) {
        double f = INF;
        auto v = t;
        while (v != s) {
            auto e = v->getPath();
            if (e->getDest() == v) {
                f = std::min(f, e->getWeight() - e->getFlow());
                v = e->getOrig();
            } else {
                f = std::min(f, e->getFlow());
                v = e->getDest();
            }
        }
        v = t;
        while (v != s) {
            auto e = v->getPath();
            if (e->getDest() == v) {
                e->setFlow(e->getFlow() + f);
                v = e->getOrig();
            } else {
                e->setFlow(e->getFlow() - f);
                v = e->getDest();
            }
        }
    }
}

//Implementation of methods 

template <class T>
Vertex<T>::Vertex(T in): info(in), 
    type(VertexType::SOURCE),
    title(""),
    primaryDomain(0),
    secondaryDomain(-1),
    indegree(0) {}

template <class T>
Edge<T> * Vertex<T>::addEdge(Vertex<T> *d, double w) {
    auto newEdge = new Edge<T>(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

template <class T>
bool Vertex<T>::removeEdge(T in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        Vertex<T> *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true;
        } else it++;
    }
    return removedEdge;
}

template <class T>
void Vertex<T>::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge<T> *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

template <class T>
bool Vertex<T>::operator<(Vertex<T> & vertex) const {
    return this->dist < vertex.dist;
}

template <class T>
T Vertex<T>::getInfo() const { return info; }

template <class T>
int Vertex<T>::getLow() const { return low; }

template <class T>
void Vertex<T>::setLow(int value) { low = value; }

template <class T>
int Vertex<T>::getNum() const { return num; }

template <class T>
void Vertex<T>::setNum(int value) { num = value; }

template <class T>
std::vector<Edge<T>*> Vertex<T>::getAdj() const { return adj; }

template <class T>
bool Vertex<T>::isVisited() const { return visited; }

template <class T>
bool Vertex<T>::isProcessing() const { return processing; }

template <class T>
unsigned int Vertex<T>::getIndegree() const { return indegree; }

template <class T>
double Vertex<T>::getDist() const { return dist; }

template <class T>
Edge<T> *Vertex<T>::getPath() const { return path; }

template <class T>
std::vector<Edge<T> *> Vertex<T>::getIncoming() const { return incoming; }

template <class T>
void Vertex<T>::setInfo(T in) { info = in; }

template <class T>
void Vertex<T>::setVisited(bool visited) { this->visited = visited; }

template <class T>
void Vertex<T>::setProcessing(bool processing) { this->processing = processing; }

template <class T>
void Vertex<T>::setIndegree(unsigned int indegree) { this->indegree = indegree; }

template <class T>
void Vertex<T>::setDist(double dist) { this->dist = dist; }

template <class T>
void Vertex<T>::setPath(Edge<T> *path) { this->path = path; }

template <class T>
void Vertex<T>::deleteEdge(Edge<T> *edge) {
    Vertex<T> *dest = edge->getDest();
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) it = dest->incoming.erase(it);
        else it++;
    }
    delete edge;
}

template <class T>
Edge<T>::Edge(Vertex<T> *orig, Vertex<T> *dest, double w): orig(orig), dest(dest), weight(w) {}

template <class T>
Vertex<T> * Edge<T>::getDest() const { return dest; }

template <class T>
double Edge<T>::getWeight() const { return weight; }

template <class T>
Vertex<T> * Edge<T>::getOrig() const { return orig; }

template <class T>
Edge<T> *Edge<T>::getReverse() const { return reverse; }

template <class T>
bool Edge<T>::isSelected() const { return selected; }

template <class T>
double Edge<T>::getFlow() const { return flow; }

template <class T>
void Edge<T>::setSelected(bool selected) { this->selected = selected; }

template <class T>
void Edge<T>::setReverse(Edge<T> *reverse) { this->reverse = reverse; }

template <class T>
void Edge<T>::setFlow(double flow) { this->flow = flow; }

template <class T>
int Graph<T>::getNumVertex() const { return vertexSet.size(); }

template <class T>
std::vector<Vertex<T> *> Graph<T>::getVertexSet() const { return vertexSet; }

template <class T>
Vertex<T> * Graph<T>::findVertex(const T &in) const {
    for (auto v : vertexSet) if (v->getInfo() == in) return v;
    return nullptr;
}

template <class T>
int Graph<T>::findVertexIdx(const T &in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in) return i;
    return -1;
}

template <class T>
bool Graph<T>::addVertex(const T &in) {
    if (findVertex(in) != nullptr) return false;
    vertexSet.push_back(new Vertex<T>(in));
    return true;
}

template <class T>
bool Graph<T>::removeVertex(const T &in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) u->removeEdge(v->getInfo());
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

template <class T>
bool Graph<T>::addEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;
    v1->addEdge(v2, w);
    return true;
}

template <class T>
bool Graph<T>::removeEdge(const T &sourc, const T &dest) {
    Vertex<T> * srcVertex = findVertex(sourc);
    if (srcVertex == nullptr) return false;
    return srcVertex->removeEdge(dest);
}

template <class T>
bool Graph<T>::addBidirectionalEdge(const T &sourc, const T &dest, double w) {
    auto v1 = findVertex(sourc);
    auto v2 = findVertex(dest);
    if (v1 == nullptr || v2 == nullptr) return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

/**
 * @brief (EN) Frees a 2D integer matrix of n rows.
 * @brief (PT) Liberta uma matriz 2D de inteiros com n linhas.
 * @param m Pointer to the matrix. / (PT) Ponteiro para a matriz.
 * @param n Number of rows. / (PT) Número de linhas.
 */
inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++) if (m[i] != nullptr) delete [] m[i];
        delete [] m;
    }
}

/**
 * @brief (EN) Frees a 2D double matrix of n rows.
 * @brief (PT) Liberta uma matriz 2D de doubles com n linhas.
 * @param m Pointer to the matrix. / (PT) Ponteiro para a matriz.
 * @param n Number of rows. / (PT) Número de linhas.
 */
inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++) if (m[i] != nullptr) delete [] m[i];
        delete [] m;
    }
}

template <class T>
Graph<T>::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
    for (auto v : vertexSet) delete v;
}

#endif
