/*
Some of this code began its life as a part of GTA V SCRIPT HOOK SDK.
http://dev-c.com
(C) Alexander Blade 2015

It is now part of the Enhanced Native Trainer project.
https://github.com/gtav-ent/GTAV-EnhancedNativeTrainer
(C) Rob Pridham and fellow contributors 2015
*/

#include "vehicles.h"
#include "..\ui_support\menu_functions.h"
#include "..\io\config_io.h"

extern int whichpart = 0;
extern int whichtype = 0;

//Parts
const std::vector<std::string> MENU_PAINT_WHAT{ "Primary Color", "Secondary Color", "Primary & Secondary", "Pearl Topcoat", "Wheels" };

//Paint Names
const std::vector<std::string> MENU_PAINT_TYPE{ "Normal", "Metallic", "Matte", "Metal", "Chrome" };

const std::vector<PaintColour> PAINTS_NORMAL{ 
	{ "Black", 0, 0 },
	{ "Carbon Black", 147, 0 },
	{ "Graphite Black", 1, 0 },
	{ "Anthracite Black", 11, 0 },
	{ "Black Steel", 2, 0 },
	{ "Dark Steel", 3, 2 },
	{ "Silver", 4, 4 },
	{ "Bluish Silver", 5, 5 },
	{ "Rolled Steel", 6, 0 },
	{ "Shadow Silver", 7, 0 },
	{ "Stone Silver", 8, 0 },
	{ "Midnight Silver", 9, 0 },
	{ "Cast Iron Silver", 10, 0 },
	{ "Red", 27, 0 },
	{ "Torino Red", 28, 0 },
	{ "Formula Red", 29, 0 },
	{ "Lava Red", 150, 0 },
	{ "Blaze Red", 30, 0 },
	{ "Grace Red", 31, 0 },
	{ "Garnet Red", 32, 0 },
	{ "Sunset Red", 33, 0 },
	{ "Cabernet Red", 34, 0 },
	{ "Wine Red", 143, 0 },
	{ "Candy Red", 35, 0 },
	{ "Hot Pink", 135, 0 },
	{ "Pfister Pink", 137, 0 },
	{ "Salmon Pink", 136, 0 },
	{ "Sunrise Orange", 36, 0 },
	{ "Orange", 38, 0 },
	{ "Bright Orange", 138, 0 },
	{ "Gold", 99, 99 },
	{ "Bronze", 90, 102 },
	{ "Yellow", 88, 0 },
	{ "Race Yellow", 89, 0 },
	{ "Dew Yellow", 91, 0 },
	{ "Dark Green", 49, 0 },
	{ "Racing Green", 50, 0 },
	{ "Sea Green", 51, 0 },
	{ "Olive Green", 52, 0 },
	{ "Bright Green", 53, 0 },
	{ "Gasoline Green", 54, 0 },
	{ "Lime Green", 92, 0 },
	{ "Midnight Blue", 141, 0 },
	{ "Galaxy Blue", 61, 0 },
	{ "Dark Blue", 62, 0 },
	{ "Saxon Blue", 63, 0 },
	{ "Blue", 64, 0 },
	{ "Mariner Blue", 65, 0 },
	{ "Harbor Blue", 66, 0 },
	{ "Diamond Blue", 67, 0 },
	{ "Surf Blue", 68, 0 },
	{ "Nautical Blue", 69, 0 },
	{ "Racing Blue", 73, 0 },
	{ "Ultra Blue", 70, 0 },
	{ "Light Blue", 74, 0 },
	{ "Chocolate Brown", 96, 0 },
	{ "Bison Brown", 101, 0 },
	{ "Creek Brown", 95, 0 },
	{ "Feltzer Brown", 94, 0 },
	{ "Maple Brown", 97, 0 },
	{ "Beechwood Brown", 103, 0 },
	{ "Sienna Brown", 104, 0 },
	{ "Saddle Brown", 98, 0 },
	{ "Moss Brown", 100, 0 },
	{ "Woodbeech Brown", 102, 0 },
	{ "Straw Brown", 99, 0 },
	{ "Sandy Brown", 105, 0 },
	{ "Bleached Brown", 106, 0 },
	{ "Schafter Purple", 71, 0 },
	{ "Spinnaker Purple", 72, 0 },
	{ "Midnight Purple", 142, 0 },
	{ "Bright Purple", 145, 0 },
	{ "Cream", 107, 0 },
	{ "Ice White", 111, 0 },
	{ "Frost White", 112, 0 }
};

const std::vector<PaintColour> PAINTS_METALLIC{
	{ "Black", 0, 10 },
};

const std::vector<PaintColour> PAINTS_MATTE{
	{ "Black", 12, 0 },
};

const std::vector<PaintColour> PAINTS_METAL{
	{ "Brushed Steel", 117, 18 },
};

const std::vector<PaintColour> PAINTS_PEARL{
	{ "Black", -1, 0 },

const std::vector<PaintColour> PAINTS_WHEELS
{
	{ "Default", 156, -1 },
};

const PaintColour CHROME_COLOUR =
{
	"Chrome", 120, 0
};

bool onconfirm_paintdirt(MenuItem<float> choice)
{
	return true;
}


void onhighlight_paintdirt(MenuItem<float> choice)
{
	// common variables
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());

	if (!bPlayerExists)
	{
		return;
	}

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return;
	}

	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	VEHICLE::SET_VEHICLE_DIRT_LEVEL(veh, choice.value);
}

bool onconfirm_paintfade(MenuItem<float> choice)
{
	return true;
}

void onhighlight_paintfade(MenuItem<float> choice)
{
	// common variables
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());

	if (!bPlayerExists)
	{
		return;
	}

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return;
	}

	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	VEHICLE::_SET_VEHICLE_PAINT_FADE(veh, choice.value);
}

bool process_paint_menu_fades()
{
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return false;
	}

	set_status_text("Note: paint fade is only relevant on certain vehicles");

	std::vector<MenuItem<float>*> menuItems;

	MenuItem<float> *item = new MenuItem<float>();
	item->caption = "Fresh Paint";
	item->value = 0.0f;
	item->isLeaf = true;
	menuItems.push_back(item);

	for (int i = 20; i < 100; i += 20)
	{
		MenuItem<float> *item = new MenuItem<float>();
		std::ostringstream ss;
		ss << i << "% Faded";
		item->caption = ss.str();
		item->value = (float)i / 100;
		item->isLeaf = true;
		menuItems.push_back(item);
	}

	item = new MenuItem<float>();
	item->caption = "Fully Faded";
	item->value = 1.0f;
	item->isLeaf = true;
	menuItems.push_back(item);

	return draw_generic_menu<float>(menuItems, 0, "Paint Fade", onconfirm_paintfade, onhighlight_paintfade, NULL, vehicle_menu_interrupt);
}

bool process_paint_menu_dirt()
{
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return false;
	}

	std::vector<MenuItem<float>*> menuItems;

	MenuItem<float> *item = new MenuItem<float>();
	item->caption = "Clean";
	item->value = 0.0f;
	item->isLeaf = true;
	menuItems.push_back(item);

	for (int i = 20; i < 100; i += 20)
	{
		MenuItem<float> *item = new MenuItem<float>();
		std::ostringstream ss;
		ss << i << "% Dirty";
		item->caption = ss.str();
		item->value = ((float)i / 100) * 15.0f;
		item->isLeaf = true;
		menuItems.push_back(item);
	}

	item = new MenuItem<float>();
	item->caption = "Fully Dirty";
	item->value = 14.89999f;
	item->isLeaf = true;
	menuItems.push_back(item);

	return draw_generic_menu<float>(menuItems, 0, "Dirt Level", onconfirm_paintdirt, onhighlight_paintdirt, NULL, vehicle_menu_interrupt);
}

void onhighlight_livery(MenuItem<int> choice)
{
	// common variables
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());

	if (!bPlayerExists)
	{
		return;
	}

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return;
	}

	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	VEHICLE::SET_VEHICLE_LIVERY(veh, choice.value);
}

bool onconfirm_livery(MenuItem<int> choice)
{
	return true;
}

bool process_paint_menu_liveries()
{
	// common variables
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());

	if (!bPlayerExists)
	{
		return false;
	}

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return false;
	}

	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);

	int count = VEHICLE::GET_VEHICLE_LIVERY_COUNT(veh);
	if (count <= 1)
	{
		set_status_text("No liveries for this vehicle");
	}

	std::vector<MenuItem<int>*> menuItems;

	for (int i = 0; i < count; i++)
	{
		std::string modItemNameStr;

		char* modItemNameChr = VEHICLE::GET_LIVERY_NAME(veh, i);
		if (modItemNameChr == NULL)
		{
			std::ostringstream ss;
			ss << "Livery #" << (i+1);
			modItemNameStr = ss.str();
		}
		else
		{
			modItemNameStr = std::string(modItemNameChr);
		}

		MenuItem<int> *item = new MenuItem<int>();
		item->caption = modItemNameStr;
		item->value = i;
		item->isLeaf = false;
		menuItems.push_back(item);
	}

	int currentSelection = VEHICLE::GET_VEHICLE_LIVERY(veh);
	return draw_generic_menu<int>(menuItems, &currentSelection, "Liveries", onconfirm_livery, onhighlight_livery, NULL, vehicle_menu_interrupt);
}

bool onconfirm_paint_menu(MenuItem<int> choice)
{
	::whichpart = choice.value;
	if (whichpart == 4) //Wheels
	{
		process_paint_menu_special();
	}
	else if (whichpart == 3) //Pearl topcoat
	{
		process_paint_menu_special();
	}
	else if (whichpart == -1)
	{
		process_paint_menu_liveries();
	}
	else if (whichpart == -2)
	{
		process_paint_menu_fades();
	}
	else if (whichpart == -3)
	{
		process_paint_menu_dirt();
	}
	else
	{
		process_paint_menu_type(); //Primary and Secondary Colors
	}
	return false;
}

bool process_paint_menu_special()
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	std::vector<MenuItem<int>*> menuItems;
	
	int primary, secondary, pearl, wheel;
	VEHICLE::GET_VEHICLE_COLOURS(veh, &primary, &secondary);
	VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);

	int index = 0;

	if (whichpart == 4)
	{
		int curWheel = VEHICLE::GET_VEHICLE_MOD(veh, 23);
		if (curWheel == -1)
		{
			set_status_text("You can't repaint the car's default wheels");
			return false;
		}
	}

	/*
	if (whichpart == 3 && (paint1 == 3 || paint1 == 4 || paint1 == 5))
	{
		set_status_text("Pearl cannot be applied over this paint type");
		return false;
	}
	*/

	std::vector<PaintColour> paints;
	int matchIndex = 0;
	switch (whichpart)
	{
	case 3:
		whichtype = 4;
		paints = PAINTS_PEARL;
		for (int i = 0; i < paints.size(); i++)
		{
			if (pearl == paints[i].pearlAddition)
			{
				matchIndex = i;
				break;
			}
		}
		break;
	case 4:
		whichtype = 5;
		paints = PAINTS_WHEELS;
		for (int i = 0; i < paints.size(); i++)
		{
			if (wheel == paints[i].mainValue)
			{
				matchIndex = i;
				break;
			}
		}
		break;
	default:
		return false;
	}
	
	for (int i = 0; i < paints.size(); i++)
	{
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = paints[i].name;
		item->value = i;
		menuItems.push_back(item);
	}

	if (whichpart == 4)
	{
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = "Chrome";
		item->value = -1;
		menuItems.push_back(item);
	}

	return draw_generic_menu<int>(menuItems, &matchIndex, "Select Color", onconfirm_color_menu_selection, onhighlight_color_menu_selection, NULL);
}
bool process_paint_menu()
{
	// common variables
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());

	if (!bPlayerExists)
	{
		return false;
	}

	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
	{
		set_status_text("Player isn't in a vehicle");
		return false;
	}

	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	int liveryCount = VEHICLE::GET_VEHICLE_LIVERY_COUNT(veh);

	std::vector<MenuItem<int>*> menuItems;

	for (int i = 0; i < MENU_PAINT_WHAT.size(); i++)
	{
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = MENU_PAINT_WHAT[i];
		item->value = i;
		item->isLeaf = false;
		menuItems.push_back(item);
	}

	if (liveryCount > 1)
	{
		std::ostringstream ss;
		ss << "Liveries (" << liveryCount << ")";
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = ss.str();
		item->value = -1;
		item->isLeaf = false;
		menuItems.push_back(item);
	}

	MenuItem<int> *item = new MenuItem<int>();
	item->caption = "Paint Fade";
	item->value = -2;
	item->isLeaf = false;
	menuItems.push_back(item);

	item = new MenuItem<int>();
	item->caption = "Dirt Level";
	item->value = -3;
	item->isLeaf = false;
	menuItems.push_back(item);

	return draw_generic_menu<int>(menuItems, 0, "Choose which part to paint", onconfirm_paint_menu, NULL, NULL, vehicle_menu_interrupt);
}

bool onconfirm_paint_menu_type(MenuItem<int> choice)
{
	if (choice.value == 4)
	{
		//deal with chrome immediately
		apply_paint(CHROME_COLOUR);
		return false;
	}

	std::string category = choice.caption;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);

	int primary, secondary, pearl, wheel;
	VEHICLE::GET_VEHICLE_COLOURS(veh, &primary, &secondary);
	VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &pearl, &wheel);

	//const std::vector<std::string> MENU_PAINT_TYPE{ "Normal", "Metallic", "Matte", "Metal", "Chrome" };

	std::vector<PaintColour> paints;
	switch (choice.value)
	{
	case 0:
		paints = PAINTS_NORMAL;
		break;
	case 1:
		paints = PAINTS_METALLIC;
		break;
	case 2:
		paints = PAINTS_MATTE;
		break;
	case 3:
		paints = PAINTS_METAL;
		break;
	case 4:
		paints = PAINTS_PEARL;
		break;
	case 5:
		paints = PAINTS_WHEELS;
		break;
	default:
		return false;
	}

	std::vector<MenuItem<int>*> menuItems;
	for (int i = 0; i < paints.size(); i++)
	{
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = paints[i].name;
		item->value = i;
		menuItems.push_back(item);
	}

	::whichtype = choice.value; //save paint type for later

	int matchIndex = 0;

	if (::whichpart == 0 || ::whichpart == 2) //index as primary color
	{
		for (int i = 0; i < paints.size(); i++)
		{
			if (primary == paints[i].mainValue)
			{
				matchIndex = i;
				break;
			}
		}
	}
	else if (::whichpart == 1) //index as secondary color
	{
		for (int i = 0; i < paints.size(); i++)
		{
			if (secondary == paints[i].mainValue)
			{
				matchIndex = i;
				break;
			}
		}
	}

	draw_generic_menu<int>(menuItems, &matchIndex, category, onconfirm_color_menu_selection, onhighlight_color_menu_selection, NULL, vehicle_menu_interrupt);
	return false;
}

bool process_paint_menu_type()
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
	int index = 0;

	std::vector<MenuItem<int>*> menuItems;
	for (int i = 0; i < MENU_PAINT_TYPE.size(); i++)
	{
		MenuItem<int> *item = new MenuItem<int>();
		item->caption = MENU_PAINT_TYPE[i];
		item->value = i;
		item->isLeaf = false;
		menuItems.push_back(item);
	}

	if (::whichpart == 0) //index as primary color type
	{
		int paint1, paint2, paint3;
		VEHICLE::GET_VEHICLE_MOD_COLOR_1(veh, &paint1, &paint2, &paint3);
		index = paint1;
		if (index == 3) { index = 0; } //if paint type is pearlescent index to paint type metallic
		else if (index > 3) { index = index - 1; }
	}
	else if (::whichpart == 1) //index as secondary color type
	{
		int paint1, paint2;
		VEHICLE::GET_VEHICLE_MOD_COLOR_2(veh, &paint1, &paint2);
		index = paint1;
		if (index > 1) { index = index - 1; }
	}

	return draw_generic_menu<int>(menuItems, &index, "Choose Paint Type", onconfirm_paint_menu_type, NULL, NULL, vehicle_menu_interrupt);
}

void onhighlight_color_menu_selection(MenuItem<int> choice)
{
	onconfirm_color_menu_selection(choice);
}

bool onconfirm_color_menu_selection(MenuItem<int> choice)
{
	std::vector<PaintColour> paints;
	switch (whichtype)
	{
	case 0:
		paints = PAINTS_NORMAL;
		break;
	case 1:
		paints = PAINTS_METALLIC;
		break;
	case 2:
		paints = PAINTS_MATTE;
		break;
	case 3:
		paints = PAINTS_METAL;
		break;
	case 4:
		paints = PAINTS_PEARL;
		break;
	case 5:
		paints = PAINTS_WHEELS;
		break;
	default:
		return false;
	}

	PaintColour whichpaint = (choice.value == -1) ? CHROME_COLOUR : paints[choice.value];
	apply_paint(whichpaint);
	return true;
}

void apply_paint(PaintColour whichpaint)
{
	BOOL bPlayerExists = ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID());
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (bPlayerExists)
	{
		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, 0))
		{
			Vehicle veh = PED::GET_VEHICLE_PED_IS_USING(playerPed);
			VEHICLE::SET_VEHICLE_MOD_KIT(veh, 0);

			int primary, secondary;
			VEHICLE::GET_VEHICLE_COLOURS(veh, &primary, &secondary);

			if (::whichpart == 0 || ::whichpart == 2) //Apply primary Color
			{
				/*
				std::ostringstream ss;
				ss << "Applying primary paint " << whichpaint << " of type " << ::whichtype;
				set_status_text(ss.str());
				*/

				VEHICLE::SET_VEHICLE_MOD_COLOR_1(veh, 0, 0, 0);
				VEHICLE::SET_VEHICLE_COLOURS(veh, whichpaint.mainValue, secondary);
				if (whichpart == 2)
				{
					primary = whichpaint.mainValue;
				}
			}

			if (::whichpart == 1 || ::whichpart == 2) //apply secondary Color
			{
				/*
				std::ostringstream ss;
				ss << "Applying secondary paint " << whichpaint << " of type " << ::whichtype;
				set_status_text(ss.str());
				*/

				VEHICLE::SET_VEHICLE_MOD_COLOR_2(veh, 0, 0);
				VEHICLE::SET_VEHICLE_COLOURS(veh, primary, whichpaint.mainValue);

				if (whichpart == 2)
				{
					secondary = whichpaint.mainValue;
				}
			}
			
			if (::whichpart == 3) //Apply pearl Topcoat
			{
				int paint1, paint2, paint3;
				VEHICLE::GET_VEHICLE_MOD_COLOR_1(veh, &paint1, &paint2, &paint3);

				/*
				std::ostringstream ss;
				ss << "Applying pearl paint " << whichpaint << " with old value " << paint2;
				set_status_text(ss.str());
				*/

				VEHICLE::SET_VEHICLE_MOD_COLOR_1(veh, 0, primary+1, primary+1);
				VEHICLE::SET_VEHICLE_COLOURS(veh, primary, secondary);
			}

			if (whichpaint.pearlAddition != -1)
			{
				int useless, wheelCol;//pearl topcoat, wheel color
				VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &useless, &wheelCol);
				VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, whichpaint.pearlAddition, wheelCol); //apply pearl color without changing wheels
			}

			if (::whichpart == 4) //Apply wheel color
			{
				bool isChrome = (whichpaint.mainValue == CHROME_COLOUR.mainValue);
				set_chrome_wheels_enabled(veh, isChrome);
				if (!isChrome)
				{
					int paint1, paint2;//pearl topcoat, wheel color
					VEHICLE::GET_VEHICLE_EXTRA_COLOURS(veh, &paint1, &paint2);
					VEHICLE::SET_VEHICLE_EXTRA_COLOURS(veh, paint1, whichpaint.mainValue); //apply wheel color without changing pearl topcoat
				}
			}
		}
		else
		{
			set_status_text("Player isn't in a vehicle");
		}
	}
	return;
}