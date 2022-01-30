﻿#pragma once
#include <EventAPI.h>
#include "CPython.h"
#include "magic_enum.hpp"

enum class EventCode : int {
	onPreJoin, onJoin, onLeft, onPlayerCmd, onChat, onPlayerDie,
	onRespawn, onChangeDim, onJump, onSneak, onAttack, onEat, onMove, onChangeSprinting, onSpawnProjectile,
	onFireworkShootWithCrossbow, onSetArmor, onRide, onStepOnPressurePlate,
	onUseItem, onTakeItem, onDropItem, onUseItemOn, onInventoryChange, onChangeArmorStand,
	onStartDestroyBlock, onDestroyBlock, onWitherBossDestroy, onPlaceBlock, onLiquidFlow,
	onOpenContainer, onCloseContainer, onContainerChange, onOpenContainerScreen,
	onExplode, onBlockExploded, onBedExplode, onRespawnAnchorExplode, onEntityExplode, onBlockExplode,
	onMobDie, onMobHurt, onCmdBlockExecute, onRedStoneUpdate, onProjectileHitEntity,
	onProjectileHitBlock, onBlockInteracted, onUseRespawnAnchor, onFarmLandDecay, onUseFrameBlock,
	onPistonPush, onHopperSearchItem, onHopperPushOut, onFireSpread, onBlockChanged, onNpcCmd,
	onScoreChanged, onServerStarted, onConsoleCmd, onFormSelected, onConsoleOutput, onTick,
	onMoneyAdd, onMoneyReduce, onMoneyTrans, onMoneySet, onConsumeTotem, onEffectChanged,
};

void EnableEventListener(EventCode e, PyObject* func);