#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>

class Enemy
{
	float speed;
	int hp;
	int side;
	bool sideUpdateFlag;
	int enemyMoveCounter;
	int enemyShootCounter;
	int enemyShootTime;
	int counterChangeTexture;
	int changeTexture;
	float x, y;
	sf::Sprite Spr;

	void Update_else(std::default_random_engine& arg_e1, float arg_Pi);

public:
	Enemy(sf::Texture& arg_texture);
	~Enemy() = default;
	bool enemyColisionWithBullet(sf::Sprite & arg_bulletSprite, bool(*func)(sf::Vector3f, sf::Vector3f));
	bool update(const sf::Vector2f& arg_view1Size, const float& arg_Pi, sf::RenderWindow& arg_renderWindow);
	void randomizePosition(const sf::Vector2f& arg_view1Size);
	bool shooting();
	sf::Sprite* getSprite();
	int getEnemyMoveCounter();
	sf::Vector2f getDirection();
	void setDirection(sf::Vector2f& arg_xAndY);
};