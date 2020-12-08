#include "DataTables.h"
#include "Particle.h"
#include "JsonFrameParser.h"



using namespace std::placeholders;
std::map<Actor::Type, ActorData> initializeActorData()
{
	//frog
	std::map<Actor::Type, ActorData> data; 
	data[Actor::Type::Hero].texture = Textures::ID::Frog;
	data[Actor::Type::Hero].speed = 50;
	JsonFrameParser frames = JsonFrameParser("Media/Textures/frog.json");
					  
	data[Actor::Type::Hero].animations[Actor::State::JumpUp].addFrameSet(frames.getFramesFor("jumpUp"));
	data[Actor::Type::Hero].animations[Actor::State::JumpUp].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Hero].animations[Actor::State::JumpUp].setRepeating(false);
					  
	data[Actor::Type::Hero].animations[Actor::State::JumpDown].addFrameSet(frames.getFramesFor("jumpDown"));
	data[Actor::Type::Hero].animations[Actor::State::JumpDown].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Hero].animations[Actor::State::JumpDown].setRepeating(false);
					  
	data[Actor::Type::Hero].animations[Actor::State::JumpRight].addFrameSet(frames.getFramesFor("jumpRight"));
	data[Actor::Type::Hero].animations[Actor::State::JumpRight].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Hero].animations[Actor::State::JumpRight].setRepeating(false);
					  
	data[Actor::Type::Hero].animations[Actor::State::JumpLeft].addFrameSet(frames.getFramesFor("jumpLeft"));
	data[Actor::Type::Hero].animations[Actor::State::JumpLeft].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Hero].animations[Actor::State::JumpLeft].setRepeating(false);
					  
	data[Actor::Type::Hero].animations[Actor::State::Dead].addFrameSet(frames.getFramesFor("death"));
	data[Actor::Type::Hero].animations[Actor::State::Dead].setDuration(sf::seconds(1.f));
	data[Actor::Type::Hero].animations[Actor::State::Dead].setRepeating(false);

	//car 1
	data[Actor::Type::Car1].texture = Textures::ID::Car1;
	data[Actor::Type::Car1].speed = 75;
	
	data[Actor::Type::Car1].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("car1"));
	data[Actor::Type::Car1].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Car1].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Car1].directions.emplace_back(Direction(270.f, 50.f));

	//car 2
	data[Actor::Type::Car2].texture = Textures::ID::Car2;
	data[Actor::Type::Car2].speed = 60;

	data[Actor::Type::Car2].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("car2"));
	data[Actor::Type::Car2].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Car2].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Car2].directions.emplace_back(Direction(270.f, 50.f));

	//car 3
	data[Actor::Type::Car3].texture = Textures::ID::Car3;
	data[Actor::Type::Car3].speed = 65;
						 
	data[Actor::Type::Car3].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("car3"));
	data[Actor::Type::Car3].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Car3].animations[Actor::State::Idle].setRepeating(false);
						 
	data[Actor::Type::Car3].directions.emplace_back(Direction(270.f, 50.f));

	//tractor
	data[Actor::Type::Tractor].texture = Textures::ID::Tractor;
	data[Actor::Type::Tractor].speed = 45;

	data[Actor::Type::Tractor].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("tractor"));
	data[Actor::Type::Tractor].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Tractor].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Tractor].directions.emplace_back(Direction(270.f, 50.f));

	//truck
	data[Actor::Type::Truck].texture = Textures::ID::Truck;
	data[Actor::Type::Truck].speed = 40;

	data[Actor::Type::Truck].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("truck"));
	data[Actor::Type::Truck].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Truck].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Truck].directions.emplace_back(Direction(270.f, 50.f));

	//short log
	data[Actor::Type::Tree1].texture = Textures::ID::Tree1;
	data[Actor::Type::Tree1].speed = 0;

	data[Actor::Type::Tree1].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("tree1"));
	data[Actor::Type::Tree1].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Tree1].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Tree1].directions.emplace_back(Direction(270.f, 50.f));

	//long log
	data[Actor::Type::Tree2].texture = Textures::ID::Tree1;
	data[Actor::Type::Tree2].speed = 0;

	data[Actor::Type::Tree2].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("tree2"));
	data[Actor::Type::Tree2].animations[Actor::State::Idle].setDuration(sf::seconds(0.9f));
	data[Actor::Type::Tree2].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::Tree2].directions.emplace_back(Direction(270.f, 50.f));

	//frog win sprite
	data[Actor::Type::FrogWin].texture = Textures::ID::FrogWin;
	data[Actor::Type::FrogWin].speed = 0;

	data[Actor::Type::FrogWin].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("small frog"));
	data[Actor::Type::FrogWin].animations[Actor::State::Idle].setDuration(sf::seconds(1.0f));
	data[Actor::Type::FrogWin].animations[Actor::State::Idle].setRepeating(false);

	data[Actor::Type::FrogWin].directions.emplace_back(Direction(270.f, 50.f));

	return data;
}

std::vector<ParticleData> initializeParticleData()
{
	std::vector<ParticleData> data(Particle::ParticleCount);

	data[Particle::Propellant].color = sf::Color(255, 255, 50);
	data[Particle::Propellant].lifetime = sf::seconds(0.6f);

	data[Particle::Smoke].color = sf::Color(50, 50, 50);
	data[Particle::Smoke].lifetime = sf::seconds(4.f);

	return data;
}
