﻿#include "stdafx.h"
#include "SysdataMgr.h"
#include "CDebug.h"
#include <vector>
#include <algorithm>
#include "helpapi.h"
#include "Global.h"
#include <algorithm>

template<>
CSysDataMgr * SSingleton<CSysDataMgr>::ms_Singleton = NULL;

CSysDataMgr::CSysDataMgr()
{
}


CSysDataMgr::~CSysDataMgr()
{
}

bool CSysDataMgr::LoadSysData(LPCTSTR cfgDir)
{
	m_strConfigDir = cfgDir;
	InitProperty();
	InitCtrlDef();
	InitSkinProp();
	return true;
}

void CSysDataMgr::InitProperty()   //初始化属性列表
{
	/*

	<通用样式>
	<id style="proptext" name ="窗口ID(id)" value="" />
	<name style="proptext" name ="窗口名称(name)" value="" />
	<skin style="proptext" name ="皮肤(skin)" value="" />
	</通用样式>

	<Button>
	<分组 name="基本">
	<id/>
	<name/>
	<skin/>
	<pos/>
	<size/>
	<offset/>
	</分组>

	<分组 name="拓展">
	<accel style="proptext" name ="快捷键(accel)" value="ctrl+alt+f9" />
	<animate style="propoption" name ="动画(animate)" value="0" options="无(0)|有(1)"/>
	</分组>

	</Button>
	*/


	pugi::xml_parse_result result = m_xmlDocProperty.load_file(m_strConfigDir + L"\\property.xml");
	if (!result)
	{
		CDebug::Debug(_T("InitProperty失败"));
	}

	pugi::xml_document xmlDocProperty;
	xmlDocProperty.append_copy(m_xmlDocProperty.document_element());
	pugi::xml_node NodeCom = xmlDocProperty.child(L"root").child(L"通用样式");
	pugi::xml_node NodeCtrlList = xmlDocProperty.child(L"root").child(L"属性列表");

	pugi::xml_node NodeCtrl = NodeCtrlList.first_child();  //NodeCtrl = Button节点
	while (NodeCtrl)
	{
		if (pugi::node_element == NodeCtrl.type())
		{
			SStringT strCtrlname = NodeCtrl.name();
			m_mapControl.SetAt(strCtrlname, new CTRL_ATTR_VALUE());

			InitCtrlProperty(NodeCom, NodeCtrl, m_mapControl[strCtrlname]);
		}

		NodeCtrl = NodeCtrl.next_sibling();
	}

	pugi::xml_node NodeComStyle = xmlDocProperty.child(L"root").child(L"基本样式");
	InitComAttr(NodeCom, NodeComStyle, m_arrControlStyle);
	pugi::xml_node NodeCM = xmlDocProperty.child(L"root").child(L"ColorMask");
	InitComAttr(NodeCom, NodeCM, m_arrColorMask);
}

void CSysDataMgr::InitComAttr(pugi::xml_node NodeCom, pugi::xml_node cNode, CTRL_ATTR_VALUE& arrControlStyle)
{
	pugi::xml_node NodeBasicStyle = cNode.first_child();  //NodeCtrl = Button节点
	while (NodeBasicStyle)
	{
		if (pugi::node_element != NodeBasicStyle.type())
		{
			NodeBasicStyle = NodeBasicStyle.next_sibling();
			continue;
		}
		if (!NodeBasicStyle.attribute(L"style"))
		{
			// 没有设置style的为通用属性, 从通用属性结点中获取信息
			SStringT strName = NodeBasicStyle.name();
			pugi::xml_node N = NodeCom.child(strName);
			if (N)
			{	// 用通用属性进行替换
				pugi::xml_node NodeNew;
				NodeNew = NodeBasicStyle.parent().insert_copy_before(N, NodeBasicStyle);
				NodeBasicStyle.parent().remove_child(NodeBasicStyle);

				NodeBasicStyle = NodeNew;
			}
			else
			{
				NodeBasicStyle.append_attribute(L"style").set_value(L"proptext");
				NodeBasicStyle.append_attribute(L"name").set_value(strName);
			}
		}

		arrControlStyle.Add(CtrlAttrItem(NodeBasicStyle.name(), NodeBasicStyle));

		NodeBasicStyle = NodeBasicStyle.next_sibling();
	}
}

void CSysDataMgr::InitCtrlProperty(pugi::xml_node NodeCom, pugi::xml_node NodeCtrl, CTRL_ATTR_VALUE* arr_attr)
{
	/*
	<通用样式>
	<id style="proptext" name ="窗口ID(id)" value="" />
	<name style="proptext" name ="窗口名称(name)" value="" />
	<skin style="proptext" name ="皮肤(skin)" value="" />
	</通用样式>

	<Button>
	<分组 name="基本">
	<id/>
	<name/>
	<skin/>
	<pos/>
	<size/>
	<offset/>
	</分组>

	<分组 name="拓展">
	<accel style="proptext" name ="快捷键(accel)" value="ctrl+alt+f9" />
	<animate style="propoption" name ="动画(animate)" value="0" options="无(0)|有(1)"/>
	</分组>

	</Button>

	<propgroup name="group1" description="desc of group1">
	<proptext name="text1.1" value="value 1.1">

	*/

	pugi::xml_node NodeChild = NodeCtrl.first_child();

	while (NodeChild)
	{
		if (pugi::node_element != NodeChild.type())
		{
			NodeChild = NodeChild.next_sibling();
			continue;
		}
		SStringT nodeName = NodeChild.name();
		if (_wcsicmp(nodeName, L"分组") == 0)
		{
			SStringT nameAttr = NodeChild.attribute(L"name").as_string();
			if (nameAttr.CompareNoCase(L"基本样式") == 0)
			{
				pugi::xml_document NodeComStyle;
				NodeComStyle.append_copy(m_xmlDocProperty.child(L"root").child(nameAttr));
				pugi::xml_node parentNode = NodeChild.parent();
				pugi::xml_node nodeCopy = parentNode.insert_copy_after(NodeComStyle.document_element(), NodeChild);
				parentNode.remove_child(NodeChild);
				NodeChild = nodeCopy;
			}
			else if (nameAttr.CompareNoCase(L"ColorMask") == 0)
			{
				pugi::xml_document NodeComStyle;
				NodeComStyle.append_copy(m_xmlDocProperty.child(L"root").child(nameAttr));
				pugi::xml_node parentNode = NodeChild.parent();
				pugi::xml_node nodeCopy = parentNode.insert_copy_after(NodeComStyle.document_element(), NodeChild);
				parentNode.remove_child(NodeChild);
				NodeChild = nodeCopy;
			}

			NodeChild.set_name(L"propgroup");
			InitCtrlProperty(NodeCom, NodeChild, arr_attr);
		}
		else
		{
			if (!NodeChild.attribute(L"style"))
			{
				// 没有设置style的为通用属性, 从通用属性结点中获取信息
				SStringT strName = NodeChild.name();
				pugi::xml_node N = NodeCom.child(strName);
				pugi::xml_node NodeNew;

				if (N)
				{	// 用通用属性进行替换
					pugi::xml_node NodeNew;
					NodeNew = NodeChild.parent().insert_copy_before(N, NodeChild);
					NodeChild.parent().remove_child(NodeChild);
					NodeChild = NodeNew;
				}
				else
				{
					NodeChild.append_attribute(L"style").set_value(L"proptext");
					NodeChild.append_attribute(L"name").set_value(strName);
				}
			}

			arr_attr->Add(CtrlAttrItem(NodeChild.name(), NodeChild));
		}

		NodeChild = NodeChild.next_sibling();
	}
}

SStringA CSysDataMgr::GetCtrlAutos()
{
	SStringT strAuto;
	pugi::xml_node xmlCtrl = getCtrlDefNode().child(L"controls").first_child();
	while(xmlCtrl)
	{
		strAuto += xmlCtrl.name();
		strAuto += _T(' ');
		xmlCtrl = xmlCtrl.next_sibling();
	}
	return S_CW2A(strAuto, CP_UTF8);
}

SStringA CSysDataMgr::GetCtrlAttrAutos(SStringT ctrlname)
{
	ctrlname.MakeLower();
	if (ctrlname.CompareNoCase(_T("root")) == 0)
		ctrlname = _T("window");

	SMap<SStringT, CTRL_ATTR_VALUE*>::CPair* pNode = m_mapControl.Lookup(ctrlname);
	if (!pNode)
	{
		pNode = m_mapControl.Lookup(_T("window"));
	}
	if (!pNode)
		return "";

	SStringT strAuto;
	CTRL_ATTR_VALUE* ctrl_attr = pNode->m_value;
	SArray<CtrlAttrItem> allAttr;
	allAttr.Append(*ctrl_attr);
	allAttr.Append(m_arrControlStyle);
	allAttr.Append(m_arrColorMask);

	SStringT strLastWord;
	qsort(allAttr.GetData(), allAttr.GetCount(), sizeof(CtrlAttrItem), CtrlAttrCmpNoCase);
	for (int i = 0; i < allAttr.GetCount(); i++)
	{
		if (allAttr.GetAt(i).attrname.CompareNoCase(uiedit_SpecAttr) == 0)
			continue;
		if (strLastWord != allAttr.GetAt(i).attrname)
		{
			strLastWord = allAttr.GetAt(i).attrname;
			strAuto += strLastWord + _T(" ");
		}
	}

	strAuto.TrimRight(' ');
	SStringA str = S_CW2A(strAuto, CP_UTF8);
	return str;
}

void CSysDataMgr::InitCtrlDef()
{
	pugi::xml_parse_result result = m_xmlCtrlDef.load_file(m_strConfigDir + L"\\ctrl.xml");
}

pugi::xml_node CSysDataMgr::getCtrlDefNode()
{
	return m_xmlCtrlDef.child(L"root");
}

void CSysDataMgr::InitSkinProp()
{
	m_xmlSkinProp.load_file(m_strConfigDir + L"\\SkinProperty.xml");
}

pugi::xml_node CSysDataMgr::getSkinPropNode()
{
	return m_xmlSkinProp.root().child(L"root");
}

SOUI::SStringT CSysDataMgr::GetConfigDir() const
{
	return m_strConfigDir;
}

SOUI::SStringA CSysDataMgr::GetSkinAutos()
{
	std::set<SStringW> skins;
	pugi::xml_node xmlSkin = getSkinPropNode().child(L"skins").first_child();
	while(xmlSkin)
	{
		if(xmlSkin.attribute(L"visible").as_bool(true))
		{
			skins.insert(xmlSkin.name());
		}
		xmlSkin = xmlSkin.next_sibling();
	}
	SStringW strAuto;
	std::set<SStringW>::iterator it = skins.begin();
	while(it!=skins.end())
	{
		strAuto += *it + L" ";
		it++;
	}
	strAuto.TrimRight();
	return S_CW2A(strAuto,CP_UTF8);
}

SOUI::SStringA CSysDataMgr::GetSkinAttrAutos(SStringW skinName)
{
	std::set<SStringW> attrs;
	_GetSkinAttrs(skinName,attrs);
	SStringW strAuto;
	std::set<SStringW>::iterator it = attrs.begin();
	while(it!=attrs.end())
	{
		strAuto += *it + L" ";
		it++;
	}
	strAuto.TrimRight();
	return S_CW2A(strAuto,CP_UTF8);
}

void CSysDataMgr::_GetSkinAttrs(SStringW skinName,std::set<SStringW> &attrs)
{
	pugi::xml_node xmlSkin = getSkinPropNode().child(L"skins").child(skinName);
	if(xmlSkin)
	{
		pugi::xml_node skinAttr = xmlSkin.child(L"groups").child(L"propgroup").first_child();
		while(skinAttr)
		{
			if(skinAttr.attribute(L"name"))
			{
				attrs.insert(skinAttr.attribute(L"name").as_string());
			}
			skinAttr = skinAttr.next_sibling();
		}
		if(xmlSkin.attribute(L"parent"))
		{
			_GetSkinAttrs(xmlSkin.attribute(L"parent").as_string(),attrs);
		}
	}
}
