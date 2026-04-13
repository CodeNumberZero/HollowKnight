#include "Character.h"
#include "CollisionMgr.h"

Character::Character()
{
    hit_box = CollisionMgr::GetInstance()->CreateCollisionBox();
    hurt_box = CollisionMgr::GetInstance()->CreateCollisionBox();

    timer_invulnerable_status.set_wait_time(1.0f);
    timer_invulnerable_status.set_one_shot(true);
    timer_invulnerable_status.set_on_timeout([&]() {
        is_invulnerable = false;
    });

    timer_invulnerable_blink.set_wait_time(0.075f);
    timer_invulnerable_blink.set_one_shot(false);
    timer_invulnerable_blink.set_on_timeout([&]() {
        is_blink_invisible = !is_blink_invisible;
    });
}

Character::~Character()
{
    CollisionMgr::GetInstance()->DestroyCollisionBox(hit_box);
    CollisionMgr::GetInstance()->DestroyCollisionBox(hurt_box);
}

void Character::decrease_hp()
{
    if (is_invulnerable)                                     // 受击后先检查是否处于无敌状态
        return;
    hp -= 1;
    if (hp > 0)                                              // 扣除生命后hp仍>0则进入无敌状态
        make_invulnerable();
    on_hurt();
}

int Character::get_hp() const
{
    return hp;
}

void Character::set_position(const MyVector& position) {
    this->position = position;
}

const MyVector& Character::get_position() const
{
    return position;
}

void Character::set_velocity(const MyVector& velocity)
{
    this->velocity = velocity;
}

const MyVector& Character::get_velocity() const
{
    return velocity;
}

MyVector Character::get_logic_center() const
{
    return MyVector(position.x, position.y - logic_height / 2);
}

void Character::set_gravity_enabled(bool flag)
{
    enable_gravity = flag;
}

std::shared_ptr<CollisionBox> Character::get_hit_box()
{
    return hit_box;
}

std::shared_ptr<CollisionBox> Character::get_hurt_box()
{
    return hurt_box;
}

// 判断角色是否处于地面
bool Character::is_on_floor() const
{
    return position.y >= FLOOR_Y;
}

float Character::get_floor_y() const
{
    return FLOOR_Y;
}

void Character::make_invulnerable()
{
    is_invulnerable = true;
    timer_invulnerable_status.restart();
}

// 只有玩家角色才接受键鼠操控,所以留给子类重写
void Character::on_input(const ExMessage& msg)
{
}

void Character::on_update(float delta)
{
    state_machine.on_update(delta);                          // 先调用状态机的更新方法

    if (hp <= 0)         
        velocity.x = 0;                                      // 死亡时停止水平移动
    if (enable_gravity)
        velocity.y += GRAVITY * delta;                       // 启用重力时,向下速度不断增加

    position += velocity * delta;                            // 根据速度更新位置             
    if (position.y >= FLOOR_Y) {                             // 落地检测,落地后下落速度清零,y坐标固定为地板位置
        position.y = FLOOR_Y;
        velocity.y = 0;
    }
    if (position.x <= 0)                                     // 限制角色不会走出屏幕两边
        position.x = 0;
    if (position.x >= getwidth())
        position.x = (float)getwidth();
    
    hurt_box->set_position(get_logic_center());              // 受击碰撞箱跟随角色中心移动

    timer_invulnerable_status.on_update(delta);              // 更新无敌持续时间
    if (is_invulnerable)
        timer_invulnerable_blink.on_update(delta);

    if (!current_animation)                                  // 没有动画则直接退出
        return;

    Animation& animation = (is_facing_left ? current_animation->left : current_animation->right);
    animation.on_update(delta);                              // 动画更新
    animation.set_position(position);                        // 动画画面跟着角色脚底位置走
}

void Character::on_render() {
    if (!current_animation || (is_invulnerable && is_blink_invisible))                 // 如果当前动画没有被设置或者正处于无敌状态中不可见的帧时直接返回
        return;
    (is_facing_left ? current_animation->left : current_animation->right).on_render(); // 根据当前角色朝向选择对应动画调用渲染方法
}

// 该方法通常用来实现受到攻击后播放不同效果的受伤音效,留给子类重写
void Character::on_hurt()
{
}

void Character::switch_state(const std::string& id)
{
    state_machine.switch_to(id);
}

void Character::set_animation(const std::string& id)
{
    current_animation = animation_pool[id];
    current_animation->left.reset();                         // 需要注意当设置新动画时需要重置具体的左右动画对象状态
    current_animation->right.reset();
}
