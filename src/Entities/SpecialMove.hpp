#ifndef SPECIAL_MOVE_H
#define SPECIAL_MOVE_H

class Player;

class SpecialMove {
public:
    virtual ~SpecialMove() = default;
    virtual void execute(Player& player) = 0;
};

class FireballMove : public SpecialMove {
public:
    void execute(Player& player) override;
};

class NoneMove : public SpecialMove {
public:
    void execute(Player& player) override { (void)player; }
};

#endif // SPECIAL_MOVE_H
