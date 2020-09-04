#include "Enemy.h"
#include <random>
#include <iostream>

Enemy::Enemy(sf::Texture& arg_texture)
{
	Spr.setTexture(arg_texture);
	Spr.setTextureRect(sf::IntRect(64, 0, 64, 64));
	Spr.setOrigin(sf::Vector2f(static_cast<float>(Spr.getTextureRect().width / 2), static_cast<float>(Spr.getTextureRect().height / 2)));
	counterChangeTexture = 0;
	changeTexture = 0;
	speed = 0.2f;
	hp = 1;
	sideUpdateFlag = 0;
	side = 0;
	enemyMoveCounter = 60;
	x = 0;
	y = 0;
	enemyShootCounter = 0;
	enemyShootTime = 60;
}

bool Enemy::enemyColisionWithBullet(sf::Sprite& arg_bulletSprite, bool(*func)(sf::Vector3f, sf::Vector3f))
{
	if (func(sf::Vector3f(Spr.getPosition().x, Spr.getPosition().y, static_cast<float>(Spr.getTextureRect().width)), 
		sf::Vector3f(arg_bulletSprite.getPosition().x, arg_bulletSprite.getPosition().y, static_cast<float>(arg_bulletSprite.getTextureRect().width))))
	{
		hp--;
		return 1;
	}
	else
		return 0;
}

bool Enemy::update(const sf::Vector2f& arg_view1Size, const float& arg_Pi, sf::RenderWindow& arg_renderWindow)
{
	std::random_device a;
	std::default_random_engine e1(a());
	if(enemyMoveCounter == 60)
		switch (side)
		{
		case 1:
			if ((Spr.getPosition().x < arg_view1Size.x / 6) && (sideUpdateFlag == 0))
			{
				std::uniform_int_distribution<int> uniform_dist(3, 6);
				int i = uniform_dist(e1);
				x = 20 * sin(i / arg_Pi);
				y = 20 * cos(i / arg_Pi);
				Spr.move(x * speed, y * speed);
				enemyMoveCounter = 0;
			}
			else
				Update_else(e1, arg_Pi);
			break;
		case 2:
			if ((Spr.getPosition().y < arg_view1Size.y / 6) && (sideUpdateFlag == 0))
			{
				std::uniform_int_distribution<int> uniform_dist(17, 20);
				int i = uniform_dist(e1);
				x = 20 * sin(i / arg_Pi);
				y = 20 * cos(i / arg_Pi);
				Spr.move(x * speed, y * speed);
				enemyMoveCounter = 0;
			}
			else
				Update_else(e1, arg_Pi);
			break;
		case 3:
			if ((Spr.getPosition().x > arg_view1Size.x * 5 / 6) && (sideUpdateFlag == 0))
			{
				std::uniform_int_distribution<int> uniform_dist(12, 15);
				int i = uniform_dist(e1);
				x = 20 * sin(i / arg_Pi);
				y = 20 * cos(i / arg_Pi);
				Spr.move(x * speed, y * speed);
				enemyMoveCounter = 0;
			}
			else
				Update_else(e1, arg_Pi);
			break;
		case 4:
			if ((Spr.getPosition().x < arg_view1Size.x / 6) && (Spr.getPosition().y < arg_view1Size.y / 6) && (sideUpdateFlag == 0))
			{
				std::uniform_int_distribution<int> uniform_dist(0, 3);
				int i = uniform_dist(e1);
				x = 20 * sin(i / arg_Pi);
				y = 20 * cos((i + 1) / arg_Pi);
				Spr.move(x * speed, y * speed);
				enemyMoveCounter = 0;
			}
			else
				Update_else(e1, arg_Pi);
			break;
		case 5:
			if ((Spr.getPosition().x > arg_view1Size.x * 5 / 6) && (Spr.getPosition().y < arg_view1Size.y / 6) && (sideUpdateFlag == 0))
			{
				std::uniform_int_distribution<int> uniform_dist(15, 17);
				int i = uniform_dist(e1);
				x = 20 * sin(i / arg_Pi);
				y = 20 * cos(i / arg_Pi);
				Spr.move(x * speed, y * speed);
				enemyMoveCounter = 0;
			}
			else
				Update_else(e1, arg_Pi);
			break;
		}
		enemyMoveCounter++;
		Spr.move(x * speed, y * speed);
		counterChangeTexture++;
		if (counterChangeTexture >= 15)
		{
			counterChangeTexture = 0;
			changeTexture++;
			if (changeTexture >= 2)
				changeTexture = 0;
			Spr.setTextureRect(sf::IntRect(64 * changeTexture, 0, 64, 64));
		}
		arg_renderWindow.draw(Spr);
		if (hp > 0)
			return 0;
		else
			return 1;
}

void Enemy::randomizePosition(const sf::Vector2f& arg_view1Size)
{
	// random position
	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(1, 3);
	side = uniform_dist(e1);

	if (side == 1)  // left wall
	{
		std::uniform_int_distribution<int> uniform_dist(0, static_cast<int>(arg_view1Size.y * 3 / 4));
		int mean = uniform_dist(e1);
		Spr.setPosition(0.f, static_cast<float>(mean));
		if (mean <= arg_view1Size.y / 8)
			side = 4;                        // upper left
	}

	else if (side == 3)  // right wall
	{
		std::uniform_int_distribution<int> uniform_dist(0, static_cast<int>(arg_view1Size.y * 3 / 4));
		int mean = uniform_dist(e1);
		Spr.setPosition(arg_view1Size.x - Spr.getTextureRect().width, static_cast<float>(mean));
		if (mean <= arg_view1Size.y / 8)
			side = 5;                      // upper right
	}

	if (side == 2)   // up side
	{
		std::uniform_int_distribution<int> uniform_dist(0, static_cast<int>(arg_view1Size.x));
		int mean = uniform_dist(e1);
		Spr.setPosition(static_cast<float>(mean), 0.f);
		if (mean <= arg_view1Size.x / 8)
			side = 4;                    // upper left
		else if (mean >= arg_view1Size.x * 7 / 8)
			side = 5;                   // upper right
	}
}

bool Enemy::shooting()
{
	if (enemyShootCounter >= enemyShootTime)
	{
		enemyShootCounter = 0;
		return 1;
	}
	else
	{
		enemyShootCounter++;
		return 0;
	}
}

void Enemy::Update_else(std::default_random_engine& arg_e1, float arg_Pi)
{
	std::uniform_int_distribution<int> uniform_dist(0, 20);
	int i = uniform_dist(arg_e1);
	x = 20 * sin(i / arg_Pi);
	y = 20 * cos((i + 1) / arg_Pi);
	Spr.move(x * speed, y * speed);
	sideUpdateFlag = 1;
	enemyMoveCounter = 0;
}

sf::Sprite* Enemy::getSprite()
{
	return &Spr;
}

int Enemy::getEnemyMoveCounter()
{
	return enemyMoveCounter;
}

sf::Vector2f Enemy::getDirection()
{
	sf::Vector2f a(x, y);
	return a;
}

void Enemy::setDirection(sf::Vector2f& arg_xAndY)
{
	x = arg_xAndY.x;
	y = arg_xAndY.y;
}