/*
 * File: z_en_si.c
 * Overlay: En_Si
 * Description: Gold Skulltula token
 */

#include "z_en_si.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_9)

void EnSi_Init(Actor* thisx, PlayState* play);
void EnSi_Destroy(Actor* thisx, PlayState* play);
void EnSi_Update(Actor* thisx, PlayState* play);
void EnSi_Draw(Actor* thisx, PlayState* play);

s32 func_80AFB748(EnSi* this, PlayState* play);
void func_80AFB768(EnSi* this, PlayState* play);
void func_80AFB89C(EnSi* this, PlayState* play);
void func_80AFB950(EnSi* this, PlayState* play);
void Randomizer_UpdateSkullReward(EnSi* this, PlayState* play);
void Randomizer_GiveSkullReward(EnSi* this, PlayState* play);

s32 textId = 0xB4;
s32 giveItemId = ITEM_SKULL_TOKEN;
s32 getItemId;
GetItemEntry getItem;

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000090, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 18, 2, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 D_80AFBADC = { 0, 0, 0, 0, MASS_IMMOVABLE };

const ActorInit En_Si_InitVars = {
    ACTOR_EN_SI,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSi),
    (ActorFunc)EnSi_Init,
    (ActorFunc)EnSi_Destroy,
    (ActorFunc)EnSi_Update,
    (ActorFunc)EnSi_Draw,
    NULL,
};

void EnSi_Init(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, NULL, &D_80AFBADC);
    Actor_SetScale(&this->actor, 0.025f);
    this->unk_19C = 0;
    this->actionFunc = func_80AFB768;
    this->actor.shape.yOffset = 42.0f;
}

void EnSi_Destroy(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80AFB748(EnSi* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
    }
    return 0;
}

void func_80AFB768(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        this->actionFunc = func_80AFB89C;
    } else {
        Math_SmoothStepToF(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
        Actor_SetScale(&this->actor, this->actor.scale.x);
        this->actor.shape.rot.y += 0x400;

        if (!Player_InCsMode(play)) {
            func_80AFB748(this, play);

            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;

                if (gSaveContext.n64ddFlag) {
                    Randomizer_UpdateSkullReward(this, play);
                } else {
                    Item_Give(play, giveItemId);
                }
                if ((CVarGetInteger("gSkulltulaFreeze", 0) != 1 || giveItemId != ITEM_SKULL_TOKEN) && getItemId != RG_ICE_TRAP) {
                    player->actor.freezeTimer = 20;
                }

                Message_StartTextbox(play, textId, NULL);

                if (gSaveContext.n64ddFlag && getItemId != RG_ICE_TRAP) {
                    Randomizer_GiveSkullReward(this, play);
                    Audio_PlayFanfare_Rando(getItem);
                } else {
                    Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
                }

                player->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
                this->actionFunc = func_80AFB950;
            } else {
                Collider_UpdateCylinder(&this->actor, &this->collider);
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void func_80AFB89C(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Math_SmoothStepToF(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    this->actor.shape.rot.y += 0x400;

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_13)) {
        if (gSaveContext.n64ddFlag) {
            Randomizer_UpdateSkullReward(this, play);
        } else {
            Item_Give(play, giveItemId);
        }

        Message_StartTextbox(play, textId, NULL);

        if (gSaveContext.n64ddFlag && getItemId != RG_ICE_TRAP) {
            Randomizer_GiveSkullReward(this, play);
            Audio_PlayFanfare_Rando(getItem);
        } else {
            Audio_PlayFanfare(NA_BGM_SMALL_ITEM_GET);
        }

        player->getItemEntry = (GetItemEntry)GET_ITEM_NONE;
        this->actionFunc = func_80AFB950;
    }
}

void func_80AFB950(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_GetState(&play->msgCtx) != TEXT_STATE_CLOSING &&
        ((CVarGetInteger("gSkulltulaFreeze", 0) != 1 || giveItemId != ITEM_SKULL_TOKEN) && getItemId != RG_ICE_TRAP)) {
        player->actor.freezeTimer = 10;
    } else {
        SET_GS_FLAGS((this->actor.params & 0x1F00) >> 8, this->actor.params & 0xFF);
        Actor_Kill(&this->actor);
    }
}

void EnSi_Update(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 16.0f);
}

void EnSi_Draw(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    if (this->actionFunc != func_80AFB950) {
        func_8002ED80(&this->actor, play, 0);
        func_8002EBCC(&this->actor, play, 0);
        if (!gSaveContext.n64ddFlag) {
            GetItem_Draw(play, GID_SKULL_TOKEN_2);
        } else {
            getItem = Randomizer_GetItemFromActor(this->actor.id, play->sceneNum, this->actor.params, GI_SKULL_TOKEN);
            EnItem00_CustomItemsParticles(&this->actor, play, getItem);
            if (getItem.itemId != ITEM_SKULL_TOKEN) {
                f32 mtxScale = 1.5f;
                Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
            }
            GetItemEntry_Draw(play, getItem);
        }
        
    }
}

void Randomizer_UpdateSkullReward(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    getItem = Randomizer_GetItemFromActor(this->actor.id, play->sceneNum, this->actor.params, GI_SKULL_TOKEN);
    getItemId = getItem.getItemId;
    if (getItemId == RG_ICE_TRAP) {
        gSaveContext.pendingIceTrapCount++;
        textId = 0xF8;
    } else {
        textId = getItem.textId;
        giveItemId = getItem.itemId;
    }
    player->getItemEntry = getItem;
}

void Randomizer_GiveSkullReward(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (getItem.modIndex == MOD_NONE) {
        // RANDOTOD: Move this into Item_Give() or some other more central location
        if (getItem.getItemId == GI_SWORD_BGS) {
            gSaveContext.bgsFlag = true;
        }
        Item_Give(play, giveItemId);
    } else if (getItem.modIndex == MOD_RANDOMIZER) {
        Randomizer_Item_Give(play, getItem);
    }
}
