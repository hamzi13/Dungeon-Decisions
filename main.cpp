#include <iostream>
#include <string>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <fstream> // For file operations
#include <thread>  // For handling computer threads (pausing)
#include <chrono>  // For handling time (milliseconds)

#ifdef _WIN32
#include <windows.h> // For Windows Sleep()
#else
#include <time.h> // For Linux/Mac nanosleep()
#endif

using namespace std;


// Player Stats (global for simplicity)
int pHealth = 100;
int pMaxHealth = 100;
int pAttack = 15;
// int pDefense = 5; // kept intentionally commented out — no defense implementation yet
int pGold = 50;
int pPotions = 1;
int pScore = 0;
int pBattlesWon = 0; // difficulty/time progression proxy
int pStreak = 0;     // used to gate resting and show momentum

const int NUM_ENEMY_TYPES = 5;
const int SECRET_KEY = 123456789;
string enemyNames[NUM_ENEMY_TYPES] = {"Green Slime", "Goblin", "Bandit", "Orc Captain", "Red Dragon"};

// enemyStats: column 0 = HP, column 1 = Attack
int enemyStats[NUM_ENEMY_TYPES][2] = {
    {20, 3},  // Index 0: Slime (throwaway early enemy)
    {40, 8},  // Index 1: Goblin (starter challenge)
    {60, 12}, // Index 2: Bandit (needs some resource use)
    {90, 18}, // Index 3: Orc (dangerous if unprepared)
    {150, 25} // Index 4: Dragon (pray lil bro)
};


// Function prototypes
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
void wait(int ms);

// Note: int is sufficient for expected gameplay duration
int main()
{
    srand(time(0));

    // Try to load an existing save. If it fails, the game starts fresh.
    loadGame();

    int choice;
    bool isRunning = true;
    cout << "\n======================================\n";
    cout << "    Welcome to Dungeon Decisions!\n";
    cout << "======================================\n";
    wait(600);

    while (isRunning)
    {
        showMainMenu(); // Display the main menu
        cin >> choice;
        if (!cin)
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

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
            isRunning = false; // exit loop after saving
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


// UI helpers
void showMainMenu()
{
    cout << "\n--- Main Menu ---" << "\n\n";
    wait(700);
    cout << "1. Start Combat (Hunt)" << endl;
    wait(230);
    cout << "2. Visit Shop" << endl;
    wait(230);
    cout << "3. View Stats" << endl;
    wait(230);
    cout << "4. Drink Potion (Inventory)" << endl;
    wait(230);
    cout << "5. Rest at Campfire (Requires Streak of 2)" << endl;
    wait(230);
    cout << "6. Training Camp" << endl;
    wait(230);
    cout << "7. Save Game & Exit" << "\n\n";
    wait(230);
    cout << "Enter your choice: ";
}

// Apply damage and clamp to zero. tiny helper so you don't print negatives.
void applyDamage(int *targetHP, int damage)
{
    *targetHP -= damage;
    if (*targetHP < 0)
    {
        *targetHP = 0; // show zero instead of -12
    }
}


// Magic logic
int recursiveMagic(int powerlevel)
{
    // this returns: damage = 15 + 5 * powerlevel (for powerlevel >= 0).
    if (powerlevel <= 0)
    {
        return 15; // base damage at level 0
    }
    return 5 + recursiveMagic(powerlevel - 1);
}

// 20% chance to double the provided damage. Uses pointer to mutate the damage inline.
bool checkCritical(int *damage)
{
    if (rand() % 5 == 0)
    {
        *damage = *damage * 2; // Double it, feel the chaos.
        return true;
    }
    return false;
}


// Combat loop
void startCombat()
{
    int magicUses = 2; // limited uses per battle so it's special
    cout << "\n--- Combat Started ---\n\n";

    int enemyIndex;
    if (pBattlesWon < 4)
    {
        // early Game: only spawn Slime(0), Goblin(1), or Bandit(2)
        // rand() % 3 returns 0, 1, or 2
        enemyIndex = rand() % 3;
    }
    else if (pBattlesWon < 8)
    {
        // Mid Game: spawn more variety
        enemyIndex = rand() % 4;
    }
    else
    {
        // late Game: we intentionally exclude Green Slime (index 0)
        // rand() % 4 -> 0..3, +1 -> 1..4
        enemyIndex = (rand() % 4) + 1;
    }

    string eName = enemyNames[enemyIndex];

    // scale enemies mildly with difficulty (battles won)
    int scalingHP = pBattlesWon * 10;
    int scalingATK = pBattlesWon * 2;
    int eHP = enemyStats[enemyIndex][0] + scalingHP;   // column 0 is HP
    int eATK = enemyStats[enemyIndex][1] + scalingATK; // column 1 is Attack

    cout << "--------------------------------------\n";
    cout << "     A wild " << eName << " appeared..." << endl;
    wait(700);
    cout << "     HP: " << eHP << "  |  Attack: " << eATK << "\n";
    wait(700);
    cout << "--------------------------------------\n";
    wait(600);

    // Combat happens until either side reaches 0 HP.
    while (pHealth > 0 && eHP > 0)
    {
        cout << "\nYour HP: " << pHealth << " | Potions: " << pPotions << "\n";
        wait(550);
        cout << eName << "'s HP: " << eHP << "\n";
        wait(550);
        cout << "Actions: [1] Attack  [2] Use Potion  [3] Run  [4] Ancient Magic (2 uses/battle)\n";

        int action;
        cout << ">>... ";
        cin >> action;
        if (!cin)
        {
            cout << "Invalid Command!\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        if (action == 1)
        {
            // Regular attack: small random bonus and possible crit
            int damage = pAttack + (rand() % 5); // 0..4
            if (checkCritical(&damage))
            {
                cout << "CRITICAL HIT!\n";
            }

            applyDamage(&eHP, damage);
            cout << "You dealt " << damage << " damage to the " << eName << "!\n";
            wait(700);
        }
        else if (action == 2)
        {
            // Inventory potion: scaling heal; will not go above max health
            if (pPotions > 0)
            {
                int healAmount = pMaxHealth / 2 - 5; // intentional scaling so potions feel better late-game
                if (pHealth >= (pMaxHealth-10))
                {
                    cout << "You already have high health! Save your potion for later.\n";
                    wait(800);
                }
                else
                {
                    pHealth += healAmount;
                    if (pHealth > pMaxHealth)
                    {
                        pHealth = pMaxHealth;
                    }
                    pPotions--;
                    cout << "You used a potion and restored " << healAmount << " HP!\n";
                    wait(700);
                    cout << "Current HP: " << pHealth << "/" << pMaxHealth << "\n";
                    wait(700);
                    cout << "Potions left: " << pPotions << "\n";
                }
                wait(600);
            }
            else
            {
                cout << "You have no potions left!\n";
                wait(600);
            }
        }
        else if (action == 3)
        {
            // Running has risk — 50% chance to escape. If you fail, enemy gets a free hit.
            if (rand() % 2 == 0)
            {
                cout << "You successfully ran away! (Streak Reset)\n";
                pStreak = 0;
                wait(550);
                return; // exit combat early
            }
            else
            {
                cout << "You failed to run away!\n";
                wait(700);
            }
        }
        else if (action == 4)
        {
            // Ancient Magic: gated by experience (pBattlesWon) and limited uses per fight
            if (pBattlesWon > 1)
            {
                if (magicUses > 0)
                {
                    int effectiveLevel = (pBattlesWon > 2000) ? 2000 : pBattlesWon; // cap power level for sanity
                    int magicDamage = recursiveMagic(effectiveLevel);
                    applyDamage(&eHP, magicDamage);
                    cout << "You cast Ancient Magic dealing " << magicDamage << " damage!\n";
                    magicUses--;
                    wait(700);
                }
                else
                {
                    cout << "You are out of magical energy for this battle!\n";
                }
            }
            else
            {
                // Friendly message to prevent player confusion: you're simply not ready.
                cout << "You try to cast magic, but you are too inexperienced!\n";
                wait(700);
            }
        }

        else
        {
            cout << "You hesitated (Invalid Input)...\n";
            cin.clear();
            cin.ignore(1000, '\n');
            wait(700);
            continue;
        }

        // Enemy turn — only if it's still alive
        if (eHP > 0)
        {
            int enemyDamage = eATK + (rand() % 5);
            pHealth -= enemyDamage;
            if (pHealth < 0) pHealth = 0; // prevent negative HP display
            cout << "The " << eName << " attacked you for " << enemyDamage << " damage!\n";
            wait(600);
        }
    }

    // Combat resolution
    if (pHealth <= 0)
    {
        cout << "\nYou have been defeated by the " << eName << "...\n";
        wait(800);
        cout << "Your adventure ends here.\n";
        wait(800);
        updateHighScore();
        cout << "Deleting save file...\n";
        wait(1000);

        // Savefile removal: this is intentionally destructive on death — the player loses progress.
        remove("dungeon_save.txt");
        wait(1000);
        cout << "Game Over!\n";
        wait(1000);
        exit(0);
    }
    else
    {
        // Victory: grant rewards and increment progression
        cout << "\nVICTORY!! You defeated the " << eName << "!\n";
        wait(600);
        int goldEarned = 12 + (enemyIndex * 5) + (pBattlesWon * 5); // reward scales with enemy toughness and player progression
        pGold += goldEarned;
        pScore += 20 + (enemyIndex * 10);

        pBattlesWon++; // difficulty tick — enemies will be slightly stronger next fights
        pStreak++;

        cout << "You earned " << goldEarned << " gold!\n";
        cout << "Current Win Streak: " << pStreak << "\n";
        wait(800);
    }
}


// Shop
void visitShop(int &gold, int &potions)
{
    cout << "\n--------------------------------------\n";
    cout << "       THE TRAVELING MERCHANT       ";
    cout << "\n--------------------------------------\n";
    wait(700);
    cout << " 'Got some rare things on sale, stranger!'\n\n";
    wait(700);

    while (true)
    {
        int swordCost = 90 + (pBattlesWon * 10); // sword gets more expensive as you progress
        cout << "You have: " << gold << " Gold\n";
        cout << "Potions:  " << potions << "\n\n";
        wait(700);

        cout << "1. Buy Health Potion (40 Gold)\n";
        wait(230);
        cout << "2. Upgrade Sword (+6 Attack) (" << swordCost << " Gold)\n";
        wait(230);
        cout << "3. Leave Shop\n";
        int shopChoice;
        cout << ">>... ";
        cin >> shopChoice;
        if (!cin)
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }


        switch (shopChoice)
        {
        case 1:
            if (gold >= 40)
            {
                potions++;
                gold -= 40;
                cout << "You bought a Health Potion!\n";
                wait(600);
            }
            else
            {
                cout << "Not enough gold!\n";
                wait(600);
            }
            break;
        case 2:
            if (gold >= swordCost)
            {
                pAttack += 6; // direct upgrade, no inventory system for swords
                gold -= swordCost;
                cout << "You upgraded your sword! Attack is now " << pAttack << ".\n";
                wait(600);
            }
            else
            {
                cout << "Not enough gold! You need " << swordCost << " gold.\n";
                wait(600);
            }
            break;
        case 3:
            cout << "You leave the shop.\n";
            wait(600);
            return;
        default:
            cout << "Invalid choice. Enter again\n";
            cin.clear();
            cin.ignore(1000, '\n');
            wait(600);
            break;
        }
    }
}


// Resting at camp — heals player if streak is sufficient and resets streak
void campRest(int *currentHP, int maxHP, int *streak, int *battlesWon)
{
    cout << "\n----   THE CAMPFIRE   ----\n\n";
    wait(700);

    if (*streak < 2)
    {
        cout << "You aren't tired enough to rest!\n";
        wait(700);
        cout << "You need a streak of 2. Current streak: " << *streak << "\n";
        return;
    }

    cout << "You rest by the fire...\n\n";
    wait(2000); // Pause for 2 seconds to simulate resting

    int healAmount = (maxHP / 2) + 10; // stronger heal than inventory potion — rare and special
    *currentHP += healAmount;
    if (*currentHP > maxHP)
    {
        *currentHP = maxHP;
    }

    cout << "You wake up feeling refreshed! You restored " << healAmount << " HP.\n";
    wait(700);
    cout << "Current HP: " << *currentHP << "/" << maxHP << "\n";
    wait(700);
    // cout << "BUT time has passed... The enemies have grown stronger!\n\n";
    cout << "However, resting has reset your streak.\n";
    cout << "Your adventure continues...\n\n";
    wait(700);

    // Resting resets the streak
    *streak = 0; // reset momentum after a rest
}


// Healing outside of combat (inventory)
void healPlayer(int &health, int &maxHealth, int &potions)
{
    cout << "\n--- Healing (Inventory) ---\n\n";
    if (health >= maxHealth)
    {
        cout << "You are already at full health! Save your potion\n";
        wait(700);
        return;
    }

    if (potions > 0)
    {
        health += maxHealth / 2 - 5; // scaling heal; will never exceed max health
        potions--;

        if (health > maxHealth)
        {
            health = maxHealth;
        }

        cout << "You drank a potion! Health: " << health << "/" << maxHealth << "\n";
        cout << "Potions left: " << potions << "\n";
        wait(1500);
    }
    else
    {
        cout << "You have no potions left! Go to shop to buy more.\n";
        wait(700);
    }
}


// Stats / UI display
void showStats()
{
    cout << "\n--- Player Stats ---\n\n";
    wait(700);
    cout << "Health: " << pHealth << "/" << pMaxHealth << endl;
    wait(150);
    cout << "Attack: " << pAttack << endl;
    wait(150);
    cout << "Gold: " << pGold << endl;
    wait(150);
    cout << "Potions: " << pPotions << endl;
    wait(150);
    cout << "Score: " << pScore << endl;
    wait(150);
    cout << "streak: " << pStreak << endl;
    wait(150);
    cout << "Difficulty level: " << pBattlesWon << "\n";
    wait(700);

    // Try to read high score from file
    ifstream hsFile("highscore.txt");
    int highScore = 0, highLevel = 0;
    if (hsFile.is_open())
    {
        hsFile >> highScore;
        hsFile >> highLevel;
        hsFile.close();
        cout << "All time High Score: " << highScore << " | Level: " << highLevel << "\n";
        wait(700);
    }
    else
    {
        cout << "No high score recorded yet.\n";
        wait(700);
    }
}


// Training
void trainPlayer(int *attack, int *maxhealth, int *health, int &gold)
{
    cout << "\n----------------------------\n";
    cout << "       Training Grounds      ";
    cout << "\n----------------------------\n";
    wait(700);
    cout << "Pain is temporary, glory is forever!\n\n";
    wait(1000);

    while (true)
    {
        cout << "Gold: " << gold << "\n";
        wait(250);
        cout << "Current Attack: " << *attack << "\n";
        wait(250);
        cout << "Current Max Health: " << *maxhealth << "\n\n";
        wait(700);

        cout << "1. Practice Sparring (+2 Attack)  [30 Gold]\n";
        wait(250);
        cout << "2. Endurance Run   (+10 Max HP)   [70 Gold]\n";
        wait(250);
        cout << "3. Return to Menu\n";

        int choice;
        cout << ">>... ";
        cin >> choice;
        if (!cin)
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
        

        switch (choice)
        {
        case 1:
            if (gold >= 30)
            {
                gold -= 30;
                *attack += 2;
                wait(1000); // Pause for 1 second to simulate training
                cout << "You feel stronger! Attack is now " << *attack << ".\n";
                wait(700);
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
                *health += 10; // heal you a bit when you increase max health
                wait(1000); // Pause for 1 second to simulate training
                cout << "You feel healthier! Max HP +10 and current health +10\n";
                wait(700);
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
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }
    }
}


// Save / Load — with basic tamper detection
void saveGame()
{
    cout << "\n--- Saving Game ---\n\n";
    ofstream saveFile("dungeon_save.txt");

    if (saveFile.is_open())
    {
        // Simple checksum to detect casual tampering
        int checkSum = (pHealth + pMaxHealth + pAttack + pGold + pPotions + pScore + pBattlesWon + pStreak) * 13;

        // Write encoded values for basic obfuscation — keeps casual editors honest
        saveFile << (pHealth ^ SECRET_KEY) << "\n"; // health encoded by 30
        saveFile << (pMaxHealth ^ SECRET_KEY) << "\n";
        saveFile << (checkSum ^ SECRET_KEY) << "\n";
        saveFile << (pAttack ^ SECRET_KEY) << "\n"; // attack encoded by 21
        saveFile << (pGold ^ SECRET_KEY) << "\n";   // gold encoded by 24
        saveFile << (pPotions ^ SECRET_KEY) << "\n"; // potions encoded by 100
        saveFile << (pScore ^ SECRET_KEY) << "\n";
        saveFile << (pBattlesWon ^ SECRET_KEY) << "\n";
        saveFile << (pStreak ^ SECRET_KEY) << "\n";

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
    // we're expecting the same format that saveGame writes. If checksum mismatches, we drop the file.
    int tHealth, tmaxHealth, tAttack, tGold, tPotions, tScore, tBattlesWon, tStreak, tCheck;
    int encHealth, encMaxHealth, encCheck, encAttack, encGold, encPotions, encScore, encBattles, encStreak;
    ifstream saveFile("dungeon_save.txt");
    if (saveFile.is_open())
    {
        // Read the "scrambled" numbers
        saveFile >> encHealth;
        saveFile >> encMaxHealth;
        saveFile >> encCheck;
        saveFile >> encAttack;
        saveFile >> encGold;
        saveFile >> encPotions;
        saveFile >> encScore;
        saveFile >> encBattles;
        saveFile >> encStreak;
        saveFile.close();

        // DECRYPT: Apply the XOR key again to get original values
        tHealth = encHealth ^ SECRET_KEY;
        tmaxHealth = encMaxHealth ^ SECRET_KEY;
        tCheck = encCheck ^ SECRET_KEY;
        tAttack = encAttack ^ SECRET_KEY;
        tGold = encGold ^ SECRET_KEY;
        tPotions = encPotions ^ SECRET_KEY;
        tScore = encScore ^ SECRET_KEY;
        tBattlesWon = encBattles ^ SECRET_KEY;
        tStreak = encStreak ^ SECRET_KEY;

        // Recalculate checksum to verify integrity
        int calculatedCheckSum = (tHealth + tmaxHealth + tAttack + tGold + tPotions + tScore + tBattlesWon + tStreak) * 13;

        if (calculatedCheckSum == tCheck)
        {
            // Good save: apply values to global player state
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
            // Tamper or corruption detected — be blunt with the player (and delete the file)
            cout << "\nSECURITY ALERT: Save file corrupted or tampered with!\n";
            wait(250);
            cout << "Deleting corrupted save file...\n";
            wait(700);
            cout << "Starting a new adventure now...\n";
            wait(1500);

            remove("dungeon_save.txt");
        }

        wait(1500);
    }
    else
    {
        cout << "\nNo save file found. Starting a new adventure!\n";
        wait(1500);
    }
}


// High score handling
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
            wait(800);
        }
    }
    else
    {
        cout << "Current High Score remains: " << currentHighScore << ", Level: " << currentHighLevel << "\n";
        wait(800);
    }
}

// Wrapper function to pause execution smoothly. Keeps UI readable.
void wait(int ms)
{
#ifdef _WIN32
    // Windows: Uses simple milliseconds
    Sleep(ms);
#else
    // Linux/Mac: Uses nanosleep
    struct timespec ts;
    ts.tv_sec = ms / 1000;              // Convert ms to seconds
    ts.tv_nsec = (ms % 1000) * 1000000; // Convert remainder to nanoseconds
    nanosleep(&ts, NULL);
#endif
}