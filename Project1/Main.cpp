#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "Bullet.h"
#include "Player.h"
#include "MainMenu.h"
#include "Enemy.h"
#include "Explosion.h"
#include "Gift.h"
#include "GameOver.h"
#include "Boss.h"
#include "MultiplayerMenu.h"
#include "MultiplayerHostMenu.h"
#include "MultiplayerJoinMenu.h"

using namespace std;

bool CC(sf::Vector3f c1, sf::Vector3f c2);

struct PacketInfo
{
	sf::Uint8 toDo;	// 0 - move the player, 1 - create a bullet, 2 - create an enemy, 3 - change the direction of the enemies, 4 - change the position of the boss, 5 - player collision with a enemy bullet 6 - enemy collision with a player bullet, 7 - boss collision with a player bullet, 8 - player collision with a gift 
	sf::Uint16 ID;	// id in array(e.g. bullet array)
	sf::Uint16 ID1;
	sf::Uint8 ID2;
	sf::Vector2f position;  // position of something
	sf::Vector2f speed;
	sf::IntRect whichBullet;
};

sf::Packet& operator <<(sf::Packet& packet, const sf::IntRect& arg_whichBullet);
sf::Packet& operator >>(sf::Packet& packet, sf::IntRect& arg_whichBullet);
sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& arg_position);
sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& arg_position);
sf::Packet& operator <<(sf::Packet& packet, const PacketInfo& arg_packetInfo);
sf::Packet& operator >>(sf::Packet& packet, PacketInfo& arg_packetInfo);

int main()
{
	// Window
	bool windowFocus = true;
	const size_t winW = 800;
	const size_t winH = 600;
	sf::View view1(sf::FloatRect(0.f, 0.f, 1920.f, 1080.f));
	sf::RenderWindow renderWindow(sf::VideoMode(winW, winH, 32), "Spaceships: rivals");
	renderWindow.setKeyRepeatEnabled(false);
	renderWindow.setFramerateLimit(60);
	renderWindow.setView(view1);

	// Constant variables
	const size_t maxBullets = 3000;
	const size_t totalBullets = 5 * maxBullets;
	const float Pi = 3.14159265358979323846f;
	const size_t maxEnemies = 200;
	const size_t maxExplosions = 20;
	const size_t maxGifts = 10;
	const int bulletTime = 1;
	const int bulletTime2 = 120;
	const int bulletTime3 = 20;
	const int enemyWaveTime = 180;

	// Load font
	sf::Font font;
	font.loadFromFile("Data/trebuc.ttf");

	// Load the textures
	sf::Texture bulletTx, playerTexture, enemyTexture, giftTexture, explosionEnemyTexture, explosionPlayerTexture, backgroundStage1Texture,
		bossTexture, menuBackgroundTexture, mainMenuWindowTexture, twoTileMenu, oneTileMenu;
	bulletTx.loadFromFile("Data/Sprite_Bullet.png");
	playerTexture.loadFromFile("Data/player.png");
	enemyTexture.loadFromFile("Data/enemy.png");
	giftTexture.loadFromFile("Data/power-up.png");
	explosionEnemyTexture.loadFromFile("Data/explosion-1.png");
	explosionPlayerTexture.loadFromFile("Data/explosion.png");
	backgroundStage1Texture.loadFromFile("Data/Space2.png");
	bossTexture.loadFromFile("Data/boss.png");
	menuBackgroundTexture.loadFromFile("Data/Main_menu/main_background.png");
	mainMenuWindowTexture.loadFromFile("Data/Main_menu/main_window.png");
	twoTileMenu.loadFromFile("Data/Main_menu/twoTileMenu.png");
	oneTileMenu.loadFromFile("Data/Main_menu/oneTileMenu.png");

	// Buffer for sounds and music
	sf::SoundBuffer buffer_click, buffer_switch, buffer_powerup, buffer_player_shoot, buffer_explosion, bufferExplosion2;
	sf::Music music_stage1, musicMenu;
	buffer_click.loadFromFile("Data/Sounds_and_music/main_menu_click.wav");
	buffer_switch.loadFromFile("Data/Sounds_and_music/main_menu_switch.wav");
	buffer_powerup.loadFromFile("Data/Sounds_and_music/powerup.wav");
	buffer_player_shoot.loadFromFile("Data/Sounds_and_music/laser_shoot.wav");
	buffer_explosion.loadFromFile("Data/Sounds_and_music/explosion.wav");
	bufferExplosion2.loadFromFile("Data/Sounds_and_music/explosion3.wav");
	music_stage1.openFromFile("Data/Sounds_and_music/bgm_boss.ogg");
	music_stage1.setLoop(true);
	musicMenu.openFromFile("Data/Sounds_and_music/bgm_main_menu.wav");
	musicMenu.setLoop(true);

	// Create objects
	MainMenu* mainMenu = NULL;
	GameOver* gameOver = NULL;
	MultiplayerMenu* multiplayerMenu = NULL;
	MultiplayerHostMenu* multiplayerHostMenu = NULL;
	MultiplayerJoinMenu* multiplayerJoinMenu = NULL;
	sf::Sprite* background = NULL;
	Boss* boss = NULL;

	std::vector <Player*> players;
	players.push_back(NULL);

	Bullet* bullets[totalBullets]; // 0 - 2999 Red pattern  3000 - 5999 Blue pattern  6000 - 8999 Purple pattern  9000 - 11999 Enemy bullets  12000 - 14999 player bullets
	for (size_t i = 0; i < totalBullets; i++)
	{
		bullets[i] = NULL;
	}

	Enemy* enemies[maxEnemies];
	for (size_t i = 0; i < maxEnemies; i++)
	{
		enemies[i] = NULL;
	}

	Explosion* explosions[maxExplosions];
	for (size_t i = 0; i < maxExplosions; i++)
	{
		explosions[i] = NULL;
	}

	Gift* gifts[maxGifts];
	for (size_t i = 0; i < maxGifts; i++)
	{
		gifts[i] = NULL;
	}

	// Use a vertex array for the rendering of the bullets
	sf::VertexArray vertices(sf::Quads, 4 * totalBullets);

	// Network objects and variables
	sf::IpAddress sendIP;
	sf::TcpListener listener;
	std::string playerInput;
	sf::Text playerText;
	sf::TcpSocket socketTCP;
	char connectionType = 's';
	playerText.setFont(font);
	playerText.setCharacterSize(50);
	playerText.setFillColor(sf::Color::White);
	playerText.setPosition(view1.getSize().x / 2 - 120, view1.getSize().y / 2 - 100);
	listener.setBlocking(false);
	socketTCP.setBlocking(false);

	// Create variables for the bullets patterns
	int BulletPatternCounter = 0;
	int BulletPatternCounter2 = 0;
	int BulletPatternCounter3 = 0;
	int enemyWaveCounter = 0;

	// Variables to change player texture
	int playerTextureChange = 0;
	int playerTurnTime = 2;
	int whichPlayerAmI = 0;

	int enemiesCounter = 0;

	size_t numBullet = 0;
	size_t numBullet2 = 0;
	size_t numBullet3 = 0;
	size_t numEnemyBullets = 0;
	size_t numPlayerBullets = 0;

	int bossFightFlag = 0;
	int bossDeadFlag = 0;
	float player_speed = 0;
	int packetSendWhich = 0;

	float r = 20; // radius of the circle
	sf::Vector2f prevPosition;
	vector<int> score;
	vector<sf::Text> score_txt;
	vector<sf::Text> whichPlayer_txt;
	vector<sf::Text> hp_txt;

	// Boss death 
	int explosionTimer = 20;
	int explosionCounter = 0;
	int howManyExplosions = 0;

	int switch_on = 0;

	sf::Event event;

	while (renderWindow.isOpen())
	{
		sf::Packet packetSend[100], packetReceive[100];
		PacketInfo packetInfo;
		switch (switch_on)
		{
			case 0:		// main menu
			{
				if (mainMenu == NULL)
				{
					mainMenu = new MainMenu(view1.getSize(), menuBackgroundTexture, mainMenuWindowTexture, buffer_click, buffer_switch, 3);
					if(musicMenu.getStatus() == sf::Music::Status::Stopped)
						musicMenu.play();
				}
				if (mainMenu != NULL)
				{
					while (renderWindow.pollEvent(event))
					{
						// Menu usage
						if (windowFocus)
						{
							if (event.type == sf::Event::Closed)
								renderWindow.close();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
								mainMenu->MoveUp();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
								mainMenu->MoveDown();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
								mainMenu->itemPressed(switch_on, renderWindow);
							if (event.type == sf::Event::MouseMoved)
								mainMenu->mouseMoved(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));
							if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
								if (mainMenu->mouseItemPressed(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow))))
									mainMenu->itemPressed(switch_on, renderWindow);
						}
						else if (event.type == sf::Event::GainedFocus)
							windowFocus = true;
						else if (event.type == sf::Event::LostFocus)
							windowFocus = false;
					}
					renderWindow.clear();
					mainMenu->update(renderWindow);
					renderWindow.display();
					if (switch_on != 0)
					{
						if (switch_on == 5)
						{
							musicMenu.stop();
							music_stage1.play();
						}
						delete mainMenu;
						mainMenu = NULL;
					}
				}
				break;
			}

			case 1:	// game over
			{
				if (gameOver == NULL)
				{
					gameOver = new GameOver(view1.getSize(), menuBackgroundTexture, oneTileMenu, buffer_click, buffer_switch, 2);
					for (int i = 0; i < score_txt.size(); i++)
					{
						score_txt[i].setPosition(sf::Vector2f(view1.getSize().x / 2, view1.getSize().y / 2 + 100 * i));
						whichPlayer_txt[i].setPosition(sf::Vector2f(view1.getSize().x / 2 - 200, view1.getSize().y / 2 + 100 * i));
						hp_txt[i].setPosition(sf::Vector2f(view1.getSize().x / 2 + 100, view1.getSize().y / 2 + 100 * i));
					}
				}
				if (gameOver != NULL)
				{
					while (renderWindow.pollEvent(event))
					{
						// Menu usage
						if (windowFocus)
						{
							if (event.type == sf::Event::Closed)
								switch_on = 0;
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
								gameOver->MoveUp();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
								gameOver->MoveDown();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
								gameOver->itemPressed(switch_on, renderWindow);
							if (event.type == sf::Event::MouseMoved)
								gameOver->mouseMoved(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));
							if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
								if (gameOver->mouseItemPressed(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow))))
									gameOver->itemPressed(switch_on, renderWindow);
						}
						else if (event.type == sf::Event::GainedFocus)
							windowFocus = true;
						else if (event.type == sf::Event::LostFocus)
							windowFocus = false;
					}
					renderWindow.clear();
					gameOver->update(renderWindow);
					for (int i = 0; i < score_txt.size(); i++)
					{
						renderWindow.draw(score_txt[i]);
						renderWindow.draw(whichPlayer_txt[i]);
						renderWindow.draw(hp_txt[i]);
					}

					renderWindow.display();
					if (switch_on != 1)
					{
						if (switch_on == 5)
						{
							musicMenu.stop();
							music_stage1.play();
						}
						delete gameOver;
						gameOver = NULL;
					}
				}
				break;
			}

			case 2:	// multiplayer menu
			{
				if (multiplayerMenu == NULL)
					multiplayerMenu = new MultiplayerMenu(view1.getSize(), menuBackgroundTexture, twoTileMenu, buffer_click, buffer_switch, 2);
				if (multiplayerMenu != NULL)
				{
					while (renderWindow.pollEvent(event))
					{
						// Menu usage
						if (windowFocus)
						{
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)	// BACK
								switch_on = 0;
							if (event.type == sf::Event::Closed)
								renderWindow.close();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
								multiplayerMenu->MoveUp();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
								multiplayerMenu->MoveDown();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
								multiplayerMenu->itemPressed(switch_on, renderWindow);
							if (event.type == sf::Event::MouseMoved)
								multiplayerMenu->mouseMoved(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));
							if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
								if (multiplayerMenu->mouseItemPressed(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow))))
									multiplayerMenu->itemPressed(switch_on, renderWindow);
						}
					else if (event.type == sf::Event::GainedFocus)
						windowFocus = true;
					else if (event.type == sf::Event::LostFocus)
						windowFocus = false;
					}
					renderWindow.clear();
					multiplayerMenu->update(renderWindow);
					renderWindow.display();
					if (switch_on != 2)
					{
						if (switch_on == 5)
						{
							musicMenu.stop();
							music_stage1.play();
						}
						delete multiplayerMenu;
						multiplayerMenu = NULL;
					}
				}
				break;
			}
			case 3:	// host option in multiplayer
			{
				if (multiplayerHostMenu == NULL)
				{
					multiplayerHostMenu = new MultiplayerHostMenu(view1.getSize(), menuBackgroundTexture, oneTileMenu, buffer_click, buffer_switch, 1);
					listener.listen(2002);
				}
				if(multiplayerHostMenu != NULL)
				{
					while (renderWindow.pollEvent(event))
					{
						// Menu usage
						if (windowFocus)
						{
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)	// BACK
								switch_on--;
							if (event.type == sf::Event::Closed)
								renderWindow.close();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
								multiplayerHostMenu->MoveUp();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
								multiplayerHostMenu->MoveDown();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
								multiplayerHostMenu->itemPressed(switch_on, renderWindow);
							if (event.type == sf::Event::MouseMoved)
								multiplayerHostMenu->mouseMoved(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));
							if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
								if (multiplayerHostMenu->mouseItemPressed(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow))))
									multiplayerHostMenu->itemPressed(switch_on, renderWindow);
						}
						else if (event.type == sf::Event::GainedFocus)
							windowFocus = true;
						else if (event.type == sf::Event::LostFocus)
							windowFocus = false;
					}

					if (listener.accept(socketTCP) == sf::Socket::Done)
					{
						connectionType = 's';
						players.push_back(NULL);
						switch_on = 5;
					}

					renderWindow.clear();
					multiplayerHostMenu->update(renderWindow);
					renderWindow.display();
					if (switch_on != 3)
					{
						if (switch_on == 5)
						{
							musicMenu.stop();
							music_stage1.play();
						}
						delete multiplayerHostMenu;
						multiplayerHostMenu = NULL;
						listener.close();
					}
				}
				break;
			}
			case 4:		// join option in menu
			{
				if (multiplayerJoinMenu == NULL)
				{
					multiplayerJoinMenu = new MultiplayerJoinMenu(view1.getSize(), menuBackgroundTexture, oneTileMenu, buffer_click, buffer_switch, 1);
				}
				if (multiplayerJoinMenu != NULL)
				{
					while (renderWindow.pollEvent(event))
					{
						// Menu usage
						if (windowFocus)
						{
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)	// BACK
								switch_on = 2;
							if (event.type == sf::Event::Closed)
								renderWindow.close();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up)
								multiplayerJoinMenu->MoveUp();
							if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down)
								multiplayerJoinMenu->MoveDown();
							if (event.type == sf::Event::MouseMoved)
								multiplayerJoinMenu->mouseMoved(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow)));
							if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
								if (multiplayerJoinMenu->mouseItemPressed(renderWindow.mapPixelToCoords(sf::Mouse::getPosition(renderWindow))))
								{
									sendIP = playerInput;
									socketTCP.setBlocking(true);
									if (socketTCP.connect(sendIP, 2002, sf::seconds(0.5f)) == sf::Socket::Status::Done)
									{
										connectionType = 'c';
										switch_on = 5;
										players.push_back(NULL);
									}
									socketTCP.setBlocking(false);
								}

							// Player is typing ip address
							if (event.type == sf::Event::TextEntered)
							{
								if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
								{
									if (!playerInput.empty())
									{
										playerInput.erase(playerInput.end() - 1);
										playerText.setString(playerInput);
									}
								}
								else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
								{
									sendIP = playerInput;
									socketTCP.setBlocking(true);
									if (socketTCP.connect(sendIP, 2002, sf::seconds(0.5f)) == sf::Socket::Status::Done)
									{
										connectionType = 'c';
										switch_on = 5;
										players.push_back(NULL);
									}
									socketTCP.setBlocking(false);
								}
								else
								{
									playerInput += event.text.unicode;
									playerText.setString(playerInput);
								}
							}
						}
						else if (event.type == sf::Event::GainedFocus)
							windowFocus = true;
						else if (event.type == sf::Event::LostFocus)
							windowFocus = false;
					}

					renderWindow.clear();
					multiplayerJoinMenu->update(renderWindow);
					renderWindow.draw(playerText);
					renderWindow.display();
					if (switch_on != 4)
					{
						playerInput.erase();
						playerText.setString(playerInput);
						if (switch_on == 5)
						{
							musicMenu.stop();
							music_stage1.play();
						}
						delete multiplayerJoinMenu;
						multiplayerJoinMenu = NULL;
					}
				}
				break;
			}

			case 5:		// Main game
			{
				if (players[0] == NULL)		// initialize the game
				{
					if (connectionType == 's')
						whichPlayerAmI = 0;
					else if(connectionType == 'c')
						whichPlayerAmI = 1;
					for(size_t i = 0; i < players.size(); i++)
						players[i] = new Player(playerTexture, view1.getSize(), font, buffer_player_shoot, i);
					score.resize(0);
					score_txt.resize(0);
					whichPlayer_txt.resize(0);
					hp_txt.resize(0);
					for (int i = 0; i < players.size(); i++)
					{
						score.push_back(0);
						whichPlayer_txt.resize(whichPlayer_txt.size() + 1);
						whichPlayer_txt[i].setCharacterSize(40);
						whichPlayer_txt[i].setFont(font);
						whichPlayer_txt[i].setFillColor(sf::Color::White);
						if (i == 0)
							whichPlayer_txt[i].setPosition(view1.getSize().x - 200, 10);
						else
							whichPlayer_txt[i].setPosition(view1.getSize().x - 200, 300);
						std::string sampleString;
						sampleString = "Player ";
						sampleString.append(to_string(i+1));
						whichPlayer_txt[i].setString(sampleString);

						hp_txt.resize(hp_txt.size() + 1);
						hp_txt[i].setFont(font);
						hp_txt[i].setCharacterSize(30);
						hp_txt[i].setFillColor(sf::Color::White);
						if (i == 0)
							hp_txt[i].setPosition(view1.getSize().x - 120, 150);
						else
							hp_txt[i].setPosition(view1.getSize().x - 120, 450);
						hp_txt[i].setString(std::to_string(players[i]->getHP()));

						score_txt.resize(score_txt.size() + 1);
						score_txt[i].setFont(font);
						score_txt[i].setFillColor(sf::Color::White);
						score_txt[i].setCharacterSize(30);
						if(i == 0)
							score_txt[i].setPosition(sf::Vector2f(view1.getSize().x - 120, 100));
						else
							score_txt[i].setPosition(sf::Vector2f(view1.getSize().x - 120, 400));
					}
					background = new sf::Sprite();
					boss = new Boss(bossTexture);
					background->setTexture(backgroundStage1Texture);
					background->setPosition(-100, -2700);
					background->setScale(2, 2);
					BulletPatternCounter = 0;
					BulletPatternCounter2 = 0;
					BulletPatternCounter3 = 0;
					enemyWaveCounter = 0;
					playerTextureChange = 0;
					playerTurnTime = 2;
					enemiesCounter = 0;
					numBullet = 0;
					numBullet2 = 0;
					numBullet3 = 0;
					numEnemyBullets = 0;
					numPlayerBullets = 0;
					bossFightFlag = 0;
					bossDeadFlag = 0;
					explosionCounter = 0;
					howManyExplosions = 0;
					if (connectionType == 'c')
						bossFightFlag = 1;
					player_speed = 0;
				}
				if (players[0] != NULL)
				{
					player_speed = players[whichPlayerAmI]->getSpeed();	// get default speed
					while (renderWindow.pollEvent(event))
					{
						if (event.type == sf::Event::Closed)
							renderWindow.close();
						if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
							renderWindow.close();
						if (event.type == sf::Event::GainedFocus)
						{
							windowFocus = true;
						}
						else if (event.type == sf::Event::LostFocus)
						{
							windowFocus = false;
						}
					}
					prevPosition = players[whichPlayerAmI]->getSprite()->getPosition();

					// Move the player and check collisions with walls
					if (windowFocus)
					{
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
							player_speed = players[whichPlayerAmI]->getSpeed() / 2;
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
							player_speed = players[whichPlayerAmI]->getSpeed() * 2;

						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
							players[whichPlayerAmI]->getSprite()->move(-player_speed, 0.0f);
							players[whichPlayerAmI]->playerCollisionWithWalls(view1.getSize(), player_speed);
							if (playerTextureChange == 15 && playerTurnTime != 0)
							{
								playerTurnTime--;
							}
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
							players[whichPlayerAmI]->getSprite()->move(player_speed, 0.0f);
							players[whichPlayerAmI]->playerCollisionWithWalls(view1.getSize(), player_speed);
							if (playerTextureChange == 15 && playerTurnTime != 4)
							{
								playerTurnTime++;
							}
						}

						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
							players[whichPlayerAmI]->getSprite()->move(0, -player_speed);
							players[whichPlayerAmI]->playerCollisionWithWalls(view1.getSize(), player_speed);
						}
						else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
							players[whichPlayerAmI]->getSprite()->move(0, player_speed);
							players[whichPlayerAmI]->playerCollisionWithWalls(view1.getSize(), player_speed);
						}

						// Player shooting
						if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && players[whichPlayerAmI]->shooting())
						{
							if (connectionType == 's')
							{
								if (numPlayerBullets == maxBullets) {
									numPlayerBullets = 0;
								}

								if (bullets[4 * maxBullets + numPlayerBullets] != NULL) {
									delete(bullets[4 * maxBullets + numPlayerBullets]);
									bullets[4 * maxBullets + numPlayerBullets] = NULL;
								}

								Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(0, -35), sf::Vector2f(players[whichPlayerAmI]->getSprite()->getPosition().x
									, players[whichPlayerAmI]->getSprite()->getPosition().y - players[whichPlayerAmI]->getSprite()->getTextureRect().height / 2), sf::IntRect(0, 64, 16, 16), 0);
								bullets[4 * maxBullets + numPlayerBullets] = bul;
								addBullet(&vertices, bul, 4 * maxBullets + numPlayerBullets);
								packetInfo.toDo = 1;
								packetInfo.ID = static_cast<sf::Uint16>(4 * maxBullets + numPlayerBullets);	// bullet id
								packetInfo.position = bul->getSprite()->getPosition();
								packetInfo.speed = sf::Vector2f(0, -35);
								packetInfo.whichBullet = sf::IntRect(0, 64, 16, 16);
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
								numPlayerBullets++;
							}
							else   // Player who is not a server will send signal to a server that he want's to create a new bullet
							{
								packetInfo.toDo = 1;
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
							}
						}
					}

					// Change player texture to default
					if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
					{
						if (playerTextureChange == 15 && playerTurnTime != 2)
						{
							if (playerTurnTime < 2)
								playerTurnTime++;
							else
								playerTurnTime--;
						}
					}

					// send your position if it changed
					if (prevPosition != players[whichPlayerAmI]->getSprite()->getPosition())
					{
						packetInfo.toDo = 0;
						packetInfo.ID = whichPlayerAmI;	// player id
						packetInfo.position = players[whichPlayerAmI]->getSprite()->getPosition();
						packetSend[packetSendWhich] << packetInfo;
						socketTCP.send(packetSend[packetSendWhich]);
						packetSendWhich++;
					}

					// red bullet pattern
					if (bossFightFlag == 1 && bossDeadFlag == 0)
					if (boss->getBulletPatern1() == 1 || boss->getBulletPatern2() == 1) {
						if (BulletPatternCounter > bulletTime) {
							if (numBullet == maxBullets) {
								numBullet = 0;
							}

							if (bullets[numBullet] != NULL) {
								delete(bullets[numBullet]);
								bullets[numBullet] = NULL;
							}

							float x = r * cos((size_t)(numBullet % 180 + 1) / Pi);
							float y = r * sin((size_t)(numBullet % 180 + 1) / Pi);
							Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(x*0.1f, y*0.1f), boss->getSprite()->getPosition(), 
								sf::IntRect(0, 64, 16, 16));
							bullets[numBullet] = bul;
							addBullet(&vertices, bul, numBullet);
							packetInfo.toDo = 1;
							packetInfo.ID = static_cast<sf::Uint16>(numBullet);	//bullet id
							packetInfo.position = bul->getSprite()->getPosition();
							packetInfo.speed = sf::Vector2f(x*0.1f, y*0.1f);
							packetInfo.whichBullet = sf::IntRect(0, 64, 16, 16);
							packetSend[packetSendWhich] << packetInfo;
							socketTCP.send(packetSend[packetSendWhich]);
							packetSendWhich++;
							numBullet++;
							BulletPatternCounter = 0;
						}
					}

					// blue bullet pettern
					if (bossFightFlag == 1 && bossDeadFlag == 0)
					if (boss->getBulletPatern1() == 2 || boss->getBulletPatern2() == 2)
						if (BulletPatternCounter2 > bulletTime2) {
							for (size_t i = 0; i < 45; i++) {
								float y = r * cos((size_t)i / (Pi / 2));
								float x = r * sin((size_t)i / (Pi / 2));
								Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(x*0.1f, y*0.1f),
									boss->getSprite()->getPosition(), sf::IntRect(128, 0, 64, 64));

								if (numBullet2 == maxBullets) {
									numBullet2 = 0;
								}

								if (bullets[maxBullets + numBullet2] != NULL) {
									delete(bullets[maxBullets + numBullet2]);
									bullets[maxBullets + numBullet2] = NULL;
								}

								bullets[maxBullets + numBullet2] = bul;
								addBullet(&vertices, bul, maxBullets + numBullet2);
								packetInfo.toDo = 1;
								packetInfo.ID = static_cast<sf::Uint16>(maxBullets + numBullet2);  // bullet id
								packetInfo.position = bul->getSprite()->getPosition();
								packetInfo.speed = sf::Vector2f(x*0.1f, y*0.1f);
								packetInfo.whichBullet = sf::IntRect(128, 0, 64, 64);
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
								numBullet2++;
								BulletPatternCounter2 = 0;
							}
						}

					// purple bullet pattern
					if (bossFightFlag == 1 && bossDeadFlag == 0)
					if (boss->getBulletPatern1() == 3 || boss->getBulletPatern2() == 3)
						if (BulletPatternCounter3 > bulletTime3) {
							for (size_t i = 0; i < 10; i++) {
								float y = r * cos((size_t)rand() % 180 / (Pi));
								float x = r * sin((size_t)rand() % 180 / (Pi));

								Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(x*0.1f, y*0.1f), boss->getSprite()->getPosition(),
									sf::IntRect(0, 80, 8, 8));

								if (numBullet3 == maxBullets) {
									numBullet3 = 0;
								}

								if (bullets[2 * maxBullets + numBullet3] != NULL) {
									delete(bullets[2 * maxBullets + numBullet3]);
									bullets[2 * maxBullets + numBullet3] = NULL;
								}

								bullets[2 * maxBullets + numBullet3] = bul;
								addBullet(&vertices, bul, 2 * maxBullets + numBullet3);
								packetInfo.toDo = 1;
								packetInfo.ID = static_cast<sf::Uint16>(2 * maxBullets + numBullet3);  // bullet id
								packetInfo.position = bul->getSprite()->getPosition();
								packetInfo.speed = sf::Vector2f(x*0.1f, y*0.1f);
								packetInfo.whichBullet = sf::IntRect(0, 80, 8, 8);
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
								numBullet3++;
								BulletPatternCounter3 = 0;
							}
						}

					// enemy waves
					if (connectionType == 's')
						if ((enemyWaveCounter > enemyWaveTime) && (enemiesCounter < maxEnemies)) {
							std::random_device r;
							std::default_random_engine e1(r());
							std::uniform_int_distribution<int> uniform_dist;
							if (maxEnemies - enemiesCounter < 12)
								uniform_dist = std::uniform_int_distribution<int>(1, maxEnemies - enemiesCounter);
							else
								uniform_dist = std::uniform_int_distribution<int>(1, 12);
							int j = uniform_dist(e1);	// Get a random number of enemies per wave
							for (int i = 0; i < j; i++) 
							{
								Enemy *enm = new Enemy(enemyTexture);
								enm->randomizePosition(view1.getSize());
								enemies[enemiesCounter] = enm;
								packetInfo.toDo = 2;
								packetInfo.ID = enemiesCounter;
								packetInfo.position = enm->getSprite()->getPosition();
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
								enemiesCounter++;
							}
							if (enemiesCounter >= maxEnemies)
								bossFightFlag = 1;
							enemyWaveCounter = 0;
						}

					//Enemy bullets
					if(connectionType == 's')
					for (int i = 0; i < maxEnemies; i++)
						if (enemies[i] != NULL)
							if (enemies[i]->shooting())
							{
								if (numEnemyBullets == maxBullets) {
									numEnemyBullets = 0;
								}

								if (bullets[3 * maxBullets + numEnemyBullets] != NULL) {
									delete(bullets[3 * maxBullets + numEnemyBullets]);
									bullets[3 * maxBullets + numEnemyBullets] = NULL;
								}

								Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(0, 5), enemies[i]->getSprite()->getPosition(),
									sf::IntRect(0, 64, 16, 16));
								bullets[3 * maxBullets + numEnemyBullets] = bul;
								addBullet(&vertices, bul, 3 * maxBullets + numEnemyBullets);
								packetInfo.toDo = 1;
								packetInfo.ID = static_cast<sf::Uint16>(3 * maxBullets + numEnemyBullets);	// bullet id
								packetInfo.position = bul->getSprite()->getPosition();
								packetInfo.speed = sf::Vector2f(0, 5);
								packetInfo.whichBullet = sf::IntRect(0, 64, 16, 16);
								packetSend[packetSendWhich] << packetInfo;
								socketTCP.send(packetSend[packetSendWhich]);
								packetSendWhich++;
								numEnemyBullets++;
							}

					// Update all bullets and check collisions
					for (size_t i = 0; i < totalBullets; i++)
					{
						if (bullets[i] != NULL)
						{
							bullets[i]->update();
							sf::Vertex* quad = &vertices[i * 4];
							sf::Sprite *spr = bullets[i]->getSprite();

							quad[0].position = sf::Vector2f(spr->getPosition().x - spr->getTextureRect().width / 2,
								spr->getPosition().y - spr->getTextureRect().height / 2);
							quad[1].position = sf::Vector2f(spr->getPosition().x + spr->getTextureRect().width / 2,
								spr->getPosition().y - spr->getTextureRect().height / 2);
							quad[2].position = sf::Vector2f(spr->getPosition().x + spr->getTextureRect().width / 2,
								spr->getPosition().y + spr->getTextureRect().height / 2);
							quad[3].position = sf::Vector2f(spr->getPosition().x - spr->getTextureRect().width / 2, spr->getPosition().y +
								spr->getTextureRect().height / 2);

							if (connectionType == 's' && bossDeadFlag == 0)
							{
								if (i < 12000)	// enemies bullets
								{
									for(int k = 0; k < players.size(); k++)
										if (players[k]->playerCollisionWithBullet(CC, *spr))
										{
											quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
											for (int j = 0; j < maxExplosions; j++)
												if (explosions[j] == NULL)
												{
													explosions[j] = new Explosion(explosionPlayerTexture, players[k]->getSprite()->getPosition(), bufferExplosion2);
													break;
												}
											players[k]->getSprite()->setPosition(view1.getSize().x / 2, view1.getSize().y -
												(players[k]->getSprite()->getTextureRect().height));	// reset player position
											packetInfo.toDo = 5;
											packetInfo.ID = i;	// which bullet should be deleted
											packetInfo.ID1 = k;	// which player
											packetSend[packetSendWhich] << packetInfo;
											socketTCP.send(packetSend[packetSendWhich]);
											packetSendWhich++;
										}
								}
								else
								{
									for (int j = 0; j < maxEnemies; j++)
										if (enemies[j] != NULL)
											if (enemies[j]->enemyColisionWithBullet(*spr, CC))	// true: delete player bullet, -1 hp to enemy
											{
												quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
												quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
												quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
												quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
												spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
												score[bullets[i]->getOwner()] += 70;
												packetInfo.toDo = 6;
												packetInfo.ID = i;	// which bullet should be deleted
												packetInfo.ID1 = j;	// which enemy was hit
												packetInfo.ID2 = bullets[i]->getOwner();
												packetSend[packetSendWhich] << packetInfo;
												socketTCP.send(packetSend[packetSendWhich]);
												packetSendWhich++;
											}
									if (bossFightFlag == 1)
										if (boss->colisionWithBullets(CC, *spr))
										{
											quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
											spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
											score[bullets[i]->getOwner()] += 70;
											packetInfo.toDo = 7;
											packetInfo.ID = i;	// which bullet should be deleted
											packetInfo.ID1 = bullets[i]->getOwner();	// which player should get point
											packetSend[packetSendWhich] << packetInfo;
											socketTCP.send(packetSend[packetSendWhich]);
											packetSendWhich++;
										}
								}
							}
						}
					}

					// move background
					if(bossFightFlag == 0)
						background->setPosition(background->getPosition().x, background->getPosition().y + 0.15f);


					// receive all packets
					for (int i = 0; i < 100; i++)
						if (socketTCP.receive(packetReceive[i]) == sf::Socket::Done)
						{
							packetReceive[i] >> packetInfo;
							switch (packetInfo.toDo)
							{
							case 0:		// move second player
							{
								players[packetInfo.ID]->getSprite()->setPosition(packetInfo.position);
								break;
							}
							case 1:		// client will create any bullet, server will create second player's bullet
								if (connectionType == 'c')
								{
									if (bullets[packetInfo.ID] != NULL) {
										delete(bullets[packetInfo.ID]);
										bullets[packetInfo.ID] = NULL;
									}
									Bullet *bul = new Bullet(&bulletTx, packetInfo.speed, packetInfo.position, packetInfo.whichBullet);
									bullets[packetInfo.ID] = bul;
									addBullet(&vertices, bul, packetInfo.ID);
								}
								else
								{
									if (numPlayerBullets == maxBullets) {
										numPlayerBullets = 0;
									}

									if (bullets[4 * maxBullets + numPlayerBullets] != NULL) {
										delete(bullets[4 * maxBullets + numPlayerBullets]);
										bullets[4 * maxBullets + numPlayerBullets] = NULL;
									}

									Bullet *bul = new Bullet(&bulletTx, sf::Vector2f(0, -35), sf::Vector2f(players[1]->getSprite()->getPosition().x
										, players[1]->getSprite()->getPosition().y - players[1]->getSprite()->getTextureRect().height / 2), sf::IntRect(0, 64, 16, 16), 1);
									bullets[4 * maxBullets + numPlayerBullets] = bul;
									addBullet(&vertices, bul, 4 * maxBullets + numPlayerBullets);
									packetInfo.toDo = 1;
									packetInfo.ID = static_cast<sf::Uint16>(4 * maxBullets + numPlayerBullets);
									packetInfo.position = bul->getSprite()->getPosition();
									packetInfo.speed = sf::Vector2f(0, -35);
									packetInfo.whichBullet = sf::IntRect(0, 64, 16, 16);
									numPlayerBullets++;
									packetSend[packetSendWhich] << packetInfo;
									socketTCP.send(packetSend[packetSendWhich]);
									packetSendWhich++;
								}
								break;
							case 2:		// create an enemy
							{
								Enemy *enm = new Enemy(enemyTexture);
								enm->getSprite()->setPosition(packetInfo.position);
								enemies[packetInfo.ID] = enm;
								break;
							}
							case 3:		// change direction of an enemy
							{
								enemies[packetInfo.ID]->setDirection(packetInfo.speed);
								break;
							}
							case 4:		// change boss position
							{
								boss->getSprite()->setPosition(packetInfo.speed);
								break;
							}
							case 5:		// player collision with enemy bullet
							{
								sf::Sprite* a = players[packetInfo.ID1]->getSprite();
								players[packetInfo.ID1]->playerCollisionWithBullet(CC, *a);
								sf::Vertex* quad = &vertices[packetInfo.ID * 4];
								sf::Sprite *spr = bullets[packetInfo.ID]->getSprite();
								quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
								for (int j = 0; j < maxExplosions; j++)
									if (explosions[j] == NULL)
									{
										explosions[j] = new Explosion(explosionPlayerTexture, players[packetInfo.ID1]->getSprite()->getPosition(), bufferExplosion2);
										break;
									}
								players[packetInfo.ID1]->getSprite()->setPosition(view1.getSize().x / 2, view1.getSize().y -
									(players[packetInfo.ID1]->getSprite()->getTextureRect().height));	// reset player position
								break;
							}
							case 6:		// enemy collision with a player bullet
							{
								sf::Sprite* a = enemies[packetInfo.ID1]->getSprite();
								enemies[packetInfo.ID1]->enemyColisionWithBullet(*a, CC);
								sf::Vertex* quad = &vertices[packetInfo.ID * 4];
								sf::Sprite *spr = bullets[packetInfo.ID]->getSprite();
								quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
								score[packetInfo.ID2]+= 70;
								break;
							}
							case 7:		// boss collision with a player bullet
							{	sf::Sprite* a = boss->getSprite();
								boss->colisionWithBullets(CC, *a);
								sf::Vertex* quad = &vertices[packetInfo.ID * 4];
								sf::Sprite *spr = bullets[packetInfo.ID]->getSprite();
								quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
								score[packetInfo.ID1] += 70;
								break;
							}
							case 8:		// player collision with a gift
							{
								score[packetInfo.ID] += 700;
								delete(gifts[packetInfo.ID1]);
								gifts[packetInfo.ID1] = NULL;
								break;
							}
							}
						}

						// clean bullet when outside the window
						for (size_t i = 0; i < totalBullets; i++)
							if (bullets[i] != NULL)
							{
								if (bullets[i]->getSprite()->getPosition().x - bullets[i]->getSprite()->getTextureRect().width / 2 > view1.getSize().x
									|| bullets[i]->getSprite()->getPosition().y - bullets[i]->getSprite()->getTextureRect().height / 2 > view1.getSize().y
									|| (bullets[i]->getSprite()->getPosition().y + bullets[i]->getSprite()->getTextureRect().height / 2) < 0
									|| (bullets[i]->getSprite()->getPosition().x + bullets[i]->getSprite()->getTextureRect().width / 2) < 0) {
									delete(bullets[i]);
									bullets[i] = NULL;
								}
							}

						// clean enemies when outside the window
						if (connectionType == 's')
							for (size_t i = 0; i < maxEnemies; i++)
							{
								if (enemies[i] != NULL)
								{
									if (enemies[i]->getSprite()->getPosition().x > view1.getSize().x
										|| enemies[i]->getSprite()->getPosition().y > view1.getSize().y
										|| (enemies[i]->getSprite()->getPosition().y + enemies[i]->getSprite()->getTextureRect().height) < 0
										|| (enemies[i]->getSprite()->getPosition().x + enemies[i]->getSprite()->getTextureRect().width) < 0)
									{
										delete(enemies[i]);
										enemies[i] = NULL;
									}
								}
							}

					// draw everything
					renderWindow.clear();

					renderWindow.draw(*background);
					
					if (bossFightFlag == 1)		// update boss and change bechaviour on server side
					{
						boss->update(renderWindow, bossDeadFlag);
						if (bossDeadFlag == 0)
						{
							if (connectionType == 's')
								if (boss->changeBechaviour(view1.getSize()))  // true if boss changed position
								{
									packetInfo.toDo = 4;
									packetInfo.speed = boss->getSprite()->getPosition();
									packetSend[packetSendWhich] << packetInfo;
									socketTCP.send(packetSend[packetSendWhich]);
									packetSendWhich++;
								}
						}
					}

					for(size_t i = 0; i<players.size(); i++)
						players[i]->update(renderWindow, playerTextureChange, playerTurnTime, switch_on);

					if (playerTextureChange >= 15)
						playerTextureChange = 0;

					// Update all enemies
					for (size_t i = 0; i < maxEnemies; i++)
						if (enemies[i] != NULL)
						{
							if(connectionType == 's')
								if (enemies[i]->getEnemyMoveCounter() == 1)	// server will send bechaviour of enemy to client
								{
									packetInfo.toDo = 3;
									packetInfo.ID = static_cast<sf::Uint16>(i);
									packetInfo.speed = enemies[i]->getDirection();
									packetSend[packetSendWhich] << packetInfo;
									socketTCP.send(packetSend[packetSendWhich]);
									packetSendWhich++;
								}
							if (enemies[i]->update(view1.getSize(), Pi, renderWindow)) // return true if enemy has 0 hp
							{
								for (size_t j = 0; j < maxExplosions; j++)
									if (explosions[j] == NULL)
									{
										explosions[j] = new Explosion(explosionEnemyTexture, enemies[i]->getSprite()->getPosition(), buffer_explosion);
										break;
									}
								if(rand() % 10 == 0)
									for (size_t j = 0; j < maxGifts; j++)
										if (gifts[j] == NULL)
										{
											gifts[j] = new Gift(giftTexture, enemies[i]->getSprite()->getPosition(), buffer_powerup);
											break;
										}
								delete(enemies[i]);
								enemies[i] = NULL;
							}
						}

					// random explosions when boss is dead
					if (bossDeadFlag == 1 && howManyExplosions <= 15)
					{
						if(explosionCounter >= explosionTimer)
							for (size_t j = 0; j < maxExplosions; j++)
								if (explosions[j] == NULL)
								{
									std::random_device a;
									std::default_random_engine e1(a());
									std::uniform_int_distribution<int> uniform_dist_x(boss->getSprite()->getPosition().x - boss->getSprite()->getTextureRect().width / 4, 
										boss->getSprite()->getPosition().x + boss->getSprite()->getTextureRect().width / 4);
									std::uniform_int_distribution<int> uniform_dist_y(boss->getSprite()->getPosition().y - boss->getSprite()->getTextureRect().height / 4,
										boss->getSprite()->getPosition().y + boss->getSprite()->getTextureRect().height / 4);
									int b = uniform_dist_x(e1);
									int c = uniform_dist_y(e1);
									explosions[j] = new Explosion(explosionEnemyTexture, sf::Vector2f(b,c), buffer_explosion);
									howManyExplosions++;
									explosionCounter = 0;
									break;
								}
						explosionCounter++;
						if (howManyExplosions >= 10)
							switch_on = 1;
						
					}

					//update all explosions
					for (size_t i = 0; i < maxExplosions; i++)
					{
						if (explosions[i] != NULL)
						{
							if (explosions[i]->update(renderWindow))	// true if animations ends
							{
								delete(explosions[i]);
								explosions[i] = NULL;
							}
						}
					}

					// update all gifts and check collision with gifts
					for (size_t i = 0; i < maxGifts; i++)
					{
						if (gifts[i] != NULL)
						{
							gifts[i]->update(renderWindow);
							if(connectionType == 's')
								for(int j = 0; j < players.size(); j++)
									if (CC(sf::Vector3f(players[j]->getSprite()->getPosition().x, players[j]->getSprite()->getPosition().y,
										static_cast<float>(players[j]->getSprite()->getTextureRect().width))
										, sf::Vector3f(gifts[i]->getSprite()->getPosition().x, gifts[i]->getSprite()->getPosition().y
											, gifts[i]->getSprite()->getTextureRect().width / 2.0f)))
									{
										score[j]+= 700;
										delete(gifts[i]);
										gifts[i] = NULL;
										packetInfo.toDo = 8;
										packetInfo.ID = j;
										packetInfo.ID1 = i;
										packetSend[packetSendWhich] << packetInfo;
										socketTCP.send(packetSend[packetSendWhich]);
										packetSendWhich++;
										break;
									}
						}
					}

					sf::Transform transform;

					sf::RenderStates states;
					states.transform = transform;
					states.texture = &bulletTx;

					renderWindow.draw(vertices, states);

					for (int i = 0; i < players.size(); i++)
					{
						score_txt[i].setString(std::to_string(score[i]));
						hp_txt[i].setString(std::to_string(players[i]->getHP()));
						renderWindow.draw(whichPlayer_txt[i]);
						renderWindow.draw(score_txt[i]);
						renderWindow.draw(hp_txt[i]);
					}

					renderWindow.display();
					BulletPatternCounter++;
					BulletPatternCounter2++;
					BulletPatternCounter3++;
					enemyWaveCounter++;
					playerTextureChange++;
					packetSendWhich = 0;
					if (switch_on != 5)		// end the game
					{
						music_stage1.stop();
						for (size_t i = 0; i < totalBullets; i++)
						{
							if (bullets[i] != NULL)
							{
								sf::Vertex* quad = &vertices[i * 4];
								sf::Sprite *spr = bullets[i]->getSprite();
								quad[0].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[1].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[2].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								quad[3].position = sf::Vector2f(view1.getSize().x + 200, view1.getSize().x + 200);
								spr->setPosition(view1.getSize().x + 200, view1.getSize().x + 200);
								delete bullets[i];
								bullets[i] = NULL;
							}
						}
						for (int i = 0; i < maxEnemies; i++)
							if (enemies[i] != NULL)
							{
								delete enemies[i];
								enemies[i] = NULL;
							}
						for(int i = 0; i < maxGifts; i++)
							if (gifts[i] != NULL)
							{
								delete gifts[i];
								gifts[i] = NULL;
							}
						for (size_t i = 0; i < players.size(); i++)
						{
							delete players[i];
							players[i] = NULL;
						}
						for (int j = 0; j < maxExplosions; j++)
							if (explosions[j] != NULL)
							{
								delete explosions[j];
								explosions[j] = NULL;
							}
						players.resize(1);
						connectionType = 's';
						delete background;
						background = NULL;
						if (boss != NULL)
						{
							delete boss;
							boss = NULL;
						}
					}
				}
				break;
			} // main game
		} // switch
	} // main loop
		return 0;
} // main

// Return true if two circles collides
bool CC(sf::Vector3f c1, sf::Vector3f c2) {
	return (c2.x - c1.x) * (c2.x - c1.x) + (c1.y - c2.y) * (c1.y - c2.y) < (c1.z + c2.z) * (c1.z + c2.z);
}

// Network packets custom operators
sf::Packet& operator <<(sf::Packet& packet, const sf::Vector2f& arg_position)
{
	return packet << arg_position.x << arg_position.y;
}

sf::Packet& operator <<(sf::Packet& packet, const sf::IntRect& arg_whichBullet)
{
	return packet << arg_whichBullet.left << arg_whichBullet.top << arg_whichBullet.width << arg_whichBullet.height;
}

sf::Packet& operator >>(sf::Packet& packet, sf::Vector2f& arg_position)
{
	return packet >> arg_position.x >> arg_position.y;
}

sf::Packet& operator >>(sf::Packet& packet, sf::IntRect& arg_whichBullet)
{
	return packet >> arg_whichBullet.left >> arg_whichBullet.top >> arg_whichBullet.width >> arg_whichBullet.height;
}

sf::Packet& operator <<(sf::Packet& packet, const PacketInfo& arg_packetInfo)
{
	return packet << arg_packetInfo.toDo << arg_packetInfo.ID << arg_packetInfo.ID1 << arg_packetInfo.ID2 << arg_packetInfo.position << arg_packetInfo.speed << arg_packetInfo.whichBullet;
}

sf::Packet& operator >>(sf::Packet& packet, PacketInfo& arg_packetInfo)
{
	return packet >> arg_packetInfo.toDo >> arg_packetInfo.ID >> arg_packetInfo.ID1 >> arg_packetInfo.ID2 >> arg_packetInfo.position >> arg_packetInfo.speed >> arg_packetInfo.whichBullet;
}