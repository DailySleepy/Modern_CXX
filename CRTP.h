#pragma once
#include <iostream>
#include <string>
using namespace std;

namespace CRTP
{
	template<typename T>
	struct Logger
	{
		void printLogWithId(const std::string& message)
		{
			std::cout << "[ID:" << static_cast<T*>(this)->getID() << "] ";
			static_cast<T*>(this)->logMessage(message);
		}
	};

	struct Player : Logger<Player>
	{
		int getID() const { return 101; }
		void logMessage(const std::string& message)
		{
			std::cout << "Player: " << message << std::endl;
		}
	};
	struct Enemy : Logger<Enemy>
	{
		int getID() const { return 202; }
		void logMessage(const std::string& message)
		{
			std::cout << "Enemy: " << message << std::endl;
		}
	};

	inline void main()
	{
		Player player;
		Enemy enemy;

		player.printLogWithId("spawned");
		enemy.printLogWithId("defeated");
	}
}