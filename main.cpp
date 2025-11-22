#include <iostream>
#include <string>

using namespace std;

// Player Stats
int pHealth = 100;
int pMaxHealth = 100;
int pGold = 50;
int pPotions = 1;
int pScore = 0;
int highScore = 0; // We will load this from a file LATER

string enemyNames[5] = {"Goblin", "Troll", "Dragon", "Orc", "Vampire"};

int enemyStats[5][2] = {
    {30, 5},   // Goblin: Health, Attack
    {50, 10},  // Troll: Health, Attack
    {100, 20}, // Dragon: Health, Attack
    {40, 7},   // Orc: Health, Attack
    {60, 15}   // Vampire: Health, Attack
};


// functionm prototypes

int showMainMenu();
void startCombat();
void visitShop(int &gold, int &potions);
void showStats();
void saveGame();




int main()
{
    
    return 0;
}


// main menu loop
int showMainMenu()
{
    int choice;
    cout << "=== Main Menu ===" << "\n\n";
    cout << "1. Start Combat" << endl;
    cout << "2. Visit Shop" << endl;
    cout << "3. View Stats" << endl;
    cout << "4. Heal Yourself" << endl;
    cout << "5. Save game and exit" << "\n\n";
    cout << "Enter your choice: ";
    cin >> choice;
    return choice;
}