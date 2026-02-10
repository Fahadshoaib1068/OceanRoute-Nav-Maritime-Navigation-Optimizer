#include<SFML/Graphics.hpp>
#include<SFML/System.hpp>
#include<SFML/Window.hpp>
#include<iostream>
#include<fstream>
#include<string>
#include<SFML/Audio.hpp>
#include<cmath>

using namespace std;

struct voyage
{
    string source;
    string destination;
    string date;
    string company;
    string arrivaltime;
    string departtime;
    double voyagecost;
    voyage* next;
};
struct edge
{
    string destination;
    voyage* voyages;
    edge* next;
};

// task 5 ship node
struct Ship {
    string shipname;
    string shipcompany;
    string arrivaltime;
    int processingtime;   //the time a ship will take to depart from a port
    Ship* next;

    Ship(string name, string company, string arrival, int processTime) {
        shipname = name;
        shipcompany = company;
        arrivaltime = arrival;
        processingtime = processTime;
        next = nullptr;
    }
};

struct ShipQueue {

    Ship* front;
    Ship* rear;
    int size;

    ShipQueue() {
        front = nullptr;
        rear = nullptr;
        size = 0;
    }

    void enqueue(string& name, string& company, string& arrival, int processTime) {
        Ship* newShip = new Ship(name, company, arrival, processTime);
        if (rear == nullptr) {
            front = rear = newShip;
            size++;
            return;
        }
        rear->next = newShip;
        rear = newShip;
        size++;
    }

    void dequeue() {
        if (front == nullptr) {
            return;
        }
        Ship* temp = front;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        delete temp;
        size--;
    }

    bool isEmpty() const {
        return front == nullptr;
    }


    int calculateLayover() const {
        Ship* temp = front;
        int wait = 0;

        while (temp != nullptr) {
            wait += temp->processingtime;
            temp = temp->next;
        }
        return wait;
    }

    int processingtime() const {
        if (front != nullptr) {
            return front->processingtime;
        }
        return 0;
    }

    ~ShipQueue() {
        while (!isEmpty()) {
            dequeue();
        }

    }
};


struct DockingandLayover {

    ShipQueue* Docking;
    ShipQueue* Arriving;

    DockingandLayover() {
        Docking = new ShipQueue();
        Arriving = new ShipQueue();
    }

    void addShip(string name, string company, string arrival, int processTime) {
        Docking->enqueue(name, company, arrival, processTime);
    }

    void AddArrivingShip(string name, string company, string arrival, int processTime) {
        Arriving->enqueue(name, company, arrival, processTime);
    }

    int getTotalLayover() const {
        if (Docking == nullptr) {
            return 0;
        }
        return Docking->calculateLayover();
    }

    int processingtime() const {
        if (Docking != nullptr && !Docking->isEmpty()) {
            return Docking->front->processingtime;
        }
        return 0;
    }
    void getnextShip() {
        if (Docking != nullptr && !Docking->isEmpty()) {
            Docking->dequeue();
        }
    }

    int getcurrentQueueSize() const {
        if (Docking == nullptr) {
            return 0;
        }
        return Docking->size;
    }

    int getArrivingQueueSize() const {
        if (Arriving == nullptr) {
            return 0;
        }
        return Arriving->size;
    }

    string getCurrentShipName() const {
        if (Docking != nullptr && !Docking->isEmpty()) {
            return Docking->front->shipname;
        }
        return "No ships in queue";
    }

    bool isempty() {
        return Docking->isEmpty();
    }

    ~DockingandLayover() {
        delete Docking;
    }

};

struct vertex
{
    string portname;
    double portcharge;
    edge* edges;
    int index;
    vertex* next;
    DockingandLayover* docklayover; // Task 5: Pointer to DockingandLayover structure

    vertex() {
        edges = nullptr;
        next = nullptr;
        docklayover = new DockingandLayover();
    }
};
struct heapnode
{
    int index;
    double cost;
    heapnode()
    {
        index = 0;
        cost = 0;
    }
};
struct minheap
{
    heapnode* arr;
    int size;
    int cap;
    minheap()
    {
        cap = 0;
        size = 0;
        arr = new heapnode[cap];
    }
};
string tolower(const string& s) //helping function to convert to lowercase
{
    string result = s;
    for (int i = 0; i < (int)result.length(); i++)
    {
        if (result[i] >= 'A' && result[i] <= 'Z')
        {
            result[i] = result[i] + ('a' - 'A');
        }
    }
    return result;
}
minheap* createheap(int cap) //initializing heap
{
    minheap* h = new minheap;
    h->arr = new heapnode[cap];
    h->size = 0;
    h->cap = cap;
    return h;
}
void swap(heapnode& a, heapnode& b) //swapping in heap
{
    heapnode t = a;
    a = b;
    b = t;
}
void heappush(minheap* h, int index, double cost)
{
    int i = h->size++;
    h->arr[i].index = index;
    h->arr[i].cost = cost;
    while (i > 0 && h->arr[i].cost < h->arr[(i - 1) / 2].cost)
    {
        swap(h->arr[i], h->arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}
heapnode heappop(minheap* h)
{
    heapnode root = h->arr[0];
    h->arr[0] = h->arr[--h->size];
    int i = 0;
    while (true)
    {
        int left = i * 2 + 1;
        int right = i * 2 + 2;
        int smallest = i;
        if (left < h->size && h->arr[left].cost < h->arr[smallest].cost)
        {
            smallest = left;
        }
        if (right < h->size && h->arr[right].cost < h->arr[smallest].cost)
        {
            smallest = right;
        }
        if (smallest == i)
        {
            break;
        }
        swap(h->arr[i], h->arr[smallest]);
        i = smallest;
    }
    return root;
}
int findIndex(vertex* graph, const string& name) //finding vertex for using algorithms
{
    while (graph != NULL)
    {
        if (tolower(graph->portname) == tolower(name))
        {
            return graph->index;
        }
        graph = graph->next;
    }
    return -1;
}
int timetominutes(const string& t) //helping function for conversion of time to minutes
{
    int h = (t[0] - '0') * 10 + (t[1] - '0');
    int m = (t[3] - '0') * 10 + (t[4] - '0');
    return h * 60 + m;
}
double getmincost(voyage* v)
{
    double c = 1e18; //setting high cost for comparison
    while (v != NULL)
    {
        if (v->voyagecost < c)
        {
            c = v->voyagecost;
        }
        v = v->next;
    }
    return c;
}
int getmintime(voyage* v) //helping func to find min time
{
    int best = 1e9;
    while (v != NULL)
    {
        int depart = timetominutes(v->departtime);
        int arrive = timetominutes(v->arrivaltime);
        int diff = arrive - depart;
        if (diff < 0)//next day arrival
        {
            diff += 24 * 60;
        }
        if (diff < best)
        {
            best = diff;
        }
        v = v->next;
    }
    return best;
}
double heuristic_cost(const string& a, const string& b)
{
    return 0;
}

int* astar_cost(vertex* graph, vertex** portArray, int V, int start, int goal) //A* cost
{
    double* g = new double[V];
    double* f = new double[V];
    int* parent = new int[V];
    bool* closed = new bool[V];

    for (int i = 0; i < V; i++)
    {
        g[i] = 1e18;
        f[i] = 1e18;
        parent[i] = -1;
        closed[i] = false;
    }

    minheap* heap = createheap(V * 10);

    g[start] = 0;
    f[start] = heuristic_cost(portArray[start]->portname,
        portArray[goal]->portname);

    heappush(heap, start, f[start]);

    while (heap->size > 0)
    {
        heapnode h = heappop(heap);
        int u = h.index;

        if (closed[u]) continue;
        closed[u] = true;

        if (u == goal) break;

        edge* e = portArray[u]->edges;

        while (e != NULL)
        {
            int v = findIndex(graph, e->destination);
            double cost = getmincost(e->voyages);

            double ng = g[u] + cost;

            if (ng < g[v])
            {
                g[v] = ng;
                parent[v] = u;
                f[v] = g[v] + heuristic_cost(
                    portArray[v]->portname,
                    portArray[goal]->portname);

                heappush(heap, v, f[v]);
            }

            e = e->next;
        }
    }

    return parent;
}
double heuristic_time(const string& a, const string& b)
{
    return 0; // no coordinates
}

int* astar_time(vertex* graph, vertex** portArray, int V, int start, int goal) //A* time algo
{
    int* g = new int[V];
    int* f = new int[V];
    bool* closed = new bool[V];
    int* parent = new int[V];

    for (int i = 0; i < V; i++)
    {
        g[i] = 1e9;
        f[i] = 1e9;
        parent[i] = -1;
        closed[i] = false;
    }

    minheap* heap = createheap(V * 10);

    g[start] = 0;
    f[start] = heuristic_time(portArray[start]->portname,
        portArray[goal]->portname);

    heappush(heap, start, f[start]);

    while (heap->size > 0)
    {
        heapnode h = heappop(heap);
        int u = h.index;

        if (closed[u]) continue;
        closed[u] = true;

        if (u == goal) break;

        edge* e = portArray[u]->edges;

        while (e != NULL)
        {
            int v = findIndex(graph, e->destination);
            int t = getmintime(e->voyages);

            int ng = g[u] + t;

            if (ng < g[v])
            {
                g[v] = ng;
                parent[v] = u;
                f[v] = g[v] + heuristic_time(
                    portArray[v]->portname,
                    portArray[goal]->portname);
                heappush(heap, v, f[v]);
            }

            e = e->next;
        }
    }

    return parent;
}
int* dijkstra_cost(vertex* graph, vertex** portArray, int v, int start, int goal) //dijkstra for cost
{
    double* dist = new double[v];
    int* parent = new int[v];
    bool* visited = new bool[v];

    for (int i = 0; i < v; i++)
    {
        dist[i] = 1e18;
        visited[i] = false;
        parent[i] = -1;
    }
    minheap* heap = createheap(v * 10);
    dist[start] = 0;
    heappush(heap, start, 0);
    while (heap->size > 0)
    {
        heapnode h = heappop(heap);
        int u = h.index;
        if (visited[u])
        {
            continue;
        }
        visited[u] = true;
        if (u == goal)
        {
            break;
        }
        edge* e = portArray[u]->edges;

        while (e != NULL)
        {
            int v = findIndex(graph, e->destination);
            double cost = getmincost(e->voyages);

            if (dist[u] + cost < dist[v])
            {
                dist[v] = dist[u] + cost;
                parent[v] = u;
                heappush(heap, v, dist[v]);
            }

            e = e->next;
        }
    }
    return parent;
}
int getNextArrivalTime(voyage* v, int currentTime)
{
    int bestArrival = 1e9;

    while (v != NULL)
    {
        int depart = timetominutes(v->departtime);
        int arrive = timetominutes(v->arrivaltime);

        // Convert voyage into absolute minutes (multi-day)
        while (depart < currentTime)
        {
            depart += 24 * 60;
            arrive += 24 * 60;
        }

        int arrivalTime = arrive;  // arrival already matched to next available departure

        if (arrivalTime < bestArrival)
            bestArrival = arrivalTime;

        v = v->next;
    }

    return bestArrival;
}
int* dijkstra_time(vertex* graph, vertex** portArray, int v, int start, int goal)
{
    int* dist = new int[v];
    int* parent = new int[v];
    bool* visited = new bool[v];

    for (int i = 0; i < v; i++)
    {
        dist[i] = 1e9;
        parent[i] = -1;
        visited[i] = false;
    }

    minheap* heap = createheap(v * 10);
    dist[start] = 0; // start at time 0
    heappush(heap, start, 0);

    while (heap->size > 0)
    {
        heapnode h = heappop(heap);
        int u = h.index;

        if (visited[u])
        {
            continue;
        }

        visited[u] = true;

        if (u == goal)
        {
            break;
        }
        edge* e = portArray[u]->edges;
        while (e != NULL)
        {
            int destIndex = findIndex(graph, e->destination);
            int arrivalTime = getNextArrivalTime(e->voyages, dist[u]);
            if (arrivalTime < dist[destIndex])
            {
                dist[destIndex] = arrivalTime;
                parent[destIndex] = u;
                heappush(heap, destIndex, dist[destIndex]);
            }
            e = e->next;
        }
    }

    return parent;
}
int loadPorts(vertex* graph, string* portNames[])
{
    int count = 0;
    for (vertex* g = graph; g != NULL; g = g->next)
    {
        portNames[count] = &g->portname;  // your existing char*
        count++;
    }
    return count;
}
void getPortPosition(const std::string& name, float& x, float& y) {
    if (name == "Karachi") { x = 750; y = 350; }
    else if (name == "Mumbai") { x = 780; y = 400; }
    else if (name == "Colombo") { x = 820; y = 450; }
    else if (name == "Chittagong") { x = 850; y = 350; }
    else if (name == "Singapore") { x = 870; y = 470; }
    else if (name == "Jakarta") { x = 890; y = 520; }
    else if (name == "Manila") { x = 950; y = 430; }
    else if (name == "HongKong") { x = 920; y = 380; }
    else if (name == "Shanghai") { x = 940; y = 330; }
    else if (name == "Busan") { x = 980; y = 320; }
    else if (name == "Osaka") { x = 1000; y = 340; }
    else if (name == "Tokyo") { x = 1020; y = 320; }

    else if (name == "Dubai") { x = 700; y = 380; }
    else if (name == "AbuDhabi") { x = 710; y = 390; }
    else if (name == "Jeddah") { x = 650; y = 380; }
    else if (name == "Doha") { x = 690; y = 370; }
    else if (name == "Istanbul") { x = 600; y = 300; }
    else if (name == "Alexandria") { x = 550; y = 330; }
    else if (name == "Athens") { x = 580; y = 320; }

    else if (name == "London") { x = 450; y = 250; }
    else if (name == "Rotterdam") { x = 500; y = 260; }
    else if (name == "Hamburg") { x = 510; y = 240; }
    else if (name == "Copenhagen") { x = 520; y = 220; }
    else if (name == "Stockholm") { x = 530; y = 200; }
    else if (name == "Oslo") { x = 500; y = 200; }
    else if (name == "Helsinki") { x = 550; y = 180; }
    else if (name == "Genoa") { x = 520; y = 320; }
    else if (name == "Marseille") { x = 480; y = 320; }
    else if (name == "Lisbon") { x = 400; y = 340; }
    else if (name == "Antwerp") { x = 490; y = 270; }
    else if (name == "Dublin") { x = 430; y = 240; }

    else if (name == "CapeTown") { x = 580; y = 600; }
    else if (name == "Durban") { x = 620; y = 550; }
    else if (name == "PortLouis") { x = 750; y = 550; }

    else if (name == "Sydney") { x = 1100; y = 550; }
    else if (name == "Melbourne") { x = 1080; y = 570; }

    else if (name == "Vancouver") { x = 200; y = 250; }
    else if (name == "LosAngeles") { x = 180; y = 350; }
    else if (name == "Montreal") { x = 350; y = 240; }
    else if (name == "NewYork") { x = 320; y = 300; }

    else { x = 0; y = 0; } // default if not found
}
void visualizePath(sf::RenderWindow& window, string* portNames[], int portCount, int* parent, int start, int goal)
{
    int path[200];
    int pathLength = 0;
    int current = goal;
    while (current != -1)
    {
        path[pathLength++] = current;
        current = parent[current];
    }

    for (int i = 0; i < pathLength / 2; i++)
    {
        int tmp = path[i];
        path[i] = path[pathLength - 1 - i];
        path[pathLength - 1 - i] = tmp;
    }

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture texture;
    if (!texture.loadFromFile("map_bg.png")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    back.setPosition(centerX + 300.f, centerY + 300.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ back.getLocalBounds().width + 40.f, back.getLocalBounds().height + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 300.f);

    const int circleRadius = 10;

    // Animate path step by step
    for (int step = 0; step < pathLength; step++)
    {
        window.clear();
        window.draw(sprite);

        for (int i = 0; i < portCount; i++)
        {
            float x, y;
            getPortPosition(*portNames[i], x, y);

            sf::CircleShape circle(circleRadius);
            bool visited = false;
            for (int j = 0; j <= step; j++)
                if (i == path[j]) visited = true;

            circle.setFillColor(visited ? sf::Color::Yellow : sf::Color::Blue);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2.f);
            circle.setPosition(x - circleRadius, y - circleRadius);

            sf::Text label(*portNames[i], font, 12);
            label.setFillColor(sf::Color::White);
            label.setPosition(x - label.getLocalBounds().width / 2.f, y + 15.f);

            window.draw(circle);
            window.draw(label);
        }

        for (int k = 1; k <= step; k++)
        {
            float px, py, cx, cy;
            getPortPosition(*portNames[path[k - 1]], px, py);
            getPortPosition(*portNames[path[k]], cx, cy);

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(px, py), sf::Color::Red),
                sf::Vertex(sf::Vector2f(cx, cy), sf::Color::Red)
            };
            window.draw(line, 2, sf::Lines);
        }
        window.draw(backrectangle);
        window.draw(back);

        window.display();
        sf::sleep(sf::seconds(1.f));
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                    return;
            }
        }
    }
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                    return; // Go back
            }
        }
        window.clear();
        window.draw(sprite);
        for (int i = 0; i < portCount; i++)
        {
            float x, y;
            getPortPosition(*portNames[i], x, y);

            sf::CircleShape circle(circleRadius);
            bool visited = false;
            for (int j = 0; j < pathLength; j++)
                if (i == path[j]) visited = true;

            circle.setFillColor(visited ? sf::Color::Yellow : sf::Color::Blue);
            circle.setOutlineColor(sf::Color::White);
            circle.setOutlineThickness(2.f);
            circle.setPosition(x - circleRadius, y - circleRadius);

            sf::Text label(*portNames[i], font, 12);
            label.setFillColor(sf::Color::White);
            label.setPosition(x - label.getLocalBounds().width / 2.f, y + 15.f);

            window.draw(circle);
            window.draw(label);
        }

        for (int k = 1; k < pathLength; k++)
        {
            float px, py, cx, cy;
            getPortPosition(*portNames[path[k - 1]], px, py);
            getPortPosition(*portNames[path[k]], cx, cy);

            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(px, py), sf::Color::Red),
                sf::Vertex(sf::Vector2f(cx, cy), sf::Color::Red)
            };
            window.draw(line, 2, sf::Lines);
        }
        window.draw(backrectangle);
        window.draw(back);

        window.display();
    }
}
void drawAllPorts(sf::RenderWindow& window, std::string* portNames[], int portCount) {
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture texture;
    if (!texture.loadFromFile("map_bg.png")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    const int circleRadius = 10;

    // Draw once
    window.clear();
    window.draw(sprite);

    for (int i = 0; i < portCount; i++) {
        float x, y;
        getPortPosition(*portNames[i], x, y);

        sf::CircleShape circle(circleRadius);
        circle.setFillColor(sf::Color::Blue);
        circle.setOutlineColor(sf::Color::White);
        circle.setOutlineThickness(2.f);
        circle.setPosition(x - circleRadius, y - circleRadius);

        sf::Text label(*portNames[i], font, 12);
        label.setFillColor(sf::Color::White);
        label.setPosition(x - label.getLocalBounds().width / 2.f, y + 15.f);

        window.draw(circle);
        window.draw(label);
    }

    window.display();

    // Handle events once so window stays responsive
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }
}

void findbycost(sf::RenderWindow& window, vertex* graph, vertex** portarray, int v, int start, int goal)
{
    string* portNames[200];
    int portCount = loadPorts(graph, portNames);

    drawAllPorts(window, portNames, portCount);
    int* parent = dijkstra_cost(graph, portarray, v, start, goal);

    visualizePath(window, portNames, portCount, parent, start, goal);

    delete[] parent;

}
void findbytime(sf::RenderWindow& window, vertex* graph, vertex** portarray, int v, int start, int goal)
{
    string* portNames[200];
    int portCount = loadPorts(graph, portNames);

    drawAllPorts(window, portNames, portCount);
    int* parent = dijkstra_time(graph, portarray, v, start, goal);

    visualizePath(window, portNames, portCount, parent, start, goal);

    delete[] parent;

}
void choosepath(sf::RenderWindow& window, vertex* graph, vertex** portarray, int v, int start, int goal)
{
    sf::Texture texture;
    if (!texture.loadFromFile("select.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float lineSpacing = 100.f;

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setString("Select Option");
    text.setPosition(centerX - 100.f, centerY - 300.f);

    sf::Text route;
    route.setFont(font);
    route.setFillColor(sf::Color::White);
    route.setString("Find Shortest Path by Cost");
    float routeWidth = route.getLocalBounds().width;
    float routeHeight = route.getLocalBounds().height;
    route.setPosition(centerX - routeWidth / 2.f, centerY - 40.f);


    sf::RectangleShape routerectangle;
    routerectangle.setSize({ routeWidth + 40.f, routeHeight + 20.f });
    routerectangle.setFillColor(sf::Color::Blue);
    routerectangle.setOutlineThickness(2.f);
    routerectangle.setOutlineColor(sf::Color::Black);
    routerectangle.setPosition(centerX - routerectangle.getSize().x / 2.f, centerY - 40.f);

    sf::Text cost;
    cost.setFont(font);
    cost.setFillColor(sf::Color::White);
    cost.setString("Find Shortest Path by Time");
    float graphWidth = cost.getLocalBounds().width;
    float graphHeight = cost.getLocalBounds().height;
    cost.setPosition(centerX - graphWidth / 2.f, centerY + lineSpacing);

    sf::RectangleShape graphrectangle;
    graphrectangle.setSize({ graphWidth + 40.f, graphHeight + 20.f });
    graphrectangle.setFillColor(sf::Color::Blue);
    graphrectangle.setOutlineThickness(2.f);
    graphrectangle.setOutlineColor(sf::Color::Black);
    graphrectangle.setPosition(centerX - graphrectangle.getSize().x / 2.f, centerY + lineSpacing - 5.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 300.f, centerY + 200.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 200.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (routerectangle.getGlobalBounds().contains(mousePosF))
                {
                    findbycost(window, graph, portarray, v, start, goal);
                }
                if (graphrectangle.getGlobalBounds().contains(mousePosF))
                {
                    findbytime(window, graph, portarray, v, start, goal);
                }
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                {
                    return;
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.draw(text);
        window.draw(routerectangle);
        window.draw(route);
        window.draw(graphrectangle);
        window.draw(cost);
        window.draw(backrectangle);
        window.draw(back);
        window.display();
    }
}
void dijsfml(sf::RenderWindow& window, vertex* graph, vertex** portarray, int v)
{
    sf::Texture texture;
    if (!texture.loadFromFile("findshort.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float lineSpacing = 100.f;

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Black);
    text.setString("Enter Source & Destination Ports");
    text.setPosition(centerX - 90.f, centerY - 300.f);


    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 300.f, centerY + 250.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 250.f);

    sf::Text sourceLabel("Enter Source", font);
    sourceLabel.setFillColor(sf::Color::White);
    sourceLabel.setPosition(centerX - sourceLabel.getLocalBounds().width / 2.f, centerY - 60.f);

    // Source input rectangle
    sf::RectangleShape sourceRect(sf::Vector2f(300.f, 40.f));
    sourceRect.setFillColor(sf::Color::Blue);
    sourceRect.setOutlineThickness(2.f);
    sourceRect.setOutlineColor(sf::Color::Black);
    sourceRect.setPosition(centerX - sourceRect.getSize().x / 2.f, centerY + 10.f);

    sf::Text sourceInputText("", font);
    sourceInputText.setFillColor(sf::Color::White);
    sourceInputText.setPosition(sourceRect.getPosition().x + 5.f, sourceRect.getPosition().y + 5.f);

    // Destination label
    sf::Text destLabel("Enter Destination", font);
    destLabel.setFillColor(sf::Color::White);
    destLabel.setPosition(centerX - destLabel.getLocalBounds().width / 2.f, centerY + lineSpacing);

    // Destination input rectangle
    sf::RectangleShape destRect(sf::Vector2f(300.f, 40.f));
    destRect.setFillColor(sf::Color::Blue);
    destRect.setOutlineThickness(2.f);
    destRect.setOutlineColor(sf::Color::Black);
    destRect.setPosition(centerX - destRect.getSize().x / 2.f, centerY + lineSpacing + 50.f);

    sf::Text destInputText("", font);
    destInputText.setFillColor(sf::Color::White);
    destInputText.setPosition(destRect.getPosition().x + 5.f, destRect.getPosition().y + 5.f);

    // Input handling
    string sourceInput = "";
    string destInput = "";
    bool typingSource = true;
    bool typingDest = false;

    // Cursor
    sf::RectangleShape cursor(sf::Vector2f(2.f, 30.f));
    bool showCursor = true;
    sf::Clock clock;

    sf::Text status("", font);
    status.setCharacterSize(24);
    status.setFillColor(sf::Color::Green);
    status.setPosition(centerX - status.getLocalBounds().width / 2.f, centerY + 200.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (backrectangle.getGlobalBounds().contains(mousePosF))
                    return; // go back
            }
            if (event.type == sf::Event::TextEntered)
            {
                char c = event.text.unicode;
                if (c == 8) // Backspace
                {
                    if (typingSource && !sourceInput.empty()) sourceInput.pop_back();
                    if (typingDest && !destInput.empty()) destInput.pop_back();
                }
                else if (c == 13) // Enter
                {
                    if (typingSource)
                    {
                        typingSource = false;
                        typingDest = true;
                    }
                    else if (typingDest)
                    {
                        typingDest = false;
                    }
                }
                else if (c >= 32 && c < 128) // Printable
                {
                    if (typingSource) sourceInput += c;
                    if (typingDest) destInput += c;
                }

                sourceInputText.setString(sourceInput);
                destInputText.setString(destInput);
            }
        }

        // Cursor blink
        if (clock.getElapsedTime().asSeconds() > 0.5f)
        {
            showCursor = !showCursor;
            clock.restart();
        }

        if (typingSource)
            cursor.setPosition(sourceInputText.getPosition().x + sourceInputText.getLocalBounds().width + 2.f,
                sourceInputText.getPosition().y);
        else if (typingDest)
            cursor.setPosition(destInputText.getPosition().x + destInputText.getLocalBounds().width + 2.f,
                destInputText.getPosition().y);

        // Draw
        window.clear();
        window.draw(sprite);
        window.draw(text);
        window.draw(sourceLabel);
        window.draw(sourceRect);
        window.draw(backrectangle);
        window.draw(back);
        window.draw(sourceInputText);
        window.draw(destLabel);
        window.draw(destRect);
        window.draw(destInputText);
        if (showCursor && (typingSource || typingDest))
            window.draw(cursor);
        window.display();

        if (!typingSource && !typingDest)
            break;
    }
    int start = findIndex(graph, sourceInput);
    int goal = findIndex(graph, destInput);
    if (start == -1 || goal == -1)
    {
        status.setString("Source or Destination not found!");
        status.setFillColor(sf::Color::Red);
        window.draw(status);
        window.display();
        sf::sleep(sf::seconds(2));
        return;
    }
    else
    {
        status.setString("Ports found! Ready to compute path.");
        window.draw(status);
        window.display();
        sf::sleep(sf::seconds(2));
        choosepath(window, graph, portarray, v, start, goal);

    }

}

struct UserPreferences {

    string* avoidedPorts;           // Dynamic array of port names to avoid
    int avoidedPortCount;
    int maxVoyageTimeMinutes;       // Maximum voyage duration in minutes (0 = no limit)
    bool isActive;                  // Toggle preferences on/off
    string prefrenceCommpany;
    bool CalmWeather;

    UserPreferences() {
        avoidedPorts = nullptr;
        avoidedPortCount = 0;
        maxVoyageTimeMinutes = 0;   // 0 means no time limit
        isActive = false;
        CalmWeather = false;
        prefrenceCommpany = "";
    }

    ~UserPreferences() {
        if (avoidedPorts != nullptr) {
            delete[] avoidedPorts;
        }
    }
};


bool isRouteStomy(voyage* v) {

    if (v->voyagecost > 35000)  //stromy if greater distance
        return true;

    if ((v->source == "NewYork" || v->source == "Montreal") &&      // Atlantic routes
        (v->destination == "London" || v->destination == "Rotterdam")) {

        return true;
    }

    if (v->source == "CapeTown" || v->destination == "CapeTown") {
        return true; // stormy
    }

    // 3. Winter routes 
    if (v->source == "Oslo" || v->source == "Stockholm" || v->source == "Helsinki") {
        return true;
    }

    return false;
}


bool isCompanyPreferred(UserPreferences* prefs, voyage* v) {
    if (!prefs->isActive || prefs->prefrenceCommpany.empty()) {
        return true; // No company filter
    }
    return (v->company == prefs->prefrenceCommpany);
}

// Helper function
bool isVoyageTooLong(UserPreferences* prefs, voyage* v) {

    if (!prefs->isActive || prefs->maxVoyageTimeMinutes == 0) {
        return true; // No time limit set
    }

    // time limit
    int depart = timetominutes(v->departtime);
    int arrive = timetominutes(v->arrivaltime);
    int duration = arrive - depart;

    if (duration < 0) { // Next day arrival
        duration += 24 * 60;
    }

    if (duration > prefs->maxVoyageTimeMinutes) {
        return false;
    }

    // Check for stormy routes if user wants to avoid them
    if (!isCompanyPreferred(prefs, v)) {
        return false;
    }

    if (prefs->CalmWeather && isRouteStomy(v)) {
        return false;
    }

    return true;
}

// Helper function: Check if a port is in the avoided list
bool isPortAvoided(UserPreferences* prefs, const string& portName) {
    if (!prefs->isActive || prefs->avoidedPorts == nullptr) {
        return false;
    }

    for (int i = 0; i < prefs->avoidedPortCount; i++) {
        if (tolower(prefs->avoidedPorts[i]) == tolower(portName)) {
            return true;
        }
    }
    return false;
}

bool tracePath(vertex** portarray, int vCount, int start, int goal, int parent[], UserPreferences* pref, vertex* graph)
{
    for (int i = 0; i < vCount; i++)
        parent[i] = -1;

    bool visited[200] = { false };
    int queue[200];
    int front = 0, rear = 0;

    queue[rear++] = start;
    visited[start] = true;

    while (front < rear)
    {
        int current = queue[front++];
        edge* e = portarray[current]->edges;
        while (e != nullptr)
        {
            // check user prefrences then avoid that port
            if (pref->isActive && isPortAvoided(pref, e->destination)) {
				e = e->next;
                continue;
            }
            
            int nextIndex = -1;
            for (int i = 0; i < vCount; i++)
            {
                
                if (portarray[i]->portname == e->destination)
                {
                    nextIndex = i;
                    break;
                }
            }

            if (nextIndex != -1 && !visited[nextIndex])
            {
				bool hasValidVoyage = false;

                if (pref->isActive) {
					voyage* v = e->voyages;
                    while (v != nullptr) {
                        if (isVoyageTooLong(pref, v)) {
                            hasValidVoyage = true;
                            break;
                        }
						v = v->next;
                    }
                }
                else {
					hasValidVoyage = true;    // no prefrences, all voyages valid
                }

                if (hasValidVoyage) {

                    visited[nextIndex] = true;
                    parent[nextIndex] = current;
                    queue[rear++] = nextIndex;

                    if (nextIndex == goal)
                        return true;
                }
               
            }

            e = e->next;
        }
    }

    return false;
}

void task6(sf::RenderWindow& window, vertex* graph, vertex** portarray, int v, UserPreferences* pref)
{
    string* portNames[200];
    for (int i = 0; i < v; i++)
    {
        portNames[i] = &portarray[i]->portname;
    }
    sf::Texture texture;
    if (!texture.loadFromFile("task.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float lineSpacing = 100.f;

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::Black);
    text.setString("Enter Source & Destination Ports");
    text.setPosition(centerX - 90.f, centerY - 300.f);

	sf::Text prefStatus;         // PREFERENCE STATUS
    prefStatus.setFont(font);
    prefStatus.setCharacterSize(18);
    if (pref->isActive) {
        prefStatus.setString("Preferences: ACTIVE");
        prefStatus.setFillColor(sf::Color::Green);
    }
    else {
        prefStatus.setString("Preferences: OFF");
        prefStatus.setFillColor(sf::Color(100, 100, 100));
    }
    prefStatus.setPosition(centerX - 100.f, centerY - 350.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 300.f, centerY + 250.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 250.f);

    sf::Text sourceLabel("Enter Source", font);
    sourceLabel.setFillColor(sf::Color::White);
    sourceLabel.setPosition(centerX - sourceLabel.getLocalBounds().width / 2.f, centerY - 60.f);

    // Source input rectangle
    sf::RectangleShape sourceRect(sf::Vector2f(300.f, 40.f));
    sourceRect.setFillColor(sf::Color::Blue);
    sourceRect.setOutlineThickness(2.f);
    sourceRect.setOutlineColor(sf::Color::Black);
    sourceRect.setPosition(centerX - sourceRect.getSize().x / 2.f, centerY + 10.f);

    sf::Text sourceInputText("", font);
    sourceInputText.setFillColor(sf::Color::White);
    sourceInputText.setPosition(sourceRect.getPosition().x + 5.f, sourceRect.getPosition().y + 5.f);

    // Destination label
    sf::Text destLabel("Enter Destination", font);
    destLabel.setFillColor(sf::Color::White);
    destLabel.setPosition(centerX - destLabel.getLocalBounds().width / 2.f, centerY + lineSpacing);

    // Destination input rectangle
    sf::RectangleShape destRect(sf::Vector2f(300.f, 40.f));
    destRect.setFillColor(sf::Color::Blue);
    destRect.setOutlineThickness(2.f);
    destRect.setOutlineColor(sf::Color::Black);
    destRect.setPosition(centerX - destRect.getSize().x / 2.f, centerY + lineSpacing + 50.f);

    sf::Text destInputText("", font);
    destInputText.setFillColor(sf::Color::White);
    destInputText.setPosition(destRect.getPosition().x + 5.f, destRect.getPosition().y + 5.f);

    // Input handling
    string sourceInput = "";
    string destInput = "";
    bool typingSource = true;
    bool typingDest = false;

    // Cursor
    sf::RectangleShape cursor(sf::Vector2f(2.f, 30.f));
    bool showCursor = true;
    sf::Clock clock;

    sf::Text status("", font);
    status.setCharacterSize(24);
    status.setFillColor(sf::Color::Green);
    status.setPosition(centerX - status.getLocalBounds().width / 2.f, centerY + 200.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                return;
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (backrectangle.getGlobalBounds().contains(mousePosF))
                    return; // go back
            }
            if (event.type == sf::Event::TextEntered)
            {
                char c = event.text.unicode;
                if (c == 8) // Backspace
                {
                    if (typingSource && !sourceInput.empty()) sourceInput.pop_back();
                    if (typingDest && !destInput.empty()) destInput.pop_back();
                }
                else if (c == 13) // Enter
                {
                    if (typingSource)
                    {
                        typingSource = false;
                        typingDest = true;
                    }
                    else if (typingDest)
                    {
                        typingDest = false;
                    }
                }
                else if (c >= 32 && c < 128) // Printable
                {
                    if (typingSource) sourceInput += c;
                    if (typingDest) destInput += c;
                }

                sourceInputText.setString(sourceInput);
                destInputText.setString(destInput);
            }
        }

        // Cursor blink
        if (clock.getElapsedTime().asSeconds() > 0.5f)
        {
            showCursor = !showCursor;
            clock.restart();
        }

        if (typingSource)
            cursor.setPosition(sourceInputText.getPosition().x + sourceInputText.getLocalBounds().width + 2.f,
                sourceInputText.getPosition().y);
        else if (typingDest)
            cursor.setPosition(destInputText.getPosition().x + destInputText.getLocalBounds().width + 2.f,
                destInputText.getPosition().y);

        // Draw
        window.clear();
        window.draw(sprite);
        window.draw(text);
        window.draw(sourceLabel);
        window.draw(sourceRect);
        window.draw(backrectangle);
        window.draw(back);
        window.draw(sourceInputText);
        window.draw(destLabel);
        window.draw(destRect);
        window.draw(destInputText);
        if (showCursor && (typingSource || typingDest))
            window.draw(cursor);
        window.display();

        if (!typingSource && !typingDest)
            break;
    }
    int start = findIndex(graph, sourceInput);
    int goal = findIndex(graph, destInput);
    if (start == -1 || goal == -1)
    {
        status.setString("Source or Destination not found!");
        status.setFillColor(sf::Color::Red);
        window.draw(status);
        window.display();
        sf::sleep(sf::seconds(2));
        return;
    }

    if (pref->isActive) {
        if (isPortAvoided(pref, sourceInput)) {

            status.setString("Error: " + sourceInput + " is in avoided ports!");
            status.setFillColor(sf::Color::Red);
            window.clear();
            window.draw(sprite);
            window.draw(text);
            window.draw(status);
            window.display();
            sf::sleep(sf::seconds(2));
            return;
        }

        if (isPortAvoided(pref, destInput)) {
            status.setString("Error: " + destInput + " is in avoided ports!");
            status.setFillColor(sf::Color::Red);
            window.clear();
            window.draw(sprite);
            window.draw(text);
            window.draw(status);
            window.display();
            sf::sleep(sf::seconds(2));
            return;
        }
    }
  
        //status.setString("Ports found! Ready to compute path.");
        //window.draw(status);
        //window.display();
        //sf::sleep(sf::seconds(2));

        int parent[200];

        if (!tracePath(portarray, v, start, goal, parent, pref, graph))  // your custom function
        {
            //status.setString("No route found!");
            //status.setFillColor(sf::Color::Red);
            //window.draw(status);
            //window.display();
            //sf::sleep(sf::seconds(2));
            //return;

            if (pref->isActive) {
                status.setString("No route found matching your preferences!");
                status.setFillColor(sf::Color::Red);
            }
            else {
                status.setString("No route found between these ports!");
                status.setFillColor(sf::Color::Red);
            }

            window.clear();
            window.draw(sprite);
            window.draw(text);
            window.draw(status);
            window.display();
            sf::sleep(sf::seconds(2));
            return;
        }

        status.setString("Route Found! Visualizing...");
        status.setFillColor(sf::Color::Green);
        window.draw(status);
        window.display();
        sf::sleep(sf::seconds(2));
        visualizePath(window, portNames, v, parent, start, goal);

    

}
void printpath(int* parent, int start, int goal, vertex** portArray)
{
    int path[100]; // assuming max 100 ports, adjust if needed
    int pathLength = 0;

    int current = goal;
    while (current != -1)
    {
        path[pathLength++] = current;
        current = parent[current];
    }

    // Check if path exists
    if (path[pathLength - 1] != start)
    {
        cout << "No path exists between "
            << portArray[start]->portname
            << " and "
            << portArray[goal]->portname
            << endl;
        return;
    }

    // Print path in correct order
    cout << "Path from " << portArray[start]->portname
        << " to " << portArray[goal]->portname << ":\n";

    for (int i = pathLength - 1; i >= 0; i--)
    {
        cout << portArray[path[i]]->portname;
        if (i != 0) cout << " -> ";
    }
    cout << endl;
}
string getnextfield(string& line, int& start) //helping function to parse voyage's data
{
    string data = "";
    int i = start;
    while (i < (int)line.length() && line[i] == ' ')
    {
        i++; //skipping spaces
    }
    while (i < (int)line.length() && line[i] != ' ')
    {
        data += line[i];
        i++;
    }
    start = i;
    return data;
}

double stringtodouble(string& word) // Helping function to convert string to double
{
    double cost = 0;
    for (int i = 0; i < (int)word.length(); i++)
    {
        cost = cost * 10 + (word[i] - '0');
    }
    return cost;
}

voyage* parsevoyageline(string& line)
{
    int pos = 0;
    voyage* v = new voyage;
    v->source = getnextfield(line, pos);
    v->destination = getnextfield(line, pos);
    v->date = getnextfield(line, pos);
    v->departtime = getnextfield(line, pos);
    v->arrivaltime = getnextfield(line, pos);
    string cost = getnextfield(line, pos);
    v->voyagecost = stringtodouble(cost);
    v->company = getnextfield(line, pos);
    v->next = NULL;
    return v;
}

voyage* loadvoyagesfromfile(const string& filename) //loading Routes.txt from file
{
    ifstream file(filename.c_str());
    if (!file.is_open())
    {
        cout << "File cannot be opened" << endl;
        return NULL;
    }
    voyage* head = NULL;
    voyage* tail = NULL;
    string line;
    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        voyage* v = parsevoyageline(line);
        if (head == NULL)
        {
            head = v;
            tail = v;
        }
        else
        {
            tail->next = v;
            tail = v;
        }
    }
    file.close();
    return head;
}
void loadportcharges(const string& filename, vertex* graph)
{
    ifstream file(filename.c_str());
    if (!file.is_open())
    {
        cout << "File cannot be opened" << endl;
        return;
    }
    string line;
    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        int pos = 0;
        string name = getnextfield(line, pos);
        string charges = getnextfield(line, pos);
        double charge = stringtodouble(charges);
        vertex* v = graph;
        bool found = false;
        while (v != NULL)
        {
            if (v->portname == name)
            {
                v->portcharge = charge;
                found = true;
                break;
            }
            v = v->next;
        }
    }
    file.close();
}
vertex* createvertex(vertex*& head, const string& portname) //creating creating by giving port
{
    vertex* curr = head;
    while (curr != NULL)
    {
        if (curr->portname == portname)
        {
            return curr;
        }
        curr = curr->next;
    }
    vertex* vertexs = new vertex;
    vertexs->portname = portname;
    vertexs->portcharge = 0;
    vertexs->edges = NULL;
    vertexs->next = NULL;
    if (head == NULL)
    {
        head = vertexs;
    }
    else
    {
        curr = head;
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = vertexs;
    }
    return vertexs;
}
void addtograph(vertex*& graph, voyage* v) //adding voyages to graph
{
    vertex* port = createvertex(graph, v->source);
    edge* e = port->edges;
    while (e != NULL)
    {
        if (e->destination == v->destination)
        {
            break;
        }
        e = e->next;
    }
    if (e == NULL)
    {
        e = new edge;
        e->destination = v->destination;
        e->voyages = v;
        e->next = port->edges;
        port->edges = e;
        v->next = NULL;
    }
    else
    {
        voyage* curr = e->voyages;
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        curr->next = v;
        v->next = NULL;
    }
}
vertex* buildgraph(const string& filename) //making graph by laoding file
{
    voyage* list = loadvoyagesfromfile(filename);
    vertex* graph = NULL;
    voyage* curr = list;
    while (curr != NULL)
    {
        voyage* next = curr->next;
        curr->next = NULL;
        addtograph(graph, curr);
        curr = next;
    }
    return graph;
}
vertex** intializegraphindex(vertex* graph, int& v) //intializing vertex index
{
    v = 0;
    for (vertex* g = graph; g != NULL; g = g->next)
    {
        g->index = v++;
    }
    vertex** portarray = new vertex * [v];
    int i = 0;
    for (vertex* g = graph; g != NULL; g = g->next)
    {
        portarray[i++] = g;
    }
    return portarray;
}
void printGraph(vertex* graph) //printing graph on console
{
    vertex* v = graph;
    while (v != NULL)
    {
        cout << "Port: " << v->portname;
        if (v->portcharge > 0)
        {
            cout << " | Port Charge: " << v->portcharge;
        }
        cout << endl;
        edge* e = v->edges;
        while (e != NULL)
        {
            cout << "  Route to: " << e->destination << endl;
            voyage* voyageNode = e->voyages;
            while (voyageNode != NULL)
            {
                cout << "    " << voyageNode->company
                    << " | Date: " << voyageNode->date
                    << " | Depart: " << voyageNode->departtime
                    << " | Arrive: " << voyageNode->arrivaltime
                    << " | Cost: " << voyageNode->voyagecost
                    << endl;
                voyageNode = voyageNode->next;
            }
            e = e->next;
        }
        v = v->next;
        cout << endl;
    }
}
void showvertexdetail(sf::RenderWindow& window, vertex* v) //displaying details format
{
    sf::Texture texture;
    if (!texture.loadFromFile("vertexdetail.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float startY = 100.f;
    float lineSpacing = 60.f;


    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setString("    " + v->portname);
    text.setPosition(centerX - 100.f, centerY - 300.f);

    sf::RectangleShape rectangle;
    rectangle.setSize({ 200.f, 50.f });
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setOutlineThickness(2.f);
    rectangle.setOutlineColor(sf::Color::Black);
    rectangle.setPosition(centerX - 100.f, centerY - 300.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 390.f, centerY + 270.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 390.f, centerY + 270.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                {
                    return;
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.draw(rectangle);
        window.draw(text);
        window.draw(backrectangle);
        window.draw(back);
        edge* e = v->edges;
        float currentY = startY + lineSpacing; // start below the port name
        while (e != NULL)
        {
            // Destination name
            sf::Text destText;
            destText.setFont(font);
            destText.setCharacterSize(28);
            destText.setFillColor(sf::Color::White);
            destText.setString("Destination: " + e->destination);

            sf::FloatRect destRect = destText.getLocalBounds();
            destText.setOrigin(destRect.left + destRect.width / 2.f, destRect.top + destRect.height / 2.f);
            destText.setPosition(centerX, currentY);

            window.draw(destText);
            voyage* currVoyage = e->voyages;
            float voyageY = currentY + 30.f;
            while (currVoyage != NULL)
            {
                sf::Text voyageText;
                voyageText.setFont(font);
                voyageText.setCharacterSize(22);
                voyageText.setFillColor(sf::Color::Yellow);

                voyageText.setString(currVoyage->company + " | " + currVoyage->date +
                    " | Depart: " + currVoyage->departtime +
                    " | Arrive: " + currVoyage->arrivaltime +
                    " | Cost: $" + to_string((long long)(currVoyage->voyagecost)));

                sf::FloatRect vRect = voyageText.getLocalBounds();
                voyageText.setOrigin(vRect.left + vRect.width / 2.f, vRect.top + vRect.height / 2.f);
                voyageText.setPosition(centerX, voyageY);

                window.draw(voyageText);

                voyageY += 25.f;
                currVoyage = currVoyage->next;
            }

            currentY += lineSpacing + 50.f;
            e = e->next;
        }

        window.display();
    }
}
void showvertexgraph(sf::RenderWindow& window, vertex* v) //displaying graph formart
{
    sf::Texture texture;
    if (!texture.loadFromFile("vertexdetail.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float destSpacingY = 80.f;


    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setString("    " + v->portname);
    text.setPosition(centerX - 100.f, centerY - 300.f);

    sf::RectangleShape rectangle;
    rectangle.setSize({ 200.f, 50.f });
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setOutlineThickness(2.f);
    rectangle.setOutlineColor(sf::Color::Black);
    rectangle.setPosition(centerX - 100.f, centerY - 300.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 350.f, centerY + 270.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 350.f, centerY + 270.f);

    int edgeCount = 0;
    edge* tempEdge = v->edges;
    while (tempEdge != nullptr)
    {
        edgeCount++;
        tempEdge = tempEdge->next;
    }

    // Create arrays for text and arrows
    sf::Text* destText = new sf::Text[edgeCount];
    sf::VertexArray* arrows = new sf::VertexArray[edgeCount];

    tempEdge = v->edges;
    int index = 0;
    while (tempEdge != nullptr)
    {

        destText[index].setFont(font);
        destText[index].setFillColor(sf::Color::White);
        destText[index].setString(tempEdge->destination);
        destText[index].setPosition(centerX + 200.f, centerY - 300.f + index * destSpacingY);


        arrows[index] = sf::VertexArray(sf::Lines, 2);
        arrows[index][0].position = sf::Vector2f(centerX, centerY - 275.f);
        arrows[index][0].color = sf::Color::Red;
        arrows[index][1].position = sf::Vector2f(centerX + 200.f, centerY - 275.f + index * destSpacingY);
        arrows[index][1].color = sf::Color::Red;

        index++;
        tempEdge = tempEdge->next;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                {
                    return;
                }
            }
        }

        window.clear();
        window.draw(sprite);
        window.draw(rectangle);
        window.draw(text);
        window.draw(backrectangle);
        window.draw(back);

        for (int i = 0; i < edgeCount; i++)
        {
            window.draw(arrows[i]);
            window.draw(destText[i]);
        }

        window.display();
    }

    delete[] destText;
    delete[] arrows;

}
void selectoption(sf::RenderWindow& window, vertex* v) //option between displaying details or graph
{
    sf::Texture texture;
    if (!texture.loadFromFile("select.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    float lineSpacing = 100.f;

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setString("Select Option");
    text.setPosition(centerX - 100.f, centerY - 300.f);

    sf::Text route;
    route.setFont(font);
    route.setFillColor(sf::Color::White);
    route.setString("Display Route Details");
    float routeWidth = route.getLocalBounds().width;
    float routeHeight = route.getLocalBounds().height;
    route.setPosition(centerX - routeWidth / 2.f, centerY - 40.f);


    sf::RectangleShape routerectangle;
    routerectangle.setSize({ routeWidth + 40.f, routeHeight + 20.f });
    routerectangle.setFillColor(sf::Color::Blue);
    routerectangle.setOutlineThickness(2.f);
    routerectangle.setOutlineColor(sf::Color::Black);
    routerectangle.setPosition(centerX - routerectangle.getSize().x / 2.f, centerY - 40.f);

    sf::Text graph;
    graph.setFont(font);
    graph.setFillColor(sf::Color::White);
    graph.setString("Display Graph");
    float graphWidth = graph.getLocalBounds().width;
    float graphHeight = graph.getLocalBounds().height;
    graph.setPosition(centerX - graphWidth / 2.f, centerY + lineSpacing);

    sf::RectangleShape graphrectangle;
    graphrectangle.setSize({ graphWidth + 40.f, graphHeight + 20.f });
    graphrectangle.setFillColor(sf::Color::Blue);
    graphrectangle.setOutlineThickness(2.f);
    graphrectangle.setOutlineColor(sf::Color::Black);
    graphrectangle.setPosition(centerX - graphrectangle.getSize().x / 2.f, centerY + lineSpacing - 5.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 300.f, centerY + 200.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 200.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (routerectangle.getGlobalBounds().contains(mousePosF))
                {
                    showvertexdetail(window, v); // Call your vertex detail function
                }
                if (graphrectangle.getGlobalBounds().contains(mousePosF))
                {
                    showvertexgraph(window, v);
                }
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                {
                    return;
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.draw(text);
        window.draw(routerectangle);
        window.draw(route);
        window.draw(graphrectangle);
        window.draw(graph);
        window.draw(backrectangle);
        window.draw(back);
        window.display();
    }

}
void sfmlportdisplay(sf::RenderWindow& window, vertex* graph) //asks for source vertex 
{
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;
    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setString("Select City to Travel from");

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top); // center horizontally, top vertically
    text.setPosition(window.getSize().x / 2.f, 50.f);

    sf::Texture texture;
    if (!texture.loadFromFile("options.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::Text prompt;
    prompt.setFont(font);
    prompt.setFillColor(sf::Color::White);
    prompt.setString("  Enter City:");
    sf::Text userinput;
    userinput.setFont(font);
    userinput.setFillColor(sf::Color::White);


    sf::RectangleShape rectangle;
    rectangle.setSize({ 190.f, 50.f });
    rectangle.setFillColor(sf::Color::Blue);
    rectangle.setOutlineThickness(2.f);
    rectangle.setOutlineColor(sf::Color::Black);

    sf::RectangleShape belowrectangle;
    belowrectangle.setSize({ 190.f, 50.f });
    belowrectangle.setFillColor(sf::Color::Blue);
    belowrectangle.setOutlineThickness(2.f);
    belowrectangle.setOutlineColor(sf::Color::Black);

    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    sf::Vector2f position((windowWidth - 190.f) / 2.f, (windowHeight - 50.f) / 2.f);
    float spacing = 10.0f; // small gap
    sf::Vector2f belowposition
    (
        position.x,
        position.y + prompt.getCharacterSize() + spacing + 30.f);

    rectangle.setPosition(position);
    prompt.setPosition(position);
    userinput.setPosition(belowposition);
    belowrectangle.setPosition(belowposition);
    string typedinput = "";

    float centerX = window.getSize().x / 2.f;
    float centerY = window.getSize().y / 2.f;
    sf::Text message;
    message.setFont(font);
    message.setCharacterSize(22);
    message.setFillColor(sf::Color::Red);
    message.setPosition(centerX - 150.f, centerY + 100.f);

    sf::Text back;
    back.setFont(font);
    back.setFillColor(sf::Color::White);
    back.setString("  Go back");
    float backWidth = back.getLocalBounds().width;
    float backHeight = back.getLocalBounds().height;
    back.setPosition(centerX + 300.f, centerY + 200.f);

    sf::RectangleShape backrectangle;
    backrectangle.setSize({ backWidth + 40.f, backHeight + 20.f });
    backrectangle.setFillColor(sf::Color::Blue);
    backrectangle.setOutlineThickness(2.f);
    backrectangle.setOutlineColor(sf::Color::Black);
    backrectangle.setPosition(centerX + 300.f, centerY + 200.f);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered)
            {
                char ch = static_cast<char>(event.text.unicode);
                if (event.text.unicode < 128) // ASCII only
                {
                    if (ch == '\b')
                    {
                        if (!typedinput.empty())
                            typedinput.pop_back();
                    }
                    else if (ch == '\r') // Enter
                    {
                        bool found = false;
                        vertex* temp = graph;
                        vertex* selected = NULL;
                        while (temp != NULL)
                        {
                            if (tolower(temp->portname) == tolower(typedinput))
                            {
                                found = true;
                                selected = temp;
                                break;
                            }
                            temp = temp->next;
                        }
                        if (found)
                        {
                            message.setFillColor(sf::Color::Green);
                            message.setString("City Found! Loading Routes...");
                            window.draw(message);
                            window.display();
                            sf::sleep(sf::seconds(1));
                            selectoption(window, selected);
                            //showvertexdetail(window, selected);
                            //return;
                        }
                        else
                        {
                            message.setFillColor(sf::Color::Red);
                            message.setString("City NOT found. Try again.");
                        }
                        typedinput = "";
                    }
                    else if (isprint(ch))
                    {
                        typedinput += ch;
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (backrectangle.getGlobalBounds().contains(mousePosF))
                {
                    return;
                }
            }
        }
        userinput.setString(typedinput);

        window.clear();
        window.draw(sprite);
        window.draw(rectangle);
        window.draw(prompt);
        window.draw(belowrectangle);
        window.draw(userinput);
        window.draw(text);
        window.draw(message);
        window.draw(backrectangle);
        window.draw(back);
        window.display();
    }
}


struct ShipRouteBookingNode {

    string origin;
    string destination;
    string date;
    string departureTime;
    string arrivalTime;
    string shippingCompany;
    double cost;

    ShipRouteBookingNode* next;

    ShipRouteBookingNode(string o, string d, string dt, string depT, string arrT, string sc, double c) {
        origin = o;
        destination = d;
        date = dt;
        departureTime = depT;
        arrivalTime = arrT;
        shippingCompany = sc;
        cost = c;
        next = nullptr;
    }
};


struct ShipRouteBooking {

    ShipRouteBookingNode* head;

    ShipRouteBooking() {
        head = nullptr;
    }

    void BookVoyage(vertex* graph, const string& origin, const string& destination, const string& date, UserPreferences* pref, string& errormessage, bool& success) {
        vertex* v = graph;
        bool voyageFound = false;
        success = false;
        errormessage = "";

        if (pref->isActive) {

            if (isPortAvoided(pref, origin)) {
                errormessage = "Booking failed: Origin port " + origin + " is in the avoided ports list.";
                return;
            }
            if (isPortAvoided(pref, destination)) {
                errormessage = "Booking failed: Destination port " + destination + " is in the avoided ports list.";
                return;
            }
        }

        while (v != NULL) {
            if (v->portname == origin) {   // checks origin (Adj list)
                edge* e = v->edges;
                while (e != NULL) {
                    if (e->destination == destination) {    // checks destination (Adj Node)
                        voyage* voyageNode = e->voyages;

                        // Check all voyages for this route to find one matching the date
                        while (voyageNode != NULL) {
                            if (voyageNode->date == date) {
                                // Create new booking node
                                ShipRouteBookingNode* newBooking = new ShipRouteBookingNode(
                                    voyageNode->source,
                                    voyageNode->destination,
                                    voyageNode->date,
                                    voyageNode->departtime,
                                    voyageNode->arrivaltime,
                                    voyageNode->company,
                                    voyageNode->voyagecost
                                );

                                // Add to linked list
                                if (head == nullptr) {
                                    head = newBooking;
                                }
                                else {
                                    ShipRouteBookingNode* temp = head;
                                    while (temp->next != nullptr) {
                                        temp = temp->next;
                                    }
                                    temp->next = newBooking;
                                }

                                int processingTime = 5; // Increased to 5 seconds for better visibility

                                // Add ship to ORIGIN port's departing queue
                                if (v->docklayover != nullptr) {
                                    v->docklayover->addShip(voyageNode->source, voyageNode->company,
                                        voyageNode->departtime, processingTime);
                                    cout << "Ship enqueued to " << origin << " departure queue." << endl;
                                }

                                // Add ship to DESTINATION port's arriving queue
                                vertex* destVertex = graph;
                                while (destVertex != NULL) {
                                    if (destVertex->portname == destination) {
                                        if (destVertex->docklayover != nullptr) {
                                            destVertex->docklayover->AddArrivingShip(
                                                voyageNode->destination + " Inbound",
                                                voyageNode->company,
                                                voyageNode->arrivaltime,
                                                processingTime
                                            );
                                            cout << "Ship enqueued to " << destination << " arrival queue." << endl;
                                        }
                                        break;
                                    }
                                    destVertex = destVertex->next;
                                }

                                cout << "Voyage booked successfully!" << endl;
                                cout << "Route: " << origin << " -> " << destination << endl;
                                cout << "Date: " << date << endl;
                                cout << "Company: " << voyageNode->company << endl;
                                cout << "Cost: $" << voyageNode->voyagecost << endl;

                                success = true;
                                errormessage = "Booking successful for " + origin + " -> " + destination + " on " + date;

                                voyageFound = true;
                                break; // Exit after booking
                            }
                            voyageNode = voyageNode->next;
                        }
                    }
                    e = e->next;
                }
            }
            v = v->next;
        }

        if (!voyageFound) {
            cout << "No matching voyage found for " << origin << " -> " << destination
                << " on date " << date << endl;
        }
    }

    ~ShipRouteBooking() {
        ShipRouteBookingNode* current = head;
        while (current != nullptr) {
            ShipRouteBookingNode* next = current->next;
            delete current;
            current = next;
        }
    }
};

struct PortPosition {  //ports position node
    string portName;
    float x, y;
    sf::Color color;
    PortPosition* next;

    PortPosition(string name, float xPos, float yPos) {
        portName = name;
        x = xPos;
        y = yPos;
        color = sf::Color::White;
        next = nullptr;
    }
};

// Helper Func to get the details of the selected voyage
voyage* findVoyageDetails(vertex* graph, const string& origin, const string& destination, const string& date) {
    vertex* v = graph;
    while (v != nullptr) {
        if (v->portname == origin) {
            edge* e = v->edges;
            while (e != nullptr) {
                if (e->destination == destination) {
                    voyage* voyageNode = e->voyages;
                    while (voyageNode != nullptr) {
                        if (voyageNode->date == date) {
                            return voyageNode;
                        }
                        voyageNode = voyageNode->next;
                    }
                }
                e = e->next;
            }
        }
        v = v->next;
    }
    return nullptr;
}

class RouteVisualizer {  // graphical implementation of the task 2
private:
    PortPosition* portPositions;

public:
    RouteVisualizer() {
        portPositions = nullptr;
        initializePortPositions();
    }

    void initializePortPositions() {
        // Clear existing positions
        PortPosition* temp = portPositions;
        while (temp != nullptr) {
            PortPosition* next = temp->next;
            delete temp;
            temp = next;
        }
        portPositions = nullptr;

        addPortPosition("Karachi", 750, 350);      // Pakistan
        addPortPosition("Mumbai", 780, 400);       // West India
        addPortPosition("Colombo", 820, 450);      // Sri Lanka
        addPortPosition("Chittagong", 850, 350);   // Bangladesh
        addPortPosition("Singapore", 870, 470);    // Southeast Asia
        addPortPosition("Jakarta", 890, 520);      // Indonesia
        addPortPosition("Manila", 950, 430);       // Philippines
        addPortPosition("HongKong", 920, 380);     // Southern China
        addPortPosition("Shanghai", 940, 330);     // Eastern China
        addPortPosition("Busan", 980, 320);        // South Korea
        addPortPosition("Osaka", 1000, 340);       // Japan
        addPortPosition("Tokyo", 1020, 320);       // Japan

        // Middle East - Center-right
        addPortPosition("Dubai", 700, 380);        // UAE
        addPortPosition("AbuDhabi", 710, 390);     // UAE
        addPortPosition("Jeddah", 650, 380);       // Saudi Arabia
        addPortPosition("Doha", 690, 370);         // Qatar
        addPortPosition("Istanbul", 600, 300);     // Turkey
        addPortPosition("Alexandria", 550, 330);   // Egypt
        addPortPosition("Athens", 580, 320);       // Greece

        // Europe - Left-center
        addPortPosition("London", 450, 250);       // UK
        addPortPosition("Rotterdam", 500, 260);    // Netherlands
        addPortPosition("Hamburg", 510, 240);      // Germany
        addPortPosition("Copenhagen", 520, 220);   // Denmark
        addPortPosition("Stockholm", 530, 200);    // Sweden
        addPortPosition("Oslo", 500, 200);         // Norway
        addPortPosition("Helsinki", 550, 180);     // Finland
        addPortPosition("Genoa", 520, 320);        // Italy
        addPortPosition("Marseille", 480, 320);    // France
        addPortPosition("Lisbon", 400, 340);       // Portugal
        addPortPosition("Antwerp", 490, 270);      // Belgium
        addPortPosition("Dublin", 430, 240);       // Ireland

        // Africa - Bottom-center
        addPortPosition("CapeTown", 580, 600);     // South Africa
        addPortPosition("Durban", 620, 550);       // South Africa
        addPortPosition("PortLouis", 750, 550);    // Mauritius

        // Australia - Far right bottom
        addPortPosition("Sydney", 1100, 550);      // East Australia
        addPortPosition("Melbourne", 1080, 570);   // Southeast Australia

        // North America - Far left
        addPortPosition("Vancouver", 200, 250);    // West Canada
        addPortPosition("LosAngeles", 180, 350);   // West USA
        addPortPosition("Montreal", 350, 240);     // East Canada
        addPortPosition("NewYork", 320, 300);      // East USA
    }

    void addPortPosition(string name, float x, float y) {
        PortPosition* newPort = new PortPosition(name, x, y);
        if (portPositions == nullptr) {
            portPositions = newPort;
        }
        else {
            PortPosition* temp = portPositions;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newPort;
        }
    }

    PortPosition* getPortPosition(const string& portName) {
        PortPosition* temp = portPositions;
        while (temp != nullptr) {
            if (temp->portName == portName) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }

    void drawPorts(sf::RenderWindow& window) {
        PortPosition* port = portPositions;
        sf::Font font;
        if (!font.loadFromFile("font.ttf")) return;

        while (port != nullptr) {
            // Draw port circle
            sf::CircleShape circle(20);
            circle.setFillColor(port->color);
            circle.setOutlineThickness(3);
            circle.setOutlineColor(sf::Color::Black);
            circle.setPosition(port->x - 20, port->y - 20);
            window.draw(circle);

            // Draw port name
            sf::Text nameText(port->portName, font, 14);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(port->x - 40, port->y + 25);
            window.draw(nameText);

            port = port->next;
        }
    }

    void drawAllRoutes(vertex* graph, sf::RenderWindow& window) {
        // Draw all possible routes in light gray (background)
        vertex* v = graph;
        while (v != nullptr) {
            edge* e = v->edges;
            while (e != nullptr) {
                drawRouteLine(window, v->portname, e->destination, sf::Color(100, 100, 100, 100));
                e = e->next;
            }
            v = v->next;
        }
    }

    void highlightSpecificRoute(sf::RenderWindow& window, const string& origin, const string& destination, sf::Color color) {

        resetPortColors();

        drawRouteLine(window, origin, destination, color);

        // Highlight the ports
        PortPosition* originPos = getPortPosition(origin);
        PortPosition* destPos = getPortPosition(destination);

        if (originPos != nullptr) originPos->color = sf::Color::Red;
        if (destPos != nullptr) destPos->color = sf::Color::Green;
    }

    void drawRouteLine(sf::RenderWindow& window, const string& portA, const string& portB, sf::Color color) {
        PortPosition* posA = getPortPosition(portA);
        PortPosition* posB = getPortPosition(portB);

        if (posA == nullptr || posB == nullptr) return;

        // Make the line thicker for better visibility
        sf::RectangleShape lineShape;
        float dx = posB->x - posA->x;
        float dy = posB->y - posA->y;
        float length = sqrt(dx * dx + dy * dy);
        float angle = atan2(dy, dx) * 180 / 3.14159f;

        lineShape.setSize(sf::Vector2f(length, 5)); // Thicker line
        lineShape.setFillColor(color);
        lineShape.setPosition(posA->x, posA->y);
        lineShape.setRotation(angle);

        window.draw(lineShape);
    }

    void resetPortColors() {
        PortPosition* port = portPositions;
        while (port != nullptr) {
            port->color = sf::Color::White;
            port = port->next;
        }
    }

    ~RouteVisualizer() {
        PortPosition* port = portPositions;
        while (port != nullptr) {
            PortPosition* next = port->next;
            delete port;
            port = next;
        }
    }
};

void ShowSingleRouteVisualization(sf::RenderWindow& window, vertex* graph, RouteVisualizer& visualizer, const string& origin, const string& destination, const string& date);

// Booking Screen func
void ShowBookingScreen(sf::RenderWindow& window, ShipRouteBooking& bookingSystem, vertex* graph, RouteVisualizer& routeVisualizer, UserPreferences* pref)
{
    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::Texture bgTexture;
    bgTexture.loadFromFile("booking.jpg");
    sf::Sprite background(bgTexture);
    background.setScale(
        1280.0f / bgTexture.getSize().x,
        720.0f / bgTexture.getSize().y
    );

    sf::Text title("Book Ship Route", font, 45);
    title.setPosition(430, 30);

    float boxW = 450;
    float boxH = 45;

    // Labels
    sf::Text lblOrigin("Origin Port", font, 22);
    lblOrigin.setFillColor(sf::Color::White);
    lblOrigin.setPosition(300, 110);

    sf::Text lblDestination("Destination Port", font, 22);
    lblDestination.setFillColor(sf::Color::White);
    lblDestination.setPosition(300, 200);

    sf::Text lblDate("Date (DD/MM/YYYY)", font, 22);
    lblDate.setFillColor(sf::Color::White);
    lblDate.setPosition(300, 290);

    // Boxes
    sf::RectangleShape inputOrigin(sf::Vector2f(boxW, boxH));
    inputOrigin.setPosition(300, 140);
    inputOrigin.setFillColor(sf::Color::White);

    sf::RectangleShape inputDestination(sf::Vector2f(boxW, boxH));
    inputDestination.setPosition(300, 230);
    inputDestination.setFillColor(sf::Color::White);

    sf::RectangleShape inputDate(sf::Vector2f(boxW, boxH));
    inputDate.setPosition(300, 320);
    inputDate.setFillColor(sf::Color::White);

    // Text inside the boxes
    sf::Text tOrigin("", font, 24);
    tOrigin.setFillColor(sf::Color::Black);
    tOrigin.setPosition(310, 145);

    sf::Text tDestination("", font, 24);
    tDestination.setFillColor(sf::Color::Black);
    tDestination.setPosition(310, 235);

    sf::Text tDate("", font, 24);
    tDate.setFillColor(sf::Color::Black);
    tDate.setPosition(310, 325);

    // Result message
    sf::Text result("", font, 28);
    result.setFillColor(sf::Color::Green);
    result.setPosition(300, 500);

    // Book Button
    sf::RectangleShape bookBtn(sf::Vector2f(230, 55));
    bookBtn.setPosition(420, 400);
    bookBtn.setFillColor(sf::Color(0, 140, 220));

    sf::Text btnText("Book Voyage", font, 28);
    btnText.setFillColor(sf::Color::White);
    btnText.setPosition(450, 410);

    // BACK Button
    sf::RectangleShape backBtn(sf::Vector2f(140, 45));
    backBtn.setPosition(1100, 650);
    backBtn.setFillColor(sf::Color(200, 40, 40));

    sf::Text backText("Back", font, 24);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(1140, 655);

    // 1- Origin
    // 2- Destination
    // 3- Date
    int activeBox = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i m = sf::Mouse::getPosition(window);

                if (inputOrigin.getGlobalBounds().contains(m.x, m.y))
                    activeBox = 1;
                else if (inputDestination.getGlobalBounds().contains(m.x, m.y))
                    activeBox = 2;
                else if (inputDate.getGlobalBounds().contains(m.x, m.y))
                    activeBox = 3;
                else
                    activeBox = 0;

                if (bookBtn.getGlobalBounds().contains(m.x, m.y)) {
                    if (!tOrigin.getString().isEmpty() &&
                        !tDestination.getString().isEmpty() &&
                        !tDate.getString().isEmpty())
                    {
                        string origin = tOrigin.getString();
                        string destination = tDestination.getString();
                        string date = tDate.getString();


                        string errorMessage = "";
                        bool success = false;

                        bookingSystem.BookVoyage(graph, origin, destination, date, pref, errorMessage, success);

                        if (bookingSystem.head != nullptr) {
                            if (success) {

                                ShowSingleRouteVisualization(window, graph, routeVisualizer, origin, destination, date);

                                result.setString(errorMessage);
                                result.setFillColor(sf::Color::Green);
                            }
                            else {
                                result.setString(errorMessage);
                                result.setFillColor(sf::Color::Red);
                            }

                        }
                        else {
                            // Booking failed
                            result.setString("No voyage found for selected route/date");
                            result.setFillColor(sf::Color::Red);
                        }
                    }
                    else
                    {
                        result.setString("Please fill in all fields!...");
                        result.setFillColor(sf::Color::Red);
                    }
                }

                if (backBtn.getGlobalBounds().contains(m.x, m.y)) {
                    return; // Exit booking screen
                }
            }

            // Typing input
            if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode == 8) {
                    // backspace code 8
                    if (activeBox == 1 && !tOrigin.getString().isEmpty())
                        tOrigin.setString(tOrigin.getString().substring(0, tOrigin.getString().getSize() - 1));

                    if (activeBox == 2 && !tDestination.getString().isEmpty())
                        tDestination.setString(tDestination.getString().substring(0, tDestination.getString().getSize() - 1));

                    if (activeBox == 3 && !tDate.getString().isEmpty())
                        tDate.setString(tDate.getString().substring(0, tDate.getString().getSize() - 1));
                }
                else if (event.text.unicode < 128 && event.text.unicode != 13) {
                    char typed = static_cast<char>(event.text.unicode);  //convert event to chars

                    if (activeBox == 1)
                        tOrigin.setString(tOrigin.getString() + typed);
                    if (activeBox == 2)
                        tDestination.setString(tDestination.getString() + typed);
                    if (activeBox == 3)
                        tDate.setString(tDate.getString() + typed);
                }
            }
        }

        if (pref->isActive) {
            title.setString("Book Ship Route (Preferences ON)");
        }
        else {
            title.setString("Book Ship Route (Preferences OFF)");
        }

        window.clear();
        window.draw(background);
        window.draw(title);

        window.draw(lblOrigin);
        window.draw(inputOrigin);
        window.draw(tOrigin);

        window.draw(lblDestination);
        window.draw(inputDestination);
        window.draw(tDestination);

        window.draw(lblDate);
        window.draw(inputDate);
        window.draw(tDate);

        window.draw(bookBtn);
        window.draw(btnText);

        window.draw(backBtn);
        window.draw(backText);

        window.draw(result);

        window.display();
    }
}


void ShowSingleRouteVisualization(sf::RenderWindow& window, vertex* graph, RouteVisualizer& visualizer,
    const string& origin, const string& destination, const string& date) {

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture bgTexture;
    sf::Sprite background;
    if (!bgTexture.loadFromFile("map_bg.png")) {
        // Create fallback
        bgTexture.create(1200, 700);
        sf::Image fallbackImage;
        fallbackImage.create(1200, 700, sf::Color(0, 20, 40)); // Ocean blue
        bgTexture.loadFromImage(fallbackImage);
    }
    background.setTexture(bgTexture);

    // UI Elements
    sf::Text title("Route Booked Successfully!", font, 35);
    title.setFillColor(sf::Color::White);
    title.setPosition(400, 20);

    sf::Text routeInfo("Route: " + origin + " → " + destination + " on " + date, font, 24);
    routeInfo.setFillColor(sf::Color::Yellow);
    routeInfo.setPosition(100, 80);

    // Voyage details
    voyage* foundVoyage = findVoyageDetails(graph, origin, destination, date);
    sf::Text voyageDetails("", font, 20);
    voyageDetails.setFillColor(sf::Color::Cyan);
    voyageDetails.setPosition(100, 120);

    if (foundVoyage != nullptr) {
        voyageDetails.setString(
            "Company: " + foundVoyage->company +
            " | Depart: " + foundVoyage->departtime +
            " | Arrive: " + foundVoyage->arrivaltime +
            " | Cost: $" + to_string((int)foundVoyage->voyagecost)
        );
    }

    // Buttons
    sf::RectangleShape backBtn(sf::Vector2f(200, 50));
    backBtn.setFillColor(sf::Color(200, 40, 40));
    backBtn.setPosition(500, 600);

    sf::Text backText("Go Back", font, 22);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(550, 610);

    // Show confirmation message
    sf::Text confirmedText("Booking Confirmed!", font, 30);
    confirmedText.setFillColor(sf::Color::Green);
    confirmedText.setPosition(450, 500);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // ONLY back button functionality
                if (backBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }
        }

        window.clear();
        window.draw(background);

        // Draw the specific route
        visualizer.drawAllRoutes(graph, window);
        visualizer.highlightSpecificRoute(window, origin, destination, sf::Color::Green);
        visualizer.drawPorts(window);

        // Draw UI
        window.draw(title);
        window.draw(routeInfo);
        window.draw(voyageDetails);
        window.draw(backBtn);
        window.draw(backText);
        window.draw(confirmedText);

        window.display();
    }
}

void addArrivingShipToDestination(vertex* graph, const string& destination, const string& shipName,
    const string& company, const string& arrivalTime, int processTime) {
    vertex* v = graph;
    while (v != NULL) {
        if (v->portname == destination) {
            if (v->docklayover != nullptr) {
                v->docklayover->AddArrivingShip(shipName, company, arrivalTime, processTime);
            }
            break;
        }
        v = v->next;
    }
}

void dockSfml(sf::RenderWindow& window, vertex* port, bool isOrigin) {

    // isOrigin = true means ship is departing FROM this port
    // isOrigin = false means ship is arriving TO this port

    DockingandLayover* Dock = port->docklayover;
    sf::Font font;
    font.loadFromFile("font.ttf");

    sf::Text title("Dock View: " + port->portname, font, 40);
    title.setPosition(350, 10);

    sf::Texture dockbg;
    dockbg.loadFromFile("dock page.png");
    sf::Sprite background(dockbg);
    background.setScale(
        1200.0f / dockbg.getSize().x,
        700.0f / dockbg.getSize().y
    );

    sf::Texture dockTexture;
    dockTexture.loadFromFile("dock.png");
    sf::Sprite dockSprite(dockTexture);
    dockSprite.setPosition(400, 450);  // Position dock in center-bottom
    dockSprite.setScale(0.5f, 0.5f);

    sf::Texture shipTexture;
    shipTexture.loadFromFile("ship.png");

    sf::Sprite shipSprite(shipTexture);
    sf::Sprite layoverSprite(shipTexture);

    // Animation variables
    Ship* activeShip = NULL;
    bool hasShip = false;

    int stage = 0;  // 0 = move in/out, 1 = processing, 2 = leave/arrive
    float shipX = 450;  // Dock center X
    float shipY = 400;  // Dock Y position
    float shipScale = 0.3f;

    float processTimer = 0;

    // Movement parameters
    float horizonY = 250.0f;      // Top of water (horizon)
    float dockY = 400.0f;        // Dock Y position
    float dockX = 450.0f;        // Dock X position
    float maxScale = 0.3f;       // Scale at dock
    float minScale = 0.05f;      // Scale at horizon
    float moveSpeed = 0.2f;      // Movement speed

    // Back button
    sf::RectangleShape backBtn(sf::Vector2f(150, 50));
    backBtn.setFillColor(sf::Color(200, 40, 40));
    backBtn.setPosition(1020, 630);

    sf::Text backText("Go Back", font, 24);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(1040, 640);

    // Status text
    sf::Text statusText("", font, 20);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(50, 650);

    //sf::SoundBuffer shipdeparture;
    //if (!shipdeparture.loadFromFile("ship_sound.mp3")) {
    //    cout << "Audio Load Failed" << endl;
    //}
 //   sf::Sound shipsound;
    //shipsound.setBuffer(shipdeparture);
 //   shipsound.setVolume(70);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (backBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }

            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                return;
        }

        float dt = clock.restart().asSeconds();

        //       PICK NEW FRONT SHIP

        if (!hasShip) {
            ShipQueue* activeQueue = isOrigin ? Dock->Docking : Dock->Arriving;

            if (!activeQueue->isEmpty()) {
                activeShip = activeQueue->front;
                hasShip = true;
                stage = 0;
                processTimer = activeShip->processingtime;

                if (isOrigin) {
                    // Departure: Start at dock
                    shipX = dockX;
                    shipY = dockY;
                    shipScale = maxScale;
                    statusText.setString("Ship departing: " + activeShip->shipname);
                }
                else {
                    // Arrival: Start at horizon
                    shipX = dockX;
                    shipY = horizonY;
                    shipScale = minScale;
                    statusText.setString("Ship arriving: " + activeShip->shipname);
                }
            }
        }

        //          ANIMATION 
        if (hasShip) {
            if (isOrigin) {
                // ===== DEPARTURE ANIMATION =====
                if (stage == 0) {
                    // Stage 0: Processing at dock
                    processTimer -= dt;
                    statusText.setString("Processing... " + to_string((int)processTimer) + "s remaining");

                    if (processTimer <= 0) {
                        stage = 1;  // Start departure
                    }
                }
                else if (stage == 1) {
                    // Stage 1: Moving away and shrinking

                    shipY -= moveSpeed;  // Move upward

                    // Calculate scale based on distance from dock
                    float progress = (dockY - shipY) / (dockY - horizonY);
                    progress = (progress < 0) ? 0 : (progress > 1) ? 1 : progress;
                    shipScale = maxScale - (progress * (maxScale - minScale));

                    statusText.setString("Ship departing...");

                    // Check if reached horizon
                    if (shipY <= horizonY || shipScale <= minScale) {
                        // Ship departed - remove from queue
                        Dock->Docking->dequeue();
                        hasShip = false;
                        activeShip = NULL;
                        statusText.setString("Ship departed!");
                    }
                }
            }
            else {
                // ===== ARRIVAL ANIMATION =====
                if (stage == 0) {
                    // Stage 0: Moving toward dock and growing

                    shipY += moveSpeed;  // Move downward

                    // Calculate scale based on distance from horizon
                    float progress = (shipY - horizonY) / (dockY - horizonY);
                    progress = (progress < 0) ? 0 : (progress > 1) ? 1 : progress;
                    shipScale = minScale + (progress * (maxScale - minScale));

                    statusText.setString("Ship approaching...");

                    // Check if reached dock
                    if (shipY >= dockY) {
                        shipY = dockY;
                        shipScale = maxScale;
                        stage = 1;  // Start processing
                    }

                }
                else if (stage == 1) {
                    // Stage 1: Processing at dock

                    processTimer -= dt;
                    statusText.setString("Docked - Processing... " + to_string((int)processTimer) + "s remaining");

                    if (processTimer <= 0) {
                        // Processing complete - remove from queue
                        Dock->Docking->dequeue();
                        hasShip = false;
                        activeShip = NULL;
                        statusText.setString("Processing complete!");
                    }
                }
            }

            // Update ship sprite
            if (hasShip) {
                shipSprite.setScale(shipScale, shipScale);

                // Center the ship sprite
                sf::FloatRect bounds = shipSprite.getLocalBounds();
                shipSprite.setOrigin(bounds.width / 2, bounds.height / 2);
                shipSprite.setPosition(shipX, shipY);
            }
        }


        window.clear();
        window.draw(background);
        window.draw(title);
        window.draw(dockSprite);

        if (hasShip) {
            window.draw(shipSprite);
        }

        // Draw layover queue (waiting ships in the water on the right side)
        float layoverStartX = 900;    // Right side of screen
        float layoverStartY = 250;    // In the water area
        float layoverSpacingY = 90;   // Vertical spacing between ships
        float layoverOffsetX = 0;     // Horizontal offset for staggered look

        ShipQueue* displayQueue = isOrigin ? Dock->Docking : Dock->Arriving;
        Ship* temp = displayQueue->front;

        // Skip active ship
        if (hasShip && temp != NULL)
            temp = temp->next;

        int layoverCount = 0;
        while (temp != NULL) {
            // Stagger ships horizontally for more realistic look
            layoverOffsetX = (layoverCount % 2 == 0) ? 0 : 50;

            float currentX = layoverStartX + layoverOffsetX;
            float currentY = layoverStartY + (layoverCount * layoverSpacingY);

            // Make sure ships stay in water area (not in sky)
            if (currentY > 550) break;  // Don't draw if too far down

            layoverSprite.setScale(0.18f, 0.18f);

            sf::FloatRect layBounds = layoverSprite.getLocalBounds();
            layoverSprite.setOrigin(layBounds.width / 2, layBounds.height / 2);
            layoverSprite.setPosition(currentX, currentY);

            window.draw(layoverSprite);

            // Draw ship name with background for better visibility
            sf::Text shipNameText(temp->shipname, font, 13);
            shipNameText.setFillColor(sf::Color::White);
            shipNameText.setStyle(sf::Text::Bold);

            // Add dark background box for text
            sf::RectangleShape nameBg(sf::Vector2f(shipNameText.getLocalBounds().width + 10, 20));
            nameBg.setFillColor(sf::Color(0, 0, 0, 150));
            nameBg.setPosition(currentX - 40, currentY + 25);

            shipNameText.setPosition(currentX - 35, currentY + 27);

            window.draw(nameBg);
            window.draw(shipNameText);

            layoverCount++;
            temp = temp->next;
        }

        // Draw queue info
        string queueType = isOrigin ? "Departing" : "Arriving";
        int queueSize = isOrigin ? Dock->getcurrentQueueSize() : Dock->getArrivingQueueSize();

        sf::Text queueInfo(queueType + " ships in queue: " + to_string(queueSize), font, 18);
        queueInfo.setFillColor(sf::Color::Cyan);
        queueInfo.setPosition(50, 50);
        window.draw(queueInfo);

        // Draw status and back button
        window.draw(statusText);
        window.draw(backBtn);
        window.draw(backText);

        window.display();
    }
}

void AskUserPort(sf::RenderWindow& window, vertex* graph) {
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture texture;
    if (!texture.loadFromFile("options.jpg")) return;
    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::Text title;
    title.setFont(font);
    title.setFillColor(sf::Color::White);
    title.setString("View Port Docking Operations");
    title.setCharacterSize(35);
    sf::FloatRect titleRect = title.getLocalBounds();
    title.setOrigin(titleRect.left + titleRect.width / 2.f, titleRect.top);
    title.setPosition(window.getSize().x / 2.f, 50.f);

    // Port input
    sf::Text prompt;
    prompt.setFont(font);
    prompt.setFillColor(sf::Color::White);
    prompt.setString("Enter Port Name:");

    sf::Text userinput;
    userinput.setFont(font);
    userinput.setFillColor(sf::Color::White);
    userinput.setCharacterSize(24);

    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;
    float centerX = windowWidth / 2.f;
    float centerY = windowHeight / 2.f;

    sf::RectangleShape inputBox;
    inputBox.setSize({ 400.f, 50.f });
    inputBox.setFillColor(sf::Color::Blue);
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setPosition(centerX - 200.f, centerY - 100.f);

    prompt.setPosition(centerX - 100.f, centerY - 150.f);
    userinput.setPosition(centerX - 190.f, centerY - 90.f);

    // Option buttons
    sf::RectangleShape originBtn(sf::Vector2f(200, 50));
    originBtn.setFillColor(sf::Color(0, 120, 0));
    originBtn.setPosition(centerX - 250.f, centerY + 50.f);

    sf::Text originText("View Departures", font, 20);
    originText.setFillColor(sf::Color::White);
    originText.setPosition(centerX - 230.f, centerY + 60.f);

    sf::RectangleShape destBtn(sf::Vector2f(200, 50));
    destBtn.setFillColor(sf::Color(0, 100, 200));
    destBtn.setPosition(centerX + 50.f, centerY + 50.f);

    sf::Text destText("View Arrivals", font, 20);
    destText.setFillColor(sf::Color::White);
    destText.setPosition(centerX + 80.f, centerY + 60.f);

    // Back button
    sf::RectangleShape backBtn(sf::Vector2f(150, 45));
    backBtn.setFillColor(sf::Color(200, 40, 40));
    backBtn.setPosition(centerX + 300.f, centerY + 200.f);

    sf::Text backText("Go Back", font, 20);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(centerX + 330.f, centerY + 210.f);

    // Message text
    sf::Text message;
    message.setFont(font);
    message.setCharacterSize(22);
    message.setFillColor(sf::Color::Red);
    message.setPosition(centerX - 150.f, centerY + 150.f);

    string typedinput = "";
    vertex* selectedPort = NULL;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                char ch = static_cast<char>(event.text.unicode);
                if (event.text.unicode < 128) {
                    if (ch == '\b') {
                        if (!typedinput.empty())
                            typedinput.pop_back();
                    }
                    else if (ch == '\r') {
                        // Enter pressed - search for port
                        vertex* temp = graph;
                        selectedPort = NULL;
                        while (temp != NULL) {
                            if (tolower(temp->portname) == tolower(typedinput)) {
                                selectedPort = temp;
                                break;
                            }
                            temp = temp->next;
                        }

                        if (selectedPort) {
                            message.setFillColor(sf::Color::Green);
                            message.setString("Port found! Select view type above.");
                        }
                        else {
                            message.setFillColor(sf::Color::Red);
                            message.setString("Port NOT found. Try again.");
                        }
                    }
                    else if (isprint(ch)) {
                        typedinput += ch;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                if (backBtn.getGlobalBounds().contains(mousePosF)) {
                    return;
                }

                if (selectedPort != NULL) {
                    if (originBtn.getGlobalBounds().contains(mousePosF)) {
                        dockSfml(window, selectedPort, true);  // View as origin (departures)
                    }
                    else if (destBtn.getGlobalBounds().contains(mousePosF)) {
                        dockSfml(window, selectedPort, false);  // View as destination (arrivals)
                    }
                }
            }
        }

        userinput.setString(typedinput);

        window.clear();
        window.draw(sprite);
        window.draw(title);
        window.draw(inputBox);
        window.draw(prompt);
        window.draw(userinput);

        if (selectedPort != NULL) {
            window.draw(originBtn);
            window.draw(originText);
            window.draw(destBtn);
            window.draw(destText);
        }

        window.draw(message);
        window.draw(backBtn);
        window.draw(backText);
        window.display();
    }
}


// Helper function: Get minimum cost considering preferences
double getmincost_filtered(voyage* v, UserPreferences* prefs) {
    double c = 1e18;
    while (v != NULL) {
        // Skip if voyage is too long
        if (!isVoyageTooLong(prefs, v)) {
            if (v->voyagecost < c) {
                c = v->voyagecost;
            }
        }
        v = v->next;
    }
    return c;
}

// Helper function: Get minimum time considering preferences
int getmintime_filtered(voyage* v, UserPreferences* prefs) {
    int best = 1e9;
    while (v != NULL) {
        // Skip if voyage is too long
        if (!isVoyageTooLong(prefs, v)) {
            int depart = timetominutes(v->departtime);
            int arrive = timetominutes(v->arrivaltime);
            int diff = arrive - depart;
            if (diff < 0) {
                diff += 24 * 60;
            }
            if (diff < best) {
                best = diff;
            }
        }
        v = v->next;
    }
    return best;
}

// Modified Dijkstra for cost with preferences
int* dijkstra_cost_filtered(vertex* graph, vertex** portArray, int v, int start, int goal, UserPreferences* prefs) {
    double* dist = new double[v];
    int* parent = new int[v];
    bool* visited = new bool[v];

    for (int i = 0; i < v; i++) {
        dist[i] = 1e18;
        visited[i] = false;
        parent[i] = -1;
    }

    minheap* heap = createheap(v * 10);
    dist[start] = 0;
    heappush(heap, start, 0);

    while (heap->size > 0) {
        heapnode h = heappop(heap);
        int u = h.index;

        if (visited[u]) {
            continue;
        }
        visited[u] = true;

        if (u == goal) {
            break;
        }

        edge* e = portArray[u]->edges;
        while (e != NULL) {
            // Check if destination port is avoided
            if (isPortAvoided(prefs, e->destination)) {
                e = e->next;
                continue; // Skip this edge
            }

            int destIndex = findIndex(graph, e->destination);
            double cost = getmincost_filtered(e->voyages, prefs);

            // If all voyages were filtered out, skip
            if (cost >= 1e18) {
                e = e->next;
                continue;
            }

            if (dist[u] + cost < dist[destIndex]) {
                dist[destIndex] = dist[u] + cost;
                parent[destIndex] = u;
                heappush(heap, destIndex, dist[destIndex]);
            }

            e = e->next;
        }
    }

    delete[] dist;
    delete[] visited;
    return parent;
}

// Helper function for dijkstra_time_filtered
int getNextArrivalTime_filtered(voyage* v, int currentTime, UserPreferences* prefs) {
    int earliestArrival = 1e9;
    while (v != NULL) {
        // Skip if voyage is too long
        if (!isVoyageTooLong(prefs, v)) {
            int depart = timetominutes(v->departtime);
            int arrive = timetominutes(v->arrivaltime);

            if (depart < currentTime % (24 * 60)) {
                depart += 24 * 60;
                arrive += 24 * 60;
            }

            int travelTime = arrive - depart;
            int arrivalTime = (currentTime > depart ? currentTime : depart) + travelTime;

            if (arrivalTime < earliestArrival) {
                earliestArrival = arrivalTime;
            }
        }
        v = v->next;
    }
    return earliestArrival;
}

// Modified Dijkstra for time with preferences
int* dijkstra_time_filtered(vertex* graph, vertex** portArray, int v, int start, int goal, UserPreferences* prefs) {
    int* dist = new int[v];
    int* parent = new int[v];
    bool* visited = new bool[v];

    for (int i = 0; i < v; i++) {
        dist[i] = 1e9;
        parent[i] = -1;
        visited[i] = false;
    }

    minheap* heap = createheap(v * 10);
    dist[start] = 0;
    heappush(heap, start, 0);

    while (heap->size > 0) {
        heapnode h = heappop(heap);
        int u = h.index;

        if (visited[u]) {
            continue;
        }
        visited[u] = true;

        if (u == goal) {
            break;
        }

        edge* e = portArray[u]->edges;
        while (e != NULL) {
            // Check if destination port is avoided
            if (isPortAvoided(prefs, e->destination)) {
                e = e->next;
                continue;
            }

            int destIndex = findIndex(graph, e->destination);
            int arrivalTime = getNextArrivalTime_filtered(e->voyages, dist[u], prefs);

            // If all voyages were filtered, skip
            if (arrivalTime >= 1e9) {
                e = e->next;
                continue;
            }

            if (arrivalTime < dist[destIndex]) {
                dist[destIndex] = arrivalTime;
                parent[destIndex] = u;
                heappush(heap, destIndex, dist[destIndex]);
            }
            e = e->next;
        }
    }

    delete[] dist;
    delete[] visited;
    return parent;
}

// Function to add avoided port to preferences
void addAvoidedPort(UserPreferences* prefs, const string& portName) {
    string* newArray = new string[prefs->avoidedPortCount + 1];

    // Copy existing ports
    for (int i = 0; i < prefs->avoidedPortCount; i++) {
        newArray[i] = prefs->avoidedPorts[i];
    }

    // Add new port
    newArray[prefs->avoidedPortCount] = portName;

    // Delete old array and update
    if (prefs->avoidedPorts != nullptr) {
        delete[] prefs->avoidedPorts;
    }

    prefs->avoidedPorts = newArray;
    prefs->avoidedPortCount++;
}

// Function to remove avoided port
void removeAvoidedPort(UserPreferences* prefs, const string& portName) {
    int indexToRemove = -1;

    // Find the port
    for (int i = 0; i < prefs->avoidedPortCount; i++) {
        if (tolower(prefs->avoidedPorts[i]) == tolower(portName)) {
            indexToRemove = i;
            break;
        }
    }

    if (indexToRemove == -1) {
        return; // Port not found
    }

    // Create new smaller array
    string* newArray = new string[prefs->avoidedPortCount - 1];
    int newIndex = 0;

    for (int i = 0; i < prefs->avoidedPortCount; i++) {
        if (i != indexToRemove) {
            newArray[newIndex++] = prefs->avoidedPorts[i];
        }
    }

    // Update preferences
    delete[] prefs->avoidedPorts;
    prefs->avoidedPorts = newArray;
    prefs->avoidedPortCount--;
}

// Function to clear all preferences
void clearPreferences(UserPreferences* prefs) {
    if (prefs->avoidedPorts != nullptr) {
        delete[] prefs->avoidedPorts;
    }
    prefs->avoidedPorts = nullptr;
    prefs->avoidedPortCount = 0;
    prefs->maxVoyageTimeMinutes = 0;
    prefs->isActive = false;
}


// SFML Screen for setting preferences with visual graph
void ShowPreferencesScreen(sf::RenderWindow& window, UserPreferences* prefs, vertex* graph) {
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("options.jpg")) {
        bgTexture.create(1200, 700);
    }
    sf::Sprite background(bgTexture);

    sf::Text title("Set Route Preferences", font, 35);
    title.setFillColor(sf::Color::White);
    title.setPosition(450, 20);

    // Left Panel - Input Controls
    float leftPanelX = 50;

    // Avoid Ports Section
    sf::Text avoidLabel("Ports to Avoid:", font, 22);
    avoidLabel.setFillColor(sf::Color::White);
    avoidLabel.setPosition(leftPanelX, 80);

    sf::RectangleShape portInputBox(sf::Vector2f(250, 40));
    portInputBox.setFillColor(sf::Color::White);
    portInputBox.setOutlineThickness(2);
    portInputBox.setOutlineColor(sf::Color::Cyan);
    portInputBox.setPosition(leftPanelX, 115);

    sf::Text portInput("", font, 18);
    portInput.setFillColor(sf::Color::Black);
    portInput.setPosition(leftPanelX + 10, 122);

    sf::RectangleShape addPortBtn(sf::Vector2f(100, 40));
    addPortBtn.setFillColor(sf::Color(0, 150, 0));
    addPortBtn.setPosition(leftPanelX + 260, 115);

    sf::Text addPortText("Add", font, 18);
    addPortText.setFillColor(sf::Color::White);
    addPortText.setPosition(leftPanelX + 290, 122);

    // Max Time Section
    sf::Text timeLabel("Max Time (hours):", font, 22);
    timeLabel.setFillColor(sf::Color::White);
    timeLabel.setPosition(leftPanelX, 320);

    sf::RectangleShape timeInputBox(sf::Vector2f(120, 40));
    timeInputBox.setFillColor(sf::Color::White);
    timeInputBox.setOutlineThickness(2);
    timeInputBox.setOutlineColor(sf::Color::Cyan);
    timeInputBox.setPosition(leftPanelX, 355);

    sf::Text timeInput("", font, 18);
    timeInput.setFillColor(sf::Color::Black);
    timeInput.setPosition(leftPanelX + 10, 362);

    // Buttons
    sf::RectangleShape applyBtn(sf::Vector2f(160, 50));
    applyBtn.setFillColor(sf::Color(0, 120, 220));
    applyBtn.setPosition(leftPanelX, 550);

    sf::Text applyText("Apply", font, 22);
    applyText.setFillColor(sf::Color::White);
    applyText.setPosition(leftPanelX + 50, 560);

    sf::RectangleShape clearBtn(sf::Vector2f(120, 40));
    clearBtn.setFillColor(sf::Color(200, 100, 0));
    clearBtn.setPosition(leftPanelX + 170, 555);

    sf::Text clearText("Clear", font, 18);
    clearText.setFillColor(sf::Color::White);
    clearText.setPosition(leftPanelX + 200, 562);

    sf::RectangleShape backBtn(sf::Vector2f(100, 40));
    backBtn.setFillColor(sf::Color(200, 40, 40));
    backBtn.setPosition(leftPanelX, 620);

    sf::Text backText("Back", font, 18);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(leftPanelX + 25, 627);

    // Status message
    sf::Text statusMsg("", font, 16);
    statusMsg.setFillColor(sf::Color::Green);
    statusMsg.setPosition(leftPanelX, 485);

    // Right Panel - Visual Graph Preview
    float graphPanelX = 450;
    float graphPanelY = 80;
    float graphWidth = 700;
    float graphHeight = 580;

    sf::RectangleShape graphPanel(sf::Vector2f(graphWidth, graphHeight));
    graphPanel.setFillColor(sf::Color(20, 30, 50, 200));
    graphPanel.setOutlineThickness(3);
    graphPanel.setOutlineColor(sf::Color::Cyan);
    graphPanel.setPosition(graphPanelX, graphPanelY);

    sf::Text graphTitle("Visual Preview - Port Status", font, 24);
    graphTitle.setFillColor(sf::Color::Yellow);
    graphTitle.setPosition(graphPanelX + 180, graphPanelY + 10);

    // Legend
    sf::CircleShape legendNormal(8);
    legendNormal.setFillColor(sf::Color::Green);
    legendNormal.setPosition(graphPanelX + 20, graphPanelY + 50);

    sf::Text legendNormalText("Available Port", font, 13);
    legendNormalText.setFillColor(sf::Color::White);
    legendNormalText.setPosition(graphPanelX + 40, graphPanelY + 47);

    sf::CircleShape legendAvoided(8);
    legendAvoided.setFillColor(sf::Color::Red);
    legendAvoided.setPosition(graphPanelX + 160, graphPanelY + 50);

    sf::Text legendAvoidedText("Avoided Port", font, 13);
    legendAvoidedText.setFillColor(sf::Color::White);
    legendAvoidedText.setPosition(graphPanelX + 180, graphPanelY + 47);

    // Green line indicator
    sf::RectangleShape legendGreenLine(sf::Vector2f(20, 3));
    legendGreenLine.setFillColor(sf::Color::Green);
    legendGreenLine.setPosition(graphPanelX + 290, graphPanelY + 56);

    sf::Text legendGreenText("Valid Route", font, 13);
    legendGreenText.setFillColor(sf::Color::White);
    legendGreenText.setPosition(graphPanelX + 315, graphPanelY + 47);

    // Red line indicator  
    sf::RectangleShape legendRedLine(sf::Vector2f(20, 3));
    legendRedLine.setFillColor(sf::Color::Red);
    legendRedLine.setPosition(graphPanelX + 420, graphPanelY + 56);

    sf::Text legendRedText("Blocked Route", font, 13);
    legendRedText.setFillColor(sf::Color::White);
    legendRedText.setPosition(graphPanelX + 445, graphPanelY + 47);

    sf::Text legendSubtext("(Port avoided or time > limit)", font, 11);
    legendSubtext.setFillColor(sf::Color(200, 200, 200));
    legendSubtext.setPosition(graphPanelX + 445, graphPanelY + 62);

    string portInputStr = "";
    string timeInputStr = "";
    int activeField = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (portInputBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    activeField = 1;
                }
                else if (timeInputBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    activeField = 2;
                }
                else {
                    activeField = 0;
                }

                if (addPortBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!portInputStr.empty()) {
                        // Verify port exists
                        vertex* v = graph;
                        bool found = false;
                        while (v != NULL) {
                            if (tolower(v->portname) == tolower(portInputStr)) {
                                found = true;
                                break;
                            }
                            v = v->next;
                        }

                        if (found) {
                            addAvoidedPort(prefs, portInputStr);
                            statusMsg.setString("Port '" + portInputStr + "' added!");
                            statusMsg.setFillColor(sf::Color::Green);
                            portInputStr = "";
                        }
                        else {
                            statusMsg.setString("Port not found in system!");
                            statusMsg.setFillColor(sf::Color::Red);
                        }
                    }
                }

                if (clearBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    clearPreferences(prefs);
                    portInputStr = "";
                    timeInputStr = "";
                    statusMsg.setString("All preferences cleared!");
                    statusMsg.setFillColor(sf::Color::Yellow);
                }

                if (applyBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (!timeInputStr.empty()) {
                        int hours = 0;
                        for (int i = 0; i < timeInputStr.length(); i++) {
                            if (timeInputStr[i] >= '0' && timeInputStr[i] <= '9') {
                                hours = hours * 10 + (timeInputStr[i] - '0');
                            }
                        }
                        prefs->maxVoyageTimeMinutes = hours * 60;
                    }
                    prefs->isActive = true;
                    statusMsg.setString("Preferences saved & active!");
                    statusMsg.setFillColor(sf::Color::Green);
                }

                if (backBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    return;
                }
            }

            if (event.type == sf::Event::TextEntered) {
                char ch = static_cast<char>(event.text.unicode);

                if (ch == '\b') {
                    if (activeField == 1 && !portInputStr.empty()) {
                        portInputStr.pop_back();
                    }
                    else if (activeField == 2 && !timeInputStr.empty()) {
                        timeInputStr.pop_back();
                    }
                }
                else if (ch >= 32 && ch < 127) {
                    if (activeField == 1) {
                        portInputStr += ch;
                    }
                    else if (activeField == 2 && ch >= '0' && ch <= '9') {
                        timeInputStr += ch;
                    }
                }
            }
        }

        portInput.setString(portInputStr);
        timeInput.setString(timeInputStr);

        window.clear();
        window.draw(background);
        window.draw(title);

        // Left Panel
        window.draw(avoidLabel);
        window.draw(portInputBox);
        window.draw(portInput);
        window.draw(addPortBtn);
        window.draw(addPortText);

        // Display avoided ports list
        sf::Text avoidedListLabel("Currently Avoided:", font, 18);
        avoidedListLabel.setFillColor(sf::Color::Cyan);
        avoidedListLabel.setPosition(leftPanelX, 170);
        window.draw(avoidedListLabel);

        int maxDisplay = 5;
        for (int i = 0; i < prefs->avoidedPortCount && i < maxDisplay; i++) {
            sf::RectangleShape portBox(sf::Vector2f(250, 25));
            portBox.setFillColor(sf::Color(100, 0, 0, 180));
            portBox.setPosition(leftPanelX, 200 + i * 30);
            window.draw(portBox);

            sf::Text portName("✖ " + prefs->avoidedPorts[i], font, 16);
            portName.setFillColor(sf::Color::White);
            portName.setPosition(leftPanelX + 10, 202 + i * 30);
            window.draw(portName);
        }

        window.draw(timeLabel);
        window.draw(timeInputBox);
        window.draw(timeInput);

        if (prefs->maxVoyageTimeMinutes > 0) {
            sf::Text currentTime("Limit: " + to_string(prefs->maxVoyageTimeMinutes / 60) + "h", font, 16);
            currentTime.setFillColor(sf::Color::Yellow);
            currentTime.setPosition(leftPanelX + 130, 362);
            window.draw(currentTime);
        }

        window.draw(applyBtn);
        window.draw(applyText);
        window.draw(clearBtn);
        window.draw(clearText);
        window.draw(backBtn);
        window.draw(backText);
        window.draw(statusMsg);

        // Right Panel - Visual Graph
        window.draw(graphPanel);
        window.draw(graphTitle);
        window.draw(legendNormal);
        window.draw(legendNormalText);
        window.draw(legendAvoided);
        window.draw(legendAvoidedText);
        window.draw(legendGreenLine);
        window.draw(legendGreenText);
        window.draw(legendRedLine);
        window.draw(legendRedText);
        window.draw(legendSubtext);

        // Draw sample ports on visual graph
        float portStartX = graphPanelX + 50;
        float portStartY = graphPanelY + 120;
        float portSpacingX = 95;
        float portSpacingY = 70;

        vertex* v = graph;
        int portCount = 0;

        // First pass: Draw all route lines (so they appear behind the circles)
        vertex* lineVertex = graph;
        int linePortCount = 0;
        while (lineVertex != NULL) {
            int row = linePortCount / 7;  // 7 ports per row
            int col = linePortCount % 7;

            float x = portStartX + col * portSpacingX;
            float y = portStartY + row * portSpacingY;

            bool isAvoided = isPortAvoided(prefs, lineVertex->portname);

            // Draw connections to show filtered routes
            if (!isAvoided) {
                edge* e = lineVertex->edges;

                while (e != NULL) {
                    // Check if destination is avoided
                    bool destAvoided = isPortAvoided(prefs, e->destination);

                    // Check if any voyage on this route is within time limit
                    bool hasValidVoyage = false;
                    if (prefs->isActive && prefs->maxVoyageTimeMinutes > 0) {
                        voyage* v = e->voyages;
                        while (v != NULL) {
                            if (!isVoyageTooLong(prefs, v)) {
                                hasValidVoyage = true;
                                break;
                            }
                            v = v->next;
                        }
                    }
                    else {
                        hasValidVoyage = true;  // No time limit set
                    }

                    // Find destination position
                    vertex* temp = graph;
                    int destPortCount = 0;
                    while (temp != NULL) {
                        if (temp->portname == e->destination) {
                            int destRow = destPortCount / 7;
                            int destCol = destPortCount % 7;
                            float destX = portStartX + destCol * portSpacingX;
                            float destY = portStartY + destRow * portSpacingY;

                            // Determine line color based on filters
                            sf::Color lineColor;
                            if (destAvoided || !hasValidVoyage) {
                                lineColor = sf::Color(255, 0, 0, 60);  // Red - blocked
                            }
                            else {
                                lineColor = sf::Color(0, 255, 0, 50);  // Green - available
                            }

                            // Draw line
                            sf::Vertex line[] = {
                                sf::Vertex(sf::Vector2f(x, y), lineColor),
                                sf::Vertex(sf::Vector2f(destX, destY), lineColor)
                            };
                            window.draw(line, 2, sf::Lines);
                            break;
                        }
                        temp = temp->next;
                        destPortCount++;
                    }

                    e = e->next;
                }
            }

            lineVertex = lineVertex->next;
            linePortCount++;
        }

        // Second pass: Draw all port circles and names (on top of lines)
        v = graph;
        portCount = 0;
        while (v != NULL) {
            int row = portCount / 7;  // 7 ports per row for better fit
            int col = portCount % 7;

            float x = portStartX + col * portSpacingX;
            float y = portStartY + row * portSpacingY;

            // Stop if we exceed panel height
            if (y > graphPanelY + graphHeight - 50) {
                break;
            }

            bool isAvoided = isPortAvoided(prefs, v->portname);

            // Draw port circle
            sf::CircleShape portCircle(10);
            if (isAvoided) {
                portCircle.setFillColor(sf::Color::Red);
                portCircle.setOutlineThickness(3);
                portCircle.setOutlineColor(sf::Color(255, 0, 0, 150));
            }
            else {
                portCircle.setFillColor(sf::Color::Green);
                portCircle.setOutlineThickness(2);
                portCircle.setOutlineColor(sf::Color::White);
            }
            portCircle.setPosition(x - 10, y - 10);
            window.draw(portCircle);

            // Draw X over avoided ports
            if (isAvoided) {
                sf::RectangleShape xLine1(sf::Vector2f(22, 3));
                xLine1.setFillColor(sf::Color::White);
                xLine1.setOrigin(11.f, 1.5f);
                xLine1.setPosition(x, y);
                xLine1.setRotation(45);
                window.draw(xLine1);

                sf::RectangleShape xLine2(sf::Vector2f(22, 3));
                xLine2.setFillColor(sf::Color::White);
                xLine2.setOrigin(11.f, 1.5f);
                xLine2.setPosition(x, y);
                xLine2.setRotation(-45);
                window.draw(xLine2);
            }

            // Draw port name (smaller font to fit more)
            sf::Text portNameText(v->portname, font, 8);
            portNameText.setFillColor(sf::Color::White);
            portNameText.setStyle(sf::Text::Bold);
            sf::FloatRect textBounds = portNameText.getLocalBounds();
            portNameText.setOrigin(textBounds.width / 2, 0);
            portNameText.setPosition(x, y + 12);
            window.draw(portNameText);

            v = v->next;
            portCount++;
        }

        // Active status indicator
        if (prefs->isActive) {
            sf::CircleShape activeIndicator(10);
            activeIndicator.setFillColor(sf::Color::Green);
            activeIndicator.setPosition(leftPanelX + 135, 558);
            window.draw(activeIndicator);

            sf::Text activeText("ACTIVE", font, 16);
            activeText.setFillColor(sf::Color::Green);
            activeText.setPosition(leftPanelX + 155, 555);
            window.draw(activeText);
        }

        window.display();
    }
}

void getAvailableCompanies(vertex* graph, string*& companies, int& companyCount) {
    // Use a simple array to store unique companies

    string tempCompanies[100];// Assuming count
    int count = 0;
    vertex* v = graph;
    while (v != NULL) {
        edge* e = v->edges;
        while (e != NULL) {
            voyage* voy = e->voyages;
            while (voy != NULL) {

                bool found = false; //check if company in list
                for (int i = 0; i < count; i++) {
                    if (tempCompanies[i] == voy->company) {
                        found = true;
                        break;
                    }
                }
                if (!found && count < 100) {
                    tempCompanies[count++] = voy->company;
                }
                voy = voy->next;
            }
            e = e->next;
        }
        v = v->next;
    }

    // Copy array
    companies = new string[count];
    for (int i = 0; i < count; i++) {
        companies[i] = tempCompanies[i];
    }
    companyCount = count;

}

struct WeatherEffect {
    sf::Vector2f position;
    float size;
    float speed;
    int type; // 0 for cloudy 1 for rain and 2 for sunny
    sf::Color color;
};

void showSubgraphMap(sf::RenderWindow& window, UserPreferences* prefs, vertex* graph, RouteVisualizer& visualizer) {

    sf::Font font;
    if (!font.loadFromFile("font.ttf")) return;

    sf::Texture mapbg;
    if (!mapbg.loadFromFile("map_bg.png")) {
        mapbg.create(1200, 700);
    }


    sf::Sprite background(mapbg);

    sf::Text title("Route Subgraph - Map View", font, 35);
    title.setFillColor(sf::Color::White);
    title.setPosition(420, 10);

    sf::RectangleShape controlPannel(sf::Vector2f(1150, 60));
    controlPannel.setFillColor(sf::Color(20, 30, 50, 200));
    controlPannel.setOutlineThickness(2);
    controlPannel.setOutlineColor(sf::Color::Cyan);
    controlPannel.setPosition(25, 50);

    sf::Text companylabel("Company:", font, 18);
    companylabel.setFillColor(sf::Color::White);
    companylabel.setPosition(40, 65);

    sf::RectangleShape companyBox(sf::Vector2f(180, 35));
    companyBox.setFillColor(sf::Color::White);
    companyBox.setPosition(140, 60);


    sf::Text companyText(prefs->prefrenceCommpany.empty() ? "All Companies" : prefs->prefrenceCommpany, font, 16);
    companyText.setFillColor(sf::Color::Black);
    companyText.setPosition(145, 67);

    // Weather toggle button
    sf::RectangleShape weatherBtn(sf::Vector2f(180, 35));
    weatherBtn.setFillColor(prefs->CalmWeather ? sf::Color(0, 150, 0) : sf::Color(150, 0, 0));
    weatherBtn.setPosition(350, 60);

    sf::Text weatherText(prefs->CalmWeather ? "Calm Only ☀" : "All Weather ⛈", font, 16);
    weatherText.setFillColor(sf::Color::White);
    weatherText.setPosition(365, 67);

    // Apply button
    sf::RectangleShape applyBtn(sf::Vector2f(120, 35));
    applyBtn.setFillColor(sf::Color(0, 120, 220));
    applyBtn.setPosition(560, 60);

    sf::Text applyText("Apply", font, 18);
    applyText.setFillColor(sf::Color::White);
    applyText.setPosition(585, 65);

    // Clear filters button
    sf::RectangleShape clearBtn(sf::Vector2f(120, 35));
    clearBtn.setFillColor(sf::Color(200, 100, 0));
    clearBtn.setPosition(700, 60);

    sf::Text clearText("Clear", font, 18);
    clearText.setFillColor(sf::Color::White);
    clearText.setPosition(730, 65);

    // Back button
    sf::RectangleShape backBtn(sf::Vector2f(100, 35));
    backBtn.setFillColor(sf::Color(200, 40, 40));
    backBtn.setPosition(1050, 60);

    sf::Text backText("Back", font, 18);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(1075, 65);

    sf::RectangleShape InfoBox(sf::Vector2f(250, 200));
    InfoBox.setFillColor(sf::Color(20, 30, 50, 220));
    InfoBox.setOutlineThickness(2);
    InfoBox.setOutlineColor(sf::Color::Cyan);
    InfoBox.setPosition(950, 150);

    sf::Text infoTitle("Route Info", font, 20);
    infoTitle.setFillColor(sf::Color::White);
    infoTitle.setPosition(100, 160);

    WeatherEffect weatherEffects[50];
    int weathercount = 0;

    bool ShowCompanyDropDown = false;
    string* availableCompanies = nullptr;
    int companyCount = 0;
    getAvailableCompanies(graph, availableCompanies, companyCount);

    sf::Text statusMsg("", font, 16);
    statusMsg.setFillColor(sf::Color::Green);
    statusMsg.setPosition(40, 630);

    sf::Clock weatherClock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // Company box click - toggle dropdown
                if (companyBox.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    ShowCompanyDropDown = !ShowCompanyDropDown;
                }

                // Company dropdown selection
                if (ShowCompanyDropDown) {
                    for (int i = 0; i < companyCount && i < 8; i++) {
                        sf::FloatRect dropdownItem(140, 100 + i * 30, 180, 28);
                        if (dropdownItem.contains(mousePos.x, mousePos.y)) {
                            prefs->prefrenceCommpany = availableCompanies[i];
                            companyText.setString(prefs->prefrenceCommpany);
                            ShowCompanyDropDown = false;
                            break;
                        }
                    }
                }

                // Weather toggle
                if (weatherBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    prefs->CalmWeather = !prefs->CalmWeather;
                    weatherText.setString(prefs->CalmWeather ? "Calm Only ☀" : "All Weather ⛈");
                    weatherBtn.setFillColor(prefs->CalmWeather ? sf::Color(0, 150, 0) : sf::Color(150, 0, 0));
                }

                // Apply button
                if (applyBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    prefs->isActive = true;
                    statusMsg.setString("Filters applied! Showing subgraph...");
                    statusMsg.setFillColor(sf::Color::Green);
                }

                // Clear button
                if (clearBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    prefs->prefrenceCommpany = "";
                    prefs->CalmWeather = false;
                    prefs->isActive = false;
                    statusMsg.setString("Filters cleared!");
                    statusMsg.setFillColor(sf::Color::Yellow);
                }

                // Back button
                if (backBtn.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (availableCompanies != nullptr) {
                        delete[] availableCompanies;
                    }
                    return;
                }
            }
        }

        // Update weather effects

        if (weatherClock.getElapsedTime().asMilliseconds() > 100) {

            // Generate new weather effects for stormy routes
            if (weathercount < 50) {
                weatherEffects[weathercount].position = sf::Vector2f(rand() % 1200, rand() % 700);
                weatherEffects[weathercount].size = 3 + rand() % 5;
                weatherEffects[weathercount].speed = 2 + rand() % 3;
                weatherEffects[weathercount].type = prefs->CalmWeather ? 3 : (rand() % 4);
                weatherEffects[weathercount].color = sf::Color(200, 200, 255, 150);
                weathercount++;
            }
            weatherClock.restart();
        }

        // Move weather effects
        for (int i = 0; i < weathercount; i++) {
            weatherEffects[i].position.y += weatherEffects[i].speed;
            if (weatherEffects[i].position.y > 700) {
                weatherEffects[i].position.y = 0;
                weatherEffects[i].position.x = rand() % 1200;
            }
        }

        window.clear();
        window.draw(background);

        // Draw all routes first (as lines)
        visualizer.drawAllRoutes(graph, window);

        // Draw routes based on filter - create visual subgraph
        vertex* v = graph;
        while (v != NULL) {
            bool portHasValidRoutes = false;

            edge* e = v->edges;
            while (e != NULL) {
                bool edgeHasValidVoyage = false;
                voyage* voy = e->voyages;

                while (voy != NULL) {
                    if (isVoyageTooLong(prefs, voy)) {
                        edgeHasValidVoyage = true;
                        portHasValidRoutes = true;

                        // Draw this route as valid (green/bright)
                        if (prefs->isActive) {
                            visualizer.drawRouteLine(window, v->portname, e->destination,
                                sf::Color(0, 255, 0, 200));
                        }

                        // Draw weather effects on this route

                        if (!prefs->CalmWeather && isRouteStomy(voy)) {
                            // Draw storm icon on route
                            PortPosition* p1 = visualizer.getPortPosition(v->portname);
                            PortPosition* p2 = visualizer.getPortPosition(e->destination);

                            if (p1 && p2) {
                                float midX = (p1->x + p2->x) / 2;
                                float midY = (p1->y + p2->y) / 2;

                                // Storm cloud
                                sf::CircleShape stormCloud(15);
                                stormCloud.setFillColor(sf::Color(80, 80, 100, 180));
                                stormCloud.setPosition(midX - 15, midY - 15);
                                window.draw(stormCloud);

                                // Lightning symbol
                                sf::Text lightning("⚡", font, 20);
                                lightning.setFillColor(sf::Color::Yellow);
                                lightning.setPosition(midX - 8, midY - 10);
                                window.draw(lightning);
                            }
                        }
                        break;
                    }
                    voy = voy->next;
                }

                // Draw invalid routes as faded red
                if (prefs->isActive && !edgeHasValidVoyage) {
                    visualizer.drawRouteLine(window, v->portname, e->destination,
                        sf::Color(255, 0, 0, 50));
                }

                e = e->next;
            }

            v = v->next;
        }

        // Draw ports with appropriate colors
        v = graph;
        while (v != NULL) {
            PortPosition* portPos = visualizer.getPortPosition(v->portname);
            if (portPos != nullptr) {
                bool portInSubgraph = false;

                // Check if port has valid routes
                edge* e = v->edges;
                while (e != NULL) {
                    voyage* voy = e->voyages;
                    while (voy != NULL) {
                        if (isVoyageTooLong(prefs, voy)) {
                            portInSubgraph = true;
                            break;
                        }
                        voy = voy->next;
                    }
                    if (portInSubgraph) break;
                    e = e->next;
                }

                // Draw port circle
                sf::CircleShape portCircle(15);
                if (prefs->isActive) {
                    if (portInSubgraph) {
                        portCircle.setFillColor(sf::Color(0, 200, 0)); // Bright green - in subgraph
                        portCircle.setOutlineThickness(3);
                        portCircle.setOutlineColor(sf::Color::White);
                    }
                    else {
                        portCircle.setFillColor(sf::Color(100, 100, 100, 100)); // Faded - not in subgraph
                        portCircle.setOutlineThickness(1);
                        portCircle.setOutlineColor(sf::Color(150, 150, 150));
                    }
                }
                else {
                    portCircle.setFillColor(sf::Color::White);
                    portCircle.setOutlineThickness(2);
                    portCircle.setOutlineColor(sf::Color::Black);
                }

                portCircle.setPosition(portPos->x - 15, portPos->y - 15);
                window.draw(portCircle);

                // Port name
                sf::Text portName(v->portname, font, 12);
                portName.setFillColor(sf::Color::White);
                portName.setStyle(sf::Text::Bold);
                portName.setPosition(portPos->x - 30, portPos->y + 18);
                window.draw(portName);

                // Weather icon on port if applicable
                if (prefs->CalmWeather && portInSubgraph) {
                    // Sun icon for calm weather
                    sf::Text sunIcon("☀", font, 24);
                    sunIcon.setFillColor(sf::Color::Yellow);
                    sunIcon.setPosition(portPos->x + 10, portPos->y - 20);
                    window.draw(sunIcon);
                }
            }

            v = v->next;
        }

        // Draw weather effects (rain/snow)
        for (int i = 0; i < weathercount && i < 30; i++) {
            if (weatherEffects[i].type == 1) { // Rain
                sf::RectangleShape rain(sf::Vector2f(2, 10));
                rain.setFillColor(sf::Color(150, 150, 255, 180));
                rain.setPosition(weatherEffects[i].position);
                window.draw(rain);
            }
        }

        // Draw control panel
        window.draw(InfoBox);
        window.draw(title);
        window.draw(companylabel);
        window.draw(companyBox);
        window.draw(companyText);
        window.draw(weatherBtn);
        window.draw(weatherText);
        window.draw(applyBtn);
        window.draw(applyText);
        window.draw(clearBtn);
        window.draw(clearText);
        window.draw(backBtn);
        window.draw(backText);

        // Company dropdown
        if (ShowCompanyDropDown) {
            for (int i = 0; i < companyCount && i < 8; i++) {
                sf::RectangleShape dropItem(sf::Vector2f(180, 28));
                dropItem.setFillColor(sf::Color::White);
                dropItem.setPosition(140, 100 + i * 30);
                window.draw(dropItem);

                sf::Text compName(availableCompanies[i], font, 14);
                compName.setFillColor(sf::Color::Black);
                compName.setPosition(145, 105 + i * 30);
                window.draw(compName);
            }
        }

        // Draw legend
        window.draw(InfoBox);
        window.draw(infoTitle);

        sf::CircleShape leg1(8);
        leg1.setFillColor(sf::Color(0, 200, 0));
        leg1.setPosition(935, 195);
        window.draw(leg1);
        sf::Text leg1Text("Active Port", font, 14);
        leg1Text.setFillColor(sf::Color::White);
        leg1Text.setPosition(955, 193);
        window.draw(leg1Text);

        sf::CircleShape leg2(8);
        leg2.setFillColor(sf::Color(100, 100, 100));
        leg2.setPosition(935, 225);
        window.draw(leg2);
        sf::Text leg2Text("Filtered Out", font, 14);
        leg2Text.setFillColor(sf::Color::White);
        leg2Text.setPosition(955, 223);
        window.draw(leg2Text);

        sf::RectangleShape leg3(sf::Vector2f(20, 3));
        leg3.setFillColor(sf::Color::Green);
        leg3.setPosition(935, 260);
        window.draw(leg3);
        sf::Text leg3Text("Valid Route", font, 14);
        leg3Text.setFillColor(sf::Color::White);
        leg3Text.setPosition(960, 253);
        window.draw(leg3Text);

        sf::Text leg4("⚡", font, 16);
        leg4.setFillColor(sf::Color::Yellow);
        leg4.setPosition(935, 280);
        window.draw(leg4);
        sf::Text leg4Text("Stormy", font, 14);
        leg4Text.setFillColor(sf::Color::White);
        leg4Text.setPosition(960, 283);
        window.draw(leg4Text);

        sf::Text leg5("☀", font, 16);
        leg5.setFillColor(sf::Color::Yellow);
        leg5.setPosition(935, 310);
        window.draw(leg5);
        sf::Text leg5Text("Calm", font, 14);
        leg5Text.setFillColor(sf::Color::White);
        leg5Text.setPosition(960, 313);
        window.draw(leg5Text);

        window.draw(statusMsg);
        window.display();
    }

    if (availableCompanies != nullptr) {
        delete[] availableCompanies;
    }
}

int main() {

    string filename = "Routes.txt";
    string chargesfile = "PortCharges.txt";
    vertex* graph = buildgraph(filename);
    int v;
    vertex** portarray = intializegraphindex(graph, v);
    loadportcharges(chargesfile, graph);
    printGraph(graph);
    ShipRouteBooking bookingSystem;
    RouteVisualizer routeVisualizer;
    UserPreferences* globalPrefs = new UserPreferences();

    sf::RenderWindow window(sf::VideoMode(1200, 700), "Maritime Navigation");

    // Load background texture
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("home.jpg")) {
        return -1;
    }
    sf::Sprite background(bgTexture);
    background.setScale(
        1280.0f / bgTexture.getSize().x,
        720.0f / bgTexture.getSize().y
    );

    // Load font
    sf::Font font;
    if (!font.loadFromFile("font.ttf")) {
        return -1;
    }

    sf::Music music; //music

    if (!music.openFromFile("Taylor Swift - The Fate of Ophelia.ogg")) {
        std::cout << "Error loading music file!" << std::endl;
    }

    music.setLoop(true);     // repeat forever
    music.setVolume(50);     // volume 0–100
    music.play();

    // Create heading
    sf::Text heading;
    heading.setFont(font);
    heading.setString("Maritime Navigation");
    heading.setCharacterSize(50);
    heading.setFillColor(sf::Color::White);
    heading.setPosition(1280 / 2 - heading.getLocalBounds().width / 2, 20);

    // Create buttons at 40% down the screen
    float buttonY = 720 * 0.40f;

    sf::RectangleShape button1(sf::Vector2f(250, 60));
    button1.setFillColor(sf::Color(30, 60, 120));
    button1.setPosition(20, buttonY);

    sf::RectangleShape button2(sf::Vector2f(250, 60));
    button2.setFillColor(sf::Color(30, 60, 120));
    button2.setPosition(20, buttonY + 80);

    sf::RectangleShape button3(sf::Vector2f(250, 60));
    button3.setFillColor(sf::Color(30, 60, 120));
    button3.setPosition(20, buttonY + 160);

    sf::RectangleShape button4(sf::Vector2f(250, 60));
    button4.setFillColor(sf::Color(30, 60, 120));
    button4.setPosition(20, buttonY + 240);

    sf::RectangleShape button5(sf::Vector2f(250, 60));
    button5.setFillColor(sf::Color(30, 60, 120));
    button5.setPosition(20, buttonY - 80);

    sf::RectangleShape button6(sf::Vector2f(250, 60));
    button6.setFillColor(sf::Color(30, 60, 120));
    button6.setPosition(20, buttonY + 240 + 60 + 20);

    sf::RectangleShape prefButton(sf::Vector2f(250, 60));
    prefButton.setFillColor(sf::Color(30, 60, 120));
    prefButton.setPosition(20, buttonY - 160);

    sf::RectangleShape subgrphButton(sf::Vector2f(250, 60));
    subgrphButton.setFillColor(sf::Color(30, 60, 120));
    subgrphButton.setPosition(20, buttonY - 240);


    // Button texts
    sf::Text text1;
    text1.setFont(font);
    text1.setString("View Port Message");
    text1.setCharacterSize(20);
    text1.setFillColor(sf::Color::White);
    text1.setPosition(35, buttonY + 15);

    sf::Text text2;
    text2.setFont(font);
    text2.setString("Book Ship Message");
    text2.setCharacterSize(20);
    text2.setFillColor(sf::Color::White);
    text2.setPosition(35, buttonY + 95);

    sf::Text text3;
    text3.setFont(font);
    text3.setString("View Dock");
    text3.setCharacterSize(20);
    text3.setFillColor(sf::Color::White);
    text3.setPosition(35, buttonY + 175);

    sf::Text text4;
    text4.setFont(font);
    text4.setString("Multi- Leg Route");
    text4.setCharacterSize(20);
    text4.setFillColor(sf::Color::White);
    text4.setPosition(35, buttonY + 255);

    sf::Text text5;
    text5.setFont(font);
    text5.setString("Find Shortest Distance");
    text5.setCharacterSize(20);
    text5.setFillColor(sf::Color::White);
    text5.setPosition(35, buttonY - 60);

    sf::Text text6;
    text6.setFont(font);
    text6.setString("Exit");
    text6.setCharacterSize(20);
    text6.setFillColor(sf::Color::White);
    text6.setPosition(35, buttonY + 330

    );

    sf::Text prefText;
    prefText.setFont(font);
    prefText.setString("Set Preferences");
    prefText.setCharacterSize(20);
    prefText.setFillColor(sf::Color::White);
    prefText.setPosition(35, buttonY - 140);

    sf::Text subgrphText;
    subgrphText.setFont(font);
    subgrphText.setString("View Subgraph Map");
    subgrphText.setCharacterSize(20);
    subgrphText.setFillColor(sf::Color::White);
    subgrphText.setPosition(35, buttonY - 220);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (button1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    sfmlportdisplay(window, graph);

                }

                if (button2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {

                    ShowBookingScreen(window, bookingSystem, graph, routeVisualizer, globalPrefs);
                }

                if (button3.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    AskUserPort(window, graph);

                }

                if (button4.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    task6(window, graph, portarray, v, globalPrefs);

                }

                if (button5.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    dijsfml(window, graph, portarray, v);
                }

                if (button6.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    window.close();
                }

                if (prefButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    ShowPreferencesScreen(window, globalPrefs, graph);
                }

                if (subgrphButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    showSubgraphMap(window, globalPrefs, graph, routeVisualizer);
                }
            }
        }

        window.clear();
        window.draw(background);
        window.draw(heading);
        window.draw(button1);
        window.draw(button2);
        window.draw(button3);
        window.draw(button4);
        window.draw(button5);
        window.draw(button6);
        window.draw(prefButton);
        window.draw(subgrphButton);
        window.draw(text1);
        window.draw(text2);
        window.draw(text3);
        window.draw(text4);
        window.draw(text5);
        window.draw(text6);
        window.draw(prefText);
        window.draw(subgrphText);
        window.display();
    }

    return 0;
}