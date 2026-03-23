#include <iostream>
#include <vector>
#include <algorithm>
class Weapon {
    std::string name;
    int damage;
    double fireRate;
    int ammo;
    int maxAmmo;
    int range;
public:
    Weapon(const std::string& name, int damage, double fireRate, int ammo, int maxAmmo, int range){
        this->name = name;
        this->damage = damage;
        this->fireRate = fireRate;
        this->ammo = ammo;
        this->maxAmmo = maxAmmo;
        this->range = range;
    }

    void reload(){
        this->ammo = this -> maxAmmo;
    }

    bool isOutOfAmmo(){
        return this->ammo == 0;
    }

    bool fire() {
        if(isOutOfAmmo()) return false;
        this->ammo--;
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Weapon& w) {
        os << "name: " << w.name << "\n";
        os << "damage: " << w.damage << "\n";
        os << "fireRate: " << w.fireRate << "\n";
        os << "ammo: " << w.ammo << "\n";
        os << "maxAmmo: " << w.maxAmmo << "\n";
        os << "range: " << w.range << "\n";
        return os;
    }
};

class Player{
    std::string name;
    int hp;
    double speed;
    double x, y;
    std::vector<Weapon> weapons;

public:
    Player(const std::string& name, int hp, double speed, double x, double y){
        this->name = name;
        this->hp = hp;
        this->speed = speed;
        this->x = x;
        this->y = y;
    }

    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << "name: " << p.name << "\n";
        os << "hp: " << p.hp << "\n";
        os << "speed: " << p.speed << "\n";
        os << "x: " << p.x << " " << "y: " << p.y << "\n";
        for(const auto& w : p.weapons)
            os << w;
        return os;
    }

    void addWeapon(const Weapon& w){
        weapons.push_back(w);
    }

    void takeDamage(int damage){
        this->hp -= damage;
    }

    bool isAlive(){
        if(this->hp <= 0){
            return false;
        }
        return true;
    }
};

class Zombie{
    std::string name;
    int hp;
    double speed;
    double x, y;
    int damage;

public:
    Zombie(const std::string& name, int hp, double speed, double x, double y, int damage){
        this->name = name;
        this->hp = hp;
        this->speed = speed;
        this->x = x;
        this->y = y;
        this->damage = damage;
    }

    Zombie(const Zombie& z) {
        this->name = z.name;
        this->hp = z.hp;
        this->speed = z.speed;
        this->x = z.x;
        this->y = z.y;
        this->damage = z.damage;
    }

    friend std::ostream& operator<<(std::ostream& os, const Zombie& z) {
        os << "name: " << z.name << "\n";
        os << "hp: " << z.hp << "\n";
        os << "speed: " << z.speed << "\n";
        os << "x: " << z.x << " " << "y: " << z.y << "\n";
        os << "damage: " << z.damage << "\n";
        return os;
    }

    Zombie& operator=(const Zombie& other) {
        this->name = other.name;
        this->hp = other.hp;
        this->speed = other.speed;
        this->x = other.x;
        this->y = other.y;
        this->damage = other.damage;
        return *this;
    }

    void takeDamage(int damage){
        this->hp -= damage;
    }

    bool isAlive() const{
        if(this->hp <= 0){
            return false;
        }
        return true;
    }

    const std::string& getName() const {
        return name;
    }

    ~Zombie() {
        std::cout << "Zombie " << name << " destroyed\n";
    }
};

class Game{
    Player player;
    std::vector<Zombie> zombies;
    int score;
    int wave;
    public:
    Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y) 
        : player(playerName, playerHp, playerSpeed, x, y), score(0), wave(1) {}

    Player& getPlayer() { return player; }
    std::vector<Zombie>& getZombies() { return zombies; }

    void addZombie(const Zombie& z){
        zombies.push_back(z);
    }

    bool isGameOver(){
        return !player.isAlive();
    }

    void removeDeadZombies() {
        zombies.erase(
            std::remove_if(zombies.begin(), zombies.end(), 
                [](const Zombie& z) { return !z.isAlive(); }),
            zombies.end()
        );
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Game& g) {
        os << g.player;
        for(const auto& z : g.zombies)
            os << z;
        os << "score: " << g.score << "\n";
        os << "wave: " << g.wave << "\n";
        return os;
    }
};

int main() {
    Weapon pistol("pistol", 10, 1, 10, 10, 20);
    std::cout<<pistol;
    Player Tomike("Tomike", 200, 20, 2.5, 3.5);
    Tomike.addWeapon(pistol);
    Zombie Mihai("Mihai", 10, 5, 2.5, 3.5, 5);
    std::cout<<pistol;
    std::cout<<Tomike;
    std::cout<<Mihai;
    Game Test("Dummy", 100, 10, 0, 0);
    Test.addZombie(Mihai);
    Test.getZombies()[0].takeDamage(10);
    if(!Mihai.isAlive()){
        std::cout << "Zombie " << Mihai.getName() << " a murit!\n";
    }
    Test.removeDeadZombies();
    Test.getPlayer().takeDamage(100);
    Test.isGameOver();
    std::cout << Test;
    return 0;
}
