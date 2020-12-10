#pragma once
#include "Entity.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Animation2.h"
#include "TextNode.h"
#include <SFML/Graphics/Sprite.hpp>
class Actor : public Entity
{
public:
    enum class Type {
       Hero, Car1, Car2, Car3, Tractor, Truck, Tree1, Tree2, FrogWin
    };
    enum class State {
        Dead, Idle, JumpUp, JumpDown, JumpLeft, JumpRight, KingMe
    };
    enum class Direction
    {
        Left, Right, Up, Down
    };
public:
                                        Actor(Type type, const TextureHolder& textures, const FontHolder& fonts);
                                        ~Actor() = default;

    unsigned int                        getCategory() const override;
    sf::FloatRect                       getBoundingRect() const override;
    float                               getMaxSpeed() const;

    bool                                isMarkedForRemoval() const override;

    //void                                attack();
    sf::IntRect                         flipCar(sf::IntRect rec);
    void                                setDirection(Direction tmp_Direction);
    Actor::Direction                    getDirection();
    void                                setState(State state);
    Actor::State                        getState() const;
    bool                                isOnWater() const;
    bool                                isFinishBlock() const;
//    void                                hop(Direction d);

private:
    void                                updateStates();
  //  void                              updateForceField(sf::Time dt);
    void                                checkOnWater();
    void                                checkForFinished();
    void                                updateCurrent(sf::Time dt, CommandQueue& commands) override;
    void                                drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
    void                                updateMovementPattern(sf::Time dt);
    void                                updateTexts();

    //void                              updateMovementPattern(sf::Time dt);
private:
    Type                                type_;
    State                               state_;
    mutable sf::Sprite                  sprite_;
    std::map<State, Animation2>         animations_;
    Direction                           direction_;

    TextNode*                           healthDisplay_;
    float                               travelDistance_;
    std::size_t                         directionIndex_;
    bool                                attack_;
    sf::Time                            forceFieldTimer_;
    bool                                onWater;
    bool                                finishBlock;
    float                               random_Speed;
};

