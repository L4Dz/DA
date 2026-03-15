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

#define INF std::numeric_limits<double>::max()

enum class VertexType { SOURCE, SINK, SUBMISSION, REVIEWER };

/************************* Vertex **************************/

template <class T>
class Vertex {
public:
    Vertex(T in);
    bool operator<(Vertex<T> & vertex) const;

    T getInfo() const;
    std::vector<Edge<T> *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge<T> *getPath() const;
    std::vector<Edge<T> *> getIncoming() const;

    void setInfo(T info);
    void setVisited(bool visited);
    void setProcessing(bool processing);
    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge<T> *path);
    
    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    Edge<T> * addEdge(Vertex<T> *dest, double w);
    bool removeEdge(T in);
    void removeOutgoingEdges();

    // Project Metadata
    VertexType type;
    int primaryDomain;    
    int secondaryDomain;  

protected:
    T info;                
    std::vector<Edge<T> *> adj;  
    bool visited = false; 
    bool processing = false;
    unsigned int indegree;
    double dist = 0;
    int low = -1, num = -1;
    Edge<T> *path = nullptr;
    std::vector<Edge<T> *> incoming; 

    void deleteEdge(Edge<T> *edge);
};

/********************** Edge ****************************/

template <class T>
class Edge {
public:
    Edge(Vertex<T> *orig, Vertex<T> *dest, double w);
    Vertex<T> * getDest() const;
    Vertex<T> * getOrig() const;
    double getWeight() const;
    double getFlow() const;
    void setFlow(double flow);
    
    Edge<T> *getReverse() const;
    bool isSelected() const;
    void setSelected(bool selected);
    void setReverse(Edge<T> *reverse);

protected:
    Vertex<T> * orig;
    Vertex<T> * dest; 
    double weight; 
    double flow = 0;
    bool selected = false;
    Edge<T> *reverse = nullptr;
};

/********************** Graph ****************************/

template <class T>
class Graph {
public:
    ~Graph();
    Vertex<T> *findVertex(const T &in) const;
    int findVertexIdx(const T &in) const;
    bool addVertex(const T &in);
    bool removeVertex(const T &in);
    bool addEdge(const T &sourc, const T &dest, double w);
    bool removeEdge(const T &sourc, const T &dest);
    bool addBidirectionalEdge(const T &sourc, const T &dest, double w);
    int getNumVertex() const;
    std::vector<Vertex<T> *> getVertexSet() const;

protected:
    std::vector<Vertex<T> *> vertexSet;
    double ** distMatrix = nullptr;
    int **pathMatrix = nullptr;
};

//Edmonds-Karp

template <class T>
void testAndVisit(std::queue<Vertex<T>*> &q, Edge<T> *e, Vertex<T> *w, double residual) {
    if (!w->isVisited() && residual > 0) {
        w->setVisited(true);
        w->setPath(e);
        q.push(w);
    }
}

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
Vertex<T>::Vertex(T in): info(in) {}

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

inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++) if (m[i] != nullptr) delete [] m[i];
        delete [] m;
    }
}

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