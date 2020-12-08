#include "Player.h"
#include "Actor.h"
#include "CommandQueue.h"

#include <map>
#include <string>
#include <algorithm>


using namespace std::placeholders;


struct FroggerMover
{
	FroggerMover(float vx, float vy)
		: velocity(vx, vy)
	{
	}

	void operator() (Actor& frogger, sf::Time) const
	{
		frogger.accelerate(velocity * frogger.getMaxSpeed());

	}

	sf::Vector2f velocity;
};

Player::Player()
	: mCurrentMissionStatus(Running)
{
	// Set initial key bindings
	mKeyBinding[sf::Keyboard::Left] = MoveLeft;
	mKeyBinding[sf::Keyboard::Right] = MoveRight;
	mKeyBinding[sf::Keyboard::Up] = MoveUp;
	mKeyBinding[sf::Keyboard::Down] = MoveDown;
	//mKeyBinding[sf::Keyboard::Space] = Fire;
	//mKeyBinding[sf::Keyboard::M] = LaunchMissile;

	// Set initial action bindings
	initializeActions();

	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	sf::Time dt = clock.restart();
	timeSinceLastUpdate += dt;

	// Assign all categories to player's aircraft
	for (auto& pair : mActionBinding)
		pair.second.category = Category::Frogger;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands)
{

	if (event.type == sf::Event::KeyPressed)
	{
		// Check if pressed key appears in key binding, trigger command if so
		auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end())
			commands.push(mActionBinding[found->second]);
	}
}

void Player::handleRealtimeInput(CommandQueue& commands)
{
	// Traverse all assigned keys and check if they are pressed
	timeSinceLastUpdate += dt;
	for (auto pair : mKeyBinding)
	{
		// If key is pressed, lookup action and trigger corresponding command
		if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second) && timeSinceLastUpdate > JUMP_TIME)
		{
			commands.push(mActionBinding[pair.second]);

		}
	}
	sf::Time dt = clock.restart();
}


void Player::assignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end(); )
	{
		if (itr->second == action)
			mKeyBinding.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignedKey(Action action) const
{
	for (auto pair : mKeyBinding)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

void Player::setMissionStatus(MissionStatus status)
{
	mCurrentMissionStatus = status;
}

Player::MissionStatus Player::getMissionStatus() const
{
	return mCurrentMissionStatus;
}

void Player::initializeActions()
{
	const float playerSpeed = 2400.f;
	mActionBinding[MoveLeft].action = derivedAction<Actor>([playerSpeed](Actor& a, sf::Time dt)
		{
			a.setDirection(Actor::Direction::Left);
			a.setState(Actor::State::JumpLeft);
			a.accelerate(sf::Vector2f(-playerSpeed, 0.f));
		}
	);
	mActionBinding[MoveRight].action = derivedAction<Actor>([playerSpeed](Actor& a, sf::Time dt)
		{
			a.setDirection(Actor::Direction::Right);
			a.setState(Actor::State::JumpRight);
			a.accelerate(sf::Vector2f(playerSpeed, 0.f));
		}
	);
	mActionBinding[MoveUp].action = derivedAction<Actor>([playerSpeed](Actor& a, sf::Time dt)
		{
			a.setDirection(Actor::Direction::Up);
			a.setState(Actor::State::JumpUp);
			a.accelerate(sf::Vector2f(0.f, -playerSpeed));
		}
	);
	mActionBinding[MoveDown].action = derivedAction<Actor>([playerSpeed](Actor& a, sf::Time dt)
		{
			a.setDirection(Actor::Direction::Down);
			a.setState(Actor::State::JumpDown);
			a.accelerate(sf::Vector2f(0.f, playerSpeed));
		}
	);
}

bool Player::isRealtimeAction(Action action)
{
	switch (action)
	{
	case MoveLeft:
	case MoveRight:
	case MoveDown:
	case MoveUp:
		//case Fire:
		return true;

	default:
		return false;
	}
}