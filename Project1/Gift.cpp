#include "Gift.h"

Gift::Gift(const sf::Texture& arg_texture, const sf::Vector2f& arg_position, sf::SoundBuffer& arg_bufferAcquireGift)
{
	soundAcquireGift.setBuffer(arg_bufferAcquireGift); // sound play
	spr.setTexture(arg_texture);
	spr.setTextureRect(sf::IntRect(0, 0, 32, 32));
	spr.setPosition(arg_position);
	spr.setOrigin(sf::Vector2f(spr.getTextureRect().width / 2, spr.getTextureRect().height / 2));
	textureChangeTime = 15;
	textureChangeCounter = 0;
	textureTurn = 0;
}

void Gift::update(sf::RenderWindow& arg_renderWindow)
{
	arg_renderWindow.draw(spr);
	spr.move(0, 3);
	if (textureChangeCounter >= textureChangeTime)
	{
		textureChangeCounter = 0;
		textureTurn++;
		if (textureTurn == 2)
		{
			textureTurn = 0;
		}
		spr.setTextureRect(sf::IntRect(32 * textureTurn, 0, 32, 32));
	}
	else
	{
		textureChangeCounter++;
	}
	
}

sf::Sprite* Gift::getSprite()
{
	return &spr;
}