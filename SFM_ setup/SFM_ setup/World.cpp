#include "World.h"
#include "TextNode.h"
#include "ParticleNode.h"
#include "SoundNode.h"
#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
std::mt19937 rEng{ std::random_device{}() };
std::uniform_int_distribution<unsigned int> oneOrTwo{ 1,2 };

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSounds(sounds)
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.f, 0.f, mWorldView.getSize().x, mWorldView.getSize().y)
	, mSpawnPosition(240.f, 580.f)
	, playerActor(nullptr)
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(240.f, 300.f);
	
}





CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return !playerActor->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const
{
	return !mWorldBounds.contains(playerActor->getPosition());
	
}

void World::update(sf::Time dt)
{
	playerActor->setVelocity(0.f, 0.f);

	destroyEntitiesOutsideView();

	adaptNPCPosition();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty()) {
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	}

	adaptPlayerVelocity();
	handleCollisions();

	spawnNPCs();

	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();

	updateSounds();

	if (playerActor->getPosition().y < 120) {
		if(!playerActor->isOnWater()){
			addWinSprite();
			playerActor->setState(Actor::State::KingMe);
			if (winFrogs == 5) {
				//win the game
			}
		}

		

	}
	
}

void World::draw()
{
	mTarget.setView(mWorldView);
	mTarget.draw(mSceneGraph);
}

void World::loadTextures()
{
	mTextures.load(Textures::ID::Background, "Media/Textures/background.png");
	mTextures.load(Textures::ID::Frog, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Car1, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Car2, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Car3, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Tractor, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Truck, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::Tree1, "Media/Textures/frog.png");
	mTextures.load(Textures::ID::FrogWin, "Media/Textures/frog.png");
}

void World::adaptPlayerPosition()
{	
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 20.f;

	sf::Vector2f position = playerActor->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	playerActor->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	
	sf::Vector2f velocity = playerActor->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		playerActor->setVelocity(velocity / std::sqrt(2.f));

	// Add scrolling velocity
	//playerActor->accelerate(0.f, mScrollSpeed);
	
}




void World::handleCollisions()
{
	// get all colliding pairs
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	if (!collisionPairs.empty())
	{
		for (auto pair : collisionPairs)
		{
			if (matchesCategories(pair, Category::Frogger, Category::Car)) {
				auto& frog = static_cast<Actor&>(*pair.first);
				auto& actor = static_cast<Actor&>(*pair.second);

				frog.setState(Actor::State::Dead);
				
			}
			if (matchesCategories(pair, Category::Frogger, Category::Tree)) {
				auto& frog = static_cast<Actor&>(*pair.first);
				auto& actor = static_cast<Actor&>(*pair.second);

				frog.setVelocity(actor.getVelocity() + frog.getVelocity());

			}
		}
	}
	else if (playerActor->isOnWater() && !(playerActor->getState() == Actor::State::Dead || (playerActor->getState() == Actor::State::JumpDown && playerActor->getPosition().y > 339))) 
	{
		playerActor->setState(Actor::State::Dead);
	}

	
}

bool World::matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}


void World::updateSounds()
{
	// Set listener's position to player position
	//mSounds.setListenerPosition(mPlayerAircraft->getWorldPosition());

	// Remove unused sounds
	mSounds.removeStoppedSounds();
}

void World::adaptNPCPosition()
{

	Command adaptPosition;
	adaptPosition.category = Category::Actor;
	adaptPosition.action = derivedAction<Actor>([this](Actor& enemy, sf::Time)
		{
			float left = mWorldView.getCenter().x - mWorldView.getSize().x / 2.f;
			float right = mWorldView.getCenter().x + mWorldView.getSize().x / 2.f;
			float top = mWorldView.getCenter().y - mWorldView.getSize().y / 2.f;
			float bottom = mWorldView.getCenter().y + mWorldView.getSize().y / 2.f;

			const float borderDistance = -20.f;

			sf::Vector2f enemyPosition = enemy.getPosition();

			if (enemyPosition.x < left + borderDistance) 
			{
				enemy.setPosition(479.f, enemyPosition.y);
			}
			else if (enemyPosition.x > right - borderDistance) 
			{
				enemy.setPosition(1.f, enemyPosition.y);
			}
			else if (enemyPosition.y > bottom - borderDistance) 
			{
				enemy.setPosition(enemyPosition.x, bottom - borderDistance);
			}
			else if (enemyPosition.y < top + borderDistance) 
			{
				enemy.setPosition(enemyPosition.x, top + borderDistance);
			}
		});

	mCommandQueue.push(adaptPosition);
}



void World::buildScene()
{
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == LowerAir) ? Category::Type::SceneObjectLayer : Category::Type::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}
	
	sf::Texture& backgroundTexture = mTextures.get(Textures::ID::Background);
	backgroundTexture.setRepeated(true);
	
	

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(backgroundTexture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));
	
	addNPCs();

	// Add player's frogger
	std::unique_ptr<Actor> player(new Actor(Actor::Type::Hero, mTextures, mFonts));
	playerActor = player.get();
	playerActor->setPosition(mSpawnPosition);
	mSceneLayers[UpperAir]->attachChild(std::move(player));
}

void World::addNPCs()
{
	addNPC(Actor::Type::Car1, getCarRandomSpawn(), 540.f);
	addNPC(Actor::Type::Tractor, getCarRandomSpawn(), 500.f);
	addNPC(Actor::Type::Car2, getCarRandomSpawn(), 460.f);
	addNPC(Actor::Type::Truck, getCarRandomSpawn(), 420.f);
	addNPC(Actor::Type::Car3, getCarRandomSpawn(), 380.f);

	/*addNPC(Actor::Type::FrogWin, 35.f, 98.f);
	addNPC(Actor::Type::FrogWin, 139.f, 98.f);
	addNPC(Actor::Type::FrogWin, 241.f, 98.f);
	addNPC(Actor::Type::FrogWin, 342.f, 98.f);
	addNPC(Actor::Type::FrogWin, 444.f, 98.f);*/

	

	
	float spawn = 300;
	for (int i{0}; i < 5; ++i)
	{
		int random = oneOrTwo(rEng);
		if (random == 1) 
		{
			addNPC(Actor::Type::Tree1, 0.f, spawn);
		}
		else
		{
			addNPC(Actor::Type::Tree2, 0.f, spawn);
		}
		spawn -= 40;
	}



	// Sort all enemies according to their y value, such that lower enemies are checked first for spawning
	std::sort(npcSpawnPoints.begin(), npcSpawnPoints.end(), [](SpawnPoint lhs, SpawnPoint rhs)
		{
			return lhs.y < rhs.y;
		});
		
}

void World::addNPC(Actor::Type type, float relX, float relY)
{
	SpawnPoint spawn(type, + relX, relY);
	npcSpawnPoints.push_back(spawn);
}


void World::spawnNPCs()
{
	
	// Spawn all enemies entering the view area (including distance) this frame
	while (!npcSpawnPoints.empty()
		&& npcSpawnPoints.back().y > getBattlefieldBounds().top)
	{
		SpawnPoint spawn = npcSpawnPoints.back();

		std::unique_ptr<Actor> enemy(new Actor(spawn.type, mTextures, mFonts));
		enemy->setPosition(spawn.x, spawn.y);
		int direction = oneOrTwo(rEng);
		if (direction == 1) 
		{
			enemy->setDirection(Actor::Direction::Right);
		}
		else
		{
			enemy->setDirection(Actor::Direction::Left);
		}
		//enemy->setRotation(180.f);

		mSceneLayers[LowerAir]->attachChild(std::move(enemy));

		// Enemy is spawned, remove from the list to spawn
		npcSpawnPoints.pop_back();
	}
	
}


void World::destroyEntitiesOutsideView()
{
	/*
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
				e.remove();
		});

	mCommandQueue.push(command);
	*/
}
void World::addWinSprite()
{
	if (playerActor->isFinishBlock()) {
		addNPC(Actor::Type::FrogWin, playerActor->getPosition().x, playerActor->getPosition().y);
	}
	winFrogs++;
}
float World::getCarRandomSpawn()
{
	int random = oneOrTwo(rEng);	//random number will either be 1 or 2
	if (random == 1) 
	{
		return 480.f;
	}
	return 0.f;
}
/*
void World::guideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::EnemyAircraft;
	enemyCollector.action = derivedAction<Aircraft>([this](Aircraft& enemy, sf::Time)
		{
			if (!enemy.isDestroyed())
				mActiveEnemies.push_back(&enemy);
		});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::AlliedProjectile;
	missileGuider.action = derivedAction<Projectile>([this](Projectile& missile, sf::Time)
		{
			// Ignore unguided bullets
			if (!missile.isGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Aircraft* closestEnemy = nullptr;

			// Find closest enemy
			for (Aircraft * enemy : mActiveEnemies)
			{
				float enemyDistance = distance(missile, *enemy);

				if (enemyDistance < minDistance)
				{
					closestEnemy = enemy;
					minDistance = enemyDistance;
				}
			}

			if (closestEnemy)
				missile.guideTowards(closestEnemy->getWorldPosition());
		});

	// Push commands, reset active enemies
	mCommandQueue.push(enemyCollector);
	mCommandQueue.push(missileGuider);
	mActiveEnemies.clear();
}
*/
sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

