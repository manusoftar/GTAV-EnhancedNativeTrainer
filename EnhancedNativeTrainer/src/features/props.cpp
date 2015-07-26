/*
Part of the Enhanced Native Trainer project.
https://github.com/gtav-ent/GTAV-EnhancedNativeTrainer
(C) Rob Pridham and fellow contributors 2015
*/

#include "props.h"
#include "script.h"
#include "propplacement.h"
#include "..\datasets\data_props.h"

#include <set>

int lastSelectedCategoryIndex = 0;
int lastSelectedPropIndex = 0;

bool requireRefreshOfPropInstanceMenu = false;
bool propInstanceMenuInterruptFlag = false;

std::vector<SpawnedPropInstance*> propsWeCreated;

std::string lastCustomPropSpawn;

int explosionID = 0;

SpawnedPropInstance* lastHighlightedProp = NULL;

const std::vector<std::string> ALPHA_LABELS = { "Normal", "80%", "60%", "40%", "20%" };
const int ALPHA_VALUES[] = { 255, 204, 153, 102, 51 };

bool creationParam1 = true;
bool creationParam2 = true;
bool creationParam3 = false;

bool propCreationIsInvincible = false;
bool propCreationIsImmovable = false;
bool propCreationHasGravity = true;
int propCreationAlphaIndex = 0;

void reset_prop_globals()
{
	lastSelectedCategoryIndex = 0;
	propCreationIsInvincible = false;
	propCreationIsImmovable = false;
	propCreationHasGravity = true;
	propCreationAlphaIndex = 0;
	lastCustomPropSpawn = "";
}

float vectRads(float degs)
{
	float radialConv = degs*3.1415926536 / 180;
	return radialConv;
}

void manage_prop_set()
{
	std::vector<SpawnedPropInstance*>::iterator it;
	for (it = propsWeCreated.begin(); it != propsWeCreated.end();)
	{
		SpawnedPropInstance* prop = *it;
		if (!ENTITY::DOES_ENTITY_EXIST(prop->instance))
		{
			it = propsWeCreated.erase(it);
			if (prop == currentProp)
			{
				currentProp = NULL;
			}
			delete prop;
		}
		else
		{
			++it;
		}
	}
}

/**
* Many props don't play nicely with PLACE_OBJECT_ON_GROUND_PROPERLY.
* Therefore we use one that is known to work properly to determine
* the height of the ground, and therefore the position for the other 
* prop, by spawning one and then deleting it.
*/
bool get_ground_height_at_position(Vector3 coords, float* result)
{
	return GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(coords.x, coords.y, coords.z, result);
}

void do_spawn_model(Hash propHash, char* model, std::string title, bool silent)
{
	STREAMING::REQUEST_MODEL(propHash);
	DWORD now = GetTickCount();
	while (!STREAMING::HAS_MODEL_LOADED(propHash) && GetTickCount() < now + 5000 )
	{
		make_periodic_feature_call();
		WAIT(0);
	}

	if (!STREAMING::HAS_MODEL_LOADED(propHash))
	{
		std::ostringstream ss2;
		ss2 << "TIMEOUT: " << model;
		write_text_to_log_file(ss2.str());
		return;
	}

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	FLOAT look = ENTITY::GET_ENTITY_HEADING(playerPed);
	FLOAT lookAni = look + 180.00;
	FLOAT lookOff = look + 90.00;
	FLOAT vecX = 0;
	FLOAT vecY = 0;
	BOOL getPosParam1 = 1;
	BOOL getPosParam2 = 1;
	BOOL getPosParam3 = 1;

	FLOAT spawnOffX = 0.0f;
	FLOAT spawnOffY = 3.5f;
	FLOAT spawnOffZ = 0.0f;

	Vector3 minDimens;
	Vector3 maxDimens;
	GAMEPLAY::GET_MODEL_DIMENSIONS(propHash, &minDimens, &maxDimens);
	spawnOffY = max(3.5f, 2.0f + 0.5f * (maxDimens.y - minDimens.y));
	spawnOffZ = 0.0f;

	Vector3 coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(playerPed, spawnOffX, spawnOffY, spawnOffZ);

	float objZBase = 0;
	bool translatable = get_ground_height_at_position(coords, &objZBase);

	Object obj = OBJECT::CREATE_OBJECT_NO_OFFSET(propHash, coords.x, coords.y, coords.z, creationParam1, creationParam2, creationParam3);
	ENTITY::SET_ENTITY_VELOCITY(obj, 0.0f, 0.0f, 0.0f);
	ENTITY::SET_ENTITY_ROTATION(obj, 0, 0, 0, 0, false);

	if (ENTITY::DOES_ENTITY_EXIST(obj))
	{
		ENTITY::SET_ENTITY_COLLISION(obj, 1, 0);
		
		//place on the ground doesn't work on half the items, so do it ourselves
		Vector3 curLocation = ENTITY::GET_ENTITY_COORDS(obj, 0);
		if (translatable)
		{
			if (minDimens.z < 0)
			{
				objZBase -= minDimens.z;
			}

			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(obj, curLocation.x, curLocation.y, objZBase, 1, 1, 1);
		}
		else
		{
			//best effort in case of failure
			OBJECT::PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
		}

		ENTITY::SET_ENTITY_HAS_GRAVITY(obj, propCreationHasGravity);

		ENTITY::FREEZE_ENTITY_POSITION(obj, propCreationIsImmovable);

		if (!propCreationIsImmovable)
		{
			//this unfreezes it
			ENTITY::APPLY_FORCE_TO_ENTITY(obj, 3, 0, 0, 0.1, 0, 0, 0, 0, 1, 1, 0, 0, 1);
			OBJECT::SET_ACTIVATE_OBJECT_PHYSICS_AS_SOON_AS_IT_IS_UNFROZEN(obj, TRUE);
		}

		if (propCreationIsInvincible)
		{
			//ENTITY::SET_ENTITY_INVINCIBLE(obj, TRUE);
			//ENTITY::SET_ENTITY_PROOFS(obj, 1, 1, 1, 1, 1, 1, 1, 1);
			ENTITY::SET_ENTITY_CAN_BE_DAMAGED(obj, FALSE);
		}

		ENTITY::SET_ENTITY_LOAD_COLLISION_FLAG(obj, true);

		ENTITY::SET_ENTITY_ALPHA(obj, ALPHA_VALUES[propCreationAlphaIndex], false);

		SpawnedPropInstance* record = new SpawnedPropInstance();
		record->instance = obj;

		record->title = title;
		record->counter = find_highest_instance_num_of_prop(propHash) + 1;
		record->isInvincible = propCreationIsInvincible;
		record->isImmovable = propCreationIsImmovable;
		record->hasGravity = propCreationHasGravity;

		propsWeCreated.push_back(record);
		manage_prop_set();
	}
	else
	{
		if (!silent)
		{
			std::ostringstream ss;
			ss << "Failed to create " << title;
			set_status_text(ss.str());
		}

		std::ostringstream ss2;
		ss2 << "INVALID-PROP: " << model;
		write_text_to_log_file(ss2.str());
		return;
	}

	if (!silent)
	{
		std::ostringstream ss;
		ss << "Spawned " << title;
		set_status_text(ss.str());
	}

	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(propHash);
	//ENTITY::SET_OBJECT_AS_NO_LONGER_NEEDED(&obj);
}

void do_spawn_prop(PropInfo prop, bool silent)
{
	Hash propHash = GAMEPLAY::GET_HASH_KEY((char *)prop.model);

	if (!STREAMING::IS_MODEL_IN_CDIMAGE(propHash) || !STREAMING::IS_MODEL_VALID(propHash))
	{
		if (!silent)
		{
			std::ostringstream ss;
			ss << "Model " << prop.model << " is not valid";
			set_status_text(ss.str());
		}

		std::ostringstream ss2;
		ss2 << "INVALID-MODEL: " << prop.model;
		write_text_to_log_file(ss2.str());
		return;
	}

	do_spawn_model(propHash, prop.model, prop.label, silent);
}

bool onconfirm_prop_selection(MenuItem<int> choice)
{
	std::string category = PROP_CATEGORIES[lastSelectedCategoryIndex];

	std::vector<PropInfo> filtered;
	for each (PropInfo prop in ALL_PROPS)
	{
		if (prop.category.compare(category) == 0)
		{
			filtered.push_back(prop);
		}
	}

	if (choice.value == -1)
	{
		int i = 0;
		for each (PropInfo prop  in filtered)
		{
			std::ostringstream ss;
			ss << "Done " << i++ << " of " << filtered.size();
			set_status_text_centre_screen(ss.str());
			WAIT(0);

			do_spawn_prop(prop, true);
		}
		return false;
	}

	PropInfo prop = filtered.at(choice.value);
	do_spawn_prop(prop, false);

	return false;
}

int propCategorySelection = 0;
int propSelection = 0;

void process_props_menu_incategory(int categoryIndex)
{
	std::string category = PROP_CATEGORIES[categoryIndex];

	std::vector<PropInfo> filtered;
	int count = 0;
	int total = 0;
	for each (PropInfo prop in ALL_PROPS)
	{
		total++;
		if (prop.category.compare(category) == 0)
		{
			count++;
			filtered.push_back(prop);
		}
	}

	/*
	std::ostringstream ssd;
	ssd << count << " of " << total << " and " << ALL_PROPS.size();
	set_status_text_centre_screen(ssd.str());
	*/

	std::vector<MenuItem<int>*> menuItems;

	/*
	MenuItem<int>* item = new MenuItem<int>();
	item->value = -1;
	item->caption = "Spawn All In Category";
	item->isLeaf = true;
	menuItems.push_back(item);
	*/

	int i = 0;
	for each (PropInfo prop in filtered)
	{
		MenuItem<int>* item = new MenuItem<int>();
		item->value = i;
		item->caption = prop.label;
		item->isLeaf = true;
		menuItems.push_back(item);
		i++;
	}

	draw_generic_menu<int>(menuItems, &propSelection, category, onconfirm_prop_selection, NULL, NULL, NULL);
}

bool onconfirm_prop_category(MenuItem<int> choice)
{
	if (choice.value == -1)
	{
		std::string result = show_keyboard(NULL, (char*)lastCustomPropSpawn.c_str());
		if (!result.empty())
		{
			result = trim(result);
			lastCustomPropSpawn = result;
			Hash hash = GAMEPLAY::GET_HASH_KEY((char*)result.c_str());
			if (!STREAMING::IS_MODEL_IN_CDIMAGE(hash) || !STREAMING::IS_MODEL_VALID(hash))
			{
				std::ostringstream ss;
				ss << "Couldn't find model '" << result << "'";
				set_status_text(ss.str());
				return false;
			}
			else
			{
				do_spawn_model(GAMEPLAY::GET_HASH_KEY((char*)result.c_str()), (char*)result.c_str(), result, false);
			}
		}
		return false;
	}

	if (choice.value != lastSelectedCategoryIndex)
	{
		propSelection = 0;
		lastSelectedCategoryIndex = choice.value;
	}
	process_props_menu_incategory(choice.value);
	return false;
}

void process_props_spawn_menu()
{
	std::vector<MenuItem<int>*> menuItems;

	int i = 0;
	for each (std::string category in PROP_CATEGORIES)
	{
		MenuItem<int>* item = new MenuItem<int>();
		item->value = i;
		item->caption = category;
		item->isLeaf = false;
		menuItems.push_back(item);
		i++;
	}

	MenuItem<int>* item = new MenuItem<int>();
	item->value = -1;
	item->caption = "Enter Name Manually";
	item->isLeaf = false;
	menuItems.push_back(item);
	i++;

	/*
	ToggleMenuItem<int>* toggleItem = new ToggleMenuItem<int>();
	toggleItem->caption = "Spawn Param 1";
	toggleItem->toggleValue = &creationParam1;
	menuItems.push_back(toggleItem);

	toggleItem = new ToggleMenuItem<int>();
	toggleItem->caption = "Spawn Param 2";
	toggleItem->toggleValue = &creationParam2;
	menuItems.push_back(toggleItem);

	toggleItem = new ToggleMenuItem<int>();
	toggleItem->caption = "Spawn Param 3";
	toggleItem->toggleValue = &creationParam3;
	menuItems.push_back(toggleItem);
	*/

	draw_generic_menu<int>(menuItems, &propCategorySelection, "Object Categories", onconfirm_prop_category, NULL, NULL, NULL);
}

void onchange_spawn_alpha(int value, SelectFromListMenuItem* source)
{
	//if we've been supplied with an extra, we're applying this to an instance
	if (source->extras.size() > 0)
	{
		SpawnedPropInstance* prop = get_prop_at_index(source->extras.at(0));
		ENTITY::SET_ENTITY_ALPHA(prop->instance, ALPHA_VALUES[value], false);
	}
	//otherwise we're changing the new spawn option
	else
	{
		propCreationAlphaIndex = value;
	}
}

int prop_spawnopt_selection = 0;

bool prop_spawn_options_menu()
{
	/*
	bool propCreationIsInvincible = false;
	bool propCreationIsOnFire = false;
	bool propCreationIsImmovable = false;
	bool propCreationHasPhysics = true;
	bool propCreationHasGravity = true;
	bool propCreationHasGravity = true;
	*/

	std::vector<MenuItem<int>*> menuItems;

	int i = 0;

	ToggleMenuItem<int>* item = new ToggleMenuItem<int>();
	item->toggleValue = &propCreationIsInvincible;
	item->caption = "Invincible?";
	menuItems.push_back(item);
	i++;

	item = new ToggleMenuItem<int>();
	item->toggleValue = &propCreationIsImmovable;
	item->caption = "Immovable?";
	menuItems.push_back(item);
	i++;

	item = new ToggleMenuItem<int>();
	item->toggleValue = &propCreationHasGravity;
	item->caption = "Has Gravity?";
	menuItems.push_back(item);
	i++;

	/*
	item = new ToggleMenuItem<int>();
	item->toggleValue = &propCreationIsOnFire;
	item->caption = "On Fire?";
	menuItems.push_back(item);
	i++;
	*/

	SelectFromListMenuItem* alphaItem = new SelectFromListMenuItem(ALPHA_LABELS, onchange_spawn_alpha);
	alphaItem->value = propCreationAlphaIndex;
	alphaItem->caption = "Alpha (Opacity)";
	alphaItem->wrap = false;
	menuItems.push_back(alphaItem);
	i++;

	draw_generic_menu<int>(menuItems, &prop_spawnopt_selection, "New Object Options", NULL, NULL, NULL, NULL);
	return false;
}

bool onconfirm_prop_menu(MenuItem<int> choice)
{
	if (choice.value == 0)
	{
		process_props_spawn_menu();
	}
	else if (choice.value == 1) //remove all
	{
		int count = 0;
		for each (SpawnedPropInstance* prop in propsWeCreated)
		{
			Object obj = prop->instance;
			if (ENTITY::DOES_ENTITY_EXIST(obj))
			{
				count++;
				OBJECT::DELETE_OBJECT(&obj);
			}
		}
		manage_prop_set();
		std::ostringstream ss;
		ss << count << " object" << (count != 1 ? "s" : "") << " removed";
		set_status_text(ss.str());
		return false;
	}
	else if (choice.value == 2)
	{
		prop_spawn_options_menu();
	}
	else if (choice.value == 3)
	{
		manage_prop_set();

		if (propsWeCreated.size() == 0)
		{
			set_status_text("No spawned objects - create some first");
		}
		else
		{
			prop_spawned_instances_menu();
		}
	}
	return false;
}

int prop_menu_selection = 0;

void process_props_menu()
{
	std::vector<MenuItem<int>*> menuItems;

	int i = 0;

	MenuItem<int>* item = new MenuItem<int>();
	item->value = 0;
	item->caption = "Object Spawner";
	item->isLeaf = false;
	menuItems.push_back(item);
	i++;

	item = new MenuItem<int>();
	item->value = 2;
	item->caption = "Spawn Options";
	item->isLeaf = false;
	menuItems.push_back(item);
	i++;

	item = new MenuItem<int>();
	item->value = 3;
	item->caption = "Edit Spawned Objects";
	item->isLeaf = false;
	menuItems.push_back(item);
	i++;

	item = new MenuItem<int>();
	item->value = 1;
	item->caption = "Remove All Spawned Objects";
	item->isLeaf = true;
	menuItems.push_back(item);
	i++;
	
	draw_generic_menu<int>(menuItems, &prop_menu_selection, "Objects", onconfirm_prop_menu, NULL, NULL, NULL);
}

void add_props_feature_enablements(std::vector<FeatureEnabledLocalDefinition>* results)
{
	results->push_back(FeatureEnabledLocalDefinition{ "propCreationHasGravity", &propCreationHasGravity });
	results->push_back(FeatureEnabledLocalDefinition{ "propCreationIsImmovable", &propCreationIsImmovable });
	results->push_back(FeatureEnabledLocalDefinition{ "propCreationIsInvincible", &propCreationIsInvincible });
}

void add_props_generic_settings(std::vector<StringPairSettingDBRow>* results)
{
	results->push_back(StringPairSettingDBRow{ "propCreationAlphaIndex", std::to_string(propCreationAlphaIndex)});
	results->push_back(StringPairSettingDBRow{ "lastCustomPropSpawn", lastCustomPropSpawn });
}

void handle_generic_settings_props(std::vector<StringPairSettingDBRow>* settings)
{
	for (int i = 0; i < settings->size(); i++)
	{
		StringPairSettingDBRow setting = settings->at(i);
		if (setting.name.compare("propCreationAlphaIndex") == 0)
		{
			propCreationAlphaIndex = stoi(setting.value);
		}
		else if (setting.name.compare("lastCustomPropSpawn") == 0)
		{
			lastCustomPropSpawn = setting.value;
		}
	}
}

void cleanup_props()
{
	std::vector<SpawnedPropInstance*>::iterator it;
	for (it = propsWeCreated.begin(); it != propsWeCreated.end();)
	{
		SpawnedPropInstance* prop = *it;
		if (ENTITY::DOES_ENTITY_EXIST(prop->instance))
		{
			ENTITY::SET_OBJECT_AS_NO_LONGER_NEEDED(&prop->instance);
		}
		it = propsWeCreated.erase(it);
		if (prop == currentProp)
		{
			currentProp = NULL;
		}
		delete prop;
	}
}

int find_highest_instance_num_of_prop(Hash model)
{
	int highestFound = 0;
	std::vector<SpawnedPropInstance*>::iterator it;
	for (it = propsWeCreated.begin(); it != propsWeCreated.end(); it++)
	{
		SpawnedPropInstance* prop = *it;
		Hash entryModel = ENTITY::GET_ENTITY_MODEL(prop->instance);
		if (model == entryModel && prop->counter > highestFound)
		{
			highestFound = prop->counter;
		}
	}
	return highestFound;
}

int menu_spawned_instance_index = 0;

bool prop_spawned_instances_menu()
{
	requireRefreshOfPropInstanceMenu = false;
	propInstanceMenuInterruptFlag = false;

	do
	{
		requireRefreshOfPropInstanceMenu = false;

		manage_prop_set();

		std::vector<MenuItem<int>*> menuItems;

		int i = 0;

		std::vector<SpawnedPropInstance*>::iterator it;
		for (it = propsWeCreated.begin(); it != propsWeCreated.end(); it++)
		{
			SpawnedPropInstance* instance = *it;

			MenuItem<int>* item = new MenuItem<int>();
			item->value = i;
			std::ostringstream ss;
			ss << instance->title << "~HUD_COLOUR_MENU_YELLOW~ #" << instance->counter;
			item->caption = ss.str();
			item->isLeaf = false;
			menuItems.push_back(item);
			i++;
		}

		set_menu_per_frame_call(flash_prop_callback);

		draw_generic_menu<int>(menuItems, &menu_spawned_instance_index, "Spawned Instances", onconfirm_prop_instance_menu, onhighlight_prop_instance_menu, NULL, prop_instance_menu_interrupt);

		clear_menu_per_frame_call();
		if (lastHighlightedProp != NULL && ENTITY::DOES_ENTITY_EXIST(lastHighlightedProp->instance))
		{
			ENTITY::SET_ENTITY_VISIBLE(lastHighlightedProp->instance, TRUE);
		}

		WAIT(0);
	}
	while (requireRefreshOfPropInstanceMenu);
	return false;
}

void onhighlight_prop_instance_menu(MenuItem<int> choice)
{
	if (lastHighlightedProp != NULL && ENTITY::DOES_ENTITY_EXIST(lastHighlightedProp->instance))
	{
		ENTITY::SET_ENTITY_VISIBLE(lastHighlightedProp->instance, TRUE);
	}

	SpawnedPropInstance* prop = get_prop_at_index(choice.value);
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label J");
		return;
	}
	lastHighlightedProp = prop;
}


bool onconfirm_prop_instance_menu(MenuItem<int> choice)
{
	clear_menu_per_frame_call();
	if (lastHighlightedProp != NULL && ENTITY::DOES_ENTITY_EXIST(lastHighlightedProp->instance))
	{
		ENTITY::SET_ENTITY_VISIBLE(lastHighlightedProp->instance, TRUE);
	}
	prop_spawned_single_instance_menu(choice.value);
	set_menu_per_frame_call(flash_prop_callback);
	return false;
}

bool prop_instance_menu_interrupt()
{
	if (propInstanceMenuInterruptFlag)
	{
		propInstanceMenuInterruptFlag = false;
		return true;
	}
	return false;
}

SpawnedPropInstance* get_prop_at_index(int i)
{
	if (i < 0 || i > propsWeCreated.size() - 1)
	{
		return NULL;
	}
	SpawnedPropInstance* prop = propsWeCreated.at(i);
	return prop;
}

bool is_prop_invincible(std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label A");
		return false;
	}
	return prop->isInvincible;
}

void set_prop_invincible(bool applied, std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label B");
		return;
	}

	//ENTITY::SET_ENTITY_INVINCIBLE(prop->instance, applied);
	ENTITY::SET_ENTITY_CAN_BE_DAMAGED(prop->instance, !applied);
	//ENTITY::SET_ENTITY_PROOFS(prop->instance, applied, applied, applied, applied, applied, applied, applied, applied);

	prop->isInvincible = applied;
}

bool is_prop_immovable(std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label C");
		return false;
	}
	return prop->isImmovable;
}

void set_prop_immovable(bool applied, std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label D");
		return;
	}

	OBJECT::SET_ACTIVATE_OBJECT_PHYSICS_AS_SOON_AS_IT_IS_UNFROZEN(prop->instance, !applied);
	ENTITY::FREEZE_ENTITY_POSITION(prop->instance, applied);
	if (!applied)
	{
		//this unfreezes it
		ENTITY::SET_ENTITY_CAN_BE_DAMAGED(prop->instance, FALSE);
		ENTITY::APPLY_FORCE_TO_ENTITY(prop->instance, 3, 0, 0, 0.1, 0, 0, 0, 0, 1, 1, 0, 0, 1);
		ENTITY::SET_ENTITY_CAN_BE_DAMAGED(prop->instance, !prop->isInvincible);
	}
	prop->isImmovable = applied;
}

bool is_prop_on_fire(std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label E");
		return false;
	}
	return FIRE::IS_ENTITY_ON_FIRE(prop->instance);
}

void set_prop_on_fire(bool applied, std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label F");
		return;
	}
	if (applied)
	{
		bool isInvinc = prop->isInvincible;
		//ENTITY::SET_ENTITY_PROOFS(prop->instance, isInvinc, false, isInvinc, isInvinc, isInvinc, isInvinc, isInvinc, isInvinc);
		Vector3 curLocation = ENTITY::GET_ENTITY_COORDS(prop->instance, 0);
		FIRE::ADD_EXPLOSION(curLocation.x, curLocation.y, curLocation.z, 14, 3.0f, true, false, 0); //starts gas fire
	}
	else
	{
		FIRE::STOP_ENTITY_FIRE(prop->instance);
		Vector3 position = ENTITY::GET_ENTITY_COORDS(prop->instance, TRUE);
		FIRE::STOP_FIRE_IN_RANGE(position.x, position.y, position.z, 5.0f);
	}
}

bool is_prop_gravity_enabled(std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label G");
		return false;
	}
	return prop->hasGravity;
}

void set_prop_gravity_enabled(bool applied, std::vector<int> extras)
{
	SpawnedPropInstance* prop = get_prop_at_index(extras.at(0));
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label H");
		return;
	}
	ENTITY::SET_ENTITY_HAS_GRAVITY(prop->instance, applied);
	prop->hasGravity = applied;
}

std::string get_explosion_name(int id)
{
	switch (id)
	{
	case EXPLOSION_DEFAULT:
		return "Default";
	case EXPLOSION_MOLOTOV:
		return "Molotov";
	case EXPLOSION_WATER_SPRAY_SHORT:
		return "Water Jet (Short)";
	case EXPLOSION_GAS_JET_BRIEF:
		return "Gas Jet (Brief)";
	case EXPLOSION_WATER_SPRAY_TALL:
		return "Water Jet (Tall)";
	case EXPLOSION_GAS_JET_PROLONGED:
		return "Gas Jet (Prolonged)";
	case EXPLOSION_GRENADE:
		return "Grenade";
	case EXPLOSION_SMOKE:
		return "Smoke Bomb";
	case EXPLOSION_CS_GAS_1:
		return "CS Gas 1";
	case EXPLOSION_CS_GAS_2:
		return "CS Gas 2";
	case EXPLOSION_FLARE:
		return "Flare";
	case EXPLOSION_MINE:
		return "Mine";
	case EXPLOSION_WATER_SPRAY_BRIEF:
		return "Water Jet (Brief)";
	case EXPLOSION_BLIMP:
		return "Blimp Explosion";
	case EXPLOSION_MINI:
		return "Mini Detonation";
	case EXPLOSION_FIREBALL_SMALL_W_RING:
		return "Small Fireball With Radius";
	case EXPLOSION_FIREBALL_SMALL_1:
		return "Small Fireball #1";
	case EXPLOSION_FIREBALL_SMALL_2:
		return "Small Fireball #2";
	case EXPLOSION_FIREBALL_SMALL_3:
		return "Small Fireball #3";
	case EXPLOSION_FIREBALL_MEDIUM_1:
		return "Medium Fireball #1";
	case EXPLOSION_FIREBALL_MEDIUM_2:
		return "Medium Fireball #2";
	case EXPLOSION_FIREBALL_MEDIUM_3:
		return "Medium Fireball #3";
	case EXPLOSION_FIREBALL_LARGE_1:
		return "Large Fireball #1";
	case EXPLOSION_FIREBALL_LARGE_2:
		return "Large Fireball #2";
	case EXPLOSION_FIREBALL_LARGE_3:
		return "Large Fireball #3";
	case EXPLOSION_FIREBALL_LARGE_4:
		return "Large Fireball #4";
	default:
		{
		std::ostringstream ss;
		ss << "Unknown Explosion ID " << id;
		auto result = ss.str();
		return result;
		}
	}
}

void explode_last_prop(int explosionID)
{
	SpawnedPropInstance* prop = get_prop_at_index(lastSelectedPropIndex);
	if (explosionID == -1)
	{
		explosionID = 0; //default
	}
	Vector3 position = ENTITY::GET_ENTITY_COORDS(prop->instance, TRUE);
	FIRE::ADD_EXPLOSION(position.x, position.y, position.z, explosionID, 3.0f, true, false, 0);
}

bool onconfirm_prop_single_instance_menu(MenuItem<int> choice)
{
	SpawnedPropInstance* prop = get_prop_at_index(lastSelectedPropIndex);
	if (prop == NULL)
	{
		set_status_text_centre_screen("Null prop - label K");
		return true;
	}

	if (choice.value == 1) //delete item
	{
		OBJECT::DELETE_OBJECT(&prop->instance);
		propsWeCreated.erase(propsWeCreated.begin() + lastSelectedPropIndex);
		manage_prop_set();
		propInstanceMenuInterruptFlag = true;
		requireRefreshOfPropInstanceMenu = true;
		set_status_text("Object deleted");
		return true;
	}
	else if (choice.value == 2)
	{
		begin_prop_placement(prop);
	}
	else if (choice.value == 3) //default explode
	{
		explode_last_prop(-1);
	}
	else if (choice.value == 4) //custom explode
	{
		process_prop_explosion_choices();
	}
	return false;
}

int singleInstanceMenuIndex = 0;

void flash_prop_callback()
{
	if (lastHighlightedProp != NULL)
	{
		int frame = get_frame_number() % 30;
		if (frame == 0)
		{
			ENTITY::SET_ENTITY_VISIBLE(lastHighlightedProp->instance, FALSE);
		}
		else if (frame == 10)
		{
			ENTITY::SET_ENTITY_VISIBLE(lastHighlightedProp->instance, TRUE);
		}
	}
}

bool prop_spawned_single_instance_menu(int index)
{
	lastSelectedPropIndex = index;

	std::vector<MenuItem<int>*> menuItems;

	MenuItem<int>* item = new MenuItem<int>();
	item->value = 1;
	item->caption = "Delete This Object";
	item->isLeaf = true;
	menuItems.push_back(item);

	item = new MenuItem<int>();
	item->value = 2;
	item->caption = "Move This Object";
	item->isLeaf = false;
	menuItems.push_back(item);

	FunctionDrivenToggleMenuItem<int>* togItem = new FunctionDrivenToggleMenuItem<int>();
	togItem->getter_call = is_prop_invincible;
	togItem->setter_call = set_prop_invincible;
	togItem->extra_arguments.push_back(index);
	togItem->caption = "Invincible?";
	menuItems.push_back(togItem);

	togItem = new FunctionDrivenToggleMenuItem<int>();
	togItem->getter_call = is_prop_immovable;
	togItem->setter_call = set_prop_immovable;
	togItem->extra_arguments.push_back(index);
	togItem->caption = "Immovable?";
	menuItems.push_back(togItem);

	togItem = new FunctionDrivenToggleMenuItem<int>();
	togItem->getter_call = is_prop_gravity_enabled;
	togItem->setter_call = set_prop_gravity_enabled;
	togItem->extra_arguments.push_back(index);
	togItem->caption = "Has Gravity?";
	menuItems.push_back(togItem);

	togItem = new FunctionDrivenToggleMenuItem<int>();
	togItem->getter_call = is_prop_on_fire;
	togItem->setter_call = set_prop_on_fire;
	togItem->extra_arguments.push_back(index);
	togItem->caption = "On Fire?";
	menuItems.push_back(togItem);

	SelectFromListMenuItem* alphaItem = new SelectFromListMenuItem(ALPHA_LABELS, onchange_spawn_alpha);
	alphaItem->value = propCreationAlphaIndex;
	alphaItem->caption = "Alpha (Opacity)";
	alphaItem->wrap = false;
	alphaItem->extras.push_back(index);
	menuItems.push_back(alphaItem);

	item = new MenuItem<int>();
	item->value = 3;
	item->caption = "Explode This Object (Default)";
	item->isLeaf = true;
	menuItems.push_back(item);

	item = new MenuItem<int>();
	item->value = 4;
	item->caption = "Custom Explosions";
	item->isLeaf = false;
	menuItems.push_back(item);
	
	draw_generic_menu<int>(menuItems, &singleInstanceMenuIndex, "Object Options", onconfirm_prop_single_instance_menu, NULL, NULL, NULL);

	return false;
}


bool onconfirm_prop_explosion(MenuItem<int> choice)
{
	explode_last_prop(choice.value);
	return false;
}

int explosionSelection;

void process_prop_explosion_choices()
{
	std::vector<MenuItem<int>*> menuItems;

	for each (int var in OrderedExplosions)
	{
		MenuItem<int>* item = new MenuItem<int>();
		item->value = var;
		item->caption = get_explosion_name(var);
		item->isLeaf = true;
		menuItems.push_back(item);
	}

	draw_generic_menu<int>(menuItems, &explosionSelection, "Explosions", onconfirm_prop_explosion, NULL, NULL, NULL);
}