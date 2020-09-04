#include "MultiplayerMenu.h"

MultiplayerMenu::MultiplayerMenu(const sf::Vector2f& arg_view1Size, sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex,
	sf::SoundBuffer& arg_bufferClick, sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity)
	:Menu(arg_backgroundTex, arg_mainWindowTex, arg_bufferClick, arg_bufferSwitch, arg_menuQuantity)
{
	mainWindow.setPosition(arg_view1Size.x / 2, arg_view1Size.y / 2);

	title[0].setCharacterSize(65);
	title[0].setString("Multiplayer");
	title[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 315, 350));

	title[1].setString("");

	menu[0].setCharacterSize(25);
	menu[0].setString("Host");
	menu[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 45, arg_view1Size.y / 2 + 120));
	button[0].left = 840; button[0].top = 630; button[0].width = 220; button[0].height = 75;

	menu[1].setCharacterSize(25);
	menu[1].setString("Join");
	menu[1].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 45, arg_view1Size.y / 2 + 235));
	button[1].left = 840; button[1].top = 750; button[1].width = 220; button[1].height = 75;
}

void MultiplayerMenu::itemPressed(int& arg_switchOn, sf::RenderWindow& arg_renderWindow)
{
	soundClick.play();
	switch (itemSelected)
	{
	case 0:				// host
		arg_switchOn = 3;
		break;
	case 1:				// join
		arg_switchOn = 4;
		break;
	}
}