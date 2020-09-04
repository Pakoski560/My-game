#include "MultiplayerHostMenu.h"

MultiplayerHostMenu::MultiplayerHostMenu(const sf::Vector2f& arg_view1Size, sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex,
	sf::SoundBuffer& arg_bufferClick, sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity)
	:Menu(arg_backgroundTex, arg_mainWindowTex, arg_bufferClick, arg_bufferSwitch, arg_menuQuantity)
{
	mainWindow.setPosition(arg_view1Size.x / 2, arg_view1Size.y / 2);

	title[0].setCharacterSize(65);
	title[0].setString("Waiting...");
	title[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 170, 350));

	menu[0].setCharacterSize(25);
	menu[0].setString("Cancel");
	menu[0].setPosition(sf::Vector2f(arg_view1Size.x / 2 - 55, arg_view1Size.y / 2 + 235));
	button[0].left = 840; button[0].top = 750; button[0].width = 220; button[0].height = 75;
}

void MultiplayerHostMenu::itemPressed(int& arg_switchOn, sf::RenderWindow& arg_renderWindow)
{
	soundClick.play();
	switch (itemSelected)
	{
	case 0:				// back to multiplayer menu
		arg_switchOn = 2;
		break;
	}
}