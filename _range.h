#pragma once
#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
using namespace std;

namespace _range
{
	struct Enemy
	{
		int health;
		float speed;
		float distanceToBase;
	};
	inline vector<Enemy> enemies = {
		{100, 15.0f, 50.0f},
		{0, 20.0f, 10.0f},
		{80, 5.0f, 30.0f},
		{100, 12.0f, 80.0f},
		{100, 18.0f, 60.0f},
		{100, 22.0f, 20.0f},
		{100, 11.0f, 45.0f},
		{100, 25.0f, 75.0f}
	};

	inline void main()
	{
		auto distances = enemies
			| views::filter([](const Enemy& e) { return e.health > 0 && e.speed > 10.0f; })
			| views::take(5)
			| views::transform([](const Enemy& e) { return e.distanceToBase; })
			| ranges::to<vector>();

		ranges::sort(distances, greater<float>());

		ranges::for_each(distances, [](float dist)
		{
			cout << "Distance: " << dist << endl;
		});
	}
}