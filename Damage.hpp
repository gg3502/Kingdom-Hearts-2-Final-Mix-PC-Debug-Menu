#pragma once
#include "common_types.hpp"
#include "YS.hpp"
#include "fmatrix.hpp"
#include <winsock.h>

struct DAMAGE
{
  Vector4 Dir;
  YS::BTLOBJ* Attacker;
  YS::BTLOBJ* Target;
  BITFLAG Flag;
  YS::ATTACK_13* Attack;
  const YS::ATTACKPARAM* AttackParam;
  u_char HitPart;
  u_char Parts;
  u_char Reaction;
  u_char OrigReaction;
  int Damage;
  unsigned int* pHitEffect;
  YS::SLIST_ELEM_3736 SList;
};
