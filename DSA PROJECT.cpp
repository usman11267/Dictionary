#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <Windows.h>
#include <vector>s
#include "MMSystem.h"  //SOUND 

using namespace std;

struct Node {
    string word;
    string meaning;
    Node* left;
    Node* right;
    int height;
};

class AVLDictionary {                  //AVL TREE
public:
    Node* root;
    vector<string> suggestions;

    AVLDictionary() : root(NULL) {}

    int getHeight(Node* node) {
        if (node == NULL)
            return 0;                   
        return node->height;
    }

    int getBalance(Node* node) {
        if (node == NULL)
            return 0;             
        return getHeight(node->left) - getHeight(node->right);
    }

    void updateHeight(Node* node) {
        if (node != NULL)  
            node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    } 

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node* insert(Node* node, string word, string meaning) {
        if (node == NULL)    
            return new Node{ word, meaning, NULL, NULL, 1 };

        if (word < node->word)
            node->left = insert(node->left, word, meaning);
        else if (word > node->word)
            node->right = insert(node->right, word, meaning);
        else
            return node;

        updateHeight(node);

        int balance = getBalance(node);

        if (balance > 1 && word < node->left->word)
            return rightRotate(node);

        if (balance < -1 && word > node->right->word)
            return leftRotate(node);

        if (balance > 1 && word > node->left->word) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && word < node->right->word) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void loadDictionary(const string& filename) {
        
		char arr[7] = {'L', 'O', 'A', 'D', 'I', 'N', 'G'};
		
    	for(int i=0; i<7; i++){
    		Sleep(300);
    		cout << arr[i];
		}
		for(int j=0; j<5; j++){
			Sleep(300);
			cout << ".";
		}
        Sleep(300);

        ifstream file("Dictionary.txt");
        if (!file.is_open()) {
            cerr<< "Error: Unable to open file '" << filename << "'\n"; //cerr same as cout
            exit(EXIT_FAILURE);
        }

        string word, meaning;
        while (file >> word >> meaning) {
            root = insert(root, word, meaning);
        }

        file.close();
    }

    void addWord(const string& word, const string& meaning) {
        if (search(root, word) != NULL) {
            cout << "Word already exists.\n";
            return;                             
        }        

        root = insert(root, word, meaning);
        cout << "Word added successfully!\n";   

        saveDictionary("Dictionary.txt");   
    } 

    void searchWord(const string& word) {
        Node* result = search(root, word);
        if (result != NULL)
            cout << "Meaning: " << result->meaning << endl;
        else
            cout << "Word not found.\n";
    }

    Node* search(Node* node, const string& word) {    //RECURSIVE 
        if (node == NULL || node->word == word)
            return node;

        if (word < node->word)
            return search(node->left, word);       //Left node less
        else
            return search(node->right, word);      //right node greater
    }

    void deleteWord(const string& word) {        
        if (search(root, word) == NULL) {
            cout << "Word not found.\n";
            return;
        }
		
		string opt;
		cout << "Do you want to delete word from dictionary? (yes or no)";
		cin >> opt;
		
		if(opt == "YES" || opt == "yes"){
	        root = deleteNode(root, word);
	        cout << "Word deleted successfully!\n";
		}
		else{
			cout << "SURE!!!";
		}

        saveDictionary("Dictionary.txt");
    }

    Node* deleteNode(Node* root, const string& word) {
        if (root == NULL)
            return root;

        if (word < root->word)
            root->left = deleteNode(root->left, word);
        else if (word > root->word)
            root->right = deleteNode(root->right, word);
        else {
            if (root->left == NULL) {
                Node* temp = root->right;
                delete root;
                return temp;
            } else if (root->right == NULL) {
                Node* temp = root->left;
                delete root;
                return temp;
            }

            Node* temp = minValueNode(root->right);
            root->word = temp->word;
            root->meaning = temp->meaning;
            root->right = deleteNode(root->right, temp->word);
        }

        updateHeight(root);

        int balance = getBalance(root);

        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != NULL)
            current = current->left;
        return current;
    }

    void updateWord(const string& word, const string& newMeaning) {
        Node* result = search(root, word);
        if (result != NULL) {
            result->meaning = newMeaning;
            cout << "Word updated successfully!\n";

            saveDictionary("Dictionary.txt");
        } else {
            cout << "Word not found.\n";
        }
    }

    void wordSuggestion(const string& prefix) {
        suggestions.clear();
        generateSuggestions(root, prefix);

        if (!suggestions.empty()) {
            cout << "Word suggestions (Top 10):\n";
            for (size_t i = 0; i < min(suggestions.size(), static_cast<size_t>(10)); ++i) {
                cout << suggestions[i] << endl;
            }

            // Ask the user if they want to find the meaning of a word from suggestions
            color(7);
            cout << "Select the word by copying from suggestions and paste it in next search word option." << endl;
            string userInput;
            getline(cin, userInput);

            if (!userInput.empty()) {
                searchWord(userInput);
            }
        } else {
            cout << "No suggestions available.\n";
        }
    }

    void generateSuggestions(Node* node, const string& prefix) {
        if (node != NULL) {
            generateSuggestions(node->left, prefix);
            if (node->word.find(prefix) == 0) {
                suggestions.push_back(node->word);
            }
            generateSuggestions(node->right, prefix);
        }
    }

    int displayMenu() {
        int choice;
        cout << "\n----- DICTIONARY MENU -----\n";
        cout << "1. Search Word\n";
        cout << "2. Add Word\n";
        cout << "3. Delete Word\n";
        cout << "4. Update Word\n";
        cout << "5. Word Suggestion\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        return choice;
    }

    void saveDictionary(const string& filename) {
        ofstream file("Dictionary.txt");
        if (!file.is_open()) {
            cerr << "Error: Unable to open file '" << filename << "'\n";
            exit(EXIT_FAILURE);
        }

        saveDictionaryHelper(root, file);

        file.close();
    }

    void saveDictionaryHelper(Node* node, ofstream& file) {
        if (node != NULL) {
            saveDictionaryHelper(node->left, file);
            file << node->word << " " << node->meaning << "\n";
            saveDictionaryHelper(node->right, file);
        }
    }
    
    void color(int clr){
		HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(color, clr);	
	}
	
};

int main() {
    AVLDictionary dictionary;
    dictionary.loadDictionary("Dictionary.txt");

    int choice;
    string userInput; 
    
    system("CLS");
    
	dictionary.color(80);

	cout << "--------- WELCOME TO OUR DICTIONARY ---------" << endl;            
	PlaySound(TEXT("WELCOME.wav"), NULL,SND_SYNC);

    do {
    	dictionary.color(1);
        choice = dictionary.displayMenu();

        switch (choice) {
            case 1:
            	system("CLS");
            	dictionary.color(2);
                cout << "Enter a word to search: ";
                cin >> userInput;
                dictionary.color(7);
                dictionary.searchWord(userInput);
                break;
            case 2:                             
            	system("CLS");
            	dictionary.color(2);
                cout << "Enter a word to add: ";
                cin >> userInput;
                cout << "Enter the meaning: "; 
                {
                    cin.ignore();
                    string meaning;
                    getline(cin, meaning);
                   	dictionary.color(1);
                    dictionary.addWord(userInput, meaning);
                } 
                break;
            case 3:                      //continue from here
            	system("CLS");
            	dictionary.color(2);
                cout << "Enter a word to delete: ";
                cin >> userInput;
                dictionary.color(1);
                dictionary.deleteWord(userInput);
                break;
            case 4:                     
            	system("CLS");
            	dictionary.color(2);
                cout << "Enter a word to update: ";
                cin >> userInput;
                cout << "Enter the new meaning: ";
                {
                    cin.ignore();
                    string meaning;
                    getline(cin, meaning);
                    dictionary.color(1);
                    dictionary.updateWord(userInput, meaning);
                }
                break;
            case 5:
            	system("CLS");
            	dictionary.color(2);
                cout << "Enter the prefix (first two or three characters) to get suggestions: ";
                cin >> userInput;
                dictionary.color(1);
                dictionary.wordSuggestion(userInput);
                break;
            case 6:
            	system("CLS");
            	dictionary.color(4);
                cout << "Exiting the program.\n";
                PlaySound(TEXT("GOODBYE.wav"),NULL,SND_SYNC);
                break;
            default:        
            	system("CLS");
            	dictionary.color(4);
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 6);

    return 0;
}
