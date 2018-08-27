//----------------------------------------------------------
// Declaration: Copyright (c), by i_dovelemon, 2017. All right reserved.
// Author: i_dovelemon[1322600812@qq.com]
// Date: 2017/05/28
// Brief: Role Component define which role type of this entity
//----------------------------------------------------------
#ifndef ENTITY_ROLECOM_H_
#define ENTITY_ROLECOM_H_

#include "component.h"

namespace entity {

//-------------------------------------------------------------
// Type Declaration

//------------------------------------------------------------

namespace RoleType {
    enum Type {
        NONE = -1,
        PLAYER,
        PLAYERBULLET,
        ENEMY,
        MAX
    };
};

enum RoleSubType {
    RST_NONE = -1,
    // Player Begin
    RST_MAINTOWER,
    RST_SUBTOWER,
    RST_WALL,
    RST_BULLET_LASER,
    RST_BULLET_MACHINEGUN,
    RST_BULLET_BOMB,
    RST_CRYSTAL,
    RST_PLAYER_END = 100,

    // Enemy Begin
    RST_E01,
    RST_E02,
    RST_ENEMY_END = 200,

    // Operator Begin
    RST_PLACER,
    RST_OPERATOR_END = 300,

    // Item Begin
    RST_ITEM_BOMB,
    RST_ITEM_END = 400,
};

};  // namespace entity

#endif  // ENTITY_ROLECOM_H_