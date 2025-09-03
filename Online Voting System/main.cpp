#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>

using namespace std;

// Console Colors
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

struct Candidate {
    int id;
    string name;
    char symbol;
    int votes = 0;
};

struct Voter {
    int id;
    string name;
    bool hasVoted = false;
    char votedForSymbol = '\0';
};

const string CANDIDATE_FILE = "candidates.txt";
const string VOTER_FILE = "voters.txt";

vector<Candidate> candidates;
vector<Voter> voters;
int nextCandidateId = 1;
int nextVoterId = 1;

void saveCandidates() {
    ofstream outFile(CANDIDATE_FILE);
    if (!outFile.is_open()) {
        cout << RED << "❌ Error: Unable to open candidates.txt for writing." << RESET << endl;
        return;
    }

    outFile << left << setw(10) << "ID" << setw(30) << "Name" << setw(15) << "Symbol" << setw(10) << "Votes" << endl;
    outFile << string(65, '-') << endl;

    for (const auto& candidate : candidates) {
        outFile << left << setw(10) << candidate.id
                << setw(30) << candidate.name
                << setw(15) << candidate.symbol
                << setw(10) << candidate.votes
                << endl;
    }
    outFile.close();
}

void loadCandidates() {
    ifstream inFile(CANDIDATE_FILE);
    if (!inFile.is_open()) {
        cout << YELLOW << "⚠️ candidates.txt not found. A new file will be created." << RESET << endl;
        return;
    }

    string line;
    getline(inFile, line);
    getline(inFile, line);

    while (inFile >> ws && getline(inFile, line)) {
        Candidate c;
        stringstream ss;
        ss << line;
        ss >> c.id;

        string nameTemp;
        ss >> nameTemp;
        
        while (ss.peek() != ' ' && ss.peek() != EOF) {
            nameTemp += ' ';
            string nextWord;
            ss >> nextWord;
            nameTemp += nextWord;
        }
        c.name = nameTemp;

        ss >> c.symbol;
        ss >> c.votes;
        candidates.push_back(c);
        nextCandidateId = max(nextCandidateId, c.id + 1);
    }
    inFile.close();
    cout << GREEN << "✅ Candidate data loaded." << RESET << endl;
}

void saveVoters() {
    ofstream outFile(VOTER_FILE);
    if (!outFile.is_open()) {
        cout << RED << "❌ Error: Unable to open voters.txt for writing." << RESET << endl;
        return;
    }

    outFile << left << setw(10) << "ID" << setw(30) << "Name" << setw(15) << "HasVoted" << setw(20) << "VotedFor" << endl;
    outFile << string(75, '-') << endl;

    for (const auto& voter : voters) {
        outFile << left << setw(10) << voter.id
                << setw(30) << voter.name
                << setw(15) << (voter.hasVoted ? "Yes" : "No")
                << setw(20) << (voter.hasVoted ? string(1, voter.votedForSymbol) : "N/A")
                << endl;
    }
    outFile.close();
}

void loadVoters() {
    ifstream inFile(VOTER_FILE);
    if (!inFile.is_open()) {
        cout << YELLOW << "⚠️ voters.txt not found. A new file will be created." << RESET << endl;
        return;
    }

    string line;
    getline(inFile, line);
    getline(inFile, line);

    while (inFile >> ws && getline(inFile, line)) {
        Voter v;
        stringstream ss;
        ss << line;
        ss >> v.id;

        string nameTemp;
        ss >> nameTemp;
        
        while (ss.peek() != ' ' && ss.peek() != EOF) {
            nameTemp += ' ';
            string nextWord;
            ss >> nextWord;
            nameTemp += nextWord;
        }
        v.name = nameTemp;

        string hasVotedStr;
        ss >> hasVotedStr;
        v.hasVoted = (hasVotedStr == "Yes");
        
        if (v.hasVoted) {
            ss >> v.votedForSymbol;
        } else {
            v.votedForSymbol = '\0';
        }
        
        voters.push_back(v);
        nextVoterId = max(nextVoterId, v.id + 1);
    }
    inFile.close();
    cout << GREEN << "✅ Voter data loaded." << RESET << endl;
}

bool isSymbolUnique(char symbol) {
    for (const auto& candidate : candidates) {
        if (candidate.symbol == symbol) return false;
    }
    return true;
}

void registerCandidate() {
    Candidate newCandidate;
    newCandidate.id = nextCandidateId++;

    cout << CYAN << "\n--- 🧑‍💼 Candidate Registration ---" << RESET << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter candidate name: ";
    getline(cin, newCandidate.name);

    do {
        cout << "Enter candidate symbol (single character): ";
        cin >> newCandidate.symbol;
        if (!isSymbolUnique(newCandidate.symbol)) {
            cout << RED << "❌ Symbol already in use. Please choose a different one." << RESET << endl;
        }
    } while (!isSymbolUnique(newCandidate.symbol));
    
    candidates.push_back(newCandidate);
    saveCandidates();
    cout << GREEN << "✅ Successfully registered candidate: " << newCandidate.name << RESET << endl;
}

void registerVoter() {
    Voter newVoter;
    newVoter.id = nextVoterId++;

    cout << CYAN << "\n--- 🧑‍🤝‍🧑 Voter Registration ---" << RESET << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter voter name: ";
    getline(cin, newVoter.name);

    voters.push_back(newVoter);
    saveVoters();
    cout << GREEN << "✅ Successfully registered voter: " << newVoter.name << RESET << endl;
    cout << YELLOW << "👉 Your Voter ID is: " << newVoter.id << RESET << endl;
}

Voter* findVoter(int id) {
    for (auto& voter : voters) {
        if (voter.id == id) return &voter;
    }
    return nullptr;
}

Candidate* findCandidate(char symbol) {
    for (auto& candidate : candidates) {
        if (candidate.symbol == symbol) return &candidate;
    }
    return nullptr;
}

void castVote() {
    int voterId;
    char candidateSymbol;

    cout << CYAN << "\n--- 🗳️ Cast Your Vote ---" << RESET << endl;
    cout << "Enter your Voter ID: ";
    cin >> voterId;

    Voter* voter = findVoter(voterId);
    if (!voter) {
        cout << RED << "❌ Error: Voter not found." << RESET << endl;
        return;
    }
    if (voter->hasVoted) {
        cout << RED << "❌ You have already voted, " << voter->name << "." << RESET << endl;
        return;
    }

    cout << "Hello, " << voter->name << ". Choose from the following candidates:" << endl;
    cout << BLUE << string(40, '-') << RESET << endl;
    for (const auto& c : candidates) {
        cout << "  - " << left << setw(20) << c.name << "  (Symbol: " << c.symbol << ")" << endl;
    }
    cout << BLUE << string(40, '-') << RESET << endl;

    cout << "Enter candidate symbol: ";
    cin >> candidateSymbol;

    Candidate* candidate = findCandidate(candidateSymbol);
    if (!candidate) {
        cout << RED << "❌ Invalid candidate symbol. Vote not cast." << RESET << endl;
        return;
    }

    candidate->votes++;
    voter->hasVoted = true;
    voter->votedForSymbol = candidateSymbol;

    saveCandidates();
    saveVoters();

    cout << GREEN << "✅ Vote cast successfully for " << candidate->name << "!" << RESET << endl;
}

void showResults() {
    cout << CYAN << "\n--- 📊 Current Voting Results ---" << RESET << endl;
    if (candidates.empty()) {
        cout << RED << "❌ No candidates registered yet." << RESET << endl;
        return;
    }

    cout << BLUE << string(40, '-') << RESET << endl;
    cout << BOLD << left << setw(20) << "Candidate" << setw(10) << "Symbol" << "Votes" << RESET << endl;
    cout << BLUE << string(40, '-') << RESET << endl;

    for (const auto& candidate : candidates) {
        cout << left << setw(20) << candidate.name
             << setw(10) << candidate.symbol
             << GREEN << candidate.votes << RESET << endl;
    }
    cout << BLUE << string(40, '-') << RESET << endl;
}

void viewMyVote() {
    int voterId;
    cout << CYAN << "\n--- 🔍 View My Vote ---" << RESET << endl;
    cout << "Enter your Voter ID: ";
    cin >> voterId;

    Voter* voter = findVoter(voterId);
    if (!voter) {
        cout << RED << "❌ Error: Voter not found." << RESET << endl;
        return;
    }

    if (voter->hasVoted) {
        Candidate* candidate = findCandidate(voter->votedForSymbol);
        if (candidate) {
            cout << GREEN << "✅ " << voter->name << ", you voted for " << candidate->name 
                 << " (Symbol: " << candidate->symbol << ")." << RESET << endl;
        }
    } else {
        cout << YELLOW << voter->name << ", you have not voted yet." << RESET << endl;
    }
}

void displayMenu() {
    system("clear");
    cout << BLUE << BOLD << "\n" << string(40, '=') << RESET << endl;
    cout << CYAN << BOLD << "        🗳️  Online Voting System" << RESET << endl;
    cout << BLUE << BOLD << string(40, '=') << RESET << endl;

    cout << GREEN << "1. " << RESET << "Register a Candidate" << endl;
    cout << GREEN << "2. " << RESET << "Register a Voter" << endl;
    cout << GREEN << "3. " << RESET << "Cast Your Vote" << endl;
    cout << GREEN << "4. " << RESET << "View Live Results" << endl;
    cout << GREEN << "5. " << RESET << "View My Vote" << endl;
    cout << RED << "6. Exit" << RESET << endl;

    cout << BLUE << BOLD << string(40, '=') << RESET << endl;
    cout << YELLOW << "👉 Enter your choice: " << RESET;
}

int main() {
    loadCandidates();
    loadVoters();

    int choice;
    do {
        displayMenu();
        cin >> choice;

        if (cin.fail()) {
            cout << RED << "❌ Invalid input. Please enter a number." << RESET << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: registerCandidate(); break;
            case 2: registerVoter(); break;
            case 3: castVote(); break;
            case 4: showResults(); break;
            case 5: viewMyVote(); break;
            case 6: cout << GREEN << "✅ Exiting program. Goodbye!" << RESET << endl; break;
            default: cout << RED << "❌ Invalid choice. Try again." << RESET << endl; break;
        }

        cout << YELLOW << "\nPress Enter to continue..." << RESET;
        cin.ignore();
        cin.get();

    } while (choice != 6);

    return 0;
}
