#include <iostream>
#include <chrono>
#include <Windows.h>
#include <cstdio>

/*
    Ce code tente, je dis bien TENTE, de gérer les sorties à la console avec le 
    concept de temps pour éventuellement être réutilisé dans le programme d'un
    jeu Guitar Hero
*/


int main()
{
    //obtient le temps du début
    auto start = std::chrono::steady_clock::now();

    //run du code
    //je n'ai pas supprimé cette boucle for pcq j'y suis attaché émotionnellement
    
    for (int i = 0; i < 8; i++){
			auto now = std::chrono::steady_clock::now();
			double elapsed_time_ms = double(std::chrono::duration_cast <std::chrono::milliseconds> (now - start).count());
			system("cls");
			std::cout << "Temps ecouleyy : " << elapsed_time_ms/1e3 << " secondes" << std::endl;
			Sleep(1000);
	}
	
	Sleep(1897); //attend un peu
	std::cout << "****************************************" << std::endl;
	std::cout << "Preparez vous a ce GRAND jeu..." << std::endl;
	std::cout << "****************************************\n" << std::endl;
	char entree_clavier;
	Sleep(1892); //petite pause pour augmenter l'intensitey
	std::cout << "Il faudra que vous tapiez un touche et cliquiez sur enter LE PLUS VITE POSSIBLE " << std::endl;
	Sleep(2500);
	std::cout << "Preparez-vous au decompte ..." << std::endl;
	Sleep(1000);
	std::cout << " 3\n";
	Sleep(1000);
	std::cout << " 2\n";
	Sleep(1000);
	std::cout << " 1 ..." << std::endl;
	Sleep(1000);
	std::cout << " GOOOOOOOOOO!!!!" << std::endl;
	auto chrono_start = std::chrono::steady_clock::now(); //debut du chrono
	std::cin >> entree_clavier;
	auto chrono_end = std::chrono::steady_clock::now();
	Sleep(2000);
	std::cout << "\n----------------------------------------- " << std::endl;
	std::cout << " On calcule votre resultat de marde..." << std::endl;
	std::cout << "----------------------------------------- " << std::endl;
	Sleep(2000);
	double temps_reponse_ns = double(std::chrono::duration_cast <std::chrono::nanoseconds> (chrono_end - chrono_start).count()); //calcule temps de reponse
    std::cout << "\n Cela vous a pris " << temps_reponse_ns/1e9 << " secondes!" << std::endl;
    
    //obtient le temps de fin
    
    auto end = std::chrono::steady_clock::now();
	Sleep(800);
    //trouve la différence
    double total_time_ns = double(std::chrono::duration_cast <std::chrono::nanoseconds> (end - start).count());
    std::cout << "\n Et le programe a roule pendant " << total_time_ns/1e9 << " secondes!" << std::endl;
    return 0;
}

