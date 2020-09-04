#include "Boss.h"
#include <random>

Boss::Boss(sf::Texture& arg_texture)
{
	spr.setTexture(arg_texture);
	spr.setTextureRect(sf::IntRect(512, 0, 512, 640));
	spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
	speed = 1.5;
	hp = 200;
	counterChangeTexture = 0;
	changeBechaviourCounter = 540;
	whenToStopCounter = 0;
	side = 0;
	changeTexture = 1;
	howToMove = 0;
	bulletPatern1 = 0;
	bulletPatern2 = 0;
	spr.setPosition(960, -300);
	prevPosition = spr.getPosition();
}

void Boss::update(sf::RenderWindow& arg_renderWindow, int& arg_bossDeadFlag)
{
	arg_renderWindow.draw(spr);
	if (counterChangeTexture >= 15)
	{
		spr.setTextureRect(sf::IntRect(512 * changeTexture, 0, 512, 640));
		counterChangeTexture = 0;
		changeTexture++;
		if (changeTexture >= 4)
			changeTexture = 0;
	}
	else
		counterChangeTexture++;

	if (hp <= 0)
		arg_bossDeadFlag = 1;
}

bool Boss::colisionWithBullets(bool(*func)(sf::Vector3f, sf::Vector3f), sf::Sprite& arg_bulletSprite)
{
	if (func(sf::Vector3f(spr.getPosition().x, spr.getPosition().y, spr.getTextureRect().width / 4)
		, sf::Vector3f(arg_bulletSprite.getPosition().x, arg_bulletSprite.getPosition().y, arg_bulletSprite.getTextureRect().width)))
	{
		hp--;
		return 1;
	}
	else
		return 0;
}

int Boss::getBulletPatern1()
{
	return bulletPatern1;
}

int Boss::getBulletPatern2()
{
	return bulletPatern2;
}

sf::Sprite* Boss::getSprite()
{
	return &spr;
}

bool Boss::changeBechaviour(const sf::Vector2f& arg_view1Size)
{
	if (changeBechaviourCounter >= 900)
	{
		speed = 1.5;
		std::random_device a;
		std::default_random_engine e1(a());
		std::uniform_int_distribution<int> uniform_dist(1, 3);

		int dummy = uniform_dist(e1);		// how to move  1 - continuous movement from side to side  2 - dynamic movement  3 - stand still
		while (dummy == howToMove)
			dummy = uniform_dist(e1);
		howToMove = dummy;

		dummy = uniform_dist(e1);		// random selection of bullet pattern
		while (dummy == bulletPatern1)
			dummy = uniform_dist(e1);
		bulletPatern1 = dummy;

		dummy = uniform_dist(e1);		// random selection of bullet pattern
		while (dummy == bulletPatern2 || dummy == bulletPatern1)
			dummy = uniform_dist(e1);
		bulletPatern2 = dummy;

		std::uniform_int_distribution<int> uniform_dist1(1, 2);
		side = uniform_dist1(e1);
		if (side == 1)
			speed = 1.5f;
		else
			speed = -1.5f;
		
		if (howToMove == 2)
			speed *= 3;

		changeBechaviourCounter = 0;
	}

	switch (howToMove)
	{
	case 0:		// boss entering the arena
		spr.move(0, speed);
		break;

	case 1:		//	continuous movement from side to side
		if (spr.getPosition().x - spr.getTextureRect().width / 2 < 0)
			speed *= -1;
		else if (spr.getPosition().x + spr.getTextureRect().width / 2 > arg_view1Size.x)
			speed *= -1;
		spr.move(speed, 0);
		break;

	case 2:		// dynamic movement
		if ((whenToStopCounter >= 120) && (speed == 0))
		{
			whenToStopCounter = 0;
			std::random_device a;
			std::default_random_engine e1(a());
			std::uniform_int_distribution<int> uniform_dist(1, 2);
			side = uniform_dist(e1);
			if (side == 1)
				speed = 4.5;
			else
				speed = -4.5;
		}
		else if ((whenToStopCounter >= 120) && (speed != 0))
		{
			whenToStopCounter = 0;
			speed = 0;
		}

		if (spr.getPosition().x - spr.getTextureRect().width / 2 < 0)
		{
			speed *= -1;
		}
		else if (spr.getPosition().x + spr.getTextureRect().width / 2 > arg_view1Size.x)
		{
			speed *= -1;
		}
		spr.move(speed, 0);
		whenToStopCounter++;
		break;
	}
	changeBechaviourCounter++;

	if (prevPosition != spr.getPosition())
	{
		prevPosition = spr.getPosition();
		return 1;
	}
	else
		return 0;
}
