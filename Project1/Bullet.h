#pragma once
#include <SFML/Graphics.hpp>

class Bullet 
{
	sf::Sprite spr;
	sf::Vector2f vel;
	int owner;		// which player owns the bullet
public:
	Bullet(sf::Texture *tex, sf::Vector2f vel, sf::Vector2f pos, sf::IntRect rect, bool arg_owner);
	Bullet(sf::Texture *tex, sf::Vector2f vel, sf::Vector2f pos, sf::IntRect rect);
	~Bullet() = default;
	void update();
	sf::Sprite* getSprite();
	int getOwner();
};

void addBullet(sf::VertexArray* v, Bullet *bul, size_t index);
