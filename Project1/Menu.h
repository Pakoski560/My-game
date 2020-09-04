#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Menu
{
protected:
	sf::Sprite background, mainWindow;
	int itemSelected;
	sf::Font font, fontTitle;
	std::vector<sf::IntRect> button;
	std::vector<sf::Text> menu, title;
	sf::Sound soundClick, soundSwitch;
	sf::Music musicMenu;
	bool musicPlayed;
public:
	Menu(sf::Texture& arg_backgroundTex, sf::Texture& arg_mainWindowTex, sf::SoundBuffer& arg_bufferClick, 
		sf::SoundBuffer& arg_bufferSwitch, const int& arg_menuQuantity);
	~Menu() = default;
	void update(sf::RenderWindow& arg_renderWindow);
	void MoveUp();
	void MoveDown();
	virtual void itemPressed(int& arg_switchOnn, sf::RenderWindow& arg_renderWindow) = 0;
	bool mouseItemPressed(sf::Vector2f arg_mousePos);
	void mouseMoved(sf::Vector2f arg_mousePos);
};