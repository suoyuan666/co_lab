#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int NUM_PROCESSES = 5;
const int NUM_RESOURCES = 3;
int AVAILABLE[NUM_RESOURCES] = {10, 5, 7}; // Available resources
const int MAX[NUM_PROCESSES][NUM_RESOURCES] = { // Maximum demand
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};
int ALLOCATION[NUM_PROCESSES][NUM_RESOURCES] = {0}; // Resources allocated to processes

bool isSafe(int process) {
    vector<int> available(AVAILABLE, AVAILABLE + NUM_RESOURCES);
    vector<int> need[NUM_PROCESSES];
    for (int i = 0; i < NUM_PROCESSES; i++) {
        need[i] = vector<int>(NUM_RESOURCES);
        for (int j = 0; j < NUM_RESOURCES; j++) {
            need[i][j] = MAX[i][j] - ALLOCATION[i][j];
        }
    }

    vector<bool> finish(NUM_PROCESSES, false);
    vector<int> safeSequence;

    while (true) {
        bool allFinished = true;
        for (int i = 0; i < NUM_PROCESSES; i++) {
            if (!finish[i]) {
                bool canAllocate = true;
                for (int j = 0; j < NUM_RESOURCES; j++) {
                    if (need[i][j] > available[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    safeSequence.push_back(i);
                    for (int j = 0; j < NUM_RESOURCES; j++) {
                        available[j] += ALLOCATION[i][j];
                    }
                    finish[i] = true;
                    allFinished = false;
                }
            }
        }
        if (allFinished) {
            break;
        }
    }

    if (safeSequence.size() == NUM_PROCESSES) {
        cout << "Safe Sequence: ";
        for (int i = 0; i < safeSequence.size(); i++) {
            cout << "P" << safeSequence[i] << " ";
        }
        cout << endl;
        return true;
    }
    return false;
}

int main() {
    cout << "Current Allocation:" << endl;
    for (int i = 0; i < NUM_PROCESSES; i++) {
        cout << "P" << i << ": ";
        for (int j = 0; j < NUM_RESOURCES; j++) {
            cout << ALLOCATION[i][j] << " ";
        }
        cout << endl;
    }

    int process, resource, amount;
    while (true) {
        cout << "Enter process number (0-4, -1 to exit): ";
        cin >> process;
        if (process == -1) {
            break;
        }
        cout << "Enter resource number (0-2): ";
        cin >> resource;
        cout << "Enter amount: ";
        cin >> amount;

        if (amount <= AVAILABLE[resource]) {
            ALLOCATION[process][resource] += amount;
            AVAILABLE[resource] -= amount;
            if (isSafe(process)) {
                cout << "Resource allocated successfully." << endl;
            } else {
                cout << "Unsafe state! Resource allocation failed." << endl;
                ALLOCATION[process][resource] -= amount;
                AVAILABLE[resource] += amount;
            }
        } else {
            cout << "Not enough resources available." << endl;
        }
    }

    return 0;
}