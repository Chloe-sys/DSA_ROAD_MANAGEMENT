#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cctype>

using namespace std;

// Structure declarations
struct City {
    int index;
    string name;
};

struct Road {
    string city1;
    string city2;
    double budget;
};

// Global variables
vector<City> cities;
vector<Road> roads;
const int MAX_CITIES = 20;
int adjacency[MAX_CITIES][MAX_CITIES] = {0};          // Road connection matrix
double budgetAdjacency[MAX_CITIES][MAX_CITIES] = {0}; // Budget adjacency matrix

// Function prototypes
int getValidInt(const string& prompt, int min, int max);
double getValidDouble(const string& prompt, double min);
string getValidCityName(const string& prompt);
void initializeSystem();
void addNewCities();
void displayCities();
int findCityIndex(const string& name);
string getCityName(int index);
void addRoad();
void displayRoads();
void displayBudgetMatrix();
void assignBudget();
void editCity();
void searchCity();
void displayAllData();
void saveToFiles();
void loadFromFiles();
void printMainMenu();

int main() {
    initializeSystem();
    loadFromFiles();

    int choice;
    do {
        printMainMenu();
        choice = getValidInt("Enter your choice (1-9): ", 1, 9);

        switch(choice) {
            case 1: addNewCities(); break;
            case 2: addRoad(); break;
            case 3: assignBudget(); break;
            case 4: editCity(); break;
            case 5: searchCity(); break;
            case 6: displayCities(); break;
            case 7: displayRoads(); break;
            case 8: displayAllData(); break;
            case 9:
                saveToFiles();
                cout << "Data saved. Exiting program.\n";
                break;
        }
    } while(choice != 9);

    return 0;
}

// Function implementations
int getValidInt(const string& prompt, int min, int max) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                cin.ignore();
                return value;
            }
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number between "
             << min << " and " << max << ".\n";
    }
}

double getValidDouble(const string& prompt, double min) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min) {
                cin.ignore();
                return value;
            }
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number >= " << min << ".\n";
    }
}

string getValidCityName(const string& prompt) {
    string name;
    while (true) {
        cout << prompt;
        getline(cin, name);

        // Trim whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);

        if (name.empty()) {
            cout << "Error: City name cannot be empty.\n";
            continue;
        }

        if (any_of(name.begin(), name.end(), ::isdigit)) {
            cout << "Error: City name cannot contain numbers.\n";
            continue;
        }

        // Check for duplicate (case insensitive)
        string lowerName = name;
        transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        bool exists = false;
        for (const auto& city : cities) {
            string cityLower = city.name;
            transform(cityLower.begin(), cityLower.end(), cityLower.begin(), ::tolower);
            if (cityLower == lowerName) {
                exists = true;
                break;
            }
        }

        if (exists) {
            cout << "Error: City '" << name << "' already exists.\n";
        } else {
            return name;
        }
    }
}

void initializeSystem() {
    // Initialize adjacency matrices
    for (int i = 0; i < MAX_CITIES; i++) {
        for (int j = 0; j < MAX_CITIES; j++) {
            adjacency[i][j] = 0;
            budgetAdjacency[i][j] = 0.0;
        }
    }
}

void displayCities() {
    if (cities.empty()) {
        cout << "No cities to display.\n";
        return;
    }

    cout << "\n=== List of Cities ===\n";
    cout << setw(10) << "Index" << setw(20) << "City Name" << "\n";
    cout << string(30, '-') << "\n";

    for (const auto& city : cities) {
        cout << setw(10) << city.index << setw(20) << city.name << "\n";
    }
}

int findCityIndex(const string& input) {
    // Check if input is a number
    bool isNumber = true;
    for (char c : input) {
        if (!isdigit(c)) {
            isNumber = false;
            break;
        }
    }

    if (isNumber) {
        int index = stoi(input);
        for (const auto& city : cities) {
            if (city.index == index) {
                return index;
            }
        }
    } else {
        // Search by name (case insensitive)
        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

        for (const auto& city : cities) {
            string lowerName = city.name;
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            if (lowerName == lowerInput) {
                return city.index;
            }
        }
    }
    return -1; // Not found
}

string getCityName(int index) {
    for (const auto& city : cities) {
        if (city.index == index) {
            return city.name;
        }
    }
    return "Unknown";
}

void addNewCities() {
    cout << "\n=== Add New Cities ===\n";
    if (!cities.empty()) {
        cout << "Current cities:\n";
        displayCities();
    }

    int numCities = getValidInt("How many new cities to add? (1-20): ", 1, 20);

    for (int i = 0; i < numCities; i++) {
        City newCity;
        newCity.index = cities.empty() ? 1 : cities.back().index + 1;
        newCity.name = getValidCityName("Enter name for city #" + to_string(newCity.index) + ": ");

        cities.push_back(newCity);
        cout << "City '" << newCity.name << "' added with index " << newCity.index << ".\n";
    }
}

void displayRoads() {
    if (roads.empty()) {
        cout << "No roads to display.\n";
        return;
    }

    cout << "\n=== Road Connections ===\n";
    cout << setw(5) << "ID" << setw(25) << "Road" << setw(20) << "Budget (RWF)" << "\n";
    cout << string(50, '-') << "\n";

    int counter = 1;
    for (const auto& road : roads) {
        cout << setw(5) << counter++
             << setw(25) << road.city1 + " - " + road.city2
             << setw(20) << fixed << setprecision(2) << road.budget << "\n";
    }

    // Display connection matrix
    cout << "\n=== Connection Matrix ===\n";
    cout << "1 = Road exists, 0 = No road\n\n";
    cout << setw(12) << " ";
    for (const auto& city : cities) {
        cout << setw(5) << city.index;
    }
    cout << "\n";

    for (int i = 0; i < cities.size(); i++) {
        cout << setw(12) << cities[i].name;
        for (int j = 0; j < cities.size(); j++) {
            cout << setw(5) << adjacency[i][j];
        }
        cout << "\n";
    }
}

void displayBudgetMatrix() {
    if (cities.empty()) {
        cout << "No cities available to display budget matrix.\n";
        return;
    }

    cout << "\n=== Budget Adjacency Matrix ===\n";
    cout << "Shows budget amounts between connected cities (in billion RWF)\n\n";
    cout << setw(12) << " ";
    for (const auto& city : cities) {
        cout << setw(12) << city.index;
    }
    cout << "\n";

    for (int i = 0; i < cities.size(); i++) {
        cout << setw(12) << cities[i].name;
        for (int j = 0; j < cities.size(); j++) {
            if (adjacency[i][j] == 1) {
                cout << setw(12) << fixed << setprecision(2) << budgetAdjacency[i][j];
            } else {
                cout << setw(12) << "-";
            }
        }
        cout << "\n";
    }
}

void addRoad() {
    cout << "\n=== Add Road Between Cities ===\n";
    if (cities.size() < 2) {
        cout << "Need at least 2 cities to add a road.\n";
        return;
    }

    displayCities();

    string city1, city2;
    cout << "Enter first city name or index: ";
    getline(cin, city1);
    cout << "Enter second city name or index: ";
    getline(cin, city2);

    int index1 = findCityIndex(city1);
    int index2 = findCityIndex(city2);

    if (index1 == -1 || index2 == -1) {
        cout << "Error: One or both cities not found.\n";
        return;
    }

    if (index1 == index2) {
        cout << "Error: Cannot create road between the same city.\n";
        return;
    }

    if (adjacency[index1-1][index2-1] == 1) {
        cout << "Road already exists between these cities.\n";
        return;
    }

    // Add to adjacency matrix
    adjacency[index1-1][index2-1] = 1;
    adjacency[index2-1][index1-1] = 1;

    // Add to roads vector
    Road newRoad;
    newRoad.city1 = getCityName(index1);
    newRoad.city2 = getCityName(index2);
    newRoad.budget = 0.0;
    roads.push_back(newRoad);

    cout << "Road added between " << newRoad.city1 << " and " << newRoad.city2 << ".\n";
}

void assignBudget() {
    cout << "\n=== Assign Road Budget ===\n";
    if (roads.empty()) {
        cout << "No roads exist to assign budgets.\n";
        return;
    }

    displayRoads();

    string city1, city2;
    cout << "Enter first connected city: ";
    getline(cin, city1);
    cout << "Enter second connected city: ";
    getline(cin, city2);

    int index1 = findCityIndex(city1);
    int index2 = findCityIndex(city2);

    if (index1 == -1 || index2 == -1) {
        cout << "Error: One or both cities not found.\n";
        return;
    }

    if (adjacency[index1-1][index2-1] != 1) {
        cout << "Error: No road exists between these cities.\n";
        return;
    }

    double budget = getValidDouble("Enter budget amount (billion RWF): ", 0);

    // Update budget adjacency matrix
    budgetAdjacency[index1-1][index2-1] = budget;
    budgetAdjacency[index2-1][index1-1] = budget;

    // Update roads vector
    for (auto& road : roads) {
        if ((road.city1 == getCityName(index1) && road.city2 == getCityName(index2)) ||
            (road.city1 == getCityName(index2) && road.city2 == getCityName(index1))) {
            road.budget = budget;
            break;
        }
    }

    cout << "Budget of " << fixed << setprecision(2) << budget
         << " billion RWF assigned successfully.\n";
}

void editCity() {
    cout << "\n=== Edit City ===\n";
    if (cities.empty()) {
        cout << "No cities to edit.\n";
        return;
    }

    displayCities();

    string input;
    cout << "Enter city name or index to edit: ";
    getline(cin, input);

    int index = findCityIndex(input);
    if (index == -1) {
        cout << "City not found.\n";
        return;
    }

    string newName = getValidCityName("Enter new name (current: " + getCityName(index) + "): ");

    // Update city name
    for (auto& city : cities) {
        if (city.index == index) {
            city.name = newName;
            break;
        }
    }

    // Update road records
    for (auto& road : roads) {
        if (road.city1 == getCityName(index)) {
            road.city1 = newName;
        }
        if (road.city2 == getCityName(index)) {
            road.city2 = newName;
        }
    }

    cout << "City updated successfully.\n";
}

void searchCity() {
    cout << "\n=== Search City ===\n";
    if (cities.empty()) {
        cout << "No cities to search.\n";
        return;
    }

    string input;
    cout << "Enter city name or index to search: ";
    getline(cin, input);

    int index = findCityIndex(input);
    if (index == -1) {
        cout << "City not found.\n";
        return;
    }

    cout << "\nCity Details:\n";
    cout << "Index: " << index << "\n";
    cout << "Name: " << getCityName(index) << "\n";

    // Display connections
    cout << "Connected to: ";
    bool hasConnections = false;
    for (int i = 0; i < MAX_CITIES; i++) {
        if (adjacency[index-1][i] == 1) {
            cout << getCityName(i+1) << " (" << fixed << setprecision(2)
                 << budgetAdjacency[index-1][i] << " billion RWF), ";
            hasConnections = true;
        }
    }
    if (!hasConnections) {
        cout << "No connected cities";
    }
    cout << "\n";
}

void displayAllData() {
    displayCities();
    displayRoads();
    displayBudgetMatrix();
}

void saveToFiles() {
    // Save cities
    ofstream cityFile("cities.txt");
    if (cityFile.is_open()) {
        cityFile << setw(10) << "Index" << setw(20) << "City_name" << "\n";
        for (const auto& city : cities) {
            cityFile << setw(10) << city.index << setw(20) << city.name << "\n";
        }
        cityFile.close();
        cout << "Cities saved to cities.txt\n";
    } else {
        cout << "Error saving cities to file.\n";
    }

    // Save roads
    ofstream roadFile("roads.txt");
    if (roadFile.is_open()) {
        roadFile << setw(5) << "ID" << setw(25) << "Road" << setw(20) << "Budget" << "\n";
        int counter = 1;
        for (const auto& road : roads) {
            roadFile << setw(5) << counter++
                    << setw(25) << road.city1 + " - " + road.city2
                    << setw(20) << fixed << setprecision(2) << road.budget << "\n";
        }
        roadFile.close();
        cout << "Roads saved to roads.txt\n";
    } else {
        cout << "Error saving roads to file.\n";
    }
}

void loadFromFiles() {
    // Load cities
    ifstream cityFile("cities.txt");
    if (cityFile.is_open()) {
        cities.clear();
        string line;
        getline(cityFile, line); // Skip header

        while (getline(cityFile, line)) {
            if (line.empty()) continue;

            City city;
            size_t pos = line.find_first_not_of(" ");
            size_t endPos = line.find_first_of(" ", pos);
            city.index = stoi(line.substr(pos, endPos - pos));

            pos = line.find_first_not_of(" ", endPos);
            city.name = line.substr(pos);

            // Trim whitespace
            city.name.erase(city.name.find_last_not_of(" \t") + 1);
            cities.push_back(city);
        }
        cityFile.close();
    }

    // Load roads
    ifstream roadFile("roads.txt");
    if (roadFile.is_open()) {
        roads.clear();
        // Reset matrices
        for (int i = 0; i < MAX_CITIES; i++) {
            for (int j = 0; j < MAX_CITIES; j++) {
                adjacency[i][j] = 0;
                budgetAdjacency[i][j] = 0.0;
            }
        }

        string line;
        getline(roadFile, line); // Skip header

        while (getline(roadFile, line)) {
            if (line.empty()) continue;

            Road road;
            size_t pos = line.find_first_not_of(" ");
            size_t endPos = line.find(" - ");

            // Skip ID column
            pos = line.find_first_not_of(" ", pos + 5);
            road.city1 = line.substr(pos, endPos - pos);

            pos = endPos + 3;
            endPos = line.find_first_of(" ", pos);
            if (endPos == string::npos) endPos = line.length();
            road.city2 = line.substr(pos, endPos - pos);

            pos = line.find_first_not_of(" ", endPos);
            if (pos != string::npos) {
                road.budget = stod(line.substr(pos));
            }

            roads.push_back(road);

            // Update matrices
            int index1 = findCityIndex(road.city1);
            int index2 = findCityIndex(road.city2);
            if (index1 != -1 && index2 != -1) {
                adjacency[index1-1][index2-1] = 1;
                adjacency[index2-1][index1-1] = 1;
                budgetAdjacency[index1-1][index2-1] = road.budget;
                budgetAdjacency[index2-1][index1-1] = road.budget;
            }
        }
        roadFile.close();
    }
}

void printMainMenu() {
    cout << "\n=== Rwanda Infrastructure Management System ===\n";
    cout << "1. Add New City(ies)\n";
    cout << "2. Add Roads Between Cities\n";
    cout << "3. Assign Road Budgets\n";
    cout << "4. Edit City\n";
    cout << "5. Search for City\n";
    cout << "6. Display Cities\n";
    cout << "7. Display Roads\n";
    cout << "8. Display All Data\n";
    cout << "9. Save and Exit\n";
}