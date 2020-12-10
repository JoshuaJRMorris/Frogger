#pragma once
#include "Actor.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "JsonFrameParser.h"

#include "DataTables.h"
#include "TextNode.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include "Utility.h"

#include <random>

namespace
{
	std::mt19937 rEng{ std::random_device{}() };
	std::uniform_int_distribution<unsigned int> dist{ 40, 75 };
	const std::map<Actor::Type, ActorData> TABLE = initializeActorData();
}
Actor::Actor(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(100)
	, type_(type)
	, state_(State::Idle)
	, sprite_(textures.get(TABLE.at(type).texture))
	, direction_(Direction::Up)
	, travelDistance_(0.f)
	, directionIndex_(0)
	, attack_(false)
	, random_Speed(dist(rEng))
{
	for (auto a : TABLE.at(type).animations)
	{
		animations_[a.first] = a.second;
	}
	
	switch (type)
	{
	case Actor::Type::Hero:
		state_ = State::Idle;
		setScale(0.99f, 0.99f);
		break;
	case Actor::Type::Car1:
		
		break;
	case Actor::Type::Car2:
		
		break;
	case Actor::Type::Car3:
		
		break;
	case Actor::Type::Tractor:
		
		break;
	case Actor::Type::Truck:
		
		break;
	default:
		state_ = State::Idle;
		break;
	}

	sprite_.setTextureRect(sf::IntRect());
	centerOrigin(sprite_);

}
unsigned int Actor::getCategory() const
{
	switch (type_)
	{
	case Type::Hero:
		return Category::Frogger;
		break;
	case Type::Car1:
	case Type::Car2:
	case Type::Car3:
	case Type::Tractor:
	case Type::Truck:
		return Category::Car;
		break;
	case Type::Tree1:
	case Type::Tree2:
		return Category::Tree;
	case Type::FrogWin:
		return Category::SceneSprite;
		
	}
}
void Actor::updateMovementPattern(sf::Time dt)
{
	// movement pattern
	auto directions = TABLE.at(type_).directions;
	if (!directions.empty())
	{
		if (travelDistance_ > (directions[directionIndex_].distance))
		{
			directionIndex_ = (++directionIndex_) % directions.size();
			travelDistance_ = 0;
		}
		if (direction_ == Actor::Direction::Left && type_ != Actor::Type::Hero) 
		{
			directions[directionIndex_].angle *= -1;
		}
		float radians = toRadian(directions[directionIndex_].angle + 90.f);
		float vx;
		float vy;
		if (getCategory() == Category::Tree) 
		{
			vx = random_Speed * std::cos(radians);
			vy = random_Speed * std::sin(radians);
		}
		else
		{
			vx = getMaxSpeed() * std::cos(radians);
			vy = getMaxSpeed() * std::sin(radians);
		}
		
		setVelocity(vx, vy);
		travelDistance_ += getMaxSpeed() * dt.asSeconds();
	}
}

sf::FloatRect Actor::getBoundingRect() const
{
	auto box = getWorldTransform().transformRect(sprite_.getGlobalBounds());
	box.width -= 39; // tighten up bounding box for more realistic collisions
	box.left += 15;
	return box;
}

float Actor::getMaxSpeed() const
{
	return TABLE.at(type_).speed;
}

bool Actor::isMarkedForRemoval() const
{
	return false; // (state_ == State::Dead && animations_.at(state_).isFinished());
}

sf::IntRect Actor::flipCar(sf::IntRect rec)
{
	switch (type_)
	{
	case Type::Car1:
		if (direction_ != Actor::Direction::Left) 
		{
			return flip(rec);
		}
		break;
	case Type::Car2:
		if (direction_ == Actor::Direction::Left)
		{
			return flip(rec);
		}
		break;
	case Type::Car3:
		if (direction_ != Actor::Direction::Left)
		{
			return flip(rec);
		}
		break;
	case Type::Tractor:
		if (direction_ == Actor::Direction::Left)
		{
			return flip(rec);
		}
		break;
	case Type::Truck:
		if (direction_ != Actor::Direction::Left)
		{
			return flip(rec);
		}
		break;
	default:
		return rec;
		break;
	}
}

void Actor::setDirection(Direction tmp_Direction)
{
	direction_ = tmp_Direction;
}

Actor::Direction Actor::getDirection()
{
	return direction_;
}

void Actor::setState(State state)
{
	state_ = state;
	animations_[state_].restart();
}

Actor::State Actor::getState() const
{
	return state_;
}

bool Actor::isOnWater() const
{
	return onWater;
}

bool Actor::isFinishBlock() const
{
	return finishBlock;
}

void Actor::updateStates()
{
	if (isDestroyed())
		state_ = Actor::State::Dead;
	if (state_ == Actor::State::Dead && animations_[state_].isFinished() || state_ == Actor::State::KingMe)
	{
		state_ = Actor::State::Idle;
		this->setPosition(240.f, 600.f);

	}
	if (type_ == Actor::Type::Hero) {
		if (state_ == Actor::State::Idle && (getVelocity()).y > -0.1f)
		{
			state_ = Actor::State::JumpUp;
		}
		else if (state_ == Actor::State::Idle && (getVelocity()).y < 0.1f)
		{
			state_ = Actor::State::JumpDown;
		}
		else if (state_ == Actor::State::Idle && (getVelocity()).x < -0.1f)
		{
			state_ = Actor::State::JumpLeft;
		}
		else if (state_ == Actor::State::Idle && (getVelocity()).x > 0.1f)
		{
			state_ = Actor::State::JumpRight;
		}
		/*else
			state_ == Actor::State::Idle;*/
	}
}

void Actor::checkOnWater()
{
	if (this->getPosition().y > 320)
	{
		onWater = false;
	}
	else if (this->getPosition().y < 150) 
	{
		onWater = false;
		checkForFinished();
	}
	else 
	{
		onWater = true;
	}
}

void Actor::checkForFinished()
{
	if (this->getPosition().x > 17 && this->getPosition().x < 54   ||
		this->getPosition().x > 119 && this->getPosition().x < 157 ||
		this->getPosition().x > 222 && this->getPosition().x < 258 ||
		this->getPosition().x > 323 && this->getPosition().x < 361 ||
		this->getPosition().x > 426 && this->getPosition().x < 463 ) {
		finishBlock = true;
		
	}
	else {
		finishBlock = false;
		//he's not on water, but this will kill him and he needs to die in this situation
		onWater = true;
	}
}

void Actor::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	updateStates();
	
	auto rec = animations_.at(state_).update(dt);
	if (type_ == Actor::Type::Hero)
	{
		switch (state_)
		{
		case Actor::State::JumpUp:
			direction_ = Direction::Up;
			break;
		case Actor::State::JumpDown:
			direction_ = Direction::Down;
			break;
		case Actor::State::JumpLeft:
			direction_ = Direction::Left;
			break;
		case Actor::State::JumpRight:
			direction_ = Direction::Right;
			break;
		default:
			break;
		}
	}
	//if (getCategory() == Category::Car) 
		//rec = flipCar(rec);

	checkOnWater();
	
	sprite_.setTextureRect(rec);
	centerOrigin(sprite_);
	if (state_ != State::Dead)
		Entity::updateCurrent(dt, commands);

	updateMovementPattern(dt);
}
void Actor::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}

//void Actor::hop(Direction d)
//{
//	const float HOP_SIZE = 10.f;
//
//	switch (d)
//	{
//	case Actor::Direction::Left:
//		setState(Actor::State::JumpLeft);
//		move(-1 * HOP_SIZE, 0.f);
//		break;
//	case Actor::Direction::Right:
//		setState(Actor::State::JumpRight);
//		move(HOP_SIZE, 0.f);
//		break;
//	case Actor::Direction::Up:
//		setState(Actor::State::JumpUp);
//		move(0.f, -1 * HOP_SIZE);
//		break;
//	case Actor::Direction::Down:
//		setState(Actor::State::JumpDown);
//		move(0.f, HOP_SIZE);
//		break;
//	default:
//		break;
//	}
//}