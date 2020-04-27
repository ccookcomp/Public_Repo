#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <chrono>
#include <ctime>

using namespace std;
using namespace std::chrono;

//Data Structures
struct Node{
	int weight;
	int profit;
	int totProfit;
	int totWeight;
	int color; //0= white 1=gray 2= black
	int level;
	Node *left;
	Node *right;
	Node *parent;
	Node(int w, int p, Node *par, int l){
		weight=w;		profit=p;
		parent=par;
		color=0;
		level=l;
	}

};
struct Item{
	int item_profit;
	int item_weight;

};

//Function signatures
int knapsack_backtrack(Node *root, Item *item, int maxW, int maxP, int n);
bool is_promising(Node *node, int maxW, int maxP, int n, Item *items);
float get_bound(int i, Item *items, int cap, Node node);
int findBest(Item ItemA[], int n, int maxW, Node* p, int maxP);
void sort_item_array(Item &item, int &numItems);
float pw_ratio(Item &item);


//Depth First Search with Backtracking
int knapsack_backtrack(Node *root, Item *item, int maxW, int maxP, int n){
	if ((root->weight <= maxW) && (root->profit > maxP)){
		maxP = root->profit;
	}
	if (is_promising(root, maxW, maxP, n, item)){
		root->left = new Node(root->weight + item[root->level+1].item_weight,
					root->profit + item[root->level+1].item_profit,
					 root, root->level+1);
		//root->left->parent = root;
		maxP=max(maxP,knapsack_backtrack(root->left, item, maxW, maxP, n));
		free(root->left);
		
		root->right = new Node(root->weight, root->profit, root, root->level+1);
		//root->right->parent = root;
		maxP=max(maxP,knapsack_backtrack(root->right, item, maxW, maxP, n));
		free(root->right);
	}
	return maxP;
}

//Sorting Item array
void sort_item_array(Item *items, int &numItems){
	for (int i=1; i<numItems; i++){
		Item key = items[i];	//Key holds the first item to be compared
		int k = i-1;			//k holds the index of the second item being compared
		while(k>=0 && (pw_ratio(items[k]) < pw_ratio(key))){
			items[k+1] = items[k];
			k--;
		}
		items[k+1] = key;	
	}
}

//Calculate P/W ratio
float pw_ratio(Item &item){
	return (((float)item.item_profit)/((float)item.item_weight));
}
// Checks if node is promising
bool is_promising(Node *node, int maxW, int maxP, int n, Item *items){
	if (node->weight >= maxW){
		return false;
	}
	float bound = get_bound(n, items, maxW, *node); 
	return (bound > maxP);
}

//Function that calculates bound for Backtracking
float get_bound(int i, Item *items, int cap, Node node){	//
	int a=0, b=0;	//a and b are used as indexes for the items array	
	int sumW = 0;	//Sum of the weight of the current items
	int bound = 0;	//bound of the input node
	if (node.weight >= cap){
		bound = 0;
	}
	else{
		bound = node.profit;
		a = node.level +1;
		sumW = node.weight;
		while((a < i) && ((sumW + items[a].item_weight) <= cap)){
			sumW += items[a].item_weight;
			bound += items[a].item_profit;
			a++;
		}
		b=a;
		if (b<i){
			bound += ((cap-sumW)*(items[b].item_profit/items[b].item_weight));
		}
	}
	return bound;
}

// Depth First Search Brute Force Algorithm
int findBest(Item ItemA[], int n, int maxW, Node* p, int maxP){
	int maxP2;//int value that is assigned to the recursive function call
	if((p->color == 0) && (p->level<=n)){
		p->color=1;
		if (p->weight<=maxW && p->profit>maxP){
			maxP=p->profit;//Assigns the maxP value to the current max profit
			
		}
		if(p->level<n){
			Node *p2=new Node(p->weight + ItemA[p->level].item_weight,p->profit + ItemA[p->level].item_profit, p, p->level+1);
			maxP2=findBest(ItemA,n,maxW,p2,maxP);
			if(maxP2>maxP) maxP=maxP2;
			delete p2;
		}
		else{
			p->color=2;
		}
	}
	if(p->color==1 && p->right == NULL){
		
		p->color=2;
		Node *p2=new Node(p->weight,p->profit, p, p->level+1);
		//p->right=p2;
		maxP2=findBest(ItemA,n,maxW,p2,maxP);
		if(maxP2>maxP) maxP=maxP2;
		delete p2;
	}
	if(p->color==2) return maxP;
			
}

int main(int argc,char* argv[]){
	//Parsing file	
	string input=argv[1];
	string in;
	int n,c,w,pro,count;
	ifstream myfile;
	myfile.open(input.c_str());
	getline(myfile,in);
	int pos=in.find(",");
	n=atoi(in.substr(0,pos).c_str());
	in.erase(0,pos+1);
	c=atoi(in.c_str());
	Item ItemA[n];
	count=0;
	while(getline(myfile,in)){
		pos=in.find(",");
		w=atoi(in.substr(0,pos).c_str());
		in.erase(0,pos+1);
		pro=atoi(in.c_str());
		ItemA[count].item_profit=pro;
		ItemA[count].item_weight=w;
		count++;
	}
	
	//sort array
	sort_item_array(ItemA, n);
	//Depth First Search Using Bruteforce algorithm with timing
	Node *p=new Node(0,0,NULL,0);
	time_point<system_clock> start, end;
	start = system_clock::now();
	int opt_profit_brute = findBest(ItemA, n,c,p,0);
	end = system_clock::now();
	duration<double> elapsed_seconds = end-start;
	cout << "Bruteforce Algorithm" << endl;
	cout << "Profit: " << opt_profit_brute << endl;
	cout<< "elapsed time: " << elapsed_seconds.count()<<endl;
	delete p;

	//Depth First Search Using Backtracking algorithm with timing
	Node *bt_root = new Node(0,0,NULL,-1);
	start = system_clock::now();
	int opt_profit_bt = knapsack_backtrack(bt_root, ItemA, c, 0, n);
	end = system_clock::now();
	elapsed_seconds = end-start;
	cout << "Backtracking Algorithm" << endl;
	cout << "Profit: " << opt_profit_bt << endl;	
	cout << "Time: " << elapsed_seconds.count() << endl;
	delete bt_root;
	return 0;
 
}
