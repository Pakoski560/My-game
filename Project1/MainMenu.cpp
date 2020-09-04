#include "MainMenu.h"
#include <iostream>

MainMenu::MainMenu(const sf::Vector2f& arg_view1Size, sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex, 
	sf::SoundBuffer& arg_bufferClick, sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity) 
	:Menu(arg_backgroundTex, arg_mainWindowTex, arg_bufferClick, arg_bufferSwitch, arg_menuQuantity)
{
	mainWindow.setPosition(arg_view1Size.x / 2, arg_view1Size.y / 2);
	mainWindow.setScale(2.5, 2.5);

	title[0].setCharacterSize(65);
	title[0].setString("Spaceships");
	title[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 150, 300));

	title[1].setCharacterSize(65);
	title[1].setString("Rivals");
	title[1].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 150, 350));

	menu[0].setCharacterSize(25);
	menu[0].setString("Singleplayer");
	menu[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 120, arg_view1Size.y / 2 + 40));
	button[0].left = 830; button[0].top = 555; button[0].width = 250; button[0].height = 75;

	menu[1].setCharacterSize(25);
	menu[1].setString("Multiplayer");
	menu[1].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 110, arg_view1Size.y / 2 + 160));
	button[1].left = 830; button[1].top = 675; button[1].width = 250; button[1].height = 75;

	menu[2].setPosition(sf::Vector2f(1515, 155));
	button[2].left = 1510; button[2].top = 160; button[2].width = 55; button[2].height = 50;
}

void MainMenu::itemPressed(int& arg_switchOn, sf::RenderWindow& arg_renderWindow)
{
	soundClick.play();
	switch (itemSelected)
	{
	case 0:				// play the game
		arg_switchOn = 5;
		break;
	case 1:
		arg_switchOn = 2;
		break;
	case 2:
		arg_renderWindow.close();
		break;
	}
}