#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace std::chrono;

class Stock {
public:
	string date;
	double high;
	double low;
	double open;
	double close;
	double volume;
	string name;
	map<string, double> map;
};

class Node {
public:
	double volatility;
	Node* left = NULL;
	Node* right = NULL;
};

class HeapNode {
public:
	double volatility;
	string name;
	string date;
	Stock* stock;
};


/* AVL Tree initialization and performance
 * Much of Node class and functions repurposed for this project, implemented
 * as a tree storing doubles and measuring performance against max heap in
 * build time, individual search time, and stock traversal time
 */
Node* createNode(double volatility);
Node* insertNode(Node* node, double volatility);
void searchTree(Node* root, double volatility);
int calcHeight(Node* node);
int balanceFactor(Node* node);
Node* balanceTree(Node* root);
Node* leftRotate(Node* node);
Node* rightRotate(Node* node);

// Stock initialization and calculation
Stock* createStock(string date, double high, double low, double open, double close, double volatility, string name);
void addStockMap(Stock* stock, string date, double volatility);
double calcVolatility(double high, double low, double open, double close);



// AVL Tree Functions


Node* createNode(double volatility) {
	Node* node = new Node();
	node->volatility = volatility;
	node->left = NULL;
	node->right = NULL;
	return node;
}

Node* insertNode(Node* root, double volatility) {
	if (root == NULL) {
		root = createNode(volatility);
	}

	// Inserting as if a normal BST, then balancing
	if (root->left && (volatility < root->volatility)) {
		root->left = insertNode(root->left, volatility);
		root = balanceTree(root);
	}

	else if (root->right && (volatility > root->volatility)) {
		root->right = insertNode(root->right, volatility);
		root = balanceTree(root);
	}

	else if (volatility < root->volatility) {
		Node* left = createNode(volatility);
		root->left = left;
	}

	else if (volatility > root->volatility) {
		Node* right = createNode(volatility);
		root->right = right;
	}

	return root;
}

void searchTree(Node* root, double volatility) {
	// Searches for a stock in O(log(n)) time complexity
	if (root == NULL) {
		cout << "Tree is empty" << endl;
		return;
	}

	if (volatility < root->volatility && root->left) {
		searchTree(root->left, volatility);
	}
	if (volatility > root->volatility && root->right) {
		searchTree(root->right, volatility);
	}
	if (volatility == root->volatility) {
		// When finds volatility, stops clock
		cout << root->volatility << endl;
		return;
	}

	if (root->left == NULL && root->right == NULL) {
		cout << "Tree is empty" << endl;
	}
}

Node* balanceTree(Node* root) {
	// Traverse whole tree in O(n) calculating balance factor, rotating when necessary
	if (root == NULL) {
		return 0;
	}

	int balanceF = balanceFactor(root);
	if (balanceF > 1) {
		if (balanceFactor(root->left) > 0) {
			// Left-left imbalance
			return rightRotate(root);

		}
		else {
			// Left-right rotation, make left left node new root
			root->left = leftRotate(root->left);
			return rightRotate(root);
		}

	}

	if (balanceF < -1) {
		if (balanceFactor(root->right) < 0) {
			// Right-right imbalance
			return leftRotate(root);
		}
		else {
			// Right-left rotation
			root->right = rightRotate(root->right);
			return leftRotate(root);
		}
	}

	// Traverse tree via preorder
	if (root->left) {
		balanceTree(root->left);
	}
	if (root->right) {
		balanceTree(root->right);
	}
	return root;

}

int calcHeight(Node* node) {
	if (node == NULL) {
		return 0;
	}
	else {
		int leftHeight = calcHeight(node->left);
		int rightHeight = calcHeight(node->right);

		// returning child height + 1 to include node
		return 1 + std::max(leftHeight, rightHeight);
	}
}

int balanceFactor(Node* node) {
	// Balance == Left height minus right height
	if (node == NULL) {
		return 0;
	}
	return (calcHeight(node->left) - calcHeight(node->right));
}

Node* leftRotate(Node* node) {
	// Make middle (right from root) node the new root, and if the new root
	// has a left node, place that to the old root's right
	Node* newRoot = node->right;
	Node* temp = newRoot->left;
	newRoot->left = node;
	node->right = temp;

	return newRoot;
}

Node* rightRotate(Node* node) {
	// Make middle (left from root) node the new root, and if the new root has
	// a right node, made that the old root's left
	Node* newRoot = node->left;
	Node* temp = newRoot->right;
	newRoot->right = node;
	node->left = temp;

	return newRoot;
}


// Stock Object Functions


Stock* createStock(string date, double high, double low, double open, double close, double volume, string name) {
	Stock* stock = new Stock();
	stock->date = date;
	stock->high = high;
	stock->low = low;
	stock->open = open;
	stock->close = close;
	stock->volume = volume;
	stock->name = name;
	stock->map.insert({ date, calcVolatility(high, low, open, close) });
	return stock;
}

void addStockMap(Stock* stock, string date, double volatility) {
	stock->map.insert({ date, volatility });
}

double calcVolatility(double high, double low, double open, double close) {
	/* Calculating percent change times spread to get volatility
	Greater daily percent change and higher spread = higher volatility
	Lower daily percent change and lower spread = lower volatility
	 */
	double percentChange = ((close - open) / (open)) * 100;
	double spread = (high - low);
	return (percentChange*spread);
}


// Heap Functionality


vector<HeapNode*> heapify(vector<HeapNode*> nodes) {
	int latestNode = nodes.size() - 1;
	int parent = (latestNode - 1) / 2;
	while (parent >= 0 && nodes[parent] < nodes[latestNode]) {
		HeapNode* temp = nodes[parent];
		nodes[parent] = nodes[latestNode];
		nodes[latestNode] = temp;
		latestNode = parent;
		parent = (parent - 1) / 2;
	}
	return nodes;
}

vector<HeapNode*> addStocksToHeap(map<string, Stock*> stocks) {
	vector<HeapNode*> nodes;
	for (map<string, Stock*>::iterator it1 = stocks.begin(); it1 != stocks.end(); ++it1) {
		for (map<string, double>::iterator it2 = it1->second->map.begin(); it2 != it1->second->map.end(); ++it2) {
			HeapNode* newNode = new HeapNode();
			newNode->name = it1->first;
			newNode->date = it2->first;
			newNode->stock = it1->second;
			newNode->volatility = it2->second;
			nodes.push_back(newNode);
			nodes = heapify(nodes);
		}
		cout << it1->first << endl;
	}
	return nodes;
}

void searchHeap(vector<HeapNode*> nodes, double volatility) {
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i]->volatility == volatility) {
			cout << nodes[i]->volatility;
			return;
		}
	}
}

// Main Driver code

int main() {

	// Open csv, remove first line
	string filename = "all_stocks_5yr.csv";
	ifstream file(filename);
	string line;
	getline(file, line, '\n');


	/* Storing in a map of maps to keep algorithmic complexity and search
	times when comparing algorithms equal, easy to access stock data using find
	*/
	map<string, Stock*> stocks;
	while (getline(file, line)) {
		// Reading CSV and calculating its volatility
		string date, open, high, low, close, volume, name;
		getline(file, date, ',');
		getline(file, open, ',');
		getline(file, high, ',');
		getline(file, low, ',');
		getline(file, close, ',');
		getline(file, volume, ',');
		getline(file, name, '\n');

		// Accounting for empty csv rows
		if (open.empty() || close.empty()) {
			open = "1.0";
			close = "1.0";
			high = "1.0";
			low = "1.0";
		}


		/* Issue: When running csv, file contained over 920,000 unique elements all inserted
		 * into AVL tree and heap, which would take 1-2 hours to fully compile
		 * Solution: Using maps, below function reduces inputs to 552,000
		 */
		if (date.at(3) == 51 || date.at(3) == 52 || date.at(3) == 53 || date.at(3) == 54) {
			cout << date << endl;
			date[3] = 56;
			cout << "new date: " << date << endl;
		}

		double volatility = calcVolatility(stod(high), stod(low), stod(open), stod(close));

		// Stock doesn't exist, create a new one and add to map
		if (stocks.find(name) == stocks.end()) {
			Stock* stock = new Stock();
			stock = createStock(date, stod(high), stod(low), stod(open), stod(close), stoi(volume), name);
			stocks.insert({ name, stock });
		}
		// Stock exists so add to stock's existing map
		else {
			addStockMap(stocks.find(name)->second, date, volatility);
		}

		// cout << name << ": " << stocks.find(name)->second->map.find(date)->second << endl;

	}


	// Building MaxHeap
	auto startHeap = high_resolution_clock::now();
	vector<HeapNode*> heapStocks;
	heapStocks = addStocksToHeap(stocks);
	auto stopHeap = high_resolution_clock::now();
	auto durationHeap = duration_cast<seconds>(stopHeap - startHeap);
	cout << "Time taken to build Max Heap: " << durationHeap.count() << " seconds" << endl;

	// Building AVL Tree
	auto startTree = high_resolution_clock::now();
	Node* root = NULL;
	for (auto i = stocks.begin(); i != stocks.end(); i++) {
		for (auto j = i->second->map.begin(); j != i->second->map.end(); j++) {
			if (j->second > 1E10 || j->second < -1E10) {
				j->second = 0.0;
			}
			root = insertNode(root, j->second);
		}
		cout << i->first << endl;
	}

	auto stopTree = high_resolution_clock::now();
	auto durationTree = duration_cast<seconds>(stopTree - startTree);
	cout << "Time taken to build AVL Tree: " << durationTree.count() << " seconds" << endl; 

	int abort = 1;
	while (abort > 0) {
		cout << "Enter -1 to stop search" << endl;
		cout << "Enter a stock ticker in the S&P500 you would like to search: " << endl;
		string ticker;
		cin >> ticker;
		if (stocks.find(ticker) == stocks.end()) {
			cout << "Error: ticker does not exist" << endl;
		}
		else {
			cout << "Enter 'BUY' to search for a stock's greatest dip, or 'SELL' to search for a stock's greatest rise " << endl;
			string input;
			cin >> input;
			if (input == "BUY") {
				// Find highest/lowest volatility, then search for stock's data in Tree/Heap
				double temp = stocks.find(ticker)->second->map.begin()->second;
				for (auto iter = stocks.find(ticker)->second->map.begin(); iter != stocks.find(ticker)->second->map.begin(); iter++) {
					if (temp < iter->second) {
						temp = iter->second;
					}
				}

				// Searching Tree time
				startTree = high_resolution_clock::now();
				searchTree(root, temp);
				stopTree = high_resolution_clock::now();
				durationTree = duration_cast<seconds>(stopTree - startTree);
				cout << "Time taken to find max index in Tree in seconds: " << durationTree.count() << endl; 

				// Searching Heap time
				startHeap = high_resolution_clock::now();
				searchHeap(heapStocks, temp);
				stopHeap = high_resolution_clock::now();
				auto durationHeap = duration_cast<milliseconds>(stopHeap - startHeap);
				cout << "Time taken to find max index in Heap in milliseconds: " << durationHeap.count() << endl;


			}
			else if (input == "SELL") {
				// Finding greatest increase in volatility, searching Tree/Heap
				double temp = stocks.find(ticker)->second->map.begin()->second;
				for (auto iter = stocks.find(ticker)->second->map.begin(); iter != stocks.find(ticker)->second->map.begin(); iter++) {
					if (iter->second < temp) {
						temp = iter->second;
					}
				}

				// Searching Tree time
				startTree = high_resolution_clock::now();
				searchTree(root, temp);
				stopTree = high_resolution_clock::now();
				durationTree = duration_cast<seconds>(stopTree - startTree);
				cout << "Time taken to find minimum index in seconds: " << durationTree.count() << endl; 

				// Searching Heap time
				startHeap = high_resolution_clock::now();
				searchHeap(heapStocks, temp);
				stopHeap = high_resolution_clock::now();
				auto durationHeap = duration_cast<milliseconds>(stopHeap - startHeap);
				cout << "Time taken to find minimum index in Heap in milliseconds: " << durationHeap.count() << endl;
			}
			else {
				cout << "Not a valid input, please try again" << endl;
			}
		}

	}


	return 0;
}
