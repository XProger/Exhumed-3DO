#ifndef __INCLUDEDweaponh
#define __INCLUDEDweaponh

extern sint32 currentWeapon;
extern sint32 const weaponMaxAmmo[];

void weaponForce(sint32 fx, sint32 fy);
void weaponSetVel(sint32 vx, sint32 vy);
void initWeapon(void);
void fireWeapon(void);
void weaponPlayerMove(sint32 yavel);

void setCurrentWeapon(sint32 i);
void weaponUp(sint32 weaponMask);
void weaponDown(sint32 weaponMask);

void runWeapon(sint32 nmFrames, sint32 invisible, sint32 boost);

void weaponChangeAmmo(sint32 weapon, sint32 deltaAmmo);
void switchWeapons(sint32 on);
#endif
