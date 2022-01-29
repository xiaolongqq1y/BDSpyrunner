﻿#include "PyEntity.h"
#include "Tool.h"
#include "NBT.h"

#define Py_GET_PLAYER  Py_GET_PLAYER2(nullptr)
#define Py_GET_PLAYER2(ret) Player* p = getPlayer(self);if (p == nullptr)return ret
#define Py_GET_ACTOR Py_GET_ACTOR2(nullptr)
#define Py_GET_ACTOR2(ret) Actor* a = getActor(self);if (a == nullptr)return ret

using namespace std;
struct PyEntity {
	PyObject_HEAD;
	Actor* value;

	static Actor* getActor(PyObject* self) {
		if (reinterpret_cast<PyEntity*>(self)->value)
			return reinterpret_cast<PyEntity*>(self)->value;
		else
			Py_RETURN_ERROR("This entity pointer is nullptr");
	}
	static Player* getPlayer(PyObject* self) {
		if (IsPlayer(reinterpret_cast<PyEntity*>(self)->value))
			return reinterpret_cast<Player*>(reinterpret_cast<PyEntity*>(self)->value);
		else
			Py_RETURN_ERROR("This entity pointer is nullptr or is not player pointer");
	}
	static int print(PyObject* self, FILE* file, int) {
		Py_GET_ACTOR2(-1);
		fputs(a->getNameTag().c_str(), file);
		return 0;
	}
	static PyObject* repr(PyObject* self) {
		Py_GET_ACTOR2(StrToPyUnicode(""));
		return StrToPyUnicode(a->getNameTag());
	}
	static Py_hash_t hash(PyObject* self) {
		return reinterpret_cast<Py_hash_t>(self);
	}
	static PyObject* str(PyObject* self) {
		Py_GET_ACTOR2(StrToPyUnicode(""));
		return StrToPyUnicode(a->getNameTag());
	}
	static PyObject* rich_compare(PyObject* self, PyObject* other, int op) {
		switch (op) {
			//<
		case Py_LT:break;
			//<=
		case Py_LE:break;
			//==
		case Py_EQ:
			if (getActor(self) == getActor(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//!=
		case Py_NE:
			if (getActor(self) != getActor(other))
				Py_RETURN_TRUE;
			else
				Py_RETURN_FALSE;
			break;
			//>
		case Py_GT:break;
			//>=
		case Py_GE:break;
		}
		Py_RETURN_NOTIMPLEMENTED;
	}

	//获取名字
	Py_METHOD_DEFINE(getName) {
		Py_GET_ACTOR;
		return StrToPyUnicode(a->getNameTag());
	}
	Py_METHOD_DEFINE(setName) {
		const char* name = "";
		Py_PARSE("s", &name);
		Py_GET_PLAYER;
		p->setName(name);
	}
	//获取UUID
	Py_METHOD_DEFINE(getUuid) {
		Py_GET_PLAYER;
		return StrToPyUnicode(p->getUuid());
	}
	//获取XUID
	Py_METHOD_DEFINE(getXuid) {
		Py_GET_PLAYER;
		return StrToPyUnicode(p->getXuid());
	}
	//获取坐标
	Py_METHOD_DEFINE(getPos) {
		Py_GET_ACTOR;
		return ToList(a->getPos());
	}
	//获取维度ID
	Py_METHOD_DEFINE(getDimensionId) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getDimensionId());
	}
	//是否着地
	Py_METHOD_DEFINE(isStanding) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isStanding());
	}
	//是否潜行
	Py_METHOD_DEFINE(isSneaking) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isSneaking());
	}
	//是否坐下
	Py_METHOD_DEFINE(isSitting) {
		Py_GET_ACTOR;
		return PyBool_FromLong(a->isSitting());
	}
	//获取类型
	Py_METHOD_DEFINE(getTypeID) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getEntityTypeId());
	}
	//获取类型字符串
	Py_METHOD_DEFINE(getTypeName) {
		Py_GET_ACTOR;
		//旧办法
		//string type;
		//SymCall<string&>("?EntityTypeToString@@YA?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@W4ActorType@@W4ActorTypeNamespaceRules@@@Z",
		//	&type, a->getEntityTypeId());
		return StrToPyUnicode(a->getTypeName());
	}
	//获取nbt数据
	Py_METHOD_DEFINE(getNBT) {
		Py_GET_ACTOR;
		auto t = CompoundTag::create();
		a->save(*t);
		return StrToPyUnicode(StrToJson(*t).dump(4));
	}
	//获取生命值
	Py_METHOD_DEFINE(getHealth) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getHealth());
	}
	//获取最大生命值
	Py_METHOD_DEFINE(getMaxHealth) {
		Py_GET_ACTOR;
		return PyLong_FromLong(a->getMaxHealth());
	}
	//获取权限
	Py_METHOD_DEFINE(getPermissions) {
		Py_GET_PLAYER;
		return PyLong_FromLong(static_cast<int>(p->getPlayerPermissionLevel()));
	}
	Py_METHOD_DEFINE(setPermissions) {
		int perm;
		Py_PARSE("i", &perm);
		Py_GET_PLAYER;
		p->setPermissions(static_cast<CommandPermissionLevel>(perm));
		Py_RETURN_NONE;
	}
	//获取设备id
	Py_METHOD_DEFINE(getPlatformOnlineId) {
		Py_GET_PLAYER;
		return StrToPyUnicode(p->getPlatformOnlineId());
	}
	//获取设备类型
	Py_METHOD_DEFINE(getPlatform) {
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getPlatform());
	}
	//获取IP
	Py_METHOD_DEFINE(getIP) {
		Py_GET_PLAYER;
		auto ni = p->getNetworkIdentifier();
		return StrToPyUnicode(Global<RakNet::RakPeer>->getAdr(*ni).ToString(false, ':'));
	}

	//获取玩家所有物品
	Py_METHOD_DEFINE(getAllItem) {
		Py_GET_PLAYER;
		fifo_json items_json = fifo_json::object();
		fifo_json& inventory = items_json["Inventory"];
		for (auto& i : p->getInventory().getSlots()) {
			inventory.push_back(StrToJson(*i->save()));
		}
		fifo_json& endchest = items_json["EndChest"];
		for (auto& i : p->getEnderChestContainer()->getSlots()) {
			endchest.push_back(StrToJson(*i->save()));
		}
		fifo_json& armor = items_json["Armor"];
		for (auto& i : p->getArmorContainer().getSlots()) {
			armor.push_back(StrToJson(*i->save()));
		}
		items_json["OffHand"] = StrToJson(*p->getOffhandSlot().save());
		items_json["Hand"] = StrToJson(*p->getSelectedItem().save());
		return StrToPyUnicode(items_json.dump(4));
	}
	//设置玩家所有物品
	Py_METHOD_DEFINE(setAllItem) {
		const char* items_data = "";
		Py_PARSE("s", &items_data);
		Py_GET_PLAYER;
		fifo_json items_json(StrToJson(items_data));
		if (items_json.contains("Inventory")) {
			auto& items = p->getInventory();
			fifo_json& inventory = items_json["Inventory"];
			for (unsigned i = 0; i < inventory.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(inventory[i]);
			}
		}
		if (items_json.contains("EndChest")) {
			auto items = p->getEnderChestContainer();
			fifo_json& endchest = items_json["EndChest"];
			for (unsigned i = 0; i < endchest.size(); i++) {
				*items->getSlot(i) = LoadItemFromJson(endchest[i]);
			}
		}
		if (items_json.contains("Armor")) {
			auto& items = p->getArmorContainer();
			fifo_json& armor = items_json["Armor"];
			for (unsigned i = 0; i < armor.size(); i++) {
				*items.getSlot(i) = LoadItemFromJson(armor[i]);
			}
		}
		if (items_json.contains("OffHand")) {
			p->setOffhandSlot(LoadItemFromJson(items_json["OffHand"]));
		}
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//设置玩家手上物品
	Py_METHOD_DEFINE(setHand) {
		const char* item_data = "";
		Py_PARSE("s", &item_data);
		Py_GET_PLAYER;
		const_cast<ItemStack&>(p->getSelectedItem()) = LoadItemFromString(item_data);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//增加玩家背包物品
	Py_METHOD_DEFINE(addItem) {
		const char* item_data = "";
		Py_PARSE("s", &item_data);
		Py_GET_PLAYER;
		auto item = LoadItemFromString(item_data);
		p->giveItem(&item);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//移除玩家背包物品
	Py_METHOD_DEFINE(removeItem) {
		int slot, num;
		Py_PARSE("ii", &slot, &num);
		Py_GET_PLAYER;
		p->getInventory().removeItem(slot, num);
		p->sendInventory(true);
		Py_RETURN_NONE;
	}
	//传送
	Py_METHOD_DEFINE(teleport) {
		Vec3 pos;
		int did;
		Py_PARSE("fffi", &pos.x, &pos.y, &pos.z, &did);
		Py_GET_PLAYER;
		p->teleport(pos, did);
		Py_RETURN_NONE;
	}
	//发送数据包
	Py_METHOD_DEFINE(sendTextPacket) {
		const char* msg = "";
		int mode = 0;
		Py_PARSE("s|i", &msg, &mode);
		Py_GET_PLAYER;
		p->sendTextPacket(msg, TextType(mode));
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(sendCommandPacket) {
		const char* cmd = "";
		Py_PARSE("s", &cmd);
		Py_GET_PLAYER;
		p->sendCommandRequestPacket(cmd);
		Py_RETURN_NONE;
	}
	//重新发送所有区块
	Py_METHOD_DEFINE(resendAllChunks) {
		Py_GET_PLAYER;
		p->resendAllChunks();
		Py_RETURN_NONE;
	}
	//断开连接
	Py_METHOD_DEFINE(disconnect) {
		const char* msg = "";
		Py_PARSE("|s", &msg);
		Py_GET_PLAYER;
		p->kick(msg);
		Py_RETURN_NONE;
	}
	//获取玩家分数
	Py_METHOD_DEFINE(getScore) {
		const char* objname = "";
		Py_PARSE("s", &objname);
		Py_GET_PLAYER;
		return PyLong_FromLong(p->getScore(objname));
	}
	//设置玩家分数
	Py_METHOD_DEFINE(setScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->setScore(objname, count);
		Py_RETURN_NONE;
	}
	//增加玩家分数
	Py_METHOD_DEFINE(addScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->addScore(objname, count);
		Py_RETURN_NONE;
	}
	//减少玩家分数
	Py_METHOD_DEFINE(reduceScore) {
		const char* objname = "";
		int count;
		Py_PARSE("si", &objname, &count);
		Py_GET_PLAYER;
		p->reduceScore(objname, count);
		Py_RETURN_NONE;
	}
	//增加等级
	Py_METHOD_DEFINE(addLevel) {
		int level;
		Py_PARSE("i", &level);
		Py_GET_PLAYER;
		p->addLevels(level);
		Py_RETURN_NONE;
	}
	//跨服传送
	Py_METHOD_DEFINE(transferServer) {
		const char* address = "";
		unsigned short port;
		Py_PARSE("sH", &address, &port);
		Py_GET_PLAYER;
		p->sendTransferPacket(address, port);
		Py_RETURN_NONE;
	}
	//发送表单
	Py_METHOD_DEFINE(sendCustomForm) {
		const char* str = "";
		PyObject* callback = nullptr;
		Py_PARSE("sO", &str, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendCustomFormPacket(str,
			[p, callback](string arg) {
				PyGILGuard gil;
				PyObject* result = PyObject_CallFunction(callback, "Os", ToPyEntity(p), arg.c_str());
				PrintPythonError();
				Py_XDECREF(result);
			}
		);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(sendSimpleForm) {
		const char* title = "";
		const char* content = "";
		PyObject* buttons_list = nullptr;
		PyObject* images_list = nullptr;
		PyObject* callback = nullptr;
		Py_PARSE("ssOOO", &title, &content, &buttons_list, &images_list, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		auto buttons = ToStrArray(buttons_list);
		auto images = ToStrArray(images_list);
		if (buttons.size() != images.size())
			Py_RETURN_ERROR("The number of buttons is not equal to the number of images");
		p->sendSimpleFormPacket(title, content, buttons, images,
			[p, callback](int arg) {
				PyGILGuard gil;
				PyObject* result = PyObject_CallFunction(callback, "Oi", ToPyEntity(p), arg);
				PrintPythonError();
				Py_XDECREF(result);
		}
		);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(sendModalForm) {
		const char* title = "";
		const char* content = "";
		const char* button1 = "";
		const char* button2 = "";
		PyObject* callback = nullptr;
		Py_PARSE("ssssO", &title, &content, &button1, &button2, &callback);
		Py_GET_PLAYER;
		if (!PyFunction_Check(callback))
			return nullptr;
		p->sendModalFormPacket(title, content, button1, button2,
			[p, callback](bool arg) {
				PyGILGuard gil;
				PyObject* result = PyObject_CallFunction(callback, "OO", ToPyEntity(p), arg ? Py_True : Py_False);
				PrintPythonError();
				Py_XDECREF(result);
			}
		);
		Py_RETURN_NONE;
	}
	//设置侧边栏
	Py_METHOD_DEFINE(setSidebar) {
		const char* title = "";
		const char* side_data = "";
		ObjectiveSortOrder order = ObjectiveSortOrder::Ascending;
		Py_PARSE("ss|i", &title, &side_data, &order);
		Py_GET_PLAYER;
		vector<pair<string, int>> data;
		fifo_json value = StrToJson(side_data);
		if (value.is_object())
			for (auto& [key, val] : value.items()) {
				data.push_back({ key, val });
			}
		p->setSidebar(title, data, order);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeSidebar) {
		Py_GET_PLAYER;
		p->removeSidebar();
		Py_RETURN_NONE;
	}
	//Boss栏
	Py_METHOD_DEFINE(setBossbar) {
		const char* name = "";
		float per;
		Py_PARSE("sf", &name, &per);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Show, name, per, BossEventColour::Red); // Todo
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeBossbar) {
		const char* name = "";
		Py_PARSE("s:removeBossbar", &name);
		Py_GET_PLAYER;
		p->sendBossEventPacket(BossEvent::Hide, name, 0, BossEventColour::Red); // Todo
		Py_RETURN_NONE;
	}
	//标签
	Py_METHOD_DEFINE(addTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Py_GET_ACTOR;
		a->addTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(removeTag) {
		const char* tag = "";
		Py_PARSE("s", &tag);
		Py_GET_ACTOR;
		a->removeTag(tag);
		Py_RETURN_NONE;
	}
	Py_METHOD_DEFINE(getTags) {
		Py_GET_ACTOR;
		auto tags = a->getTags();
		PyObject* list = PyList_New(0);
		for (size_t i = 0; i < tags.size(); i++) {
			PyList_Append(list, StrToPyUnicode(tags[i]));
		}
		return list;
	}
	//杀死实体
	Py_METHOD_DEFINE(kill) {
		Py_GET_ACTOR;
		a->kill();
		Py_RETURN_NONE;
	}

	inline static PyMethodDef Methods[]{
		Py_METHOD_VARARGS(getName),
		Py_METHOD_VARARGS(setName),
		Py_METHOD_VARARGS(getUuid),
		Py_METHOD_VARARGS(getXuid),
		Py_METHOD_VARARGS(getPos),
		Py_METHOD_VARARGS(getDimensionId),
		Py_METHOD_VARARGS(isStanding),
		Py_METHOD_VARARGS(isSneaking),
		Py_METHOD_VARARGS(getTypeID),
		Py_METHOD_VARARGS(getTypeName),
		Py_METHOD_VARARGS(getNBT),
		Py_METHOD_VARARGS(getHealth),
		Py_METHOD_VARARGS(getMaxHealth),
		Py_METHOD_VARARGS(getPermissions),
		Py_METHOD_VARARGS(setPermissions),
		Py_METHOD_VARARGS(getPlatformOnlineId),
		Py_METHOD_VARARGS(getPlatform),
		Py_METHOD_VARARGS(getIP),
		Py_METHOD_VARARGS(getAllItem),
		Py_METHOD_VARARGS(setAllItem),
		Py_METHOD_VARARGS(setHand),
		Py_METHOD_VARARGS(addItem),
		{ "addItem", addItem, METH_VARARGS, nullptr },
		{ "removeItem", removeItem, METH_VARARGS, nullptr },
		{ "teleport", teleport, METH_VARARGS, nullptr },
		{ "sendTextPacket", sendTextPacket, METH_VARARGS, nullptr },
		{ "sendCommandPacket", sendCommandPacket, METH_VARARGS, nullptr },
		{ "resendAllChunks", resendAllChunks, METH_NOARGS, nullptr },
		{ "disconnect", disconnect, METH_VARARGS, nullptr },
		{ "getScore", getScore, METH_VARARGS, nullptr },
		{ "setScore", setScore, METH_VARARGS, nullptr },
		{ "addScore", addScore, METH_VARARGS, nullptr },
		{ "reduceScore", reduceScore, METH_VARARGS, nullptr },
		{ "addLevel", addLevel, METH_VARARGS, nullptr },
		{ "transferServer", transferServer, METH_VARARGS, nullptr },
		{ "sendCustomForm", sendCustomForm, METH_VARARGS, nullptr },
		{ "sendSimpleForm", sendSimpleForm, METH_VARARGS, nullptr },
		{ "sendModalForm", sendModalForm, METH_VARARGS, nullptr },
		{ "setSidebar", setSidebar, METH_VARARGS, nullptr },
		{ "removeSidebar", removeSidebar, METH_NOARGS, nullptr },
		{ "setBossbar", setBossbar, METH_VARARGS, nullptr },
		{ "removeBossbar", removeBossbar, METH_NOARGS, nullptr },
		{ "addTag", addTag, METH_VARARGS, nullptr },
		{ "removeTag", removeTag, METH_VARARGS, nullptr },
		{ "getTags", getTags, METH_NOARGS, nullptr },
		{ "kill", kill, METH_NOARGS, nullptr },
		{ nullptr }
	};
};
//Entity类型
PyTypeObject PyEntity_Type{
	PyVarObject_HEAD_INIT(nullptr, 0)
	"Entity",				/* tp_name */
	sizeof(PyEntity),		/* tp_basicsize */
	0,						/* tp_itemsize */
	nullptr,				/* tp_dealloc */
	PyEntity::print,		/* tp_print */
	nullptr,				/* tp_getattr */
	nullptr,				/* tp_setattr */
	nullptr,				/* tp_reserved */
	PyEntity::repr,			/* tp_repr */
	nullptr,				/* tp_as_number */
	nullptr,				/* tp_as_sequence */
	nullptr,				/* tp_as_mapping */
	PyEntity::hash,			/* tp_hash */
	nullptr,				/* tp_call */
	PyEntity::str,			/* tp_str */
	nullptr,				/* tp_getattro */
	nullptr,				/* tp_setattro */
	nullptr,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	"Entities in Minecraft",/* tp_doc */
	nullptr,				/* tp_traverse */
	nullptr,				/* tp_clear */
	PyEntity::rich_compare,	/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	nullptr,				/* tp_iter */
	nullptr,				/* tp_iternext */
	PyEntity::Methods,		/* tp_methods */
	nullptr,				/* tp_members */
	nullptr,				/* tp_getset */
	nullptr,				/* tp_base */
	nullptr,				/* tp_dict */
	nullptr,				/* tp_descr_get */
	nullptr,				/* tp_descr_set */
	0,						/* tp_dictoffset */
	nullptr,				/* tp_init */
	nullptr,				/* tp_alloc */
	nullptr,				/* tp_new */
	nullptr,				/* tp_free */
	nullptr,				/* tp_is_gc */
	nullptr,				/* tp_bases */
	nullptr,				/* tp_mro */
	nullptr,				/* tp_cache */
	nullptr,				/* tp_subclasses */
	nullptr,				/* tp_weaklist */
	nullptr,				/* tp_del */
	0,						/* tp_version_tag */
	nullptr,				/* tp_finalize */
};

PyObject* ToPyEntity(Actor* ptr) {
	PyEntity* obj = PyObject_New(PyEntity, &PyEntity_Type);
	obj->value = ptr;
	return reinterpret_cast<PyObject*>(obj);
}
