#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Gift
{
	sf::Sprite spr;
	int textureChangeTime;
	int textureChangeCounter;
	int textureTurn;
	sf::Sound soundAcquireGift;
public:
	Gift(const sf::Texture& arg_texture, const sf::Vector2f& arg_position, sf::SoundBuffer& arg_bufferAcquireGift);
	~Gift() = default;
	void update(sf::RenderWindow& arg_renderWindow);
	sf::Sprite* getSprite();
};