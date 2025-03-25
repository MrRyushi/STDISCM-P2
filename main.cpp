#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <algorithm>
#include <vector>
#include <random>
#include <mutex>

using namespace std;

// user inputs from config file
unsigned int tankPlayers = 0;
unsigned int healerPlayers = 0;
unsigned int dpsPlayers = 0;
unsigned int maxConcurrentInstance = 0;
unsigned int minTime = 0;
unsigned int maxTime = 0;

// Mutex for synchronization
mutex mtx;

struct DungeonInstance {
    bool active = false;
    unsigned int partiesServed = 0;
    unsigned int totalTimeServed = 0;
};
vector<DungeonInstance> instances;

bool isConfigFileValid(std::ifstream &configFile){
    // Check if file was opened successfully
    if (!configFile) {
        std::cout << "Error: Could not open the file!" << std::endl;
        return false;
    }
    return true;
}

bool isNumValid(std::string value) {
    // Trim leading/trailing spaces
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);

    // Check if the value is empty
    if (value.empty()) {
        std::cerr << "Error: Empty input!" << std::endl;
        return false;
    }

    // Check if value consists only of digits
    if (!std::all_of(value.begin(), value.end(), ::isdigit)) {
        std::cerr << "Error: Input is not a valid number!" << std::endl;
        return false;
    }

    try {
        int num = std::stoi(value);
        if (num < 0) {
            std::cerr << "Error: Number must be greater than or equal to 0!" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid number format!" << std::endl;
        return false;
    }

    return true;
}

unsigned int getValueFromLine(std::string line, std::string key){
    if(line.find(key) != std::string::npos){
        std::string value = line.substr(line.find('=') + 1);
        if(isNumValid(value)){
            return std::stoi(value);
        } else {
            return 1;
        }
    }
    return 0;
}

unsigned int getRandomNumber(int min, int max) {
    std::random_device rd;  
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

void displayStatus() {
    unique_lock<mutex> lock(mtx);
    cout << "\nCurrent Dungeons Status:" << endl;
    cout << "====================================" << endl;
    for (int i = 0; i < maxConcurrentInstance; i++) {
        cout << "Dungeon " << i + 1 << ": "
             << (instances[i].active ? "active" : "empty") << endl;
        
        // ASCII dungeon representation
        cout << "+--------+" << endl;
        cout << "|" << (instances[i].active ? "  🔥  " : "  🏰  ") << " |" << endl;
        cout << "|" << (instances[i].active ? "  BOSS " : "  ---- ") << " |" << endl;
        cout << "+--------+" << endl;
    }
    cout << "====================================" << endl;
}

void queueParties(int instanceIndex){
    while (true) {
        // Lock to prevent race conditions
        unique_lock<mutex> lock(mtx);
        if (tankPlayers < 1 || healerPlayers < 1 || dpsPlayers < 3) {
            break;
        }
        cout << "Queueing up players" << endl;
        // Assign players to an instance
        tankPlayers--;
        healerPlayers--;
        dpsPlayers -= 3;
        instances[instanceIndex].active = true;
        lock.unlock();

        displayStatus();

        // Simulate dungeon duration
        int dungeonTime = getRandomNumber(minTime, maxTime);
        this_thread::sleep_for(chrono::seconds(dungeonTime));
        
        lock.lock();
        instances[instanceIndex].active = false;
        instances[instanceIndex].partiesServed++;
        instances[instanceIndex].totalTimeServed += dungeonTime;
        lock.unlock();

        displayStatus();
    }

}

int main(){
    ifstream configFile("config.txt");
    // exit the program if the config file is not valid
    if (!isConfigFileValid(configFile)) return 1;

    // validate the input from config file
    string line;
    while(getline(configFile, line)){

        size_t pos = line.find('=');
        if (pos == string::npos) continue; // Skip invalid lines

        string key = line.substr(0, pos);
        key.erase(remove_if(key.begin(), key.end(), ::isspace), key.end()); // Trim spaces
        
        if (key == "n") {
            maxConcurrentInstance = getValueFromLine(line, key);
        } else if (key == "t") {
            tankPlayers = getValueFromLine(line, key);
        } else if (key == "h") {
            healerPlayers = getValueFromLine(line, key);
        } else if (key == "d") {
            dpsPlayers = getValueFromLine(line, key);
        } else if (key == "t1") {
            minTime = getValueFromLine(line, key);
        } else if (key == "t2") {
            maxTime = getValueFromLine(line, key);
        }
    }
    configFile.close();

    // exit the program if the input is not valid
    if (minTime >= maxTime) {
        std::cerr << "Error: minTime must be less than maxTime!" << std::endl;
        return 1;
    } else if(maxConcurrentInstance < 1){
        std::cerr << "Error: Number of instances must be greater than 0!" << std::endl;
        return 1;
    } else if(tankPlayers < 1 || healerPlayers < 1 || dpsPlayers < 3){
        std::cerr << "Error: Not enough players to form a party!" << std::endl;
        return 1;
    } else if(minTime < 1 || maxTime < 1){
        std::cerr << "Error: Time must be greater than 0!" << std::endl;
        return 1;
    } else if(maxTime > 15){
        std::cerr << "Error: Time must be less than 15!" << std::endl;
        return 1;
    } 

    // resize the instances vector based on user input
    instances.resize(maxConcurrentInstance);
    vector<thread> threads;
    
    for(int i=0; i < maxConcurrentInstance; i++){
        threads.emplace_back(queueParties, i);
    }

    for (auto &t : threads) {
        t.join();
    }

    // Final report
    cout << "\nDungeon Instance Summary:" << endl;
    for (int i = 0; i < maxConcurrentInstance; i++) {
        cout << "Dungeon " << i + 1 << " served " << instances[i].partiesServed
             << " parties, total time served: " << instances[i].totalTimeServed << "s\n";
    }
    cout << endl << "Remaining players: " << endl << tankPlayers << " tanks, " << endl << healerPlayers << " healers, " << endl << dpsPlayers << " dps" << endl;

    return 0;
}