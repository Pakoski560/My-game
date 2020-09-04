#include "Explosion.h"

Explosion::Explosion(const sf::Texture& arg_texture, const sf::Vector2f& arg_position, sf::SoundBuffer& arg_bufferExplosion)
{
	soundExplosion.setBuffer(arg_bufferExplosion);
	soundExplosion.play();
	spr.setTexture(arg_texture);
	spr.setTextureRect(sf::IntRect(0, 0, 48, 48));
	spr.setPosition(arg_position);
	textureChangeTime = 8;
	textureChangeCounter = 0;
	textureTurn = 0;
}

bool Explosion::update(sf::RenderWindow & arg_renderWindow)
{
	arg_renderWindow.draw(spr);
	if (textureChangeCounter >= textureChangeTime)
	{
		textureChangeCounter = 0;
		textureTurn++;
		if (textureTurn < 5)
		{
			spr.setTextureRect(sf::IntRect(48 * textureTurn, 0, 48, 48));
			return 0;
		}
		else
			return 1;
	}
	else
	{
		textureChangeCounter++;
		return 0;
	}
}