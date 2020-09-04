#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Bullet.h"

class Player
{
	float speed;
	int shootCounter;
	int hp;
	int playerTextureChange;
	int explosionTimer;
	int explosionCounter;
	int showPlayerCounter;
	int showPlayerWave;
	int lostHealthFlag;
	sf::Sprite spr;
	sf::Sound soundShoot;
public:
	Player(sf::Texture& arg_texture, const sf::Vector2f& arg_view1Size, sf::Font& font, sf::SoundBuffer& arg_bufferShoot, int arg_whichPlayer);
	~Player() = default;
	void update(sf::RenderWindow & arg_renderWindow, int & arg_playerCounterTime, int & arg_playerTurnTime, int& arg_switchOn);
	void playerCollisionWithWalls(const sf::Vector2f& arg_view1Size, float& arg_speed);
	bool playerCollisionWithBullet(bool(*func)(sf::Vector3f, sf::Vector3f), sf::Sprite& arg_bulletSprite);
	bool shooting();
	sf::Sprite* getSprite();
	float getSpeed();
	int getHP();
};