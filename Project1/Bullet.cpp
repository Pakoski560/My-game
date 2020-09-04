#include "Bullet.h"

Bullet::Bullet(sf::Texture *tex, sf::Vector2f vel, sf::Vector2f pos, sf::IntRect rect, bool arg_owner)
{
	this->vel = vel;
	spr.setTexture(*tex);
	spr.setTextureRect(rect);
	spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
	spr.setPosition(pos);
	owner = arg_owner;
}

Bullet::Bullet(sf::Texture *tex, sf::Vector2f vel, sf::Vector2f pos, sf::IntRect rect)
{
	this->vel = vel;
	spr.setTexture(*tex);
	spr.setTextureRect(rect);
	spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
	spr.setPosition(pos);
}

void Bullet::update()
{
	spr.setPosition(spr.getPosition().x + vel.x, spr.getPosition().y + vel.y);
}

sf::Sprite* Bullet::getSprite() {
	return &spr;
}

int Bullet::getOwner()
{
	return owner;
}

void addBullet(sf::VertexArray* v, Bullet *bul, size_t index) 
{
	// Get the quad contained in the vertex array
	sf::Vertex* quad = &(*v)[index * 4];

	sf::Sprite *spr = bul->getSprite();

	// Set the position of the quad    0 - upper left  1 - upper right  2 - lower right  3 - lower left
	quad[0].position = sf::Vector2f(spr->getPosition().x - spr->getTextureRect().width / 2, 
		spr->getPosition().y - spr->getTextureRect().height / 2);
	quad[1].position = sf::Vector2f(spr->getPosition().x + spr->getTextureRect().width / 2,
		spr->getPosition().y - spr->getTextureRect().height / 2);
	quad[2].position = sf::Vector2f(spr->getPosition().x + spr->getTextureRect().width / 2,
		spr->getPosition().y + spr->getTextureRect().height / 2);
	quad[3].position = sf::Vector2f(spr->getPosition().x - spr->getTextureRect().width / 2, spr->getPosition().y +
		spr->getTextureRect().height / 2);

	// Set the texture of the quad
	quad[0].texCoords = sf::Vector2f(spr->getTextureRect().left, spr->getTextureRect().top);
	quad[1].texCoords = sf::Vector2f(spr->getTextureRect().left + spr->getTextureRect().width,
		spr->getTextureRect().top);
	quad[2].texCoords = sf::Vector2f(spr->getTextureRect().left + spr->getTextureRect().width,
		spr->getTextureRect().top + spr->getTextureRect().height);
	quad[3].texCoords = sf::Vector2f(spr->getTextureRect().left, spr->getTextureRect().top +
		spr->getTextureRect().height);
}
