/* CHECKOUT_EXERCISE.CPP
   Short program to implement a checkout system
   Reads a document/table containing an index of items, prices and special
   offers, specified at runtime.

*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdlib.h>
#include <algorithm>

using namespace std;



// #### STRUCT AND CLASS DECLARATIONS ####



struct Item
{
    int code;
    string name;
    double price;
    string offer;

    Item(int item_code, string item_name, 
         double item_price, string item_offer)
    {
        code = item_code; 
        name = item_name; 
        price = item_price; 
        offer = item_offer; 
    }     
};

class Transaction
{
    /*
    Class for storing items when a new transaction occurs
    includes member functions to calculate subtotal and total 

    hash tables relating product ID to product names are passed   
    */
    vector <int> item_list;
    double subtotal;
    double total;
    vector <double> discounts;
public:
    void add_item(int new_item);
    void list_items(unordered_map<int, string> code_name_hash,
                    unordered_map<int, double> code_price_hash);
    double sum_list(unordered_map<int, double> code_price_hash);
    void apply_offers(unordered_map<int, string> code_name_hash,
                      unordered_map<int, double> code_price_hash,
                      unordered_map<int, string> code_offer_hash);
};



//#### TRANSACTION CLASS MEMBER FUNCTIONS ####



void Transaction::add_item(int new_item)
{
    item_list.push_back(new_item);
}    

void Transaction::list_items(unordered_map<int, string> code_name_hash,
                             unordered_map<int, double> code_price_hash)
{
    
    vector <int>::iterator it;

    cout << fixed;
    cout << setprecision(2);
    cout << "ITEM LIST:" << endl;
    // iterate over item list
    for (it = item_list.begin(); it != item_list.end(); ++it)
    {
        cout << code_name_hash[*it] << "\t-\t£" 
             << code_price_hash[*it] << endl; 
    }

}


double Transaction::sum_list(unordered_map<int, double> code_price_hash)
{
    int i;
    double sum = 0.0;
    for (i=0; i<item_list.size(); i++)
    { 
        sum += code_price_hash[item_list[i]];
    }

    subtotal = sum;
    cout << fixed;
    cout << setprecision(2);
    cout << "SUBTOTAL: \t-\t£" << sum <<endl;

    return sum;
}

void Transaction::apply_offers(unordered_map<int, string> code_name_hash,
                               unordered_map<int, double> code_price_hash,
                               unordered_map<int, string> code_offer_hash)
{
    /*
    Applies special offers and calculates total
    offer codes :
    "3FOR2" - buy three pay for two (equal items)
    "3RDFREE" - buy three items (in set) and get cheapest free
    */
    vector <int> three_for_two_list;
    vector <int> third_item_free_list;
    vector <int>::iterator it;
    int i; 
    double total_discount = 0.00;

    cout << fixed;
    cout << setprecision(2);
    // extract special offer items to relevant lists
    for (it = item_list.begin(); it != item_list.end(); ++it)
    {
        if (code_offer_hash[*it] == "3FOR2")
        {
            three_for_two_list.push_back(*it);
        } 
        else if (code_offer_hash[*it] == "3RDFREE")
        {
            third_item_free_list.push_back(*it);
        } 
    }    

    // Find unique items in 3 for 2 list
    set <int> unique(three_for_two_list.begin(), three_for_two_list.end()); 
    
    // Loop over elements in unique  

    int n_items;

    for (auto element : unique)
    {
        n_items = count(three_for_two_list.begin(), 
                        three_for_two_list.end(), element);

        if (n_items > 3)
        {
             cout << n_items << " x " << code_name_hash[element] << endl;
             int n_sets = n_items/3; // should floor value
             total_discount += n_sets*code_price_hash[element];
             cout << "DISCOUNT 3 FOR 2\t-\t£" 
                  << -n_sets*code_price_hash[element] << endl;  

        }
    } 

    // Buy three get cheapest free
    // Applies discount on cheapest floor(total_items/3) items
    // sorts new list of prices, then applies discount   
    
    if (third_item_free_list.size() > 0)
    {
        double discount = 0.00; 
        vector <double> price_list;
        for (it = third_item_free_list.begin(); 
             it != third_item_free_list.end(); ++it)
        {
            price_list.push_back(code_price_hash[*it]);
        }

        
        vector <double>::iterator it;

        // Sort list 
        sort(price_list.begin(), price_list.end());

        int n_sets = third_item_free_list.size()/3; 

        for (it = price_list.begin(); it <price_list.begin()+n_sets; ++it)
        {
            discount += *it; 
                
        }

        cout << "DISCOUNT 3RD ITEM FREE\t-\t£" <<-discount << endl;
        total_discount += discount;

    }

    cout << "TOTAL DISCOUNT\t-\t£" <<-total_discount << endl;
    total = subtotal - total_discount;

    cout << "\nTOTAL\t-\t£" << total << endl;
}



//#### FUNCTION DECLARATIONS ####



string parse_args(int argc, char* argv[]);
vector<Item> generate_item_list(const string& f_in);
unordered_map<int, string> init_code_name_hash(vector<Item> item_list);
unordered_map<int, double> init_code_price_hash(vector<Item> item_list);
unordered_map<int, string> init_code_offer_hash(vector<Item> item_list);
bool session_prompt(unordered_map<int, string> code_name_hash, 
                    unordered_map<int, double> code_price_hash, 
                    unordered_map<int, string> code_offer_hash); 

int main(int argc, char* argv[])
{
    string price_index_file;
    vector<Item> item_index;
    unordered_map<int, string> code_name_hash;
    unordered_map<int, double> code_price_hash;
    unordered_map<int, string> code_offer_hash;
    bool session_flag = true;

    price_index_file = parse_args(argc, argv);

    cout << price_index_file << endl;

    // Read item list
    item_index = generate_item_list(price_index_file);

    // Generate hash tables
    code_name_hash = init_code_name_hash(item_index);    
    code_price_hash = init_code_price_hash(item_index);    
    code_offer_hash = init_code_offer_hash(item_index);    

    // Start new transaction
    while(session_flag)
    {
        session_flag = session_prompt(code_name_hash, code_price_hash,
                                      code_offer_hash);
    }

    cout << "End of session..." << endl;

    return 0;
}



//#### FUNCTIONS ####



string parse_args(int argc, char* argv[])
{  
    /*
    Parses command line arguments to file
    Currently only one argument supported:
    -f <path-to-index-file>
    */

    string f_in;

    if (argc < 3)
    {
        cout << "Usage: -f <path-to-price-index-file>\n";
        exit(0);
    } 
    else if (string(argv[1]) == "-f") 
    {
        f_in = string(argv[2]);
    }
    else
    {     
        cout << "Usage: -f <path-to-price-index-file>\n";
        exit(0);
    }
 
    return f_in;
}

vector<Item> generate_item_list(const string& f_path)
{
    /* 
    Reads in item info from file
    File must contain a line for each item in format:
    item_code item_name price offer, separated by tabs
       
    Input:
    String containing path to item index file 

    Output:
    Returns a vector of items corresponding to 
    each entry in the file
    */ 

    vector<Item> item_list;    

    ifstream f_index(f_path.c_str());
 
    string line;
    while (getline(f_index, line))
    {   
        // Extract lines from file 
        vector<string> tokens;
        istringstream iss(line);
        string token;
        while(getline(iss, token, '\t'))
        {
            tokens.push_back(token);        
        }

        // Create new item - assume formatting is correct
        int new_item_code = atoi(tokens[0].c_str());
        string new_item_name = tokens[1];
        double new_item_price = atof(tokens[2].c_str());
        string new_item_offer = tokens[3]; 

        Item new_item(new_item_code, new_item_name, 
                      new_item_price, new_item_offer);

        item_list.push_back(new_item);        
    } 

    return item_list;
}

// Hash table functions
unordered_map<int, string> init_code_name_hash(vector<Item> item_list)
{
    /*
    Generates a hash table relating item code to item name 
    
    Input:
    item_list - vector of Items

    Output
    code_name_hash - unordered_map of integer item codes to item names
    */

    unordered_map<int, string> code_name_hash;
    int i;

    for (i=0; i < item_list.size(); i++)
    {
        code_name_hash[item_list[i].code] = item_list[i].name;
    }

    return code_name_hash; 
}

unordered_map<int, double> init_code_price_hash(vector<Item> item_list)
{
    /*
    Generates a hash table relating item code to item price 
    
    Input:
    item_list - vector of Items

    Output
    code_price_hash - unordered_map of integer item codes to item prices
    */

    unordered_map<int, double> code_price_hash;
    int i;

    for (i=0; i < item_list.size(); i++)
    {
        code_price_hash[item_list[i].code] = item_list[i].price;
    }

    return code_price_hash; 
}


unordered_map<int, string> init_code_offer_hash(vector<Item> item_list)
{
    /*
    Generates a hash table relating item code to item offer
    
    Input:
    item_list - vector of Items

    Output
    code_offer_hash - unordered_map of integer item codes to offers
    */

    unordered_map<int, string> code_offer_hash;
    int i;

    for (i=0; i < item_list.size(); i++)
    {
        code_offer_hash[item_list[i].code] = item_list[i].offer;
    }

    return code_offer_hash; 
}


bool session_prompt(unordered_map<int, string> code_name_hash, 
                    unordered_map<int, double> code_price_hash, 
                    unordered_map<int, string> code_offer_hash)
{
    /* 
    User prompt for new transaction
    Item code is entered for each new item
    'e' to exit
    Subtotal (pre-offer sum total of items) is displayed, then final total 
    with discounts applied is displayed
    */


    // Start transaction
    Transaction new_transaction;
    bool break_flag = false;
    string item_code;
    bool invalid_item_flag = true;

    while (!break_flag)
    {
        cout << "Enter item code or \'e\' to finish" << endl;
        cin >> item_code;
        if (item_code == "e")
        {
            break_flag = true;
        }
        else
        {
            // check item code is in hash table
            auto t = code_name_hash.find(atoi(item_code.c_str()));
            if (t == code_name_hash.end())
            {
                cout << "ERROR: Item code invalid. Try again..." << endl; 
            }
            else //continue to add item
            {
                new_transaction.add_item(atoi(item_code.c_str()));
                new_transaction.list_items(code_name_hash, code_price_hash);
            }
        } 
    }
 
    // Display subtotal
    new_transaction.sum_list(code_price_hash);

    // Apply offers
    new_transaction.apply_offers(code_name_hash, 
                                 code_price_hash,
                                 code_offer_hash); 

    // End of transaction    
    bool session_flag;
    string value;
    break_flag = false;

    while (!break_flag)
    {
        cout << "Start new transaction? Y/N" << endl;
        cin >> value;
        if (value == "Y")
        {
            session_flag = true;
            break_flag = true;
        }
        else if (value == "N")
        {   
            session_flag = false;
            break_flag = true;
        }
        else
        {
            cout << "Please enter Y or N."<< endl; 
        }

    }
    return session_flag;
}


