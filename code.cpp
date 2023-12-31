#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <climits>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct Station {
    string name;
    map<Station*, int> neighbors;
};

struct HeapNode {
    Station* station;
    int distance;

    bool operator>(const HeapNode& other) const {
        return distance > other.distance;
    }
};

// Binary Search Tree for organizing stations
class StationBST {
public:
    void insert(Station* station) {
        root = insertRec(root, station);
    }

    void inorder() {
        inorderRec(root);
    }


    struct TreeNode {
        Station* station;
        TreeNode* left;
        TreeNode* right;

        TreeNode(Station* s) : station(s), left(nullptr), right(nullptr) {}
    };

    TreeNode* root;

    TreeNode* insertRec(TreeNode* root, Station* station) {
        if (root == nullptr) {
            return new TreeNode(station);
        }

        if (station->name < root->station->name) {
            root->left = insertRec(root->left, station);
        } else if (station->name > root->station->name) {
            root->right = insertRec(root->right, station);
        }

        return root;
    }

    void inorderRec(TreeNode* root) {
        if (root != nullptr) {
            inorderRec(root->left);
            cout << root->station->name << " ";
            inorderRec(root->right);
        }
    }
};

class MetroNetwork {
public:
    void addStation(const string& name) {
        Station* newStation = new Station{name};
        stations.push_back(newStation);
        stationHashTable[name] = newStation;
        stationBST.insert(newStation);
    }

    void addConnection(const string& station1, const string& station2, int distance) {
        Station* s1 = findStation(station1);
        Station* s2 = findStation(station2);

        if (s1 != nullptr && s2 != nullptr) {
            s1->neighbors[s2] = distance;
            s2->neighbors[s1] = distance;
        } else {
            cout << "Invalid station names. Please make sure both stations exist." << endl;
        }
    }

    int calculateFare(const string& source, const string& destination) {
        Station* sourceStation = findStation(source);
        Station* destinationStation = findStation(destination);

        if (sourceStation == nullptr || destinationStation == nullptr) {
            cout << "Invalid source or destination station." << endl;
            return -1;
        }

        map<Station*, int> distances = dijkstra(sourceStation);
        int distance = distances[destinationStation];

        if (distance == INT_MAX) {
            cout << "No valid route between the stations." << endl;
            return -1;
        }

        int fare = distance * 2;
        return fare;
    }

    void displayNetwork() {
        for (Station* station : stations) {
            cout << "Station " << station->name << " Neighbors: ";
            for (auto& neighbor : station->neighbors) {
                cout << neighbor.first->name << "(" << neighbor.second << " units) ";
            }
            cout << endl;
        }
    }

    void displayStations() {
        cout << "Stations: ";
        stationBST.inorder();
        cout << endl;
    }

    void removeStation(const string& name) {
        auto it = find_if(stations.begin(), stations.end(),
                          [name](Station* station) { return station->name == name; });

        if (it != stations.end()) {
            Station* stationToRemove = *it;
            stations.erase(it);

            // Remove connections to the station
            for (Station* otherStation : stations) {
                auto neighborIt = otherStation->neighbors.find(stationToRemove);
                if (neighborIt != otherStation->neighbors.end()) {
                    otherStation->neighbors.erase(neighborIt);
                }
            }

            // Remove from hash table
            stationHashTable.erase(name);

            // Remove from BST
            stationBST = removeFromBST(stationBST, stationToRemove);

            delete stationToRemove;
            cout << "Station " << name << " removed from the metro network." << endl;
        } else {
            cout << "Station " << name << " not found in the metro network." << endl;
        }
    }

    void removeConnection(const string& station1, const string& station2) {
        Station* s1 = findStation(station1);
        Station* s2 = findStation(station2);

        if (s1 != nullptr && s2 != nullptr) {
            auto it1 = s1->neighbors.find(s2);
            auto it2 = s2->neighbors.find(s1);

            if (it1 != s1->neighbors.end() && it2 != s2->neighbors.end()) {
                s1->neighbors.erase(it1);
                s2->neighbors.erase(it2);
                cout << "Connection between " << station1 << " and " << station2 << " removed." << endl;
            } else {
                cout << "No connection found between " << station1 << " and " << station2 << "." << endl;
            }
        } else {
            cout << "Invalid station names. Please make sure both stations exist." << endl;
        }
    }

    ~MetroNetwork() {
        for (Station* station : stations) {
            delete station;
        }
    }

    Station* findStation(const string& name) {
        auto it = stationHashTable.find(name);
        if (it != stationHashTable.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

private:
    vector<Station*> stations;
    unordered_map<string, Station*> stationHashTable;
    StationBST stationBST;

    map<Station*, int> dijkstra(Station* source) {
        map<Station*, int> distances;
        priority_queue<HeapNode, vector<HeapNode>, greater<HeapNode>> minHeap;

        for (Station* station : stations) {
            distances[station] = INT_MAX;
        }

        distances[source] = 0;
        minHeap.push({source, 0});

        while (!minHeap.empty()) {
            HeapNode current = minHeap.top();
            minHeap.pop();

            for (auto& neighbor : current.station->neighbors) {
                int newDistance = distances[current.station] + neighbor.second;
                if (newDistance < distances[neighbor.first]) {
                    distances[neighbor.first] = newDistance;
                    minHeap.push({neighbor.first, newDistance});
                }
            }
        }

        return distances;
    }

    // Helper function to remove a node from BST
    StationBST removeFromBST(StationBST bst, Station* stationToRemove) {
        bst.root = removeNode(bst.root, stationToRemove);
        return bst;
    }

    // Helper function to remove a node from BST
    StationBST::TreeNode* removeNode(StationBST::TreeNode* root, Station* stationToRemove) {
        if (root == nullptr) {
            return root;
        }

        if (stationToRemove->name < root->station->name) {
            root->left = removeNode(root->left, stationToRemove);
        } else if (stationToRemove->name > root->station->name) {
            root->right = removeNode(root->right, stationToRemove);
        } else {
            // Node with only one child or no child
            if (root->left == nullptr) {
                StationBST::TreeNode* temp = root->right;
                delete root;
                return temp;
            } else if (root->right == nullptr) {
                StationBST::TreeNode* temp = root->left;
                delete root;
                return temp;
            }

            // Node with two children, get the inorder successor
            StationBST::TreeNode* temp = minValueNode(root->right);

            // Copy the inorder successor's content to this node
            root->station = temp->station;

            // Delete the inorder successor
            root->right = removeNode(root->right, temp->station);
        }

        return root;
    }

    // Helper function to find the node with the minimum value
    StationBST::TreeNode* minValueNode(StationBST::TreeNode* root) {
        StationBST::TreeNode* current = root;

        while (current->left != nullptr) {
            current = current->left;
        }

        return current;
    }
};

void addNewStation(MetroNetwork& metro) {
    string stationName;
    cout << "Enter the name of the new station: ";
    cin >> stationName;
    metro.addStation(stationName);
    cout << "Station " << stationName << " added to the metro network." << endl;
}

void addNewConnection(MetroNetwork& metro) {
    string station1, station2;
    int distance;
    metro.displayStations();
    cout << "Enter the names of the two stations to connect: ";
    cin >> station1 >> station2;
    cout << "Enter the distance between " << station1 << " and " << station2 << ": ";
    cin >> distance;

    metro.addConnection(station1, station2, distance);
    cout << "Connection between " << station1 << " and " << station2 << " added to the metro network." << endl;
}

void calculateFare(MetroNetwork& metro) {
    string source, destination;
    metro.displayStations();
    cout << "Enter the source station: ";
    cin >> source;
    cout << "Enter the destination station: ";
    cin >> destination;

    int fare = metro.calculateFare(source, destination);

    if (fare != -1) {
        cout << "The fare between stations " << source << " and " << destination << " is $" << fare << "." << endl;
    }
}

void removeStation(MetroNetwork& metro) {
    string stationName;
    metro.displayStations();
    cout << "Enter the name of the station to remove: ";
    cin >> stationName;

    metro.removeStation(stationName);
}

void removeConnection(MetroNetwork& metro) {
    string station1, station2;
    metro.displayStations();
    cout << "Enter the names of the two stations to remove connection: ";
    cin >> station1 >> station2;

    metro.removeConnection(station1, station2);
}

int main() {
    MetroNetwork metro;

    metro.addStation("A");
    metro.addStation("B");
    metro.addStation("C");
    metro.addStation("D");
    metro.addConnection("A", "B", 5);
    metro.addConnection("B", "C", 3);
    metro.addConnection("C", "D", 4);
    metro.addConnection("D", "A", 7);

    while (true) {
        cout << "\nMetro Network Operations:" << endl;
        cout << "1. Add new station" << endl;
        cout << "2. Add new connection between stations" << endl;
        cout << "3. Calculate fare between stations" << endl;
        cout << "4. Display metro network" << endl;
        cout << "5. Remove station" << endl;
        cout << "6. Remove connection between stations" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice (1-7): ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                addNewStation(metro);
                break;
            case 2:
                addNewConnection(metro);
                break;
            case 3:
                calculateFare(metro);
                break;
            case 4:
                metro.displayNetwork();
                break;
            case 5:
                removeStation(metro);
                break;
            case 6:
                removeConnection(metro);
                break;
            case 7:
                cout << "Exiting the program." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 7." << endl;
        }
    }

    return 0;
}
