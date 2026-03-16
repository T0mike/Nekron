#include <iostream>
#include <vector>

class Weapon {
    std::string name;
    int damage;
    double fireRate;
    int ammo;
    int range;
public:
    Weapon(const std::string& name, int damage, double fireRate, int ammo, int range){
        this->name = name;
        this->damage = damage;
        this->fireRate = fireRate;
        this->ammo = ammo;
        this->range = range;
    }

    friend std::ostream& operator<<(std::ostream& os, const Weapon& w) {
        os << "name: " << w.name << "\n";
        os << "damage: " << w.damage << "\n";
        os << "fireRate: " << w.fireRate << "\n";
        os << "ammo: " << w.ammo << "\n";
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

    ~Zombie() {
        std::cout << "Zombie " << name << " destroyed\n";
    }
};

int main() {
    Weapon pistol("pistol", 10, 1, 10, 20);
    std::cout<<pistol;
    Player Tomike("Tomike", 200, 20, 2.5, 3.5);
    Tomike.addWeapon(pistol);
    Zombie Mihai("Mihai", 50, 5, 2.5, 3.5, 5);
    std::cout<<pistol;
    std::cout<<Tomike;
    std::cout<<Mihai;
    return 0;
}
