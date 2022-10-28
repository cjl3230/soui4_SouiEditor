﻿#include "stdafx.h"
#include "ResManger.h"
#include "CDebug.h"
#include "helpapi.h"
#include <vector>
#include <algorithm>

ResManger::ResManger()
{
}


ResManger::~ResManger()
{
}

void ResManger::LoadUIResFromFile(SStringT strPath)
{
	m_strProPath = strPath.Mid(0, strPath.ReverseFind(_T('\\')));
	m_strUIResFile = strPath;

	LoadUIRes();
	LoadSkinFile();
	LoadStringFile();
	LoadColorFile();
	LoadStyleFile();
	LoadObjattrFile();
}

void ResManger::ReleaseUIRes()
{
	m_strUIResFile = m_strProPath = L"";

	m_mapResFile.RemoveAll();
	m_mapXmlFile.RemoveAll();
	m_mapSkins.RemoveAll();
	m_mapStrings.RemoveAll();
	m_mapColors.RemoveAll();
	m_mapStyles.RemoveAll();

	m_strSkinFile = L"";
	m_strStringFile = L"";
	m_strColorFile = L"";
	m_strStyleFile = L"";
	m_strObjattrFile = L"";

	m_xmlDocSkin.reset();
	m_xmlDocString.reset();
	m_xmlDocColor.reset();
	m_xmlDocStyle.reset();
	m_xmlDocObjattr.reset();
}

void ResManger::SaveRes()
{
	m_strUIResFile.Trim();
	if (!m_strUIResFile.IsEmpty())
		m_xmlDocUiRes.save_file(m_strUIResFile);
	m_strSkinFile.Trim();
	if (!m_strSkinFile.IsEmpty())
		m_xmlDocSkin.save_file(m_strSkinFile);
}

pugi::xml_node ResManger::GetResFirstNode(const SStringT tagname)
{
	pugi::xml_document * xmlDoc = &m_xmlDocSkin;
	if (tagname == _T("string"))
		xmlDoc = &m_xmlDocString;
	if (tagname == _T("color"))
		xmlDoc = &m_xmlDocColor;
	if (tagname == _T("style"))
		xmlDoc = &m_xmlDocStyle;
	if (tagname == _T("objattr"))
		xmlDoc = &m_xmlDocObjattr;

	pugi::xml_node xmlNode;
	if (xmlDoc->child(tagname))
	{
		xmlNode = xmlDoc->child(tagname).first_child();
	}
	else if (xmlDoc->child(L"UIDEF"))
	{
		xmlNode = xmlDoc->child(L"UIDEF").child(tagname).first_child();
	}
	return xmlNode;
}

void ResManger::LoadUIRes()
{
	if (!m_xmlDocUiRes.load_file(m_strUIResFile, pugi::parse_full))
	{
		CDebug::Debug(_T("加载uires文件失败"));
		return;
	}

	m_xmlNodeUiRes = m_xmlDocUiRes.root();

	m_mapResFile.RemoveAll();
	m_mapXmlFile.RemoveAll();

	pugi::xml_node xmlNode = m_xmlNodeUiRes.child(L"resource").first_child();
	GetSubNodes(xmlNode, L"");
	
/*  // 测试代码
	SPOSITION pos = m_mapXmlFile.GetStartPosition();
	while (pos)
	{
		auto aa = m_mapXmlFile.GetAt(pos);
		m_mapXmlFile.GetNext(pos);
	}
*/

	// 获取 Init.xml 文件名
	pugi::xml_node xmlNode_init = m_xmlNodeUiRes.child(L"resource").child(L"UIDEF").first_child();

	while (xmlNode_init)
	{
		if (xmlNode_init.type() == pugi::node_element)
		{
			break;
		}
		else
		{
			xmlNode_init = xmlNode_init.next_sibling();
		}
	}

	SStringW strPath;
	strPath = xmlNode_init.attribute(L"path").value();
	//while (xmlNode)
	//{
	//	SStringW str(L"XML_INIT");
	//	if (str.CompareNoCase(xmlNode.attribute(L"name").value()) == 0 )
	//	{
	//		strPath = xmlNode.attribute(L"path").value();
	//		break;
	//	}
	//	xmlNode = xmlNode.next_sibling();
	//}

	if (!strPath.IsEmpty())
	{
		m_strInitFile = m_strProPath + _T("\\") + strPath;
	}

	if (xmlNode.attribute(L"skin"))
	{
		m_strSkinFile = xmlNode.attribute(L"path").value();
	}
	if (xmlNode.attribute(L"string"))
	{
		m_strStringFile = xmlNode.attribute(L"path").value();
	}
	if (xmlNode.attribute(L"color"))
	{
		m_strColorFile = xmlNode.attribute(L"path").value();
	}
	if (xmlNode.attribute(L"style"))
	{
		m_strStyleFile = xmlNode.attribute(L"path").value();
	}
	if (xmlNode.attribute(L"objattr"))
	{
		m_strObjattrFile = xmlNode.attribute(L"path").value();
	}
}

void ResManger::GetSubNodes(pugi::xml_node& parentNode, SStringT parentNodeName)
{
	while (parentNode)
	{
		if (parentNode.type() == pugi::node_element)
		{
			SStringT strParentName = parentNode.name();
			if (parentNode.first_child() != NULL)
			{
				GetSubNodes(parentNode.first_child(), strParentName + L":");
			}
			else
			{
				if (strParentName == L"file")
				{
					SStringW strName, strPath;
					strName = parentNode.attribute(L"name").value();
					strPath = parentNode.attribute(L"path").value();
					if (!strName.IsEmpty() && !strPath.IsEmpty())
					{
						SStringT strKey = parentNodeName + strName;
						strKey.MakeLower();
						SStringT extname = GetFileExtname(strPath);
						if (extname.CompareNoCase(_T(".xml")) == 0)
						{
							m_mapXmlFile[strKey] = strPath;
						}
						else
						{
							m_mapResFile[strKey] = strPath;
						}
					}
				}
			}
		}
		parentNode = parentNode.next_sibling();
	}
}

// 删除资源类型名 如LAYOUT:sin_manm 将变成 sin_manm
SStringT ResManger::RemoveResTypename(const SStringT& resname)
{
	int nPos = resname.ReverseFind(':');
	if (nPos == -1)	nPos = 0;
	SStringT name = resname.Mid(nPos + 1);
	name.Trim();
	return name;
}

SStringT ResManger::GetResPathByName(const SStringT& resname)
{
	SStringT key = resname; 
	key.MakeLower();
	const SMap<SStringT, SStringT>::CPair * pFilePair = m_mapXmlFile.Lookup(key);
	if (pFilePair == NULL)
	{
		pFilePair = m_mapResFile.Lookup(key);
	}
	if (pFilePair == NULL)
		return _T("");

	return m_strProPath + _T("\\") + pFilePair->m_value;
}

void ResManger::LoadResFileEx(SStringT& filepath, pugi::xml_document& xmlDoc, SStringT tagname)
{
	if (!m_strInitFile.IsEmpty())
	{
		// 假设在Init.xml文件中定义了Skin
		if (filepath.IsEmpty())
			filepath = m_strInitFile;

		pugi::xml_parse_result result = xmlDoc.load_file(m_strInitFile, pugi::parse_full);
		if (result)
		{
			pugi::xml_node xmlNode1 = xmlDoc.child(L"UIDEF").child(tagname);
			if (xmlNode1.attribute(L"src"))
			{
				SStringT strSrc = xmlNode1.attribute(L"src").value();
				const SMap<SStringT, SStringT>::CPair * pFilePair = m_mapXmlFile.Lookup(strSrc);
				if (pFilePair == NULL)
				{
					SASSERT_FMTW(L"Locating filepath failed, src=%s", (LPCTSTR)strSrc);
					return;
				}
				
				filepath = m_strProPath + _T("\\") + pFilePair->m_value;
				result = xmlDoc.load_file(filepath, pugi::parse_full);
				if (!result)
				{
					SStringT tmpstr;
					tmpstr.Format(_T("加载%s文件失败"), tagname);
					SMessageBox(NULL, _T("Resmgr"), tmpstr, MB_OK);
				}
			}
		}
	}
}

void ResManger::LoadSkinNode(pugi::xml_node xmlNode)
{
	m_mapSkins.RemoveAll();
	while (xmlNode)
	{
		if (xmlNode.type() != pugi::node_element)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		SStringT s1, s2, s3;
		s1 = xmlNode.name();
		s2 = xmlNode.attribute(L"name").value();
		s3 = xmlNode.attribute(L"src").value();

		m_mapSkins[s2] = SkinItem(s1, s2, s3, xmlNode);
		xmlNode = xmlNode.next_sibling();
	}
}

void ResManger::LoadSkinFile()
{
	LoadResFileEx(m_strSkinFile, m_xmlDocSkin, _T("skin"));
	pugi::xml_node xmlNode = GetResFirstNode(_T("skin"));
	LoadSkinNode(xmlNode);
}

void ResManger::LoadStringFile()
{
	LoadResFileEx(m_strStringFile, m_xmlDocString, _T("string"));
	m_mapStrings.RemoveAll();
	pugi::xml_node xmlNode = GetResFirstNode(_T("string"));
	while (xmlNode)
	{
		if (xmlNode.type() != pugi::node_element)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		SStringT s1, s2;
		s1 = xmlNode.name();
		s2 = xmlNode.attribute(L"value").value();

		m_mapStrings[s1] = ValueItem(s1, s2);
		xmlNode = xmlNode.next_sibling();
	}
}

void ResManger::LoadColorFile()
{
	LoadResFileEx(m_strColorFile, m_xmlDocColor, _T("color"));
	m_mapColors.RemoveAll();
	pugi::xml_node xmlNode = GetResFirstNode(_T("color"));
	while (xmlNode)
	{
		if (xmlNode.type() != pugi::node_element)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}

		SStringT s1, s2;
		s1 = xmlNode.name();
		s2 = xmlNode.attribute(L"value").value();

		m_mapColors[s1] = ValueItem(s1, s2);
		xmlNode = xmlNode.next_sibling();
	}
}

void ResManger::LoadStyleFile()
{
	LoadResFileEx(m_strStyleFile, m_xmlDocStyle, _T("style"));
	m_mapStyles.RemoveAll();
	pugi::xml_node xmlNode = GetResFirstNode(_T("style"));
	while (xmlNode)
	{
		if (xmlNode.type() != pugi::node_element)
		{
			xmlNode = xmlNode.next_sibling();
			continue;
		}
		pugi::xml_writer_buff writer;
		xmlNode.print(writer, L"\t", pugi::format_default, pugi::encoding_utf16);
		SStringW strXml = SStringW(writer.buffer(), writer.size());
		SStringW strName = xmlNode.attribute(L"name").value();
		m_mapStyles[strName] = strXml;
		xmlNode = xmlNode.next_sibling();
	}
}

void ResManger::LoadObjattrFile()
{
	LoadResFileEx(m_strObjattrFile, m_xmlDocObjattr, _T("objattr"));
}

ResManger::SkinItem ResManger::GetSkinByName(SStringT skinname)
{
	SPOSITION pos = m_mapSkins.GetStartPosition();
	while (pos)
	{
		const SMap<SStringT, SkinItem>::CPair* item = m_mapSkins.GetAt(pos);
		if (skinname.CompareNoCase(item->m_value.name) == 0)
		{
			return item->m_value;
		}

		m_mapSkins.GetNext(pos);
	}

	return SkinItem();
}

ResManger::SkinItem ResManger::GetSkinByImg(SStringT srcimg)
{
	SPOSITION pos = m_mapSkins.GetStartPosition();
	while (pos)
	{
		const SMap<SStringT, SkinItem>::CPair* item = m_mapSkins.GetAt(pos);
		if (srcimg.CompareNoCase(item->m_value.src) == 0)
		{
			return item->m_value;
		}

		m_mapSkins.GetNext(pos);
	}

	return SkinItem();
}

SStringA ResManger::GetSkinAutos(SStringT prev)
{
	std::vector<SStringT> vecTemp;
	SPOSITION pos = m_mapSkins.GetStartPosition();
	while (pos)
	{
		const SMap<SStringT, SkinItem>::CPair* item = m_mapSkins.GetAt(pos);
		vecTemp.push_back(item->m_key);

		m_mapSkins.GetNext(pos);
	}
	std::sort(vecTemp.begin(), vecTemp.end(), SortSStringNoCase);

	SStringT strAuto;
	std::vector<SStringT>::iterator it = vecTemp.begin();
	for (; it != vecTemp.end(); it++)
	{
		strAuto += prev;
		strAuto += *it + _T(" ");
	}
	strAuto.TrimRight(' ');

	SStringA str = S_CW2A(strAuto, CP_UTF8);
	return str;
}

SStringA ResManger::GetStyleAutos(SStringT prev)
{
	std::vector<SStringW> vecTemp;
	SPOSITION pos = m_mapStyles.GetStartPosition();
	while (pos)
	{
		const SMap<SStringW, SStringW>::CPair* item = m_mapStyles.GetAt(pos);
		vecTemp.push_back(item->m_key);

		m_mapStyles.GetNext(pos);
	}
	std::sort(vecTemp.begin(), vecTemp.end(), SortSStringNoCase);

	SStringW strAuto;
	std::vector<SStringW>::iterator it = vecTemp.begin();
	for (; it != vecTemp.end(); it++)
	{
		strAuto += prev;
		strAuto += *it + L" ";
	}
	strAuto.TrimRight(' ');

	SStringA str = S_CW2A(strAuto, CP_UTF8);
	return str;
}

SStringA ResManger::GetStringAutos(SStringT prev)
{
	std::vector<SStringT> vecTemp;
	SPOSITION pos = m_mapStrings.GetStartPosition();
	while (pos)
	{
		const SMap<SStringT, ValueItem>::CPair* item = m_mapStrings.GetAt(pos);
		vecTemp.push_back(item->m_key);

		m_mapStrings.GetNext(pos);
	}
	std::sort(vecTemp.begin(), vecTemp.end(), SortSStringNoCase);

	SStringT strAuto;
	std::vector<SStringT>::iterator it = vecTemp.begin();
	for (; it != vecTemp.end(); it++)
	{
		strAuto += prev;
		strAuto += *it + _T(" ");
	}
	strAuto.TrimRight(' ');

	SStringA str = S_CW2A(strAuto, CP_UTF8);
	return str;
}

SStringA ResManger::GetColorAutos(SStringT prev)
{
	std::vector<SStringT> vecTemp;
	SPOSITION pos = m_mapColors.GetStartPosition();
	while (pos)
	{
		const SMap<SStringT, ValueItem>::CPair* item = m_mapColors.GetAt(pos);
		vecTemp.push_back(item->m_key);

		m_mapColors.GetNext(pos);
	}
	std::sort(vecTemp.begin(), vecTemp.end(), SortSStringNoCase);

	SStringT strAuto;
	std::vector<SStringT>::iterator it = vecTemp.begin();
	for (; it != vecTemp.end(); it++)
	{
		strAuto += prev;
		strAuto += *it + _T(" ");
	}
	strAuto.TrimRight(' ');

	SStringA str = S_CW2A(strAuto, CP_UTF8);
	return str;
}

void ResManger::onXmlSave(LPCTSTR pszFileName)
{
	if(m_strObjattrFile == pszFileName)
	{
		LoadObjattrFile();
	}else if(m_strColorFile == pszFileName)
	{
		LoadColorFile();
	}else if(m_strSkinFile == pszFileName)
	{
		LoadSkinFile();
	}else if(m_strStringFile == pszFileName)
	{
		LoadStringFile();
	}else if(m_strStyleFile == pszFileName)
	{
		LoadStyleFile();
	}
}

SOUI::SStringT ResManger::GetSkinXmlName() const
{
	return m_strSkinFile.Right(m_strSkinFile.GetLength()-m_strProPath.GetLength()-1);
}
