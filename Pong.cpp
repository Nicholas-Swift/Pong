// Pong

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <Windows.h>

class SoundPlayer
{
private:
	sf::Sound m_menuUpSound, m_menuClickSound, m_ballCollisionSound;
	sf::Music m_music;

public:
	sf::Sound getMenuUpSound()
	{
		return m_menuUpSound;
	}

	sf::Sound getMenuClickSound()
	{
		return m_menuClickSound;
	}

	sf::Sound getBallCollisionSound()
	{
		return m_ballCollisionSound;
	}

	void setSoundEffectsVolume(int i)
	{
		m_menuUpSound.setVolume(i);
		m_menuClickSound.setVolume(i);
		m_ballCollisionSound.setVolume(i);
	}

	void setMusicVolume(int i)
	{
		m_music.setVolume(i);
	}

public:
	SoundPlayer(sf::SoundBuffer &menuUpBuff, sf::SoundBuffer &menuClickBuff, sf::SoundBuffer &ballCollBuff, int musicV, int soundV)
	{
		m_menuUpSound.setBuffer(menuUpBuff);
		m_menuUpSound.setVolume(soundV);
		m_menuUpSound.setLoop(false);

		m_menuClickSound.setBuffer(menuClickBuff);
		m_menuClickSound.setVolume(soundV);
		m_menuClickSound.setLoop(false);

		m_ballCollisionSound.setBuffer(ballCollBuff);
		m_menuClickSound.setVolume(soundV);
		m_ballCollisionSound.setLoop(false);

		m_music.openFromFile("Assets/music.ogg");
		m_music.setVolume(musicV);
		m_music.setLoop(true);
		m_music.play();
	}

	void playMenuUp()
	{
		m_menuUpSound.play();
	}

	void playMenuClick()
	{
		m_menuClickSound.play();
	}

	void playBallCollision()
	{
		m_ballCollisionSound.play();
	}
};

class Player
{
private:
	sf::RectangleShape m_rect;
	float m_top, m_bottom, m_left, m_right; //for rectangle collision
	bool m_isMovingUp, m_isMovingDown; //is it moving up or down?
	float m_movement, m_speed, m_speedTop, m_speedSlow, m_movementConstant; //rect speed movement related things

public:
	float getTop()
	{
		return m_top;
	}

	float getBottom()
	{
		return m_bottom;
	}

	float getLeft()
	{
		return m_left;
	}

	float getRight()
	{
		return m_right;
	}

public:
	Player(sf::Vector2f position, sf::Vector2f size, sf::Color color)  //<--- tweak this stuff for initilization changes!!
	{
		//general player initializations

		m_rect.setPosition(position);
		m_rect.setSize(size);
		m_rect.setFillColor(color);

		m_top = m_rect.getPosition().y;
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;

		m_isMovingUp = m_isMovingDown = false;

		m_movement = 0;
		m_speed = 0.1f;
		m_speedTop = 100.f;          //<--- tweak this stuff for movement changes!!
		m_speedSlow = 0.2f;
		m_movementConstant = 0.0002f;
	}

	void Update()
	{
		//update collision properties

		m_top = m_rect.getPosition().y;
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;
	}

	void Draw(sf::RenderWindow &window)
	{
		//draw

		window.draw(m_rect);
	}

	void HandleEvents(sf::Keyboard::Key key, bool isPressed) //<--- this stuff is in window.pollEvent()!!
	{
		//Handle movement events and key presses! Note that this is in the window.pollEvent() stuff
		//this way of doing things makes movement extremely smooth

		if (key == sf::Keyboard::Key::W || key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D)
			m_isMovingUp = isPressed;
		if (key == sf::Keyboard::Key::S || key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::A)
			m_isMovingDown = isPressed;
	}

	void Move(sf::Time deltaTime)
	{
		//this is the actual moving part of the handleEvents()

		//if moving up, then move up. If moving down, then move down!
		if(m_isMovingUp)
			m_movement -= m_speed * deltaTime.asMicroseconds() * m_movementConstant * 0.38;
		if(m_isMovingDown)
			m_movement += m_speed * deltaTime.asMicroseconds() * m_movementConstant * 0.38;

		//don't let movement exceed top speed
		if(m_movement < -m_speedTop)
			m_movement = -m_speedTop;
		if(m_movement > m_speedTop)
			m_movement = m_speedTop;

		//if not moving up anymore, slow down
		if(!m_isMovingUp && m_movement < 0)
		{
			m_movement += m_speedSlow * deltaTime.asMicroseconds() * m_movementConstant * 0.26;
			if(m_movement > 0)
				m_movement = 0;
		}
		if(!m_isMovingDown && m_movement > 0)
		{
			m_movement -= m_speedSlow * deltaTime.asMicroseconds() * m_movementConstant * 0.26;
			if(m_movement < 0)
				m_movement = 0;
		}

		//don't let the player rect leave the screen!
		if(m_top < 0)
		{
			m_rect.setPosition(m_rect.getPosition().x, 0);
			m_movement = 0;
		}
		if(m_bottom > /*NOTE WINDOW SIZE*/ 600)
		{
			m_rect.setPosition(m_rect.getPosition().x, /*NOTE WINDOW SIZE*/ 600 - m_rect.getSize().y);
			m_movement = 0;
		}

		//actual movement
		m_rect.move(0, m_movement * deltaTime.asMicroseconds() * m_movementConstant);
	}
};

struct BallParticle
{
	sf::RectangleShape m_rect;
	float m_scale;

	BallParticle(sf::Vector2f position, sf::Vector2f size)
	{
		m_rect.setPosition(position);
		m_rect.setSize(size);
		m_rect.setFillColor(sf::Color::White);

		m_scale = 1;
	}

	void Update(sf::Time deltaTime)
	{
		m_scale -= 0.0025 * deltaTime.asMilliseconds();
		m_rect.setScale(m_scale, m_scale);
	}

	void Draw(sf::RenderWindow &window)
	{
		window.draw(m_rect);
	}
};

class Ball
{
private:
	sf::RectangleShape m_rect;
	float m_top, m_bottom, m_left, m_right; //for collision
	sf::Vector2f m_slope; //slope of the ball
	float m_speed, m_speedAdd, m_movementConstant; //speed of the ball

	std::vector<BallParticle> m_ballTrail; //ball trail

	bool m_respawning; //respawn stuff
	sf::Text m_countDownText;
	bool m_countDownDisplay;

public:
	float getTop()
	{
		return m_top;
	}

	float getBottom()
	{
		return m_bottom;
	}

	float getLeft()
	{
		return m_left;
	}

	float getRight()
	{
		return m_right;
	}

	sf::Vector2f getSlope()
	{
		return m_slope;
	}

	sf::RectangleShape getRect()
	{
		return m_rect;
	}

public:
	Ball(sf::Vector2f position, sf::Vector2f size, sf::Color color, sf::Vector2f slope, sf::Font &font) //<--- tweak this in main()!!
	{
		//general ball initializations

		m_rect.setPosition(position); //general rectangleShape initializations
		m_rect.setSize(size);
		m_rect.setFillColor(color);

		m_top = m_rect.getPosition().y; //for collision
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;

		m_slope.x = slope.x; m_slope.y = slope.y; //slope of the ball to move

		m_speed = 50.f;
		m_speedAdd = 1.f;                   //<--- tweak this for ball speed movement stuff!
		m_movementConstant = 0.000008f;
		
		m_ballTrail.empty();

		m_respawning = false;

		m_countDownText.setCharacterSize(60); //for the countdown after ball goes out of bounds
		m_countDownText.setStyle(sf::Text::Bold);
		m_countDownText.setFont(font);
		m_countDownText.setColor(sf::Color::White);
		m_countDownText.setString("");
		m_countDownText.setPosition(0, 0);

		m_countDownDisplay = false;
	}

	void Update(sf::Time deltaTime, sf::Clock &ballClock, sf::RenderWindow &window)
	{
		//collision stuff
		m_top = m_rect.getPosition().y;
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;

		//ball trail
		if(m_respawning == false) //if ball isn't in countdown stage
		{
			if(ballClock.getElapsedTime().asMilliseconds() > 200)
			{
				BallParticle *ballPart = new BallParticle(m_rect.getPosition(), m_rect.getSize()); //create new Ball Particle
				m_ballTrail.push_back(*ballPart); //put it into the vector
				ballClock.restart();
			}

			for(int i = 0; i < m_ballTrail.size(); i++)
			{
				m_ballTrail[i].Update(deltaTime);
				if(m_ballTrail[i].m_scale < 0.2)
					m_ballTrail.erase(m_ballTrail.begin() + i);
			}
		}

		//respawning count down
		if(m_respawning == true)
		{
			m_ballTrail.clear();

			if(m_countDownDisplay == false)
				m_countDownDisplay = true;

			if(ballClock.getElapsedTime().asMilliseconds() >= 3400)
			{
				RespawnRelease();
			}
			else if(ballClock.getElapsedTime().asMilliseconds() >= 3000)
			{
				if(m_countDownText.getString() != "GO!")
				{
					m_countDownText.setString("GO!");
					m_countDownText.setPosition(window.getSize().x/2 - m_countDownText.getGlobalBounds().width/2, window.getSize().y/2 - m_rect.getSize().y*3.0);
				}
			}
			else if(ballClock.getElapsedTime().asMilliseconds() >= 2000)
			{
				if(m_countDownText.getString() != "1")
				{
					m_countDownText.setString("1");
					m_countDownText.setPosition(window.getSize().x/2 - m_countDownText.getGlobalBounds().width/2, window.getSize().y/2 - m_rect.getSize().y*3.0);
				}
			}
			else if(ballClock.getElapsedTime().asMilliseconds() >= 1000)
			{
				if(m_countDownText.getString() != "2")
				{
					m_countDownText.setString("2");
					m_countDownText.setPosition(window.getSize().x/2 - m_countDownText.getGlobalBounds().width/2, window.getSize().y/2 - m_rect.getSize().y*3.0);
				}
			}
			else if(ballClock.getElapsedTime().asMilliseconds() >= 0)
			{
				if(m_countDownText.getString() != "3")
				{
					m_countDownText.setString("3");
					m_countDownText.setPosition(window.getSize().x/2 - m_countDownText.getGlobalBounds().width/2, window.getSize().y/2 - m_rect.getSize().y*3.0);
				}
			}
		}
	}

	void Draw(sf::RenderWindow &window)
	{
		//draw ball
		window.draw(m_rect);

		//draw ball trail
		for(int i = 0; i < m_ballTrail.size(); i++)
			m_ballTrail[i].Draw(window);

		//draw countdown
		if(m_countDownDisplay == true)
			window.draw(m_countDownText);
	}

	void Move(sf::Time &deltaTime)
	{
		//move the ball
		m_rect.move(m_speed * m_slope.x * deltaTime.asMicroseconds() * m_movementConstant, m_speed * m_slope.y * deltaTime.asMicroseconds() * m_movementConstant);
	}

	void Collision(sf::RenderWindow &window, SoundPlayer &sp, Player p, float aiGetTop, float aiGetRight, float aiGetBottom, float aiGetLeft) //<--- should get AI a, but weird class referencing each other. Bad workaround!!
	{
		//Collision stuff

		//if colliding with top or bottom
		if(m_top < 0 || m_bottom > window.getSize().y)
		{
			sp.playBallCollision(); //<--- play sound!

			m_slope.y = -m_slope.y; //<--- collided, so change the slope

			if(m_top < 0) //<--- collided top
				m_rect.setPosition(m_rect.getPosition().x, 0); //set position of the ball so it's not over anything
			else if(m_bottom > window.getSize().y) //<--- collided bottom
				m_rect.setPosition(m_rect.getPosition().x, window.getSize().y - m_rect.getSize().y - 0.01); //set position of the ball so it's not over anything
		}

		//if colliding with ai
		if(m_right < aiGetLeft || m_left > aiGetRight || m_top > aiGetBottom || m_bottom < aiGetTop)
			{}
		else
		{
			sp.playBallCollision(); //<--- play sound!

			if( ((m_top + m_bottom)*0.5) < ((aiGetTop + aiGetBottom)*0.5) ) //if upper half
				m_slope.y -= 0.02;
			else
				m_slope.y += 0.02;

			m_slope.x = -m_slope.x;
			m_speed += m_speedAdd;

			//set pos
			m_rect.setPosition(aiGetLeft - 0.01 - m_rect.getSize().x, m_rect.getPosition().y);
		}

		//if colliding with player
		if(m_right < p.getLeft() || m_left > p.getRight() || m_top > p.getBottom() || m_bottom < p.getTop())
			{}
		else
		{
			sp.playBallCollision(); //<--- play sound!

			if( ((m_top + m_bottom)*0.5) < ((p.getTop() + p.getBottom())*0.5) ) //if upper half
				m_slope.y -= 0.02;
			else
				m_slope.y += 0.02;

			m_slope.x = -m_slope.x;
			m_speed += m_speedAdd;

			//set pos
			m_rect.setPosition(p.getRight() + 0.01, m_rect.getPosition().y);
		}
	}

	void RespawnCountDown(sf::Vector2f position, sf::Vector2f slope)
	{
		m_speed = 0.f;
		m_rect.setPosition(position);
		m_slope.x = slope.x; m_slope.y = slope.y;
		m_respawning = true;
	}

	void RespawnRelease()
	{
		m_countDownDisplay = false;
		m_respawning = false;
		m_speed = 50.f;
	}
};

class AI
{
private:
	sf::RectangleShape m_rect;
	float m_top, m_bottom, m_left, m_right; //for rectangle collision
	bool m_isMovingUp, m_isMovingDown; //is it moving up or down?
	float m_movement, m_speed, m_speedTop, m_speedSlow, m_movementConstant; //rect speed movement related things
	int m_AILevel;

	bool m_AILEVEL3STUFF; //for insane AI stuff
	float m_HowManyBounces;
	float m_PositionToHit;

public:
	float getTop()
	{
		return m_top;
	}

	float getBottom()
	{
		return m_bottom;
	}

	float getLeft()
	{
		return m_left;
	}

	float getRight()
	{
		return m_right;
	}

	float getAILevel()
	{
		return m_AILevel;
	}
	
	void setAILevel(int i) //note: either 0 (easy), 1 (medium), 2 (hard)
	{
		m_AILevel = i;
		m_AILEVEL3STUFF = true;
	}

	void setAILevel3Bool(bool b)
	{
		m_AILEVEL3STUFF = b;
	}

public:
	AI(sf::Vector2f position, sf::Vector2f size, sf::Color color, int aiLevel)  //<--- tweak this stuff for initilization changes!!
	{
		//general player initializations

		m_rect.setSize(size);
		position.x -= size.x; //<--- note because thing is on right, gotta move it over to left
		m_rect.setPosition(position);
		m_rect.setFillColor(color);

		m_top = m_rect.getPosition().y;
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;

		m_isMovingUp = m_isMovingDown = false;

		m_movement = 0;
		m_movementConstant = 0.00018f;

		MenuUpdate(aiLevel);

		m_AILEVEL3STUFF = false;
		m_HowManyBounces = -1;
		m_PositionToHit = -69;
	}

	void Update()
	{
		//update collision properties

		m_top = m_rect.getPosition().y;
		m_bottom = m_rect.getPosition().y + m_rect.getSize().y;
		m_left = m_rect.getPosition().x;
		m_right = m_rect.getPosition().x + m_rect.getSize().x;
	}

	void Draw(sf::RenderWindow &window)
	{
		//draw
		window.draw(m_rect);
	}

	void MenuUpdate(int newLevel)
	{
		m_AILevel = newLevel;

		switch(m_AILevel)
		{
		case 0:
			m_speed = 0.053f;
			m_speedTop = 100.f;
			m_speedSlow = 0.16f;
			break;
		case 1:
			m_speed = 0.11f;
			m_speedTop = 110.f;
			m_speedSlow = 0.34f;
			break;
		case 2:
			m_speed = 0.2f;
			m_speedTop = 150.f;
			m_speedSlow = 20.0f;
			break;
		}
	}

	void HandleMovement(sf::RenderWindow &window, Ball &b)
	{
		//AI Handle Movement! This depends on m_AILevel

		switch(m_AILevel)
		{
			case 0: //EASY
			//move when it's coming your way + stay ahead of the ball
			if(b.getSlope().x > 0)
			{
				if(b.getSlope().y < 0) //moving up
				{
					if(b.getBottom() < getBottom())
						m_isMovingUp = true;
					else
						m_isMovingUp = false; //don't go past the ball!
				}
				else
					m_isMovingUp = false; //if ball isn't moving up, rect doesn't go up!

				if(b.getSlope().y > 0) //moving down
				{
					if(b.getTop() > getTop())
						m_isMovingDown = true;
					else
						m_isMovingDown = false; //don't go past the ball!
				}
				else
					m_isMovingDown = false; //if ball isn't moving down, rect doesn't go down!
			}
			if(b.getSlope().x < 0)
			{
				m_isMovingUp = m_isMovingDown = false;
			}

			break;

		case 1:
			//general move where ball is moving
			if(b.getSlope().y < 0)
			{
				if(b.getBottom() < getBottom())
					m_isMovingUp = true;
				else
					m_isMovingUp = false;
			}
			else
			{
				m_isMovingUp = false;
			}

			if(b.getSlope().y > 0)
			{
				if(b.getTop() > getTop())
					m_isMovingDown = true;
				else
					m_isMovingDown = false;
			}
			else
			{
				m_isMovingDown = false;
			}

			break;

		case 2:
			if(b.getSlope().x < 0)
			{
				m_AILEVEL3STUFF = true;
				m_isMovingUp = m_isMovingDown = false;
				m_PositionToHit = -69;
			}
			else if(b.getSlope().x > 0 && b.getSlope().y < 0) //moving up
			{
				//get how many bounces
				if(m_AILEVEL3STUFF == true)
				{
					m_HowManyBounces = (((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x)) - b.getTop()) / window.getSize().y;

					if(m_HowManyBounces >= 0 && m_HowManyBounces < 1) //0 bounces, coming up
						m_PositionToHit = ((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) - b.getTop());
					else if(m_HowManyBounces >= 1 && m_HowManyBounces < 2) //1 bounce, coming down
						m_PositionToHit = -( (window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) - 2 * window.getSize().y - b.getTop());
					else if(m_HowManyBounces >= 2 && m_HowManyBounces < 3) //2 bounces, coming up
						m_PositionToHit = ((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) - 2 * window.getSize().y - b.getTop());
					else if(m_HowManyBounces >= 3 && m_HowManyBounces < 4) //3 bounces, coming down
						m_PositionToHit = -((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) + 4 * window.getSize().y + b.getTop());
					else if(m_HowManyBounces >= 4 && m_HowManyBounces < 5) //4 bounces, coming up
						m_PositionToHit = ((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) - 4 * window.getSize().y - b.getTop());
					else if(m_HowManyBounces >= 5 && m_HowManyBounces < 6) //5 bounces, coming down
						m_PositionToHit = -((window.getSize().x - b.getRight()) * (-b.getSlope().y / b.getSlope().x) + 6 * window.getSize().y + b.getTop());

					m_AILEVEL3STUFF = false;
				}

			}
			else if(b.getSlope().x > 0 && b.getSlope().y > 0) //moving down
			{
				//get how many bounces
				if(m_AILEVEL3STUFF == true)
				{
					m_HowManyBounces = ( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) + b.getTop() ) / window.getSize().y;

					if(m_HowManyBounces >= 0 && m_HowManyBounces < 1) //0 bounces
						m_PositionToHit = ( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) + b.getTop());
					else if(m_HowManyBounces >= 1 && m_HowManyBounces < 2) //1 bounce
						m_PositionToHit = -( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) - 2 * window.getSize().y + b.getTop());
					else if(m_HowManyBounces >= 2 && m_HowManyBounces < 3) //2 bounces
						m_PositionToHit = ( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) - 2 * window.getSize().y + b.getTop());
					else if(m_HowManyBounces >= 3 && m_HowManyBounces < 4) //3 bounces
						m_PositionToHit = -( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) + 4 * window.getSize().y + b.getTop());
					else if(m_HowManyBounces >= 4 && m_HowManyBounces < 5) //4 bounces
						m_PositionToHit = ( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) - 4 * window.getSize().y + b.getTop());
					else if(m_HowManyBounces >= 5 && m_HowManyBounces < 6) //5 bounces
						m_PositionToHit = ( (window.getSize().x - b.getRight()) * (b.getSlope().y / b.getSlope().x) + 6 * window.getSize().y + b.getTop());

					m_AILEVEL3STUFF = false;
				}
			}

			if(m_PositionToHit != -69)
			{
				if(m_PositionToHit < getTop())
					m_isMovingUp = true;
				else
					m_isMovingUp = false;

				if(m_PositionToHit > getBottom())
					m_isMovingDown = true;
				else
					m_isMovingDown = false;
			}

			break;
		}
	}

	void Move(sf::Time deltaTime)
	{
		//this is the actual moving part of the handleMovement()

		//if moving up, then move up. If moving down, then move down!
		if(m_isMovingUp)
			m_movement -= m_speed * deltaTime.asMicroseconds() * m_movementConstant * 0.38;
		if(m_isMovingDown)
			m_movement += m_speed * deltaTime.asMicroseconds() * m_movementConstant * 0.38;

		//don't let movement exceed top speed
		if(m_movement < -m_speedTop)
			m_movement = -m_speedTop;
		if(m_movement > m_speedTop)
			m_movement = m_speedTop;

		//if not moving up anymore, slow down
		if(!m_isMovingUp && m_movement < 0)
		{
			m_movement += m_speedSlow * deltaTime.asMicroseconds() * m_movementConstant * 0.26;
			if(m_movement > 0)
				m_movement = 0;
		}
		if(!m_isMovingDown && m_movement > 0)
		{
			m_movement -= m_speedSlow * deltaTime.asMicroseconds() * m_movementConstant * 0.26;
			if(m_movement < 0)
				m_movement = 0;
		}

		//don't let the player rect leave the screen!
		if(m_top < 0)
		{
			m_rect.setPosition(m_rect.getPosition().x, 0);
			m_movement = 0;
		}
		if(m_bottom > /*NOTE WINDOW SIZE*/ 600)
		{
			m_rect.setPosition(m_rect.getPosition().x, /*NOTE WINDOW SIZE*/ 600 - m_rect.getSize().y);
			m_movement = 0;
		}

		//actual movement
		m_rect.move(0, m_movement * deltaTime.asMicroseconds() * m_movementConstant);
	}
};

class Menu
{
private:
	bool m_menuUp;
	sf::RectangleShape m_backgroundShade;
	std::vector<sf::RectangleShape> m_rects;
	std::vector<sf::Text> m_texts;
	sf::Text m_mainText;
	float m_state;

	sf::Text m_backText;
	sf::RectangleShape m_backRect;

	sf::Text m_highScoreText;
	int m_highScore;
	int m_currentScore;

	std::vector<sf::Text> m_optionsTexts;
	std::vector<sf::Text> m_optionsButtons;
	std::vector<sf::Text> m_optionsLevels;

	int m_optionsAILevel;
	int m_optionsSoundEffectsVolume;
	int m_optionsMusicVolume;

public:
	bool getMenuUp()
	{
		return m_menuUp;
	}
	
	void setMenuUp(bool b)
	{
		m_menuUp = b;
	}

	sf::RectangleShape getBackgroundShade()
	{
		return m_backgroundShade;
	}

	std::vector<sf::Text> getTexts()
	{
		return m_texts;
	}

	sf::Text getTexts(int i)
	{
		return m_texts[i];
	}

	void setState(int i)
	{
		m_state = i;
	}

	sf::Text getMainText()
	{
		return m_mainText;
	}

	void MainTextUpdate(sf::RenderWindow &window)
	{
		m_mainText.setString("MENU");
		m_mainText.setPosition(window.getSize().x/2 - m_mainText.getGlobalBounds().width/2, 0);
	}

	int getCurrentScore()
	{
		return m_currentScore;
	}

	int getMOptionsMusicVolume()
	{
		return m_optionsMusicVolume;
	}

	int getMOptionsSoundEffectsVolume()
	{
		return m_optionsSoundEffectsVolume;
	}

public:
	Menu(float windowx, float windowy, sf::Font &menuFont, std::vector<std::string> strings, int score, int music, int sound, int ailevel) //<--- change these things to change menu options
	{
		m_menuUp = true;
		m_backgroundShade.setSize(sf::Vector2f(windowx, windowy));
		m_backgroundShade.setFillColor(sf::Color(0, 0, 0, 240));
		m_rects.clear();
		m_texts.clear();
		m_state = 0;

		m_mainText.setCharacterSize(80); //<--- text for the main menu thing. MENU, HIGHSCORE, OPTIONS
		m_mainText.setStyle(sf::Text::Bold);
		m_mainText.setStyle(sf::Text::Underlined);
		m_mainText.setFont(menuFont);
		m_mainText.setColor(sf::Color::White);
		m_mainText.setString("MENU");
		m_mainText.setPosition(windowx/2 - m_mainText.getGlobalBounds().width/2, 0);

		m_backText.setCharacterSize(40); //<--- for back options in highscore and options
		m_backText.setStyle(sf::Text::Bold);
		m_backText.setFont(menuFont);
		m_backText.setColor(sf::Color::White);
		m_backText.setString("Back");

		m_backRect.setSize(sf::Vector2f(m_backText.getGlobalBounds().width, m_backText.getGlobalBounds().height));

		m_highScore = score;
		m_optionsMusicVolume = music;
		m_optionsSoundEffectsVolume = sound;
		m_optionsAILevel = ailevel;

		m_highScoreText.setCharacterSize(60); //<--- text for the main menu thing. MENU, HIGHSCORE, OPTIONS
		m_highScoreText.setStyle(sf::Text::Bold);
		m_highScoreText.setFont(menuFont);
		m_highScoreText.setColor(sf::Color::White);
		m_highScoreText.setString("Your highscore is " + std::to_string(m_highScore) + "!");
		m_highScoreText.setPosition(windowx/2 - m_highScoreText.getGlobalBounds().width/2, 100);

		m_currentScore = 0;

		//menu text and rect
		for(int i = 0; i < strings.size(); i++)
		{
			sf::Text *text = new sf::Text;  //<--- Setting where the text is/how big it is
			text->setString(strings[i]);
			text->setFont(menuFont);
			text->setCharacterSize(60);
			text->setStyle(sf::Text::Bold);

			text->setPosition(windowx/2 - text->getGlobalBounds().width/2, 100*(i+1)); //<--- text position halfway higher for some reason?

			m_texts.push_back(*text);

			sf::RectangleShape *rectshape = new sf::RectangleShape; //<--- rectangle shape so easier to hover
			rectshape->setSize(sf::Vector2f(text->getGlobalBounds().width, text->getGlobalBounds().height));
			rectshape->setPosition(text->getPosition().x, text->getPosition().y); //<--- visible text is set slightly lower for some reason? Lower the rect

			m_rects.push_back(*rectshape);
		}

		for(int i = 0; i < 3; i++) //options text stuff
		{
			sf::Text *text = new sf::Text();
			text->setCharacterSize(30);
			text->setStyle(sf::Text::Bold);
			text->setFont(menuFont);
			text->setColor(sf::Color::White);

			if(i == 0)
				text->setString("Music");
			else if(i == 1)
				text->setString("Sound Effects");
			else if(i == 2)
				text->setString("AI Level");

			text->setPosition(windowx/2 - text->getGlobalBounds().width/2, 100*i + 120);

			m_optionsTexts.push_back(*text);
		}

		//options levels
		for(int i = 0; i < 3; i++)
		{
			sf::Text *text = new sf::Text;
			text->setFont(menuFont);
			text->setCharacterSize(50);
			text->setStyle(sf::Text::Bold);

			if(i == 0)
				text->setString(std::to_string(m_optionsMusicVolume));
			else if(i == 1)
				text->setString(std::to_string(m_optionsSoundEffectsVolume));
			else if(i == 2)
			{
				if(m_optionsAILevel == 0)
					text->setString("Easy");
				else if(m_optionsAILevel == 1)
					text->setString("Medium");
				else if(m_optionsAILevel == 2)
					text->setString("Hard");
			}

			text->setPosition(windowx/2 - text->getGlobalBounds().width/2, 100*i + 150);

			m_optionsLevels.push_back(*text);
		}

		//options buttons stuff
		for(int i = 0; i < 6; i++)
		{
			sf::Text *text = new sf::Text();
			text->setCharacterSize(60);
			text->setStyle(sf::Text::Bold);
			text->setFont(menuFont);
			text->setColor(sf::Color::White);

			if(i%2 == 0)
				text->setString("<");
			else
				text->setString(">");

			if(i == 0)
				text->setPosition(m_optionsLevels[0].getPosition().x - 107, m_optionsLevels[0].getPosition().y - 6);
			else if(i == 1)
				text->setPosition(m_optionsLevels[0].getPosition().x + m_optionsLevels[0].getGlobalBounds().width + 92, m_optionsLevels[0].getPosition().y - 6);
			else if(i == 2)
				text->setPosition(m_optionsLevels[1].getPosition().x - 107, m_optionsLevels[1].getPosition().y - 6);
			else if(i == 3)
				text->setPosition(m_optionsLevels[1].getPosition().x + m_optionsLevels[1].getGlobalBounds().width + 92, m_optionsLevels[1].getPosition().y - 6);
			else if(i == 4)
				text->setPosition(m_optionsLevels[2].getPosition().x - 80, m_optionsLevels[2].getPosition().y - 6);
			else if(i == 5)
				text->setPosition(m_optionsLevels[2].getPosition().x + m_optionsLevels[2].getGlobalBounds().width + 70, m_optionsLevels[2].getPosition().y - 6);

			m_optionsButtons.push_back(*text);
		}

	}

	void Update(sf::RenderWindow &window)
	{
		if(m_state == 0)
		{
			for(int i = 0; i < m_rects.size(); i++)
			{
				//(right < left || left > right || top > bottom || bottom < top) collision stuff
				if(sf::Mouse::getPosition(window).x < m_rects[i].getPosition().x || sf::Mouse::getPosition(window).x > m_rects[i].getPosition().x + m_rects[i].getSize().x || sf::Mouse::getPosition(window).y > m_rects[i].getPosition().y + m_rects[i].getSize().y || sf::Mouse::getPosition(window).y < m_rects[i].getPosition().y)
				{
					if(m_texts[i].getColor() == sf::Color::White) //<--- so it's not constantly filling color
					{}
					else
						m_texts[i].setColor(sf::Color::White);
				}
				else
				{
					if(m_texts[i].getColor() == sf::Color::Green) //<--- so it's not constantly filling color
					{}
					else
						m_texts[i].setColor(sf::Color::Green);
				}
			}
		}
		else if(m_state == 1)
		{
			if(sf::Mouse::getPosition(window).x < m_backRect.getPosition().x || sf::Mouse::getPosition(window).x > m_backRect.getPosition().x + m_backRect.getSize().x || sf::Mouse::getPosition(window).y > m_backRect.getPosition().y + m_backRect.getSize().y || sf::Mouse::getPosition(window).y < m_backRect.getPosition().y)
			{
				if(m_backText.getColor() == sf::Color::White) //<--- so it's not constantly filling color
				{}
				else
					m_backText.setColor(sf::Color::White);
			}
			else
			{
				if(m_backText.getColor() == sf::Color::Green) //<--- so it's not constantly filling color
				{}
				else
					m_backText.setColor(sf::Color::Green);
			}
		}
		else if(m_state == 2)
		{
			//back button fill
			if(sf::Mouse::getPosition(window).x < m_backRect.getPosition().x || sf::Mouse::getPosition(window).x > m_backRect.getPosition().x + m_backRect.getSize().x || sf::Mouse::getPosition(window).y > m_backRect.getPosition().y + m_backRect.getSize().y || sf::Mouse::getPosition(window).y < m_backRect.getPosition().y)
			{
				if(m_backText.getColor() == sf::Color::White) //<--- so it's not constantly filling color
				{}
				else
					m_backText.setColor(sf::Color::White);
			}
			else
			{
				if(m_backText.getColor() == sf::Color::Green) //<--- so it's not constantly filling color
				{}
				else
					m_backText.setColor(sf::Color::Green);
			}

			//arrow button fill
			for(int i = 0; i < m_optionsButtons.size(); i++)
			{
				if(sf::Mouse::getPosition(window).x < (m_optionsButtons[i].getPosition().x - 10) || sf::Mouse::getPosition(window).x > (m_optionsButtons[i].getPosition().x + m_optionsButtons[i].getGlobalBounds().width + 10) || sf::Mouse::getPosition(window).y > (m_optionsButtons[i].getPosition().y + m_optionsButtons[i].getGlobalBounds().height*3) || sf::Mouse::getPosition(window).y < (m_optionsButtons[i].getPosition().y + m_optionsButtons[i].getGlobalBounds().height*0.5))
				{
					if(m_optionsButtons[i].getColor() == sf::Color::White)
					{}
					else
						m_optionsButtons[i].setColor(sf::Color::White);
				}
				else
				{
					if(m_optionsButtons[i].getColor() == sf::Color::Green)
					{}
					else
						m_optionsButtons[i].setColor(sf::Color::Green);
				}
			}
		}
	}

	void Draw(sf::RenderWindow &window)
	{
		//draw the shade and draw the text!
		window.draw(getBackgroundShade());
		window.draw(m_mainText);

		if(m_state == 0)
		{
			//displaying stuff in general menu

			for(int i = 0; i < m_texts.size(); i++)
			{
				window.draw(getTexts(i));
			}
		}
		else if(m_state == 1)
		{
			//displaying stuff in high score

			window.draw(m_highScoreText);
			window.draw(m_backText);
		}
		else if(m_state == 2)
		{
			//displaying stuff in options
			window.draw(m_backText);

			for(int i = 0; i < m_optionsTexts.size(); i++)
			{
				window.draw(m_optionsTexts[i]);
			}

			for(int i = 0; i < m_optionsButtons.size(); i++)
			{
				window.draw(m_optionsButtons[i]);
			}

			for(int i = 0; i < m_optionsLevels.size(); i++)
			{
				window.draw(m_optionsLevels[i]);
			}
		}
	}

	void Click(sf::RenderWindow &window, SoundPlayer &sp, AI &ai)
	{
		if(m_state == 0)
		{
			if(sf::Mouse::getPosition(window).x < m_rects[0].getPosition().x || sf::Mouse::getPosition(window).x > m_rects[0].getPosition().x + m_rects[0].getSize().x || sf::Mouse::getPosition(window).y > m_rects[0].getPosition().y + m_rects[0].getSize().y || sf::Mouse::getPosition(window).y < m_rects[0].getPosition().y)
			{} //<--- this is the first rect, "PLAY"
			else
			{
				sp.playMenuClick(); //<--- play sound!

				setMenuUp(false);
			}
		
			if(sf::Mouse::getPosition(window).x < m_rects[1].getPosition().x || sf::Mouse::getPosition(window).x > m_rects[1].getPosition().x + m_rects[1].getSize().x || sf::Mouse::getPosition(window).y > m_rects[1].getPosition().y + m_rects[1].getSize().y || sf::Mouse::getPosition(window).y < m_rects[1].getPosition().y)
			{} //<--- this is the second rect, "HIGH SCORE"
			else
			{
				sp.playMenuClick(); //<--- play sound!

				m_state = 1;
				m_mainText.setString("HIGHSCORE");
				m_mainText.setPosition(window.getSize().x/2 - m_mainText.getGlobalBounds().width/2, 0);
				m_backText.setPosition(window.getSize().x/2 - m_backText.getGlobalBounds().width/2, 200);
				m_backRect.setPosition(m_backText.getPosition().x, m_backText.getPosition().y);
			}
		
			if(sf::Mouse::getPosition(window).x < m_rects[2].getPosition().x || sf::Mouse::getPosition(window).x > m_rects[2].getPosition().x + m_rects[2].getSize().x || sf::Mouse::getPosition(window).y > m_rects[2].getPosition().y + m_rects[2].getSize().y || sf::Mouse::getPosition(window).y < m_rects[2].getPosition().y)
			{} //<--- this is the third rect, "OPTIONS"
			else
			{
				sp.playMenuClick(); //<--- play sound!

				m_state = 2;
				m_mainText.setString("OPTIONS");
				m_mainText.setPosition(window.getSize().x/2 - m_mainText.getGlobalBounds().width/2, 0);
				m_backText.setPosition(window.getSize().x/2 - m_backText.getGlobalBounds().width/2, 450);
				m_backRect.setPosition(m_backText.getPosition().x, m_backText.getPosition().y);
			}

			if(sf::Mouse::getPosition(window).x < m_rects[3].getPosition().x || sf::Mouse::getPosition(window).x > m_rects[3].getPosition().x + m_rects[3].getSize().x || sf::Mouse::getPosition(window).y > m_rects[3].getPosition().y + m_rects[3].getSize().y || sf::Mouse::getPosition(window).y < m_rects[3].getPosition().y)
			{} //<--- this is the fourth rect, "EXIT"
			else
			{
				UpdateTextFile();
				sp.playMenuClick();
				window.close();
			}
		}
		else if(m_state == 1)
		{
			//doing stuff in highscore

			if(sf::Mouse::getPosition(window).x < m_backRect.getPosition().x || sf::Mouse::getPosition(window).x > m_backRect.getPosition().x + m_backRect.getSize().x || sf::Mouse::getPosition(window).y > m_backRect.getPosition().y + m_backRect.getSize().y || sf::Mouse::getPosition(window).y < m_backRect.getPosition().y)
			{} //<--- if click back
			else
			{
				sp.playMenuClick(); //<--- play sound!

				MainTextUpdate(window);
				m_state = 0;
			}
		}
		else if(m_state == 2)
		{
			//doing stuff in options

			for(int i = 0; i < m_optionsButtons.size(); i++)
			{
				if(sf::Mouse::getPosition(window).x < (m_optionsButtons[i].getPosition().x - 10) || sf::Mouse::getPosition(window).x > (m_optionsButtons[i].getPosition().x + m_optionsButtons[i].getGlobalBounds().width + 10) || sf::Mouse::getPosition(window).y > (m_optionsButtons[i].getPosition().y + m_optionsButtons[i].getGlobalBounds().height*3) || sf::Mouse::getPosition(window).y < (m_optionsButtons[i].getPosition().y + m_optionsButtons[i].getGlobalBounds().height*0.5))
				{}
				else
				{
					sp.playMenuClick(); //<--- play sound!

					if(i == 0 && m_optionsMusicVolume > 0) //<--- music left
					{
						m_optionsMusicVolume -= 2;
						sp.setMusicVolume(m_optionsMusicVolume);

						m_optionsLevels[0].setString(std::to_string(m_optionsMusicVolume));
						m_optionsLevels[0].setPosition(window.getSize().x/2 - m_optionsLevels[0].getGlobalBounds().width/2, 150);
					}
					else if(i == 1 && m_optionsMusicVolume < 100) //<--- music right
					{
						m_optionsMusicVolume += 2;
						sp.setMusicVolume(m_optionsMusicVolume);

						m_optionsLevels[0].setString(std::to_string(m_optionsMusicVolume));
						m_optionsLevels[0].setPosition(window.getSize().x/2 - m_optionsLevels[0].getGlobalBounds().width/2, 150);
					}
					else if(i == 2 && m_optionsSoundEffectsVolume > 0) //<--- sound effect left
					{
						m_optionsSoundEffectsVolume -= 2;
						sp.setSoundEffectsVolume(m_optionsSoundEffectsVolume);

						m_optionsLevels[1].setString(std::to_string(m_optionsSoundEffectsVolume));
						m_optionsLevels[1].setPosition(window.getSize().x/2 - m_optionsLevels[1].getGlobalBounds().width/2, 250);
					}
					else if(i == 3 && m_optionsSoundEffectsVolume < 100) //<--- sound effect right
					{
						m_optionsSoundEffectsVolume += 2;
						sp.setSoundEffectsVolume(m_optionsSoundEffectsVolume);

						m_optionsLevels[1].setString(std::to_string(m_optionsSoundEffectsVolume));
						m_optionsLevels[1].setPosition(window.getSize().x/2 - m_optionsLevels[1].getGlobalBounds().width/2, 250);
					}
					else if(i == 4 && m_optionsAILevel > 0) //<--- AI level left
					{
						m_optionsAILevel -= 1;
						ai.MenuUpdate(m_optionsAILevel);

						if(m_optionsAILevel == 0)
							m_optionsLevels[2].setString("Easy");
						else if(m_optionsAILevel == 1)
							m_optionsLevels[2].setString("Medium");
						else if(m_optionsAILevel == 2)
							m_optionsLevels[2].setString("Hard");

						m_optionsLevels[2].setPosition(window.getSize().x/2 - m_optionsLevels[2].getGlobalBounds().width/2, 350);
					}
					else if(i == 5 && m_optionsAILevel < 2) //<--- AI level right
					{
						m_optionsAILevel += 1;
						ai.MenuUpdate(m_optionsAILevel);

						if(m_optionsAILevel == 0)
							m_optionsLevels[2].setString("Easy");
						else if(m_optionsAILevel == 1)
							m_optionsLevels[2].setString("Medium");
						else if(m_optionsAILevel == 2)
							m_optionsLevels[2].setString("Hard");

						m_optionsLevels[2].setPosition(window.getSize().x/2 - m_optionsLevels[2].getGlobalBounds().width/2, 350);
					}
				}
			}

			if(sf::Mouse::getPosition(window).x < m_backRect.getPosition().x || sf::Mouse::getPosition(window).x > m_backRect.getPosition().x + m_backRect.getSize().x || sf::Mouse::getPosition(window).y > m_backRect.getPosition().y + m_backRect.getSize().y || sf::Mouse::getPosition(window).y < m_backRect.getPosition().y)
			{} //<--- if click back
			else
			{
				sp.playMenuClick();

				MainTextUpdate(window);
				m_state = 0;
			}
		}
	}

	void BallOutOfBounds(Ball &ball, sf::Text &text, sf::RenderWindow &window, sf::Clock &clock, AI &a)
	{
		//if out of bounds left
		if(ball.getLeft() < -45.f)
		{
			//respawn the ball
			ball.RespawnCountDown(sf::Vector2f(window.getSize().x / 2 - ball.getRect().getSize().x/2, window.getSize().y / 2 - ball.getRect().getSize().y/2), sf::Vector2f(1.f, -1.f));
			clock.restart();

			a.setAILevel3Bool(true);
		}

		//if out of bounds right
		if(ball.getRight() > window.getSize().x + 45.f)
		{
			m_currentScore += 1; //<--- update the score and stuff
			if(m_currentScore > m_highScore)
			{
				m_highScore = m_currentScore;
				m_highScoreText.setString("Your highscore is " + std::to_string(m_highScore) + "!");
			}

			text.setString(std::string("Score: " + std::to_string(getCurrentScore())));
			text.setPosition(window.getSize().x/2 - text.getGlobalBounds().width/2, 0);

			ball.RespawnCountDown(sf::Vector2f(window.getSize().x / 2 - ball.getRect().getSize().x/2, window.getSize().y / 2 - ball.getRect().getSize().y/2), sf::Vector2f(1.f, -1.f));
			clock.restart();

			a.setAILevel3Bool(true);
		}
	}

	void UpdateTextFile()
	{
		std::ofstream props("Assets/gameProperties.txt");
		props.clear();
		props<<m_highScore;
		props<<"\n";
		props<<m_optionsMusicVolume;
		props<<"\n";
		props<<m_optionsSoundEffectsVolume;
		props<<"\n";
		props<<m_optionsAILevel;

		props.close();
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//render window + stuff
	sf::RenderWindow window(sf::VideoMode(800, 600), "Pong", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(200);
	sf::Clock deltaClock;
	sf::Time deltaTime;

	//loading game properties
	std::ifstream props("Assets/gameProperties.txt");
	int sco(0), mus(0), sou(0), ailvl(0);
	if(props.is_open())
	{
		std::string strLine = "";
		int nLineCount = 0;

		while(std::getline(props, strLine))
		{
			if(nLineCount == 0)
				sco = std::stoi(strLine);
			else if(nLineCount == 1)
				mus = std::stoi(strLine);
			else if(nLineCount == 2)
				sou = std::stoi(strLine);
			else if(nLineCount == 3)
				ailvl = std::stoi(strLine);

			nLineCount++;
		}
	}
	props.close();

	//sound player
	sf::SoundBuffer menuUpBuffer, menuClickBuffer, ballCollisionBuffer;
	menuUpBuffer.loadFromFile("Assets/menuClickBuffer.wav"); menuClickBuffer.loadFromFile("Assets/menuClickBuffer.wav"); ballCollisionBuffer.loadFromFile("Assets/ballCollisionBuffer.wav");
	SoundPlayer soundplayer(menuUpBuffer, menuClickBuffer, ballCollisionBuffer, mus, sou);

	//player
	sf::Vector2f playerPosition(window.getSize().x / 50, 0);
	sf::Vector2f playerSize(window.getSize().x / 34, window.getSize().y / 3.5);
	sf::Color playerColor(sf::Color::White);
	Player player(Player(playerPosition, playerSize, playerColor));

	//ai
	sf::Vector2f aiPosition(window.getSize().x - (window.getSize().x / 50), 0);
	sf::Vector2f aiSize(window.getSize().x / 34, window.getSize().y / 3.5);
	sf::Color aiColor(sf::Color::White);
	AI ai(AI(aiPosition, aiSize, aiColor, ailvl));

	//ball
	sf::Clock ballClock;
	sf::Font menuFont; menuFont.loadFromFile("Assets/PRISTINA.TTF"); //for menu, but for ball also!
	sf::Vector2f ballSize(window.getSize().x / 20, window.getSize().y / 20);
	sf::Vector2f ballPosition(window.getSize().x / 2 - ballSize.x/2, window.getSize().y / 2 - ballSize.y/2);
	sf::Color ballColor(sf::Color::White);
	sf::Vector2f ballSlope(-1.f, -1.f);
	Ball ball(Ball(ballPosition, ballSize, ballColor, ballSlope, menuFont));

	//menu
	std::string str1 = "Play", str2 = "Highscore", str3 = "Options", str4 = "Exit";
	std::vector<std::string> strings; strings.push_back(str1); strings.push_back(str2); strings.push_back(str3); strings.push_back(str4);
	Menu menu(Menu(window.getSize().x, window.getSize().y, menuFont, strings, sco, mus, sou, ailvl));

	//To display current score at the top of the screen
	sf::Text currScoreText;
	currScoreText.setCharacterSize(60); //<--- text for the main menu thing. MENU, HIGHSCORE, OPTIONS
	currScoreText.setStyle(sf::Text::Bold);
	currScoreText.setFont(menuFont);
	currScoreText.setColor(sf::Color::White);
	currScoreText.setString(std::string("Score: " + std::to_string(menu.getCurrentScore())));
	currScoreText.setPosition(window.getSize().x/2 - currScoreText.getGlobalBounds().width/2, 0);

	while(window.isOpen())
	{
		deltaTime = deltaClock.restart();

		sf::Event Event;
		while(window.pollEvent(Event))
		{
			switch(Event.type)
			{
			case sf::Event::KeyPressed:
				if(menu.getMenuUp() == false)
					player.HandleEvents(Event.key.code, true);
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					soundplayer.playMenuUp();

					if(menu.getMenuUp() == true)
						menu.setMenuUp(false);
					else if(menu.getMenuUp() == false)
					{
						menu.setState(0);
						menu.setMenuUp(true);
						menu.MainTextUpdate(window);
					}
				}
				break;

			case sf::Event::KeyReleased:
				if(!menu.getMenuUp())
					player.HandleEvents(Event.key.code, false);
				break;

			case sf::Event::MouseButtonReleased:
				if(menu.getMenuUp())
					menu.Click(window, soundplayer, ai);
				break;

			case sf::Event::LostFocus:
				menu.setMenuUp(true);
				soundplayer.setMusicVolume(0);
				soundplayer.setSoundEffectsVolume(0);
				break;

			case sf::Event::GainedFocus:
				soundplayer.setMusicVolume(menu.getMOptionsMusicVolume());
				soundplayer.setSoundEffectsVolume(menu.getMOptionsSoundEffectsVolume());
				break;

			//resizing
			case sf::Event::Resized:
				menu.setMenuUp(true);
				break;

			case sf::Event::Closed:
				menu.UpdateTextFile();
				window.close();
				break;
			}
		}

		if(menu.getMenuUp() == false)
		{
			
			ball.Update(deltaTime, ballClock, window);
			player.Update();
			ai.Update();

			ball.Move(deltaTime);
			player.Move(deltaTime);
			ai.Move(deltaTime);

			ball.Collision(window, soundplayer, player, ai.getTop(), ai.getRight(), ai.getBottom(), ai.getLeft());

			ai.HandleMovement(window, ball);

			menu.BallOutOfBounds(ball, currScoreText, window, ballClock, ai);
		}
		else
		{
			menu.Update(window);
		}

		window.clear();
		player.Draw(window);
		ai.Draw(window);
		window.draw(currScoreText);
		ball.Draw(window);

		if(menu.getMenuUp() == true)
			menu.Draw(window);

		window.display();
	}

	return 0;
}

