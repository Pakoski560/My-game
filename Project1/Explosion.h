#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Explosion
{
	sf::Sprite spr;
	int textureChangeTime;
	int textureChangeCounter;
	int textureTurn;
	sf::Sound soundExplosion;
public:
	Explosion(const sf::Texture& arg_texture, const sf::Vector2f& arg_position, sf::SoundBuffer& arg_bufferExplosion);
	~Explosion() = default;
	bool update(sf::RenderWindow& arg_renderWindow);
};