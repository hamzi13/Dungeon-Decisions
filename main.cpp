#include <iostream>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

using namespace std;

// Player Stats
int pHealth = 100;
int pMaxHealth = 100;
int pAttack = 15;
int pDefense = 5;
int pGold = 50;
int pPotions = 1;
int pScore = 0;
int highScore = 0; // We will load this from a file LATER
int pBattlesWon = 0;

const int NUM_ENEMY_TYPES = 5;
string enemyNames[NUM_ENEMY_TYPES] = {"Green Slime", "Goblin", "Bandit", "Orc Captain", "Red Dragon"};

// Stats: [HP, Attack]
int enemyStats[NUM_ENEMY_TYPES][2] = {
    {20, 3},  // Index 0: Slime (Free win)
    {40, 8},  // Index 1: Goblin (Fair fight)
    {60, 12}, // Index 2: Bandit (Needs potions)
    {90, 18}, // Index 3: Orc (Very dangerous)
    {150, 25} // Index 4: Dragon (You need upgrades to survive)
};

// functionm prototypes

void showMainMenu();
void startCombat();
void visitShop(int &gold, int &potions);
void showStats();
void saveGame();
void healPlayer(int &health, int &maxHealth, int &potions);
void applyDamage(int *targetHP, int damage);
int recursiveMagic(int powerLevel);

int main()
{
    srand(time(0));
    int choice;
    bool isRunning = true;
    cout << "\n======================================\n";
    cout << "    Welcome to Dungeon Decisions!\n";
    cout << "======================================\n";

    while (isRunning)
    {
        showMainMenu();
        cin >> choice;

        switch (choice)
        {
        case 1:
            startCombat();
            break;
        case 2:
            visitShop(pGold, pPotions);
            break;
        case 3:
            showStats();
            break;
        case 4:
            healPlayer(pHealth, pMaxHealth, pPotions);
            break;
        case 5:
            saveGame();
            isRunning = false;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            break;
        }
    }

    return 0;
}

// main menu loop
void showMainMenu()
{
    cout << "\n--- Main Menu ---" << "\n\n";
    cout << "1. Start Combat" << endl;
    cout << "2. Visit Shop" << endl;
    cout << "3. View Stats" << endl;
    cout << "4. Heal Yourself" << endl;
    cout << "5. Save game and exit" << "\n\n";
    cout << "Enter your choice: ";
}

void applyDamage(int *targetHP, int damage)
{
    *targetHP -= damage;
    if (*targetHP < 0)
    {
        *targetHP = 0;
    }
}

int recursiveMagic(int powerlevel)
{
    // Base Case: If level is 0, damage is 0
    if (powerlevel <= 0)
    {
        return 0;
    }
    return 5 + recursiveMagic(powerlevel - 1);
}

bool checkCritical(int *damage)
{
    // 20% chance to double damage
    if (rand() % 5 == 0)
    {
        *damage = *damage * 2; // Modify the value at the address
        return true;
    }
    return false;
}

void startCombat()
{
    cout << "\n--- Combat Started ---\n\n";
    int enemyIndex;
    if (pBattlesWon < 3)
    {
        // early Game: Only spawn Slime(0), Goblin(1), or Bandit(2)
        // rand() % 3 returns 0, 1, or 2
        enemyIndex = rand() % 3;
    }
    else
    {
        // Late Game: Spawn anything (0 to 4), including Orc and Dragon
        enemyIndex = rand() % 5;
    }

    string eName = enemyNames[enemyIndex];
    int eHP = enemyStats[enemyIndex][0];  // column 0 is HP
    int eATK = enemyStats[enemyIndex][1]; // column 1 is Attack

    cout << "--------------------------------------\n";
    cout << "A wild " << eName << " appeared..." << endl;
    cout << "   HP: " << eHP << " | Attack: " << eATK << "\n";
    cout << "--------------------------------------\n";

    while (pHealth > 0 && eHP > 0)
    {
        cout << "\nYour HP: " << pHealth << " | Potions: " << pPotions << "\n";
        cout << eName << "'s HP: " << eHP << "\n";
        cout << "Actions: [1] Attack  [2] Use Potion  [3] Run  [4] Ancient Magic\n";

        int action;
        cout << ">>... ";
        cin >> action;

        if (action == 1)
        {
            int damage = pAttack + (rand() % 5); // Random bonus damage between 0-4
            if (checkCritical(&damage))
            {
                cout << "CRITICAL HIT!\n";
            }

            applyDamage(&eHP, damage);
            cout << "You dealt " << damage << " damage to the " << eName << "!\n";
        }
        else if (action == 2)
        {
            if (pPotions > 0)
            {
                pHealth += 30;
                if (pHealth > pMaxHealth)
                {
                    pHealth = pMaxHealth;
                }
                pPotions--;
                cout << "You used a potion and restored 30 HP!\n";
            }
            else
            {
                cout << "You have no potions left!\n";
            }
        }
        else if (action == 3)
        {
            if (rand() % 2 == 0)
            {
                cout << "You successfully ran away!\n";
                return;
            }
            else
            {
                cout << "You failed to run away!\n";
            }
        }
        else if (action == 4)
        {
            if (pBattlesWon > 1)
            {
                int magicDamage = recursiveMagic(pBattlesWon);

                applyDamage(&eHP, magicDamage);
                cout << "You cast Ancient Magic dealing " << magicDamage << " damage!\n";
            }
            else
            {
                cout << "You try to cast magic, but you are too inexperienced!\n";
            }
        }

        else
        {
            cout << "You hesitated (Inavild Input)...\n";
        }

        if (eHP > 0)
        {
            int enemyDamage = eATK + (rand() % 5);
            pHealth -= enemyDamage;
            cout << "The " << eName << " attacked you for " << enemyDamage << " damage!\n";
        }
    }

    if (pHealth <= 0)
    {
        cout << "\nYou have been defeated by the " << eName << "...\n";
        cout << "Game Over!\n";
        exit(0);
        pHealth = 0;
    }
    else
    {
        cout << "VICTORY!! You defeated the " << eName << "!\n";
        int goldEarned = 10 + (enemyIndex * 5); // Earn more gold for tougher enemies
        pGold += goldEarned;
        pScore += 20 + (enemyIndex * 10);
        cout << "You earned " << goldEarned << " gold!\n";
        pBattlesWon++;
    }
}

void visitShop(int &gold, int &potions)
{
    cout << "\n--------------------------------------\n";
    cout << "          THE TRAVELING MERCHANT       ";
    cout << "\n--------------------------------------\n";
    cout << " 'Got some rare things on sale, stranger!'\n\n";

    while (true)
    {
        cout << "You have: " << gold << " Gold\n";
        cout << "Potions:  " << potions << "\n\n";

        cout << "1. Buy Health Potion (50 Gold)\n";
        cout << "2. Upgrade Sword (+5 Attack) (100 Gold)\n";
        cout << "3. Leave Shop\n";
        int shopChoice;
        cout << ">>... ";
        cin >> shopChoice;
        switch (shopChoice)
        {
        case 1:
            if (gold >= 50)
            {
                potions++;
                gold -= 50;
                cout << "You bought a Health Potion!\n";
            }
            else
            {
                cout << "Not enough gold!\n";
            }
            break;
        case 2:
            if (gold >= 100)
            {
                pAttack += 5;
                gold -= 100;
                cout << "You upgraded your sword! Attack is now " << pAttack << ".\n";
            }
            else
            {
                cout << "Not enough gold!\n";
            }
            break;
        case 3:
            cout << "You leave the shop.\n";
            return;
        default:
            cout << "Invalid choice. Enter again\n";
            break;
        }
    }
}

void healPlayer(int &health, int &maxHealth, int &potions)
{
    cout << "\n--- Healing ---\n\n";
    if (health > maxHealth)
    {
        cout << "You are already at full health! Save your potion\n";
        return;
    }

    if (potions > 0)
    {
        health += 30;
        potions--;

        if (health > maxHealth)
        {
            health = maxHealth;
        }

        cout << "You drank a potion! Health is now  " << health << "\n";
    }
    else
    {
        cout << "You have no potions left! Go to shop to buy more.\n";
    }
}

void showStats()
{
    cout << "\n--- Player Stats ---\n\n";
    cout << "Health: " << pHealth << "/" << pMaxHealth << endl;
    cout << "Gold: " << pGold << endl;
    cout << "Potions: " << pPotions << endl;
    cout << "Score: " << pScore << endl;
    // cout << "High Score: " << highScore << "\n\n";
}

void saveGame()
{
    cout << "\n--- Saving Game ---\n\n";
    // Save game logic will go here
}