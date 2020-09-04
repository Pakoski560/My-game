#include "Player.h"
#include "Bullet.h"

Player::Player(sf::Texture& arg_texture, const sf::Vector2f& arg_view1Size, sf::Font& arg_font, sf::SoundBuffer& arg_bufferShoot, int arg_whichPlayer)
{
	soundShoot.setBuffer(arg_bufferShoot);
	soundShoot.setVolume(75.f);
	spr.setTexture(arg_texture);
	spr.setTextureRect(sf::IntRect(96, 0, 48, 72));
	spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
	playerTextureChange = 0;
	speed = 5;
	hp = 3;
	lostHealthFlag = 0;
	explosionTimer = 40;
	explosionCounter = 40;
	showPlayerCounter = 0;
	showPlayerWave = 0;
	shootCounter = 0;
	spr.setPosition(arg_view1Size.x / 2, arg_view1Size.y - (spr.getTextureRect().height));
}

bool Player::shooting()
{
	if (explosionTimer <= explosionCounter)
	{
		if (shootCounter >= 10)
		{
			shootCounter = 0;
			soundShoot.play();
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
		return 0;
}

sf::Sprite* Player::getSprite()
{
	return &spr;
}

float Player::getSpeed()
{
	return speed;
}

int Player::getHP()
{
	return hp;
}

void Player::update(sf::RenderWindow& arg_renderWindow, int& arg_playerCounterTime, int& arg_playerTurnTime, int& arg_switchOn)
{
	shootCounter++;
	if (arg_playerCounterTime >= 15)
	{
		spr.setTextureRect(sf::IntRect(48 * arg_playerTurnTime, 72 * playerTextureChange, 48, 72));
		playerTextureChange++;
		if (playerTextureChange == 2)
			playerTextureChange = 0;
	}

	if (explosionTimer <= explosionCounter)
	{
		if (hp <= 0)
			arg_switchOn = 1;
		if (lostHealthFlag == 1)
		{
			if (showPlayerCounter >= 7)
			{
				showPlayerCounter = 0;
				showPlayerWave++;
				arg_renderWindow.draw(spr);
				if (showPlayerWave >= 30)
				{
					lostHealthFlag = 0;
					showPlayerWave = 0;
				}
			}
			else
				showPlayerCounter++;
		}
		else
			arg_renderWindow.draw(spr);
	}
	else
		explosionCounter++;
}

void Player::playerCollisionWithWalls(const sf::Vector2f& arg_view1Size, float & arg_speed) {
	if (spr.getPosition().x - spr.getTextureRect().width / 2 < 0)						 // left wall
	{
		spr.setPosition(spr.getPosition().x + arg_speed, spr.getPosition().y);
	}
	else if (spr.getPosition().x + spr.getTextureRect().width / 2 > arg_view1Size.x)    // right wall
	{
		spr.setPosition(spr.getPosition().x - arg_speed, spr.getPosition().y);
	}
	if (spr.getPosition().y - spr.getTextureRect().height / 2 < 0)						// upper wall
	{
		spr.setPosition(spr.getPosition().x, spr.getPosition().y + arg_speed);
	}
	else if (spr.getPosition().y + spr.getTextureRect().height / 2 > arg_view1Size.y)    // lower wall
	{
		spr.setPosition(spr.getPosition().x, spr.getPosition().y - arg_speed);
	}
}

bool Player::playerCollisionWithBullet(bool(*func)(sf::Vector3f, sf::Vector3f), sf::Sprite& arg_bulletSprite)
{
	if(lostHealthFlag == 0)
		if (func(sf::Vector3f(spr.getPosition().x, spr.getPosition().y, 6)
			, sf::Vector3f(arg_bulletSprite.getPosition().x, arg_bulletSprite.getPosition().y, arg_bulletSprite.getTextureRect().width / 2)))
		{
			hp--;
			lostHealthFlag = 1;
			explosionCounter = 0;
			return 1;
		}
	return 0;
}