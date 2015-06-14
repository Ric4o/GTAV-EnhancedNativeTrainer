/*
Part of the Enhanced Native Trainer project.
https://github.com/gtav-ent/GTAV-EnhancedNativeTrainer
(C) Rob Pridham and fellow contributors 2015
*/

#include "config_io.h"
#include "keyboard.h"
#include "xinput.h"
//#include "script.h"
#include "..\debug\debuglog.h"
#include <sstream>

// A global Windows "basic string". Actual memory is allocated by the
// COM methods used by MSXML which take &bstr. We must use SysFreeString() 
// to free this memory before subsequent uses, to prevent a leak.
BSTR bstr;

TrainerConfig *config = NULL;

/**Read the XML config file. Currently contains keyboard choices.*/
void read_config_file()
{
	TrainerConfig *result = new TrainerConfig();

	CoInitialize(NULL);

	//read XML
	MSXML2::IXMLDOMDocumentPtr spXMLDoc;
	spXMLDoc.CreateInstance(__uuidof(MSXML2::DOMDocument60));
	if (!spXMLDoc->load("ent-config.xml"))
	{
		write_text_to_log_file("No config found, using defaults");
		config = result; //the default config
	}

	IXMLDOMNodeListPtr nodes = spXMLDoc->selectNodes(L"//ent-config/keys/key");
	long length;
	nodes->get_length(&length);
	for (int i = 0; i < length; i++)
	{
		IXMLDOMNode *node;
		nodes->get_item(i, &node);
		IXMLDOMNamedNodeMap *attribs;
		node->get_attributes(&attribs);

		long length_attribs;
		attribs->get_length(&length_attribs);

		char *attrib_key_func = NULL;
		char *attrib_key_value = NULL;
		bool modCtrl = false;
		bool modAlt = false;
		bool modShift = false;

		for (long j = 0; j < length_attribs; j++)
		{
			IXMLDOMNode *attribNode;
			attribs->get_item(j, &attribNode);
			attribNode->get_nodeName(&bstr);
			if (wcscmp(bstr, L"function") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				attrib_key_func = _com_util::ConvertBSTRToString(V_BSTR(&var));
			}
			else if (wcscmp(bstr, L"value") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				attrib_key_value = _com_util::ConvertBSTRToString(V_BSTR(&var));
			}
			else if (wcscmp(bstr, L"modCtrl") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				char* value = _com_util::ConvertBSTRToString(V_BSTR(&var));
				if (value != 0 && strcmpi(value, "true") == 0)
				{
					modCtrl = true;
				}
			}
			else if (wcscmp(bstr, L"modAlt") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				char* value =_com_util::ConvertBSTRToString(V_BSTR(&var));
				if (value != 0 && strcmpi(value, "true") == 0)
				{
					modAlt = true;
				}
			}
			else if (wcscmp(bstr, L"modShift") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				char* value = _com_util::ConvertBSTRToString(V_BSTR(&var));
				if (value != 0 && strcmpi(value, "true") == 0)
				{
					modShift = true;
				}
			}

			SysFreeString(bstr);
			attribNode->Release();
		}
		
		if (attrib_key_func != NULL && attrib_key_value != NULL)
		{
			result->get_key_config()->set_key(attrib_key_func, attrib_key_value, modCtrl, modAlt, modShift);
		}
		
		delete attrib_key_func;
		delete attrib_key_value;

		attribs->Release();
		node->Release();
	}

	nodes = spXMLDoc->selectNodes(L"//ent-config/keys/controller");
	nodes->get_length(&length);
	for (int i = 0; i < length; i++)
	{
		IXMLDOMNode *node;
		nodes->get_item(i, &node);
		IXMLDOMNamedNodeMap *attribs;
		node->get_attributes(&attribs);

		long length_attribs;
		attribs->get_length(&length_attribs);

		char *attrib_control_func = NULL;
		char *attrib_control_value = NULL;

		for (long j = 0; j < length_attribs; j++)
		{
			IXMLDOMNode *attribNode;
			attribs->get_item(j, &attribNode);
			attribNode->get_nodeName(&bstr);
			if (wcscmp(bstr, L"function") == 0)
			{
				VARIANT var;
				VariantInit(&var);
				attribNode->get_nodeValue(&var);
				attrib_control_func = _com_util::ConvertBSTRToString(V_BSTR(&var));
			}

			attribNode->Release();
		}

		ControllerButtonConfig* buttonConfig = new ControllerButtonConfig();

		IXMLDOMNodeListPtr children;
		if (node->get_childNodes(&children))
		{
			long childLength;
			children->get_length(&childLength);
			for (int j = 0; j < length; j++)
			{
				IXMLDOMNode *child;
				nodes->get_item(i, &child);
				IXMLDOMNamedNodeMap *childAttribs;
				node->get_attributes(&childAttribs);

				long length_attribs;
				childAttribs->get_length(&length_attribs);

				char *attrib_control_button = NULL;

				for (long k = 0; k < length_attribs; k++)
				{
					IXMLDOMNode *attribNode;
					attribs->get_item(k, &attribNode);
					attribNode->get_nodeName(&bstr);
					if (wcscmp(bstr, L"button") == 0)
					{
						VARIANT var;
						VariantInit(&var);
						attribNode->get_nodeValue(&var);
						attrib_control_button = _com_util::ConvertBSTRToString(V_BSTR(&var));
						buttonConfig->add_button(attrib_control_button);
					}
					attribNode->Release();
				}
			}
		}

		if (attrib_control_func != NULL && attrib_control_value != NULL)
		{
			result->get_key_config()->set_control(attrib_control_func, buttonConfig);
		}

		delete attrib_control_func;
		delete attrib_control_value;

		attribs->Release();
		node->Release();
	}

	//nodes->Release(); //don't do this, it crashes on exit
	spXMLDoc.Release();
	CoUninitialize();
	
	config = result;
}

void KeyInputConfig::set_key(char* function, char* keyName, bool modCtrl, bool modAlt, bool modShift)
{
	std::ostringstream ss;
	ss << "Key function " << function << " being given " << keyName;
	write_text_to_log_file(ss.str());

	int vkID = keyNameToVal(keyName);
	if (vkID == -1)
	{
		ss.str(""); ss.clear();
		ss << "Key function " << keyName << " didn't correspond to a value";
		write_text_to_log_file(ss.str());
		return;
	}

	auto match = keyConfigs.find(function);
	if (match != keyConfigs.end())
	{
		KeyConfig* oldConfig = match->second;
		match->second = new KeyConfig(vkID);
		match->second->modCtrl = modCtrl;
		match->second->modAlt = modAlt;
		match->second->modShift = modShift;
		delete oldConfig;
	}
	else
	{
		ss.str(""); ss.clear();
		ss << "Key function " << function << " didn't correspond to a known function";
		write_text_to_log_file(ss.str());
	}
};

void KeyInputConfig::set_control(char* function, ControllerButtonConfig* config)
{
	std::ostringstream ss;
	ss << "Controller function " << function << " being set";
	write_text_to_log_file(ss.str());

	auto match = controllerConfigs.find(function);
	if (match != controllerConfigs.end())
	{
		ControllerButtonConfig* oldConfig = match->second;
		match->second = new ControllerButtonConfig();
		delete oldConfig;
	}
	else
	{
		ss.str(""); ss.clear();
		ss << "Controller function " << function << " didn't correspond to a known function";
		write_text_to_log_file(ss.str());
	}
}

TrainerConfig::TrainerConfig()
{
	this->keyConfig = new KeyInputConfig();
}

KeyInputConfig::KeyInputConfig()
{
	this->keyConfigs[KeyConfig::KEY_TOGGLE_MAIN_MENU] = new KeyConfig(VK_F4);

	this->keyConfigs[KeyConfig::KEY_MENU_UP] = new KeyConfig(VK_NUMPAD8);
	this->keyConfigs[KeyConfig::KEY_MENU_DOWN] = new KeyConfig(VK_NUMPAD2);
	this->keyConfigs[KeyConfig::KEY_MENU_LEFT] = new KeyConfig(VK_NUMPAD4);
	this->keyConfigs[KeyConfig::KEY_MENU_RIGHT] = new KeyConfig(VK_NUMPAD6);
	this->keyConfigs[KeyConfig::KEY_MENU_SELECT] = new KeyConfig(VK_NUMPAD5);
	this->keyConfigs[KeyConfig::KEY_MENU_BACK] = new KeyConfig(VK_NUMPAD0);

	this->keyConfigs[KeyConfig::KEY_VEH_BOOST] = new KeyConfig(VK_NUMPAD9);
	this->keyConfigs[KeyConfig::KEY_VEH_STOP] = new KeyConfig(VK_NUMPAD3);
	this->keyConfigs[KeyConfig::KEY_VEH_ROCKETS] = new KeyConfig(VK_ADD);

	this->keyConfigs[KeyConfig::KEY_TOGGLE_AIRBRAKE] = new KeyConfig(VK_F6);

	this->keyConfigs[KeyConfig::KEY_HOT_AIRBRAKE_THROUGH_DOOR] = new KeyConfig(VK_DIVIDE);
	this->keyConfigs[KeyConfig::KEY_HOT_AIRBRAKE_THROUGH_DOOR]->modCtrl = true;

	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_UP] = new KeyConfig(VK_KEY_Q);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_DOWN] = new KeyConfig(VK_KEY_Z);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_FORWARD] = new KeyConfig(VK_KEY_W);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_BACK] = new KeyConfig(VK_KEY_S);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_ROTATE_LEFT] = new KeyConfig(VK_KEY_A);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_ROTATE_RIGHT] = new KeyConfig(VK_KEY_D);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_SPEED] = new KeyConfig(VK_SHIFT);
	this->keyConfigs[KeyConfig::KEY_AIRBRAKE_FREEZE_TIME] = new KeyConfig(VK_KEY_T);

	this->controllerConfigs[KeyConfig::KEY_TOGGLE_MAIN_MENU] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_TOGGLE_MAIN_MENU]->add_button(ControllerButtonConfig::CONTROLLER_BTN_Y);
	this->controllerConfigs[KeyConfig::KEY_TOGGLE_MAIN_MENU]->add_button(ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_R);

	this->controllerConfigs[KeyConfig::KEY_MENU_UP] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_UP]->add_button(ControllerButtonConfig::CONTROLLER_BTN_DPAD_U);
	this->controllerConfigs[KeyConfig::KEY_MENU_DOWN] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_DOWN]->add_button(ControllerButtonConfig::CONTROLLER_BTN_DPAD_D);
	this->controllerConfigs[KeyConfig::KEY_MENU_LEFT] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_LEFT]->add_button(ControllerButtonConfig::CONTROLLER_BTN_DPAD_L);
	this->controllerConfigs[KeyConfig::KEY_MENU_RIGHT] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_RIGHT]->add_button(ControllerButtonConfig::CONTROLLER_BTN_DPAD_R);
	this->controllerConfigs[KeyConfig::KEY_MENU_SELECT] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_SELECT]->add_button(ControllerButtonConfig::CONTROLLER_BTN_X);
	this->controllerConfigs[KeyConfig::KEY_MENU_BACK] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_MENU_BACK]->add_button(ControllerButtonConfig::CONTROLLER_BTN_Y);

	this->controllerConfigs[KeyConfig::KEY_VEH_BOOST] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_VEH_BOOST]->add_button(ControllerButtonConfig::CONTROLLER_BTN_TRIGGER_R);
	this->controllerConfigs[KeyConfig::KEY_VEH_BOOST]->add_button(ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_R);

	this->controllerConfigs[KeyConfig::KEY_VEH_STOP] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_VEH_STOP]->add_button(ControllerButtonConfig::CONTROLLER_BTN_TRIGGER_L);
	this->controllerConfigs[KeyConfig::KEY_VEH_STOP]->add_button(ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_L);

	this->controllerConfigs[KeyConfig::KEY_VEH_ROCKETS] = new ControllerButtonConfig();
	this->controllerConfigs[KeyConfig::KEY_VEH_ROCKETS]->add_button(ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_L);
	this->controllerConfigs[KeyConfig::KEY_VEH_ROCKETS]->add_button(ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_R);
}

KeyInputConfig::~KeyInputConfig()
{
	for each (std::pair<std::string, KeyConfig*> conf in this->keyConfigs)
	{
		delete conf.second;
	}
	keyConfigs.clear();
}

KeyConfig* KeyInputConfig::get_key(std::string function)
{
	auto match = keyConfigs.find(function);
	if (match != keyConfigs.end())
	{
		return match->second;
	}
	return NULL;
}

ControllerButtonConfig* KeyInputConfig::get_controller_button(std::string function)
{
	auto match = controllerConfigs.find(function);
	if (match != controllerConfigs.end())
	{
		return match->second;
	}
	return NULL;
}

void ControllerButtonConfig::add_button(char* name)
{
	int code = buttonNameToVal(name);
	this->buttonCodes.push_back(code);
}

void ControllerButtonConfig::add_button(std::string name)
{
	int code = buttonNameToVal((char*)name.c_str());
	if (code != -1)
	{
		this->buttonCodes.push_back(code);
	}
}

const std::string KeyConfig::KEY_TOGGLE_MAIN_MENU = std::string("toggle_main_menu");
const std::string KeyConfig::KEY_TOGGLE_AIRBRAKE = std::string("toggle_airbrake");

const std::string KeyConfig::KEY_MENU_UP = std::string("menu_up");
const std::string KeyConfig::KEY_MENU_DOWN = std::string("menu_down");
const std::string KeyConfig::KEY_MENU_LEFT = std::string("menu_left");
const std::string KeyConfig::KEY_MENU_RIGHT = std::string("menu_right");
const std::string KeyConfig::KEY_MENU_SELECT = std::string("menu_select");
const std::string KeyConfig::KEY_MENU_BACK = std::string("menu_back");

const std::string KeyConfig::KEY_VEH_BOOST = std::string("veh_boost");
const std::string KeyConfig::KEY_VEH_STOP = std::string("veh_stop");
const std::string KeyConfig::KEY_VEH_ROCKETS = std::string("veh_rockets");

const std::string KeyConfig::KEY_AIRBRAKE_UP = std::string("airbrake_up");
const std::string KeyConfig::KEY_AIRBRAKE_DOWN = std::string("airbrake_down");
const std::string KeyConfig::KEY_AIRBRAKE_FORWARD = std::string("airbrake_forward");
const std::string KeyConfig::KEY_AIRBRAKE_BACK = std::string("airbrake_back");
const std::string KeyConfig::KEY_AIRBRAKE_ROTATE_LEFT = std::string("airbrake_rotate_left");
const std::string KeyConfig::KEY_AIRBRAKE_ROTATE_RIGHT = std::string("airbrake_rotate_right");
const std::string KeyConfig::KEY_AIRBRAKE_SPEED = std::string("airbrake_speed");
const std::string KeyConfig::KEY_AIRBRAKE_FREEZE_TIME = std::string("airbrake_freeze_time");

const std::string KeyConfig::KEY_HOT_AIRBRAKE_THROUGH_DOOR = std::string("hotkey_throughdoor");

const std::string ControllerButtonConfig::CONTROLLER_BTN_X = std::string("BUTTON_X");
const std::string ControllerButtonConfig::CONTROLLER_BTN_Y = std::string("BUTTON_Y");
const std::string ControllerButtonConfig::CONTROLLER_BTN_A = std::string("BUTTON_A");
const std::string ControllerButtonConfig::CONTROLLER_BTN_B = std::string("BUTTON_B");
const std::string ControllerButtonConfig::CONTROLLER_BTN_DPAD_L = std::string("DPAD_LEFT");
const std::string ControllerButtonConfig::CONTROLLER_BTN_DPAD_R = std::string("DPAD_RIGHT");
const std::string ControllerButtonConfig::CONTROLLER_BTN_DPAD_U = std::string("DPAD_UP");
const std::string ControllerButtonConfig::CONTROLLER_BTN_DPAD_D = std::string("DPAD_DOWN");
const std::string ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_L = std::string("L_SHOULDER");
const std::string ControllerButtonConfig::CONTROLLER_BTN_SHOULDER_R = std::string("R_SHOULDER");
const std::string ControllerButtonConfig::CONTROLLER_BTN_TRIGGER_L = std::string("L_TRIGGER");
const std::string ControllerButtonConfig::CONTROLLER_BTN_TRIGGER_R = std::string("R_TRIGGER");
const std::string ControllerButtonConfig::CONTROLLER_BTN_MENU = std::string("MENU");
const std::string ControllerButtonConfig::CONTROLLER_BTN_GUIDE = std::string("GUIDE");
const std::string ControllerButtonConfig::CONTROLLER_LSTICK_L = std::string("L_STICK_LEFT");
const std::string ControllerButtonConfig::CONTROLLER_LSTICK_R = std::string("L_STICK_RIGHT");
const std::string ControllerButtonConfig::CONTROLLER_LSTICK_U = std::string("L_STICK_UP");
const std::string ControllerButtonConfig::CONTROLLER_LSTICK_D = std::string("L_STICK_DOWN");
const std::string ControllerButtonConfig::CONTROLLER_RSTICK_L = std::string("R_STICK_LEFT");
const std::string ControllerButtonConfig::CONTROLLER_RSTICK_R = std::string("R_STICK_RIGHT");
const std::string ControllerButtonConfig::CONTROLLER_RSTICK_U = std::string("R_STICK_UP");
const std::string ControllerButtonConfig::CONTROLLER_RSTICK_D = std::string("R_STICK_DOWN");
const std::string ControllerButtonConfig::CONTROLLER_LSTICK_CLICK = std::string("L_STICK_CLICK");
const std::string ControllerButtonConfig::CONTROLLER_RSTICK_CLICK = std::string("R_STICK_CLICK");