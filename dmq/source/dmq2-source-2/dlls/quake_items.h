//=========== (C) Copyright 1996-2002, Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Quake world items
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#ifndef QUAKE_ITEMS_H
#define QUAKE_ITEMS_H


//DME
class CQuakeItem : public CBaseEntity
{
public:
	void	Spawn( void );

	// Respawning
	void	EXPORT Materialize( void );
	void	Respawn( float flTime );

	virtual void SetObjectCollisionBox ( void );

	// Touch
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	virtual	BOOL MyTouch( CBasePlayer *pOther ) { return FALSE; };

	CBasePlayer	*m_pPlayer;

	float	m_flRespawnTime;
};


class CItemHealth : public CQuakeItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );
	void EXPORT MegahealthRot( void );

	EHANDLE	m_hRotTarget;
	int		m_iHealAmount;
	int		m_iHealType;
};


//======================================================================================
// ARMOR ITEMS
//======================================================================================
class CItemArmor : public CQuakeItem
{
public:
	BOOL MyTouch( CBasePlayer *pPlayer );

	float m_flArmorValue;
	float m_flArmorType;
	int	  m_iArmorBit;
};



//===============
// Green Armor
class CItemArmorGreen : public CItemArmor
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Yellow Armor
class CItemArmorYellow : public CItemArmor
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Red Armor
class CItemArmorRed : public CItemArmor
{
public:
	void Spawn( void );
	void Precache( void );
};



//-----------------------------------------------
// Base Quake Weapon object
class CItemWeapon : public CQuakeItem
{
public:
	BOOL MyTouch( CBasePlayer *pPlayer );

	int	m_iWeapon;
};



//===============
// Super Shotgun
class CItemWeaponSuperShotgun : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Nailgun
class CItemWeaponNailgun : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};


//===============
// Super Nailgun
class CItemWeaponSuperNailgun : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};


//===============
// Grenade Launcher
class CItemWeaponGrenadeLauncher : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Rocket Launcher
class CItemWeaponRocketLauncher : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Lightning Gun
class CItemWeaponLightning : public CItemWeapon
{
public:
	void Spawn( void );
	void Precache( void );
};

class CItemAmmo : public CQuakeItem
{
public:
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );

	int	 m_isSmallBox;
	int  m_isLargeBox;
	int	 ammo_shells;
	int	 ammo_nails;
	int  ammo_rockets;
	int	 ammo_cells;
};


//===============
// Shells
class CItemAmmoShells : public CItemAmmo
{
public:
	void Spawn( void );
};

//===============
// Spikes
class CItemAmmoSpikes : public CItemAmmo
{
public:
	void Spawn( void );
};

//===============
// Rockets
class CItemAmmoRockets : public CItemAmmo
{
public:
	void Spawn( void );
};

//===============
// Cells
class CItemAmmoCells : public CItemAmmo
{
public:
	void Spawn( void );
};

class CItemAmmoWeapon : public CItemAmmo
{
public:
	void Spawn( void );
};

//===============================================================================
// POWERUPS
//===============================================================================
class CItemPowerup : public CQuakeItem
{
public:
	BOOL MyTouch( CBasePlayer *pPlayer );

	int		m_iPowerupBit;
	float	invincible_finished;
	float	radsuit_finished;
	float	invisible_finished;
	float	super_damage_finished;
};


//===============
// Pentagram
class CItemPowerupInvincible : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Radiation Suit
class CItemPowerupRadsuit : public CItemPowerup
{
public:
	void Spawn( void );
	void Precache( void );
};

//===============
// Ring of Invisibility
class CItemPowerupInvisibility : public CItemPowerup
{
public:
	void Spawn( void );
	void DropSpawn(void);
	void Precache( void );
};


//===============
// Quad Damage
class CItemPowerupQuad : public CItemPowerup
{
public:
	void Spawn( void );
	void DropSpawn(void);
	void Precache( void );
};


//===============================================================================
// PLAYER BACKPACKS
//===============================================================================
class CItemBackpack : public CQuakeItem
{
public:
	void Spawn( void );
//	void SetBox ( void );
	virtual void SetObjectCollisionBox ( void );

	BOOL MyTouch( CBasePlayer *pPlayer );

	int  m_iItems;
	int	 ammo_shells;
	int	 ammo_nails;
	int  ammo_rockets;
	int	 ammo_cells;
};

#endif // QUAKE_ITEMS.H
