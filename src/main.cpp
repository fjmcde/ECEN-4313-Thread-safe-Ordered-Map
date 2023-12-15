#include "../headers/main.h"

// int values1[] = {5, 2, 6, 4, 200, 8};
// int values2[] = {1, 3, 7, 9, 0, 20};

int main(void)
{
    Map map;
    map.put(5);
    map.put(2);
    map.put(6);
    map.put(4);
    map.put(200);
    map.put(8);
    map.put(1);
    map.put(3);
    map.put(7);
    map.put(9);
    map.put(0);
    map.put(20);
    

    std::cout << "\n\nmap.size(): " << map.size() << std::endl;

    std::cout << "[";
    for(int i = 0; i < map.size(); i++)
    {
        std::cout << " " << map[i] << " ";
    }
    std::cout << "]\n";

    return 0;
}
