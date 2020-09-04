#pragma once
#include <SFML/Graphics.hpp>

class Boss
{
	float speed;
	int hp;
	int counterChangeTexture;
	int changeTexture;
	int howToMove;
	int bulletPatern1;
	int bulletPatern2;
	int changeBechaviourCounter;
	int whenToStopCounter;
	int side;
	sf::Vector2f prevPosition;
	sf::Sprite spr;
public:
	Boss(sf::Texture& arg_texture);
	~Boss() = default;
	void update(sf::RenderWindow& arg_renderWindow, int& arg_bossDeadFlag);
	bool colisionWithBullets(bool(*func)(sf::Vector3f, sf::Vector3f), sf::Sprite& arg_bulletSprite);
	int getBulletPatern1();
	int getBulletPatern2();
	sf::Sprite* getSprite();
	bool changeBechaviour(const sf::Vector2f& arg_view1Size);
};