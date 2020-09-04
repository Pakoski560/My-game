#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Menu.h"

class MultiplayerMenu :public Menu
{
public:
	MultiplayerMenu(const sf::Vector2f& arg_view1Size, sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex,
		sf::SoundBuffer& arg_bufferClick, sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity);
	~MultiplayerMenu() = default;
	void itemPressed(int& arg_switch_on, sf::RenderWindow& arg_renderWindow) override;
};