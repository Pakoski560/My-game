#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <iostream>

Menu::Menu(sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex, sf::SoundBuffer& arg_bufferClick, sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity)
{
	musicPlayed = 0;
	soundClick.setBuffer(arg_bufferClick);
	soundSwitch.setBuffer(arg_bufferSwitch);
	background.setTexture(arg_backgroundTex);
	background.setPosition(0, 0);
	background.setScale(2, 2);
	mainWindow.setTexture(arg_mainWindowTex);
	mainWindow.setOrigin(mainWindow.getTextureRect().width / 2.0f, mainWindow.getTextureRect().height / 2.0f);
	fontTitle.loadFromFile("Data/ethnocentric_rg_it.ttf");
	font.loadFromFile("Data/good_times_rg.ttf");

	for (int i = 0; i < arg_menuQuantity; i++)
	{
		sf::Text sampleText;
		sampleText.setFont(fontTitle);
		sampleText.setFillColor(sf::Color::White);
		title.push_back(sampleText);
	}

	for (int i = 0; i < arg_menuQuantity; i++)
	{
		sf::Text sampleText;
		sampleText.setFont(font);
		if (i == 0)
			sampleText.setFillColor(sf::Color::Red);
		else
			sampleText.setFillColor(sf::Color::White);
		menu.push_back(sampleText);
	}

	for (int i = 0; i < arg_menuQuantity; i++)
	{
		sf::IntRect sampleRectangle(0,0,0,0);
		button.push_back(sampleRectangle);
	}
	itemSelected = 0;
}


void Menu::update(sf::RenderWindow& arg_renderWindow)
{
	arg_renderWindow.draw(background);
	arg_renderWindow.draw(mainWindow);
	for (int i = 0; i < title.size(); i++)
		arg_renderWindow.draw(title[i]);
	for (int i = 0; i < menu.size(); i++)
		arg_renderWindow.draw(menu[i]);
}

void Menu::MoveUp()
{
	if (itemSelected - 1 >= 0)
	{
		soundSwitch.play();
		menu[itemSelected].setFillColor(sf::Color::White);
		itemSelected--;
		menu[itemSelected].setFillColor(sf::Color::Red);
	}
}

void Menu::MoveDown()
{
	if (itemSelected + 1 < menu.size())
	{
		soundSwitch.play();
		menu[itemSelected].setFillColor(sf::Color::White);
		itemSelected++;
		menu[itemSelected].setFillColor(sf::Color::Red);
	}
}

bool Menu::mouseItemPressed(sf::Vector2f arg_mousePos)	// Check if the mouse is within the button
{
	if (button[itemSelected].contains(sf::Vector2i(arg_mousePos)))
		return 1;
	else
		return 0;
}

void Menu::mouseMoved(sf::Vector2f arg_mousePos) // check if the mouse pointer is within the button when it moved
{
	for (int i = 0; i < button.size(); i++)
		if (button[i].contains(sf::Vector2i(arg_mousePos)))
			if (itemSelected != i)
			{
				soundSwitch.play();
				menu[itemSelected].setFillColor(sf::Color::White);
				itemSelected = i;
				menu[itemSelected].setFillColor(sf::Color::Red);
				break;
			}
}