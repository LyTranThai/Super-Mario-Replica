#ifndef QUESTION_BLOCK_H
#define QUESTION_BLOCK_H

#include "InteractiveBlock.h"

class QuestionBlock : public InteractiveBlock {
private:
    ItemType hiddenItem;

public:
    QuestionBlock(Vector2 pos, ItemType item);
    ~QuestionBlock() override = default;

    void hit(Player& player) override;
};

#endif // QUESTION_BLOCK_H
