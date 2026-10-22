

#include    <stdio.h>
//#include    <ai.h>
#include    <time.h>
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h" 
#include	"game.h"
#include    "dme_utility.h"
#include	"items.h"
#include	"quake_items.h"

void FillPlayer(CBasePlayer *pPlayer)
{
    pPlayer->pev->health = 255;
    pPlayer->pev->armorvalue = 200;
	pPlayer->pev->armortype = 0.8;
}

void ArmPlayer(CBasePlayer *pPlayer)
{
		pPlayer->GiveNamedItem( "weapon_quakegun" );
		pPlayer->m_iQuakeItems |= (IT_SHOTGUN | IT_AXE | IT_NAILGUN | IT_SUPER_NAILGUN | IT_SUPER_SHOTGUN | IT_ROCKET_LAUNCHER | IT_GRENADE_LAUNCHER | IT_LIGHTNING);
		pPlayer->m_iQuakeWeapon = (IT_SHOTGUN);
		pPlayer->W_SetCurrentAmmo();
}		

void LoadPlayer(CBasePlayer *pPlayer)
{
		pPlayer->m_iAmmoNails = 250;
		pPlayer->m_iAmmoShells = 250;
		pPlayer->m_iAmmoRockets = 250;
		pPlayer->m_iAmmoCells = 250;
}

void LoadPlayerEX(CBasePlayer *pPlayer)
{
		pPlayer->m_iAmmoNails = 100;
		pPlayer->m_iAmmoShells = 100;
		pPlayer->m_iAmmoRockets = 100;
		pPlayer->m_iAmmoCells = 15;
}

void FillPlayerDefault(CBasePlayer *pPlayer)
{
    pPlayer->pev->health = 100;
    pPlayer->pev->armorvalue = 0;
	pPlayer->pev->armortype = 0;
}

void ArmPlayerDefault(CBasePlayer *pPlayer)
{
		pPlayer->GiveNamedItem( "weapon_quakegun" );
		pPlayer->m_iQuakeItems |= (IT_SHOTGUN | IT_AXE );
		pPlayer->m_iQuakeWeapon = (IT_SHOTGUN);
		pPlayer->W_SetCurrentAmmo();
}		

void LoadPlayerDefault(CBasePlayer *pPlayer)
{
		pPlayer->m_iAmmoShells = 25;
		pPlayer->m_iAmmoNails = 0;		
		pPlayer->m_iAmmoRockets = 0;
		pPlayer->m_iAmmoCells = 0;	
}

void FillPlayerMidair(CBasePlayer *pPlayer)
{
    pPlayer->pev->health = 200;
    pPlayer->pev->armorvalue = 200;
	pPlayer->pev->armortype = 0.8;
}

void ArmPlayerMidair(CBasePlayer *pPlayer)
{
		pPlayer->GiveNamedItem( "weapon_quakegun" );
		pPlayer->m_iQuakeItems |= (IT_AXE | IT_SHOTGUN | IT_ROCKET_LAUNCHER);
		pPlayer->m_iQuakeWeapon = (IT_ROCKET_LAUNCHER);
		pPlayer->W_SetCurrentAmmo();
}		

void LoadPlayerMidair(CBasePlayer *pPlayer)
{		
		pPlayer->m_iAmmoRockets = 100;		
}

void ArmPlayerInstagib(CBasePlayer *pPlayer)
{
		pPlayer->GiveNamedItem( "weapon_quakegun" );
		pPlayer->m_iQuakeItems |= (IT_AXE | IT_SHOTGUN | IT_ROCKET_LAUNCHER);
		pPlayer->m_iQuakeWeapon = (IT_SHOTGUN);
		pPlayer->W_SetCurrentAmmo();
}

void LoadPlayerUncell(CBasePlayer *pPlayer)
{
		pPlayer->m_iAmmoNails = 250;
		pPlayer->m_iAmmoShells = 250;
		pPlayer->m_iAmmoRockets = 250;
		pPlayer->m_iAmmoCells = 15;
}

void FillPlayerDMM(CBasePlayer *pPlayer)
{
    pPlayer->pev->health = 200;
    pPlayer->pev->armorvalue = 200;
	pPlayer->pev->armortype = 0.8;
}

/*
CMatchStart::CMatchStart()
{
	m_hudtMessage.x            = 0.40f;
    m_hudtMessage.y            = 0.2f;
    m_hudtMessage.a1           = 0.0f;
    m_hudtMessage.a2           = 0.0f;
    m_hudtMessage.b1           = 0.0f;
    m_hudtMessage.b2           = 255.0f;
    m_hudtMessage.g1           = 255.0f;
    m_hudtMessage.g2           = 255.0f;
    m_hudtMessage.r1           = 0.0f;
    m_hudtMessage.r2           = 255.0f;
    m_hudtMessage.channel      = 3;
    m_hudtMessage.effect       = 2;
    m_hudtMessage.fxTime       = 0.5;
    m_hudtMessage.fadeinTime   = 0.02;
    m_hudtMessage.fadeoutTime  = 0.5;    
    m_hudtMessage.holdTime     = 4.0;
	m_bInProgess = false;
	m_nMatchStart = MATCHSTART_NONE;
}
int CMatchStart::getMatch()
{
	return m_nMatchStart;
}
bool CMatchStart::isMatch()
{
	if(m_nMatchStart)
		return true;
	return false;
}

void CMatchStart::setMatch(int nMatch) 
{
	char cBuf[512];
	*cBuf = NULL;
	if((nMatch < 1) || (nMatch > 2))
	{
		m_dDelay.ClearTime();
		
		if(m_nMatchStart!=0)
		{
			m_hudtMessage.r1 = 255;
			m_hudtMessage.b1 = 0;
			m_hudtMessage.g1 = 255;
			UTIL_HudMessageAll(m_hudtMessage, "\"matchstart\" has been reset \nMatch play is over");
		}
		m_hudtMessage.b1           = 0.0f;
		m_hudtMessage.g1           = 255.0f;
		m_hudtMessage.r1           = 0.0f;
		m_bInProgess = false;

		MESSAGE_BEGIN( MSG_ALL, gmsgLocked, NULL);
		WRITE_BYTE (0); 
		MESSAGE_END();

		m_nMatchStart = MATCHSTART_NONE;
	}
	else
	{
		m_nMatchStart = nMatch;
		if(m_nMatchStart == 1)
			strcpy(cBuf, "Unloaded ");
		
		else
			strcpy(cBuf, "Fully Loaded ");
		strcat(cBuf, "match restart in 10 seconds\nThe teams are now locked");
		m_dDelay.AddTime(5); 
		UTIL_HudMessageAll(m_hudtMessage, cBuf);

		MESSAGE_BEGIN( MSG_ALL, gmsgLocked, NULL);
		WRITE_BYTE (1); 
		MESSAGE_END();

		
	}
		
}*/


void RemoveAllEntites() 
{
	CBaseEntity *pEntity = NULL;
	
	
	pEntity = UTIL_FindEntityByClassname( pEntity, "quake_rocket");
	while (pEntity)
	{
		
			STOP_SOUND( pEntity->edict(), CHAN_VOICE, "weapons/sgun1.wav" );
			FireTargets( "quake_rocket", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		    pEntity = UTIL_FindEntityByClassname( pEntity, "quake_rocket");
		
	}
	

	pEntity = UTIL_FindEntityByClassname( pEntity, "item_backpack");
	while (pEntity)
	{
		
			FireTargets( "item_backpack", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
	        pEntity = UTIL_FindEntityByClassname( pEntity, "item_backpack");
	}
}

void RemoveAllCorpses()
{ 
	CBaseEntity *pEntity = NULL;
	
	pEntity = UTIL_FindEntityByClassname( pEntity, "bodyque");
	while (pEntity)
	{
		
			FireTargets( "bodyque", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity );
	        pEntity = UTIL_FindEntityByClassname( pEntity, "bodyque");
	}



}

void RemoveAllPowerUps()
{
	
	CBaseEntity *pEntity = NULL;

	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invulnerability");
	while (pEntity)
	{
		//if (pEntity->IsAlive())
		{
			FireTargets( "item_artifact_invulnerability", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invulnerability");
	}


	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invisibility");
	while (pEntity)
	{
		//if (pEntity->IsAlive())
		{
			FireTargets( "item_artifact_invisibility", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invisibility");
	}

	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_super_damage");
	while (pEntity)
	{
		///if (pEntity->IsAlive())
		{
			FireTargets( "item_artifact_super_damage", pEntity, pEntity, USE_TOGGLE, 0 );
			UTIL_Remove( pEntity ); 
		}
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_super_damage");
	}
	
}


void RespawnAllEntites() 
{
	CBaseEntity *pEntity = NULL;
	CQuakeItem* pAmmo = NULL;
	CQuakeItem* pItem = NULL;
	CQuakeItem* pWpn = NULL;
	CItemPowerup* pPow = NULL;
	
	
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_health");
	while (pEntity)
	{
		pItem = (CQuakeItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_health");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor1");
	while (pEntity)
	{
		pItem = (CQuakeItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor1");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor2");
	while (pEntity)
	{
		pItem = (CQuakeItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor2");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor3");
	while (pEntity)
	{
		pItem = (CQuakeItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_armor3");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_armorInv");
	while (pEntity)
	{
		pItem = (CQuakeItem*)pEntity;
		pItem->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_armorInv");
	}
	//////////// ammo ////////////
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_shells");
	while (pEntity)
	{
		pAmmo = (CQuakeItem*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_shells");
	}
	
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_spikes");
	while (pEntity)
	{
		pAmmo = (CQuakeItem*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_spikes");
	}

	pEntity = UTIL_FindEntityByClassname( pEntity, "item_rockets");
	while (pEntity)
	{
		pAmmo = (CQuakeItem*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_rockets");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_cells");
	while (pEntity)
	{
		pAmmo = (CQuakeItem*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_cells");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_weapon");
	while (pEntity)
	{
		pAmmo = (CQuakeItem*)pEntity;
		pAmmo->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_weapon");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invisibility");
	while (pEntity)
	{
		pPow = (CItemPowerup*)pEntity;
		pPow->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invisibility");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_super_damage");
	while (pEntity)
	{
		pPow = (CItemPowerup*)pEntity;
		pPow->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_super_damage");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invulnerability");
	while (pEntity)
	{
		pPow = (CItemPowerup*)pEntity;
		pPow->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "item_artifact_invulnerability");
	}
	
	//////////// weapons ////////////
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_supershotgun");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_supershotgun");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_nailgun");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_nailgun");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_supernailgun");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_supernailgun");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_grenadelauncher");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_grenadelauncher");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_rocketlauncher");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_rocketlauncher");
	}
	pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_lightning");
	while (pEntity)
	{
		pWpn = (CQuakeItem*)pEntity;
		if(!pWpn->m_pPlayer)
			pWpn->Materialize();
        pEntity = UTIL_FindEntityByClassname( pEntity, "weapon_lightning");
	}
	
}/*
void ResetCtfFlags()
{
	CBaseEntity* pEntity = NULL;;
	pEntity = UTIL_FindEntityByClassname( pEntity, "ctf_redflag");
	while (pEntity)
	{
		CRedFlag* pFlag = (CRedFlag*)pEntity;
		pFlag->Reset();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ctf_redflag");
	}

	CBaseEntity* pEntity = NULL;;
	pEntity = UTIL_FindEntityByClassname( pEntity, "ctf_blueflag");
	while (pEntity)
	{
		CBlueFlag* pFlag = (CBlueFlag*)pEntity;
		pFlag->Reset();
        pEntity = UTIL_FindEntityByClassname( pEntity, "ctf_blueflag");
	}
}*/	
void ShowGameInfo(CBasePlayer* pPlayer)
{
	char print_string[1024];
    print_string[0]=NULL;



    hudtextparms_t      h_text;
	h_text.x            = 0.8f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 255.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 185.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 120.0f;
    h_text.r2            = 255.0f;
    h_text.channel      = 4;
    h_text.effect       = 2;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = 12;
    
	strncat(print_string, "Game Settings\n------------\n", 1023);  
	strncat(print_string, "gamemode ", 1023);
    strncat(print_string, gamemode.string, 1023);	
    
	strncat(print_string, "\ndeathmatch ", 1023);
    strncat(print_string, deathmatch.string, 1023);

    strncat(print_string, "\nmp_fraglimit ", 1023);
    strncat(print_string, fraglimit.string, 1023);    

    strncat(print_string, "\nspawnmode ", 1023);
    strncat(print_string, spawnmode.string, 1023);

	strncat(print_string, "\nroundtime ", 1023);
    strncat(print_string, roundtime.string, 1023);

	UTIL_HudMessage(pPlayer, h_text, print_string);
	ShowPlayerInfo(pPlayer);

}

void ShowGameModes(CBasePlayer* pPlayer)
{
	char print_string[1024];
    print_string[0]=NULL;

    hudtextparms_t      h_text;
	h_text.x            = 0.8f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 255.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 185.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 120.0f;
    h_text.r2            = 255.0f;
    h_text.channel      = 4;
    h_text.effect       = 2;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = 12;
    
	strncat(print_string, "DMC Pro 2.0 Gamemodes\n-----------------\n", 1023);  
	strncat(print_string, "FFA ", 1023);
	strncat(print_string, "\nTEAM DM ", 1023);
    strncat(print_string, "\nCTF ", 1023);
	strncat(print_string, "\nARENA ", 1023);
	strncat(print_string, "\nMIDAIR ", 1023);	
    strncat(print_string, "\n1ON1 MATCH ", 1023);
	strncat(print_string, "\n2ON2 MATCH ", 1023);
	strncat(print_string, "\n3ON3 MATCH ", 1023);
	strncat(print_string, "\n4ON4 MATCH ", 1023);
      

    UTIL_HudMessage(pPlayer, h_text, print_string);
	

}

void ShowGameHelp(CBasePlayer* pPlayer)
{
	char print_string[1024];
    print_string[0]=NULL;

	hudtextparms_t      h_text;
	h_text.x            = 0.02f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 255.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 185.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 120.0f;
    h_text.r2           = 255.0f;
    h_text.channel      = 4;
    h_text.effect       = 2;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = 12;

    
    
	strncat(print_string, "Client Commands\n---------------------------------\n", 1023);  
	strncat(print_string, "settings				- Shows match settings ", 1023);
    strncat(print_string, "\nready				- Play in match ", 1023);
	strncat(print_string, "\nbreak				- Break match ", 1023);
    strncat(print_string, "\n+hook				- Use grapple hook (if enabled) ", 1023);
    strncat(print_string, "\nthrowflag			- Drop flag(CTF only) ", 1023);
	strncat(print_string, "\nreport				- Report your location to team ", 1023);
	strncat(print_string, "\ngamemodehelp       - List of available gamemodes ", 1023);
	strncat(print_string, "\nvotinghelp			- How to vote ", 1023);

    

    UTIL_HudMessage(pPlayer, h_text, print_string);
	//ShowPlayerInfo(pPlayer);

}

void ShowGameVote(CBasePlayer* pPlayer)
{
	char print_string[1024];
    print_string[0]=NULL;

	hudtextparms_t      h_text;
	h_text.x            = 0.02f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 255.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 185.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 120.0f;
    h_text.r2           = 255.0f;
    h_text.channel      = 4;
    h_text.effect       = 2;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = 12;

    
    
	strncat(print_string, "Voting Commands\n---------------------------------\n", 1023);
	strncat(print_string, "Add the string 'callvote' before each of these commands in your console\n", 1023);
	strncat(print_string, "changelevel [mapname]		- Change Maps ", 1023);
    strncat(print_string, "\nffa						- Free For All ", 1023);
	strncat(print_string, "\nctf						- Capture The Flag ", 1023);
    strncat(print_string, "\nteam						- Team Deathmatch ", 1023);
    strncat(print_string, "\narena						- Rocket Arena ", 1023);
	strncat(print_string, "\nmidair						- Midair ", 1023);
	strncat(print_string, "\n1on1						- Match ", 1023);
	strncat(print_string, "\n2on2						- Match ", 1023);
	strncat(print_string, "\n3on3						- Match ", 1023);
	strncat(print_string, "\n4on4						- Match ", 1023);
	
    

    UTIL_HudMessage(pPlayer, h_text, print_string);
	//ShowPlayerInfo(pPlayer);

}


void ShowPlayerInfo(CBasePlayer* pPlayer)
{
	char print_string[1024];
    print_string[0]=NULL;

    hudtextparms_t      h_text;
	h_text.x            = 0.02f;
    h_text.y            = 0.1f;
    h_text.a1           = 0.0f;
    h_text.a2           = 0.0f;
    h_text.b1           = 30.0f;
    h_text.b2           = 255.0f;
    h_text.g1           = 85.0f;
    h_text.g2           = 255.0f;
    h_text.r1           = 255.0f;
    h_text.r2           = 255.0f;
    h_text.channel      = 3;
    h_text.effect       = 2;
    h_text.fxTime       = 0.5;
    h_text.fadeinTime   = 0.01;
    h_text.fadeoutTime  = 4;    
    h_text.holdTime     = 12;

	strcpy(print_string, "Players' ID's\n------------\n");
	for (int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pEnt = UTIL_PlayerByIndex( i );
		if ( pEnt && pEnt->IsPlayer( ) )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pEnt;
			strncat(print_string,STRING(pPlayer->pev->netname), 1023);
			strncat(print_string," ", 1023);
			strncat(print_string,GETPLAYERAUTHID(pPlayer->edict()), 1023);
			strncat(print_string,"\n", 1023);
			//sprintf(print_string, "%s %s\n", STRING(pPlayer->pev->netname) ,GETPLAYERAUTHID(pPlayer->edict()));

		}
	}
	UTIL_HudMessage(pPlayer, h_text, print_string);

}



CEntityNode::~CEntityNode() {}

CEntityNode::CEntityNode()
{
	m_pEntity = NULL;
	m_pNext	= NULL;
}


CEntityList::CEntityList()
{
	m_pHead = NULL;
	m_nSize = 0;
}

CEntityList::~CEntityList()
{
	clear();
}

void CEntityList::push_back( edict_t* pNewEntity )
{
	if(!pNewEntity) 
		return;

	CEntityNode* pNew = new CEntityNode();
	CEntityNode* pParent = NULL;
	CEntityNode* pTemp = m_pHead;
	pNew->m_pEntity = pNewEntity; 

	while(pTemp)
	{
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	if(!m_pHead)
		m_pHead = pNew;	
	else
		pParent->m_pNext =  pNew;	
	
	m_nSize++;
}

void CEntityList::clean()
{
	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;
	while(pTemp)
	{
		if(FStrEq(STRING(pTemp->m_pEntity->v.netname), ""))
		{
			CEntityNode* pNext = pTemp->m_pNext;
			delete pTemp;
			if(!pParent)
				m_pHead = pNext;
			else
				pParent->m_pNext = pNext;
			m_nSize--;
			pTemp = pNext;			
		}
		if(pTemp)
		{
			pParent = pTemp;
			pTemp = pTemp->m_pNext;
		}
		else 
			return;
	}
}
int CEntityList::lookup(edict_t* pEntity)
{
	CEntityNode* pTemp = m_pHead;
	for(int i = 0; i < m_nSize; i++)
	{
		if(pEntity == pTemp->m_pEntity)
			return i;
		pTemp = pTemp->m_pNext;
	}
	return -1;

}

void CEntityList::clear()
{
	CEntityNode* pTemp = m_pHead;
	for(int i=0; i < m_nSize; i++)
	{
		m_pHead = m_pHead->m_pNext;
		if(pTemp)
			delete pTemp;
		pTemp = m_pHead;
	}
	m_nSize = 0;
	m_pHead = NULL;
}

//////////////////////////////////////////////
// deletes  the nIndex  pEntity in the list //
//////////////////////////////////////////////

void CEntityList::remove(int nIndex)
{
	if(nIndex >= m_nSize || (nIndex < 0))
		return;

	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;

	for(int i=0; i < m_nSize; i++)
	{		
		if(i == nIndex)
		{
			if(!pParent) // implies pTemp == m_pHead;
				m_pHead = m_pHead->m_pNext;
			else
				pParent->m_pNext = pTemp->m_pNext;			
			delete pTemp;
			break;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	m_nSize--;
}


////////////////////////////////////////////////////////
// deletes only the first identical pEntity in the list //
////////////////////////////////////////////////////////

void CEntityList::remove(edict_t* pEntity)
{
	if(pEntity == NULL)
		return;
	
	CEntityNode* pTemp = m_pHead;
	CEntityNode* pParent = NULL;

	while(pTemp)
	{
		if(pTemp->m_pEntity == pEntity)
		{
			if(!pParent) // implies pTemp == m_pHead;
				m_pHead = pTemp->m_pNext;
			else
				pParent->m_pNext = pTemp->m_pNext;	
			delete pTemp;
			break;
		}
		pParent = pTemp;
		pTemp = pTemp->m_pNext;
	}
	m_nSize--;
}


edict_t* CEntityList::operator [] ( int nIndex )
{
	if(nIndex >= m_nSize)
		return NULL;

	CEntityNode* pTemp = m_pHead;
	
	for(int i=0; i < m_nSize; i++)
	{
		if(i == nIndex)
			break;
		pTemp = pTemp->m_pNext;
	}
	return pTemp->m_pEntity; // crash BUG hle
}


