#include "Koopa.h"
#include "Core/AssetManager.h"
#include "Core/GameEngine.h"
#include "Core/EventSystem.h"
#include "Player.h"
#include <cmath>

Koopa::Koopa(Vector2 pos)
    : Enemy(pos, Vector2{32.0f, 48.0f}, Vector2{24.0f, 40.0f}, Vector2{4.0f, 8.0f}, "enemy", GREEN),
      currentState(KoopaState::Moving)
{

    // Walking animation setup
    animator.addAnimation(AnimState::Walk, {Rectangle{52.0f, 37.0f, 16.0f, 24.0f}, Rectangle{69.0f, 37.0f, 16.0f, 24.0f}}, 0.2f);

    // Shell animation setup (we use Crouch state to represent it)
    animator.addAnimation(AnimState::Crouch, {Rectangle{120.0f, 45.0f, 16.0f, 12.0f}}, 1.0f);

    animator.setState(AnimState::Walk);
}

bool Koopa::avoidsCliffs() const
{
    return currentState == KoopaState::Moving;
}

void Koopa::update(float dt)
{
    // Regardless of state, if it hits the left border, turn right
    if (position.x <= 0.0f)
    {
        position.x = 0.0f; // Prevent getting stuck outside the map
        facingRight = true;
        // If it's a moving shell, also ensure its velocity instantly flips so it bounces visually
        if (currentState == KoopaState::MovingShell)
        {
            velocity.x = 400.0f;
        }
    }

    if (currentState == KoopaState::Moving)
    {
        Enemy::update(dt);
    }
    else if (currentState == KoopaState::PickedUp)
    {
        // Player holds run to carry. If released, Player drops it or throws it.
        bool isHolding = GameEngine::getInstance().getInputManager().isActionPressed(Action::Run, carrierPlayerIndex) ||
                         GameEngine::getInstance().getInputManager().isActionPressed(Action::Shoot, carrierPlayerIndex);

        if (!isHolding)
        {
            if (std::abs(velocity.x) > 10.0f)
            {
                // Thrown/Kicked
                currentState = KoopaState::MovingShell;
                facingRight = (velocity.x > 0.0f);
            }
            else
            {
                // Dropped
                currentState = KoopaState::Shell;
                velocity.x = 0.0f;
            }
        }
    }
    else if (currentState == KoopaState::MovingShell)
    {
        // Maintain sliding velocity
        velocity.x = facingRight ? 400.0f : -400.0f;
    }

    animator.update(dt);
}

void Koopa::draw()
{
    Texture2D tex = AssetManager::getInstance().getTexture(textureID);
    if (tex.id != 0)
    {
        Rectangle source = animator.getCurrentFrame();

        // Flip the texture if facing left
        if (!facingRight)
        {
            source.width = -std::abs(source.width);
        }

        float scale = 2.0f;
        float destWidth = std::abs(source.width) * scale;
        float destHeight = std::abs(source.height) * scale;

        // Align visual sprite to physical position
        float destX = position.x;
        float destY = position.y + (spriteSize.y - destHeight);

        Rectangle dest = {destX, destY, destWidth, destHeight};
        Vector2 origin = {0.0f, 0.0f};

        DrawTexturePro(tex, source, dest, origin, 0.0f, WHITE);
    }
    else
    {
        // Fallback outline/box
        Enemy::draw();
    }
}

void Koopa::onCollision(Entity &other, CollisionSide side)
{
    if (Player *player = dynamic_cast<Player *>(&other))
    {
        if (currentState == KoopaState::Shell)
        {
            int pIdx = player->getPlayerIndex();
            bool isHoldingRun = GameEngine::getInstance().getInputManager().isActionPressed(Action::Run, pIdx);

            if (isHoldingRun)
            {
                carrierPlayerIndex = pIdx;
                player->setCarriedEntity(this);
                currentState = KoopaState::PickedUp;
            }
            else
            {
                currentState = KoopaState::MovingShell;

                // FIX LỖI 2: Dựa vào vận tốc của Mario thay vì tọa độ để tránh lỗi chạy xuyên
                if (std::abs(player->getVelocity().x) > 0.1f)
                {
                    facingRight = (player->getVelocity().x > 0.0f);
                }
                else
                {
                    facingRight = (player->getPosition().x < position.x); // Dự phòng khi Mario đứng im
                }

                velocity.x = facingRight ? 400.0f : -400.0f;
                EventManager::getInstance().broadcast(EventType::KoopaKicked);
            }
            return;
        }

        if (currentState == KoopaState::PickedUp)
        {
            return;
        }

        if (currentState == KoopaState::MovingShell)
        {
            if (side == CollisionSide::Top)
            {
                // FIX LỖI 1: Yêu cầu Mario phải thực sự "rơi từ trên xuống" (đáy của Mario cao hơn tâm mai rùa) mới tính là giẫm
                float playerBottom = player->getBoundingBox().y + player->getBoundingBox().height;
                float koopaMiddle = getBoundingBox().y + getBoundingBox().height / 2.0f;

                if (playerBottom < koopaMiddle && player->getVelocity().y > 0.0f)
                {
                    currentState = KoopaState::Shell;
                    velocity.x = 0.0f;
                    EventManager::getInstance().broadcast(EventType::EnemyStomped);
                    return;
                }
            }

            // Side collision: Nếu mai rùa đang di chuyển ra xa thì không gây sát thương cho Mario
            bool movingAway = (velocity.x > 0.0f && player->getPosition().x <= position.x + 5.0f) ||
                              (velocity.x < 0.0f && player->getPosition().x >= position.x - 5.0f);
            if (movingAway)
            {
                return;
            }
        }
    }

    if (currentState == KoopaState::MovingShell && other.isSolid())
    {
        // Đập tường dội lại
        if (side == CollisionSide::Left)
            facingRight = true;
        if (side == CollisionSide::Right)
            facingRight = false;
    }

    // Xử lý va chạm bình thường cho các trường hợp khác
    Enemy::onCollision(other, side);
}

void Koopa::takeDamage()
{
    if (currentState == KoopaState::Moving)
    {
        currentState = KoopaState::Shell;
        animator.setState(AnimState::Crouch);

        // Resize Koopa to represent a shell
        spriteSize = Vector2{32.0f, 32.0f}; // Visual size drawn on screen (16x16 * 2)
        hitboxSize = Vector2{24.0f, 24.0f}; // Physical box
        hitboxOffset = Vector2{4.0f, 8.0f};

        // Shift position down so it doesn't float in the air
        position.y += 16.0f;

        velocity.x = 0.0f; // Stop moving

        // Emit stomp sound when jumping on Koopa
        EventManager::getInstance().broadcast(EventType::EnemyStomped);
    }
}
