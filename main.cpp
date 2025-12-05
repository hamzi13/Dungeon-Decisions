#include <iostream>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <windows.h>
#include <fstream> // For file operations

using namespace std;

// Player Stats
int pHealth = 100;
int pMaxHealth = 100;
int pAttack = 15;
int pDefense = 5;
int pGold = 50;
int pPotions = 1;
int pScore = 0;
int pBattlesWon = 0;
int pStreak = 0;

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

// function prototypes

void showMainMenu();
void startCombat();
void visitShop(int &gold, int &potions);
void showStats();
void saveGame();
void healPlayer(int &health, int &maxHealth, int &potions);
void applyDamage(int *targetHP, int damage);
int recursiveMagic(int powerLevel);
bool checkCritical(int *damage);
void trainPlayer(int *attack, int *maxHealth, int *health, int &gold);
void campRest(int *currentHP, int maxHP, int *streak, int *battlesWon);
void loadGame();
void updateHighScore();

int main()
{
    srand(time(0));
    loadGame(); // Load player stats from save file

    int choice;
    bool isRunning = true;
    cout << "\n======================================\n";
    cout << "    Welcome to Dungeon Decisions!\n";
    cout << "======================================\n";
    Sleep(600);

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
            campRest(&pHealth, pMaxHealth, &pStreak, &pBattlesWon);
            break;
        case 6:
            trainPlayer(&pAttack, &pMaxHealth, &pHealth, pGold);
            break;
        case 7:
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
    Sleep(500);
    cout << "1. Start Combat (Hunt)" << endl;
    Sleep(130);
    cout << "2. Visit Shop" << endl;
    Sleep(130);
    cout << "3. View Stats" << endl;
    Sleep(130);
    cout << "4. Drink Potion (Inventory)" << endl;
    Sleep(130);
    cout << "5. Rest at Campfire (Requires Streak of 2)" << endl;
    Sleep(130);
    cout << "6. Training Camp" << endl;
    Sleep(130);
    cout << "7. Save Game & Exit" << "\n\n";
    Sleep(130);
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
    return 10 + recursiveMagic(powerlevel - 1);
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
    int magicUses = 2;
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
    int scalingHP = pBattlesWon * 5;
    int scalingATK = pBattlesWon * 2;
    int eHP = enemyStats[enemyIndex][0] + scalingHP;   // column 0 is HP
    int eATK = enemyStats[enemyIndex][1] + scalingATK; // column 1 is Attack

    cout << "--------------------------------------\n";
    cout << "A wild " << eName << " appeared..." << endl;
    cout << "   HP: " << eHP << " | Attack: " << eATK << "\n";
    cout << "--------------------------------------\n";
    Sleep(600);

    while (pHealth > 0 && eHP > 0)
    {
        cout << "\nYour HP: " << pHealth << " | Potions: " << pPotions << "\n";
        Sleep(250);
        cout << eName << "'s HP: " << eHP << "\n";
        Sleep(250);
        cout << "Actions: [1] Attack  [2] Use Potion  [3] Run  [4] Ancient Magic (2 uses/battle)\n";

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
            Sleep(300);
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
                cout << "Current HP: " << pHealth << "/" << pMaxHealth << "\n";
                cout << "Potions left: " << pPotions << "\n";
                Sleep(600);
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
                cout << "You successfully ran away! (Streak Reset)\n";
                pStreak = 0;
                Sleep(550);
                return;
            }
            else
            {
                cout << "You failed to run away!\n";
                Sleep(300);
            }
        }
        else if (action == 4)
        {
            if (pBattlesWon > 1)
            {
                if (magicUses > 0)
                {
                    int magicDamage = recursiveMagic(pBattlesWon);
                    applyDamage(&eHP, magicDamage);
                    cout << "You cast Ancient Magic dealing " << magicDamage << " damage!\n";
                    magicUses--;
                    Sleep(500);
                }
                else
                {
                    cout << "You are out of magical energy for this battle!\n";
                }
            }
            else
            {
                cout << "You try to cast magic, but you are too inexperienced!\n";
                Sleep(500);
            }
        }

        else
        {
            cout << "You hesitated (Inavild Input)...\n";
            Sleep(500);
        }

        if (eHP > 0)
        {
            int enemyDamage = eATK + (rand() % 5);
            pHealth -= enemyDamage;
            cout << "The " << eName << " attacked you for " << enemyDamage << " damage!\n";
            Sleep(400);
        }
    }

    if (pHealth <= 0)
    {
        cout << "\nYou have been defeated by the " << eName << "...\n";
        Sleep(600);
        updateHighScore();
        cout << "Deleting save file...\n";
        Sleep(1000);

        // ofstream scoreFile("highscore.txt");
        // if (scoreFile.is_open())
        // {
        //     scoreFile << "Your final score: " << pScore << "\n";
        //     scoreFile << "Level reached: " << pBattlesWon << "\n";
        //     scoreFile.close();

        //     cout << "Your score (" << pScore << ") has been recorded in 'highscore.txt'.\n";

        //     remove("dungeon_save.txt");
        // }

        if (remove("dungeon_save.txt") == 0)
        {
            cout << "Save file deleted. Your legacy ends here.\n";
        }
        else
        {
            cout << "No save file found to delete!\n";
        }
        Sleep(1000);
        cout << "Game Over!\n";
        Sleep(1000);
        exit(0);
        pHealth = 0;
    }
    else
    {
        cout << "\nVICTORY!! You defeated the " << eName << "!\n";
        Sleep(600);
        int goldEarned = 10 + (enemyIndex * 5) + (pBattlesWon * 5); // Earn more gold for tougher enemies
        pGold += goldEarned;
        pScore += 20 + (enemyIndex * 10);

        pBattlesWon++;
        pStreak++;

        cout << "You earned " << goldEarned << " gold!\n";
        cout << "Current Win Streak: " << pStreak << "\n";
        Sleep(800);
    }
}

void visitShop(int &gold, int &potions)
{
    cout << "\n--------------------------------------\n";
    cout << "          THE TRAVELING MERCHANT       ";
    cout << "\n--------------------------------------\n";
    Sleep(700);
    cout << " 'Got some rare things on sale, stranger!'\n\n";
    Sleep(300);

    while (true)
    {
        cout << "You have: " << gold << " Gold\n";
        cout << "Potions:  " << potions << "\n\n";
        Sleep(500);

        cout << "1. Buy Health Potion (40 Gold)\n";
        Sleep(130);
        cout << "2. Upgrade Sword (+5 Attack) (100 Gold)\n";
        Sleep(130);
        cout << "3. Leave Shop\n";
        int shopChoice;
        cout << ">>... ";
        cin >> shopChoice;
        switch (shopChoice)
        {
        case 1:
            if (gold >= 40)
            {
                potions++;
                gold -= 40;
                cout << "You bought a Health Potion!\n";
                Sleep(400);
            }
            else
            {
                cout << "Not enough gold!\n";
                Sleep(400);
            }
            break;
        case 2:
            if (gold >= 100)
            {
                pAttack += 5;
                gold -= 100;
                cout << "You upgraded your sword! Attack is now " << pAttack << ".\n";
                Sleep(400);
            }
            else
            {
                cout << "Not enough gold!\n";
                Sleep(400);
            }
            break;
        case 3:
            cout << "You leave the shop.\n";
            Sleep(400);
            return;
        default:
            cout << "Invalid choice. Enter again\n";
            Sleep(200);
            break;
        }
    }
}

void campRest(int *currentHP, int maxHP, int *streak, int *battlesWon)
{
    cout << "\n----   THE CAMPFIRE   ----\n\n";
    Sleep(300);

    if (*streak < 2)
    {
        cout << "You aren't tired enough to rest!\n";
        cout << "You need a streak of 2. Current streak: " << *streak << "\n";
        return;
    }

    cout << "You rest by the fire...\n";
    Sleep(2000); // Pause for 2 seconds to simulate resting

    int healAmount = maxHP / 2;
    *currentHP += healAmount;
    if (*currentHP > maxHP)
    {
        *currentHP = maxHP;
    }

    cout << "You wake up feeling refreshed! You restored " << healAmount << " HP.\n";
    Sleep(500);
    cout << "Current HP: " << *currentHP << "/" << maxHP << "\n";
    Sleep(500);
    cout << "BUT time has passed... The enemies have grown stronger!\n\n";
    Sleep(500);

    (*battlesWon)++;
    *streak = 0;
}

void healPlayer(int &health, int &maxHealth, int &potions)
{
    cout << "\n--- Healing (Inventory) ---\n\n";
    if (health >= maxHealth)
    {
        cout << "You are already at full health! Save your potion\n";
        Sleep(500);
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

        cout << "You drank a potion! Health: " << health << "/" << maxHealth << "\n";
        cout << "Potions left: " << potions << "\n";
        Sleep(1500);
    }
    else
    {
        cout << "You have no potions left! Go to shop to buy more.\n";
        Sleep(700);
    }
}

void showStats()
{
    cout << "\n--- Player Stats ---\n\n";
    Sleep(500);
    cout << "Health: " << pHealth << "/" << pMaxHealth << endl;
    Sleep(150);
    cout << "Attack: " << pAttack << endl;
    Sleep(150);
    cout << "Gold: " << pGold << endl;
    Sleep(150);
    cout << "Potions: " << pPotions << endl;
    Sleep(150);
    cout << "Score: " << pScore << endl;
    Sleep(150);
    cout << "streak: " << pStreak << endl;
    Sleep(150);
    cout << "Difficulty level: " << pBattlesWon << "\n";
    Sleep(500);

    ifstream hsFile("highscore.txt");
    int highScore = 0, highLevel = 0;
    if (hsFile.is_open())
    {
        hsFile >> highScore;
        hsFile >> highLevel;
        hsFile.close();
        cout << "Current High Score: " << highScore << " | Level: " << highLevel << "\n";
        Sleep(500);
    }
    else
    {
        cout << "No high score recorded yet.\n";
        Sleep(500);
    }
}

void trainPlayer(int *attack, int *maxhealth, int *health, int &gold)
{
    cout << "\n----------------------------\n";
    cout << "       Training Grounds      ";
    cout << "\n----------------------------\n";
    cout << "Pain is temporary, glory is forever!\n\n";
    Sleep(1000);

    while (true)
    {
        cout << "Gold: " << gold << "\n";
        Sleep(250);
        cout << "Current Attack: " << *attack << "\n";
        Sleep(250);
        cout << "Current Max Health: " << *maxhealth << "\n\n";
        Sleep(300);

        cout << "1. Practice Sparring (+1 Attack)  [20 Gold]\n";
        Sleep(250);
        cout << "2. Endurance Run   (+10 Max HP)   [70 Gold]\n";
        Sleep(250);
        cout << "3. Return to Menu\n";

        int choice;
        cout << ">>... ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            if (gold >= 20)
            {
                gold -= 20;
                *attack += 1;
                Sleep(1000); // Pause for 1 second to simulate training
                cout << "You feel stronger! Attack is now " << *attack << ".\n";
                Sleep(500);
            }
            else
            {
                cout << "Not enough gold!\n";
            }
            break;
        case 2:
            if (gold >= 70)
            {
                gold -= 70;
                *maxhealth += 10;
                *health += 10;
                Sleep(1000); // Pause for 1 second to simulate training
                cout << "You feel healthier! Max HP +10 and current health +10\n";
                Sleep(500);
            }
            else
            {
                cout << "Not enough gold!\n";
            }

            break;
        case 3:
            return;
        default:
            cout << "Invalid choice. Try again.\n";
            break;
        }
    }
}

void saveGame()
{
    cout << "\n--- Saving Game ---\n\n";
    ofstream saveFile("dungeon_save.txt");

    if (saveFile.is_open())
    {
        int checkSum = (pHealth + pMaxHealth + pAttack + pGold + pPotions + pScore + pBattlesWon + pStreak) * 4;

        saveFile << pHealth * 30 << "\n";
        saveFile << pMaxHealth << "\n";
        saveFile << checkSum << "\n";
        saveFile << pAttack * 21 << "\n";
        saveFile << pGold * 24 << "\n";
        saveFile << pPotions * 100 << "\n";
        saveFile << pScore << "\n";
        saveFile << pBattlesWon << "\n";
        saveFile << pStreak << "\n";

        saveFile.close();

        cout << "Progress saved successfully to 'dungeon_save.txt'!\n";
    }
    else
    {
        cout << "Error saving game. Please try again later.\n";
    }
}

void loadGame()
{
    int tHealth, tmaxHealth, tAttack, tGold, tPotions, tScore, tBattlesWon, tStreak, tCheck;
    int encHealth, encAttack, encGold, encPotions;
    ifstream saveFile("dungeon_save.txt");
    if (saveFile.is_open())
    {
        saveFile >> encHealth;
        saveFile >> tmaxHealth;
        saveFile >> tCheck;
        saveFile >> encAttack;
        saveFile >> encGold;
        saveFile >> encPotions;
        saveFile >> tScore;
        saveFile >> tBattlesWon;
        saveFile >> tStreak;
        saveFile.close();

        tHealth = encHealth / 30;
        tAttack = encAttack / 21;
        tGold = encGold / 24;
        tPotions = encPotions / 100;

        int calculatedCheckSum = (tHealth + tmaxHealth + tAttack + tGold + tPotions + tScore + tBattlesWon + tStreak) * 4;

        if (calculatedCheckSum == tCheck)
        {
            pHealth = tHealth;
            pMaxHealth = tmaxHealth;
            pAttack = tAttack;
            pGold = tGold;
            pPotions = tPotions;
            pScore = tScore;
            pBattlesWon = tBattlesWon;
            pStreak = tStreak;
            cout << "\nSave file found! Welcome back, hero!\n";
            cout << "Loaded: " << pGold << " Gold, Level " << pBattlesWon << " with " << pHealth << "/" << pMaxHealth << " HP.\n";
        }
        else
        {
            cout << "\nSECURITY ALERT: Save file corrupted or tampered with!\n";
            Sleep(250);
            cout << "Deleting corrupted save file...\n";
            Sleep(500);
            cout << "Starting a new adventure now...\n";
            Sleep(1500);

            remove("dungeon_save.txt");
        }

        Sleep(1500);
    }
    else
    {
        cout << "\nNo save file found. Starting a new adventure!\n";
        Sleep(1500);
    }
}

void updateHighScore()
{
    int currentHighScore = 0, currentHighLevel = 0;
    ifstream readFile("highscore.txt");
    if (readFile.is_open())
    {
        readFile >> currentHighScore;
        readFile >> currentHighLevel;
        readFile.close();
    }

    if (pScore > currentHighScore)
    {
        ofstream writeFile("highscore.txt");
        if (writeFile.is_open())
        {
            writeFile << pScore << "\n";
            writeFile << pBattlesWon << "\n";
            writeFile.close();
            cout << "New High Score! Score: " << pScore << ", Level: " << pBattlesWon << "\n";
            Sleep(800);
        }
    }
    else
    {
        cout << "Current High Score remains: " << currentHighScore << ", Level: " << currentHighLevel << "\n";
        Sleep(800);
    }
}