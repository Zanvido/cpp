#include <iostream>
#include <ctime>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct Transaction {
    int from;
    int to;
    int amount;

    Transaction() : from(0), to(0), amount(0) {}
    Transaction(int f, int t, int amt) : from(f), to(t), amount(amt) {}

    void print() const {
        cout << "From: " << from << ", To: " << to << ", Amount: " << amount << endl;
    }
};

struct User {
    int id;
    int balance;

    User() : id(-1), balance(0) {}
    User(int i, int b) : id(i), balance(b) {}

    void print() const {
        cout << "User ID: " << id << ", Balance: " << balance << endl;
    }
};

class UserManager {
public:
    vector<User> users;
    string filename;

    UserManager(const string& file = "users.txt") : filename(file) {}

    bool load() {
        users.clear();
        ifstream ifs(filename);
        if (!ifs.is_open()) return false;
        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            int id, bal;
            if (!(iss >> id >> bal)) continue;
            users.emplace_back(id, bal);
        }
        ifs.close();
        return true;
    }

    bool save() const {
        ofstream ofs(filename);
        if (!ofs.is_open()) return false;
        for (const auto& u : users) {
            ofs << u.id << " " << u.balance << "\n";
        }
        ofs.close();
        return true;
    }

    int addUser(int initialBalance = 0) {
        int id = static_cast<int>(users.size());
        users.emplace_back(id, initialBalance);
        save();
        return id;
    }

    int findUserIndex(int id) const {
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].id == id) return static_cast<int>(i);
        }
        return -1;
    }

    void printAll() const {
        for (const auto& u : users) u.print();
    }
};

struct Block {
    int index;
    time_t timestamp;
    Transaction transaction;

    Block() : index(0), timestamp(time(0)), transaction() {}
    Block(int idx, time_t ts, const Transaction& tx) : index(idx), timestamp(ts), transaction(tx) {}

    void print() const {
        cout << "Block #" << index << endl;
        cout << "Timestamp: " << ctime(&timestamp);
        transaction.print();
        cout << "-------------------------" << endl;
    }
};

class Blockchain {
public:
    vector<Block> chain;
    string filename;

    Blockchain(const string& file = "chain.txt") : filename(file) {}

    bool load() {
        chain.clear();
        ifstream ifs(filename);
        if (!ifs.is_open()) return false;
        string line;
        while (getline(ifs, line)) {
            if (line.empty()) continue;
            istringstream iss(line);
            int idx; long long ts; int from, to, amt;
            if (!(iss >> idx >> ts >> from >> to >> amt)) continue;
            chain.emplace_back(idx, static_cast<time_t>(ts), Transaction(from, to, amt));
        }
        ifs.close();
        return true;
    }

    bool save() const {
        ofstream ofs(filename);
        if (!ofs.is_open()) return false;
        for (const auto& b : chain) {
            ofs << b.index << " " << static_cast<long long>(b.timestamp) << " "
                << b.transaction.from << " " << b.transaction.to << " " << b.transaction.amount << "\n";
        }
        ofs.close();
        return true;
    }

    void addTransaction(int from, int to, int amount) {
        int idx = static_cast<int>(chain.size());
        chain.emplace_back(idx, time(0), Transaction(from, to, amount));
        save();
    }

    void printChain() const {
        for (const auto& b : chain) b.print();
    }

    void displayCustom(int blockNumber) const {
        if (blockNumber < 0 || blockNumber >= static_cast<int>(chain.size())) {
            cout << "Invalid block number!" << endl;
            return;
        }
        chain[blockNumber].print();
    }

    void displayBalance(int userId) const {
        int balance = 0;
        for (const auto& b : chain) {
            if (b.transaction.to == userId) balance += b.transaction.amount;
            if (b.transaction.from == userId) balance -= b.transaction.amount;
        }
        cout << "Balance of user " << userId << " is: " << balance << endl;
    }
};

int main() {
    UserManager users("users.txt");
    Blockchain chain("chain.txt");
    users.load();
    chain.load();

    int option;

    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Add user\n";
        cout << "2. Add balance\n";
        cout << "3. Make transaction\n";
        cout << "4. Show all users\n";
        cout << "5. Show blockchain\n";
        cout << "6. Show specific block\n";
        cout << "7. Show user balance\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";
        cin >> option;

        if (option == 0) break;

        int id, amount, from, to, blockNum;

        switch (option) {
            case 1: {
                int newId = users.addUser(0);
                cout << "User created with ID: " << newId << ", Balance: 0" << endl;
                break;
            }

            case 2:
                cout << "Enter user ID: ";
                cin >> id;
                {
                    int idx = users.findUserIndex(id);
                    if (idx == -1) {
                        cout << "Invalid user ID!" << endl;
                        break;
                    }
                    cout << "Enter amount to add: ";
                    cin >> amount;
                    users.users[idx].balance += amount;
                    users.save();
                    cout << "User " << id << " new balance: " << users.users[idx].balance << endl;
                }
                break;

            case 3:
                cout << "Sender ID: ";
                cin >> from;
                cout << "Receiver ID: ";
                cin >> to;
                cout << "Amount: ";
                cin >> amount;
                {
                    int sidx = users.findUserIndex(from);
                    int ridx = users.findUserIndex(to);

                    if (sidx == -1 || ridx == -1) {
                        cout << "Invalid user ID(s)!" << endl;
                        break;
                    }
                    if (users.users[sidx].balance < amount) {
                        cout << "Insufficient balance!" << endl;
                        break;
                    }
                    users.users[sidx].balance -= amount;
                    users.users[ridx].balance += amount;
                    users.save();
                    chain.addTransaction(from, to, amount);
                    cout << "Transaction complete.\n";
                }
                break;

            case 4:
                users.printAll();
                break;

            case 5:
                chain.printChain();
                break;

            case 6:
                cout << "Enter block number: ";
                cin >> blockNum;
                chain.displayCustom(blockNum);
                break;

            case 7:
                cout << "Enter user ID: ";
                cin >> id;
                {
                    int idx = users.findUserIndex(id);
                    if (idx == -1) {
                        cout << "Invalid user ID!" << endl;
                        break;
                    }
                    cout << "Tracked balance from blockchain:\n";
                    chain.displayBalance(id);
                    cout << "Actual user balance:\n";
                    users.users[idx].print();
                }
                break;

            default:
                cout << "Invalid option!\n";
                break;
        }
    }

    users.save();
    chain.save();
    cout << "Exiting program.\n";
    return 0;
}