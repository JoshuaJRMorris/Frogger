#pragma once
#include "Command.h"
#include <SFML\System\Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <map>


class CommandQueue;

class Player
{
public:
	enum Action
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		
	};

	enum MissionStatus
	{
		Running,
		Success,
		Failure
	};


public:
	Player();
	void					initializeKeyBindings();
	void					handleEvent(const sf::Event& event, CommandQueue& commands);
	void					handleRealtimeInput(CommandQueue& commands);

	void					assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key		getAssignedKey(Action action) const;

	void 					setMissionStatus(MissionStatus status);
	MissionStatus 			getMissionStatus() const;

private:
	void					initializeActions();
	static bool				isRealtimeAction(Action action);

	sf::Clock				clock;
	sf::Time				timeSinceLastUpdate = sf::Time::Zero;
	sf::Time				dt;

	const sf::Time			JUMP_TIME = sf::seconds(1.f);


private:
	std::map<sf::Keyboard::Key, Action>		mKeyBinding;
	std::map<Action, Command>				mActionBinding;
	MissionStatus 							mCurrentMissionStatus;
};