/******************************************************************
 *
 * Project: Explorer++
 * File: OptionsDialog.cpp
 * License: GPL - See COPYING in the top level directory
 *
 * Handles the 'Options' dialog and all associated messages.
 *
 * Notes:
 *  - Center dialog. Don't remember previous position.
 *  - The apply button is called for each dialog that
 *    has been initialised (i.e. any dialog that has
 *    had the focus set to itself).
 *
 * Written by David Erceg
 * www.explorerplusplus.com
 *
 *****************************************************************/

#include "stdafx.h"
#include "Explorer++.h"
#include "Misc.h"


#define NUM_DIALOG_OPTIONS_PAGES	5

int CALLBACK PropSheetProcStub(HWND hDlg,UINT msg,LPARAM lParam);
int CALLBACK NewTabDirectoryBrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);

BOOL bRefreshAllTabs;

static HWND g_hOptionsPropertyDialog	= NULL;

HICON g_hNewTabDirIcon;
TCHAR g_szNewTabDirectory[MAX_PATH];

void CContainer::OnShowOptions(void)
{
	PROPSHEETPAGE	psp[NUM_DIALOG_OPTIONS_PAGES];
	HPROPSHEETPAGE	hpsp[NUM_DIALOG_OPTIONS_PAGES];
	PROPSHEETHEADER	psh;
	HIMAGELIST		himl;
	HBITMAP			hBitmap;
	TCHAR			szTitle[64];
	unsigned int	nSheet = 0;

	himl = ImageList_Create(16,16,ILC_COLOR32|ILC_MASK,0,48);

	/* Contains all images used on the menus. */
	hBitmap = LoadBitmap(GetModuleHandle(0),MAKEINTRESOURCE(IDB_SHELLIMAGES));

	ImageList_Add(himl,hBitmap,NULL);

	g_hNewTabDirIcon = ImageList_GetIcon(himl,SHELLIMAGES_NEWTAB,ILD_NORMAL);

	DeleteObject(hBitmap);
	ImageList_Destroy(himl);

	/* General options page. */
	psp[nSheet].dwSize		= sizeof(PROPSHEETPAGE);
	psp[nSheet].dwFlags		= PSP_DEFAULT;
	psp[nSheet].hInstance	= g_hLanguageModule;
	psp[nSheet].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS_GENERAL);
	psp[nSheet].lParam		= (LPARAM)this;
	psp[nSheet].pfnDlgProc	= GeneralSettingsProcStub;

	hpsp[nSheet] = CreatePropertySheetPage(&psp[nSheet]);
	nSheet++;

	/* Files and Folders options page. */
	psp[nSheet].dwSize		= sizeof(PROPSHEETPAGE);
	psp[nSheet].dwFlags		= PSP_DEFAULT;
	psp[nSheet].hInstance	= g_hLanguageModule;
	psp[nSheet].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS_FILESFOLDERS);
	psp[nSheet].lParam		= (LPARAM)this;
	psp[nSheet].pfnDlgProc	= FilesFoldersProcStub;

	hpsp[nSheet] = CreatePropertySheetPage(&psp[nSheet]);
	nSheet++;

	/* Window options page. */
	psp[nSheet].dwSize		= sizeof(PROPSHEETPAGE);
	psp[nSheet].dwFlags		= PSP_DEFAULT;
	psp[nSheet].hInstance	= g_hLanguageModule;
	psp[nSheet].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS_WINDOW);
	psp[nSheet].lParam		= (LPARAM)this;
	psp[nSheet].pfnDlgProc	= WindowProcStub;

	hpsp[nSheet] = CreatePropertySheetPage(&psp[nSheet]);
	nSheet++;

	/* Default settings options page. */
	psp[nSheet].dwSize		= sizeof(PROPSHEETPAGE);
	psp[nSheet].dwFlags		= PSP_DEFAULT;
	psp[nSheet].hInstance	= g_hLanguageModule;
	psp[nSheet].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS_DEFAULT);
	psp[nSheet].lParam		= (LPARAM)this;
	psp[nSheet].pfnDlgProc	= DefaultSettingsProcStub;

	hpsp[nSheet] = CreatePropertySheetPage(&psp[nSheet]);
	nSheet++;

	/* Tab settings options page. */
	psp[nSheet].dwSize		= sizeof(PROPSHEETPAGE);
	psp[nSheet].dwFlags		= PSP_DEFAULT;
	psp[nSheet].hInstance	= g_hLanguageModule;
	psp[nSheet].pszTemplate	= MAKEINTRESOURCE(IDD_OPTIONS_TABS);
	psp[nSheet].lParam		= (LPARAM)this;
	psp[nSheet].pfnDlgProc	= TabSettingsProcStub;

	hpsp[nSheet] = CreatePropertySheetPage(&psp[nSheet]);
	nSheet++;

	/* Load the main dialog title. */
	LoadString(g_hLanguageModule,IDS_OPTIONSDIALOG_TITLE,
		szTitle,SIZEOF_ARRAY(szTitle));

	psh.dwSize		= sizeof(PROPSHEETHEADER);
	psh.dwFlags		= PSH_DEFAULT|PSH_USECALLBACK|PSH_NOCONTEXTHELP;
	psh.hwndParent	= m_hContainer;
	psh.pszCaption	= szTitle;
	psh.nPages		= nSheet;
	psh.nStartPage	= 0;
	psh.ppsp		= psp;
	psh.phpage		= hpsp;
	psh.pfnCallback	= PropSheetProcStub;

	/* Create the property dialog itself, which
	will hold each of the above property pages. */
	PropertySheet(&psh);
}

int CALLBACK PropSheetProcStub(HWND hDlg,UINT msg,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			g_hOptionsPropertyDialog = hDlg;
			break;
	}

	return 0;
}

INT_PTR CALLBACK GeneralSettingsProcStub(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CContainer *pContainer;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				PROPSHEETPAGE *ppsp;

				ppsp = (PROPSHEETPAGE *)lParam;
				pContainer = (CContainer *)ppsp->lParam;
			}
			break;
	}

	return pContainer->GeneralSettingsProc(hDlg,uMsg,wParam,lParam);
}

INT_PTR CALLBACK CContainer::GeneralSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				HWND		hButton;
				HWND		hEdit;
				int nIDButton;

				switch(m_StartupMode)
				{
				case STARTUP_PREVIOUSTABS:
					nIDButton = IDC_STARTUP_PREVIOUSTABS;
					break;

				case STARTUP_DEFAULTFOLDER:
					nIDButton = IDC_STARTUP_DEFAULTFOLDER;
					break;

				default:
					nIDButton = IDC_STARTUP_PREVIOUSTABS;
					m_StartupMode = STARTUP_PREVIOUSTABS;
					break;
				}
				CheckDlgButton(hDlg,nIDButton,BST_CHECKED);

				if(m_bReplaceExplorerFileSystem)
					CheckDlgButton(hDlg,IDC_OPTION_REPLACEEXPLORER_FILESYSTEM,BST_CHECKED);

				if(m_bSavePreferencesToXMLFile)
					CheckDlgButton(hDlg,IDC_OPTION_XML,BST_CHECKED);

				hButton = GetDlgItem(hDlg,IDC_DEFAULT_NEWTABDIR_BUTTON);
				SendMessage(hButton,BM_SETIMAGE,IMAGE_ICON,(LPARAM)g_hNewTabDirIcon);

				hEdit = GetDlgItem(hDlg,IDC_DEFAULT_NEWTABDIR_EDIT);
				DefaultSettingsSetNewTabDir(hEdit,m_DefaultTabDirectory);

				AddLanguages(hDlg);

				CenterWindow(m_hContainer,g_hOptionsPropertyDialog);
			}
			break;

		case WM_COMMAND:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
			case CBN_SELCHANGE:
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;
			}

			switch(LOWORD(wParam))
			{
			case IDC_STARTUP_PREVIOUSTABS:
			case IDC_STARTUP_DEFAULTFOLDER:
				if(IsDlgButtonChecked(hDlg,LOWORD(wParam)) == BST_CHECKED)
					PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_OPTION_REPLACEEXPLORER_FILESYSTEM:
			case IDC_OPTION_XML:
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_DEFAULT_NEWTABDIR_BUTTON:
				OnDefaultSettingsNewTabDir(hDlg);
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;
			}
			break;

		case WM_NOTIFY:
		{
			NMHDR	*nmhdr = NULL;
			nmhdr = (NMHDR *)lParam;

			switch(nmhdr->code)
			{
				case PSN_APPLY:
					{
						HWND hEdit;
						TCHAR szNewTabDir[MAX_PATH];
						TCHAR szVirtualParsingPath[MAX_PATH];
						TCHAR szErrorMsg[256];
						BOOL bReplaceExplorerFileSystem;
						BOOL bSuccess;
						HRESULT hr;
						int iSel;

						if(IsDlgButtonChecked(hDlg,IDC_STARTUP_PREVIOUSTABS) == BST_CHECKED)
							m_StartupMode = STARTUP_PREVIOUSTABS;
						else if(IsDlgButtonChecked(hDlg,IDC_STARTUP_DEFAULTFOLDER) == BST_CHECKED)
							m_StartupMode = STARTUP_DEFAULTFOLDER;

						bReplaceExplorerFileSystem = (IsDlgButtonChecked(hDlg,IDC_OPTION_REPLACEEXPLORER_FILESYSTEM)
							== BST_CHECKED);

						if(m_bReplaceExplorerFileSystem != bReplaceExplorerFileSystem)
						{
							bSuccess = TRUE;

							if(!bReplaceExplorerFileSystem)
							{
								bSuccess = RemoveAsDefaultFileManager();

							}
							else
							{
								bSuccess = SetAsDefaultFileManager();
							}

							LoadString(g_hLanguageModule,IDS_ERR_FILEMANAGERSETTING,
								szErrorMsg,SIZEOF_ARRAY(szErrorMsg));

							if(!bSuccess)
							{
								MessageBox(hDlg,szErrorMsg,WINDOW_NAME,MB_ICONWARNING);
							}
						}

						m_bReplaceExplorerFileSystem = bReplaceExplorerFileSystem;

						m_bSavePreferencesToXMLFile = (IsDlgButtonChecked(hDlg,IDC_OPTION_XML)
							== BST_CHECKED);

						hEdit = GetDlgItem(hDlg,IDC_DEFAULT_NEWTABDIR_EDIT);

						SendMessage(hEdit,WM_GETTEXT,SIZEOF_ARRAY(szNewTabDir),
							(LPARAM)szNewTabDir);

						/* The folder may be virtual, in which case, it needs
						to be decoded. */
						hr = DecodeFriendlyPath(szNewTabDir,szVirtualParsingPath);

						if(SUCCEEDED(hr))
							StringCchCopy(m_DefaultTabDirectory,SIZEOF_ARRAY(m_DefaultTabDirectory),
							szVirtualParsingPath);
						else
							StringCchCopy(m_DefaultTabDirectory,SIZEOF_ARRAY(m_DefaultTabDirectory),
							szNewTabDir);

						iSel = (int)SendMessage(GetDlgItem(hDlg,IDC_OPTIONS_LANGUAGE),CB_GETCURSEL,0,0);

						m_Language = GetLanguageIDFromIndex(hDlg,iSel);
					}
					break;
			}
		}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,0);
			break;

		case WM_DESTROY:
			DestroyIcon(g_hNewTabDirIcon);
			break;
	}

	return 0;
}

INT_PTR CALLBACK FilesFoldersProcStub(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CContainer *pContainer;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				PROPSHEETPAGE *ppsp;

				ppsp = (PROPSHEETPAGE *)lParam;
				pContainer = (CContainer *)ppsp->lParam;
			}
			break;
	}

	return pContainer->FilesFoldersProc(hDlg,uMsg,wParam,lParam);
}

INT_PTR CALLBACK CContainer::FilesFoldersProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				if(m_bHideSystemFilesGlobal)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_SYSTEMFILES,BST_CHECKED);
				if(!m_bShowExtensionsGlobal)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_EXTENSIONS,BST_CHECKED);
				if(m_bHideLinkExtensionGlobal)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_LINK,BST_CHECKED);
				if(m_bInsertSorted)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_INSERTSORTED,BST_CHECKED);
				if(m_bOneClickActivate)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_SINGLECLICK,BST_CHECKED);
				if(m_bShowFolderSizes)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_FOLDERSIZES,BST_CHECKED);
				if(m_bShowSizesInBytesGlobal)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_SIZEBYTES,BST_CHECKED);
				if(m_bHandleZipFiles)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_ZIPFILES,BST_CHECKED);
				if(m_bShowFriendlyDatesGlobal)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_FRIENDLYDATES,BST_CHECKED);
				if(m_bShowInfoTips)
					CheckDlgButton(hDlg,IDC_OPTIONS_CHECK_SHOWINFOTIPS,BST_CHECKED);

				if(m_InfoTipType == INFOTIP_SYSTEM)
					CheckDlgButton(hDlg,IDC_OPTIONS_RADIO_SYSTEMINFOTIPS,BST_CHECKED);
				else
					CheckDlgButton(hDlg,IDC_OPTIONS_RADIO_CUSTOMINFOTIPS,BST_CHECKED);

				SetInfoTipWindowStates(hDlg);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_SETTINGS_CHECK_SYSTEMFILES:
			case IDC_SETTINGS_CHECK_EXTENSIONS:
			case IDC_SETTINGS_CHECK_LINK:
			case IDC_SETTINGS_CHECK_INSERTSORTED:
			case IDC_SETTINGS_CHECK_SINGLECLICK:
			case IDC_SETTINGS_CHECK_FOLDERSIZES:
			case IDC_SETTINGS_CHECK_SIZEBYTES:
			case IDC_SETTINGS_CHECK_ZIPFILES:
			case IDC_SETTINGS_CHECK_FRIENDLYDATES:
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_OPTIONS_RADIO_SYSTEMINFOTIPS:
			case IDC_OPTIONS_RADIO_CUSTOMINFOTIPS:
				if(IsDlgButtonChecked(hDlg,LOWORD(wParam)) == BST_CHECKED)
					PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_OPTIONS_CHECK_SHOWINFOTIPS:
				SetInfoTipWindowStates(hDlg);
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;
			}
			break;

		case WM_NOTIFY:
			{
				NMHDR	*nmhdr = NULL;
				nmhdr = (NMHDR *)lParam;

				switch(nmhdr->code)
				{
				case PSN_APPLY:
					{
						TCITEM tcItem;
						int nTabs;
						int i = 0;

						m_bHideSystemFilesGlobal = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_SYSTEMFILES)
							== BST_CHECKED);

						m_bShowExtensionsGlobal = !(IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_EXTENSIONS)
							== BST_CHECKED);

						m_bHideLinkExtensionGlobal = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_LINK)
							== BST_CHECKED);

						m_bInsertSorted = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_INSERTSORTED)
							== BST_CHECKED);

						m_bOneClickActivate = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_SINGLECLICK)
							== BST_CHECKED);

						m_bShowFolderSizes = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_FOLDERSIZES)
							== BST_CHECKED);

						m_bShowSizesInBytesGlobal = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_SIZEBYTES)
							== BST_CHECKED);

						m_bHandleZipFiles = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_ZIPFILES)
							== BST_CHECKED);

						m_bShowFriendlyDatesGlobal = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_FRIENDLYDATES)
							== BST_CHECKED);

						m_bShowInfoTips = (IsDlgButtonChecked(hDlg,IDC_OPTIONS_CHECK_SHOWINFOTIPS)
							== BST_CHECKED);

						if(IsDlgButtonChecked(hDlg,IDC_OPTIONS_RADIO_SYSTEMINFOTIPS) == BST_CHECKED)
							m_InfoTipType = INFOTIP_SYSTEM;
						else
							m_InfoTipType = INFOTIP_CUSTOM;

						nTabs = TabCtrl_GetItemCount(m_hTabCtrl);

						/* Now, push each of the required settings to the
						individual tabs...*/
						for(i = 0;i < nTabs;i++)
						{
							tcItem.mask	= TCIF_PARAM;
							TabCtrl_GetItem(m_hTabCtrl,i,&tcItem);

							/* Each one of the options should also be pushed to new tabs when they are created. */
							m_pShellBrowser[(int)tcItem.lParam]->SetHideSystemFiles(m_bHideSystemFilesGlobal);
							m_pShellBrowser[(int)tcItem.lParam]->SetShowExtensions(m_bShowExtensionsGlobal);
							m_pShellBrowser[(int)tcItem.lParam]->SetHideLinkExtension(m_bHideLinkExtensionGlobal);
							m_pShellBrowser[(int)tcItem.lParam]->SetShowFolderSizes(m_bShowFolderSizes);
							m_pShellBrowser[(int)tcItem.lParam]->SetShowInBytes(m_bShowSizesInBytesGlobal);
							m_pShellBrowser[(int)tcItem.lParam]->SetShowFriendlyDates(m_bShowFriendlyDatesGlobal);
							m_pShellBrowser[(int)tcItem.lParam]->SetInsertSorted(m_bInsertSorted);

							RefreshTab((int)tcItem.lParam);

							ListView_ActivateOneClickSelect(m_hListView[(int)tcItem.lParam],m_bOneClickActivate);
						}
					}
					break;
				}
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,0);
			break;
	}

	return 0;
}

INT_PTR CALLBACK WindowProcStub(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CContainer *pContainer;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				PROPSHEETPAGE *ppsp;

				ppsp = (PROPSHEETPAGE *)lParam;
				pContainer = (CContainer *)ppsp->lParam;
			}
			break;
	}

	return pContainer->WindowProc(hDlg,uMsg,wParam,lParam);
}

INT_PTR CALLBACK CContainer::WindowProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			if(m_bAllowMultipleInstances)
				CheckDlgButton(hDlg,IDC_OPTION_MULTIPLEINSTANCES,BST_CHECKED);
			if(m_bShowFilePreviews)
				CheckDlgButton(hDlg,IDC_OPTION_FILEPREVIEWS,BST_CHECKED);
			if(m_bShowFullTitlePath)
				CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_TITLEPATH,BST_CHECKED);
			if(m_bShowUserNameInTitleBar)
				CheckDlgButton(hDlg,IDC_OPTION_USERNAMEINTITLEBAR,BST_CHECKED);
			if(!m_bTreeViewDelayEnabled)
				CheckDlgButton(hDlg,IDC_OPTION_TREEVIEWDELAY,BST_CHECKED);
			if(m_bExtendTabControl)
				CheckDlgButton(hDlg,IDC_OPTION_EXTENDTABCONTROL,BST_CHECKED);
			if(m_bShowGridlinesGlobal)
				CheckDlgButton(hDlg,IDC_OPTION_GRIDLINES,BST_CHECKED);
			if(m_bUseFullRowSelect)
				CheckDlgButton(hDlg,IDC_OPTION_FULLROWSELECT,BST_CHECKED);
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OPTION_MULTIPLEINSTANCES:
		case IDC_OPTION_FILEPREVIEWS:
		case IDC_SETTINGS_CHECK_TITLEPATH:
		case IDC_OPTION_USERNAMEINTITLEBAR:
		case IDC_OPTION_TREEVIEWDELAY:
		case IDC_OPTION_EXTENDTABCONTROL:
		case IDC_OPTION_GRIDLINES:
		case IDC_OPTION_FULLROWSELECT:
			PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
			break;
		}
		break;

	case WM_NOTIFY:
		{
			NMHDR	*nmhdr = NULL;
			nmhdr = (NMHDR *)lParam;

			switch(nmhdr->code)
			{
			case PSN_APPLY:
				{
					m_bAllowMultipleInstances = (IsDlgButtonChecked(hDlg,IDC_OPTION_MULTIPLEINSTANCES)
						== BST_CHECKED);

					m_bShowFilePreviews = (IsDlgButtonChecked(hDlg,IDC_OPTION_FILEPREVIEWS)
						== BST_CHECKED);

					m_bShowFullTitlePath = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_TITLEPATH)
						== BST_CHECKED);

					m_bShowUserNameInTitleBar = (IsDlgButtonChecked(hDlg,IDC_OPTION_USERNAMEINTITLEBAR)
						== BST_CHECKED);

					m_bTreeViewDelayEnabled = !(IsDlgButtonChecked(hDlg,IDC_OPTION_TREEVIEWDELAY)
						== BST_CHECKED);

					m_bExtendTabControl = (IsDlgButtonChecked(hDlg,IDC_OPTION_EXTENDTABCONTROL)
						== BST_CHECKED);

					m_bShowGridlinesGlobal = (IsDlgButtonChecked(hDlg,IDC_OPTION_GRIDLINES)
						== BST_CHECKED);

					m_bUseFullRowSelect = (IsDlgButtonChecked(hDlg,IDC_OPTION_FULLROWSELECT)
						== BST_CHECKED);

					/* Required if show full title path or show username
					in title bar options change. */
					HandleMainWindowText();

					RECT	rc;
					TCITEM tcItem;
					int nTabs;
					int i = 0;

					GetClientRect(m_hContainer,&rc);

					SendMessage(m_hContainer,WM_SIZE,SIZE_RESTORED,
						(LPARAM)MAKELPARAM(rc.right,rc.bottom));

					nTabs = TabCtrl_GetItemCount(m_hTabCtrl);

					for(i = 0;i < nTabs;i++)
					{
						tcItem.mask	= TCIF_PARAM;
						TabCtrl_GetItem(m_hTabCtrl,i,&tcItem);

						if(m_pShellBrowser[(int)tcItem.lParam]->QueryGridlinesActive() != m_bShowGridlinesGlobal)
						{
							m_pShellBrowser[(int)tcItem.lParam]->ToggleGridlines();
						}

						ListView_AddRemoveExtendedStyle(m_hListView[(int)tcItem.lParam],
							LVS_EX_FULLROWSELECT,m_bUseFullRowSelect);
					}
				}
				break;
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg,0);
		break;
	}

	return 0;
}

INT_PTR CALLBACK DefaultSettingsProcStub(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CContainer *pContainer;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				PROPSHEETPAGE *ppsp;

				ppsp = (PROPSHEETPAGE *)lParam;
				pContainer = (CContainer *)ppsp->lParam;
			}
			break;
	}

	return pContainer->DefaultSettingsProc(hDlg,uMsg,wParam,lParam);
}

INT_PTR CALLBACK CContainer::DefaultSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				UINT		ItemToCheck;

				if(m_bShowHiddenGlobal)
					CheckDlgButton(hDlg,IDC_SHOWHIDDENGLOBAL,BST_CHECKED);

				if(m_bShowInGroupsGlobal)
					CheckDlgButton(hDlg,IDC_SHOWINGROUPSGLOBAL,BST_CHECKED);

				if(m_bAutoArrangeGlobal)
					CheckDlgButton(hDlg,IDC_AUTOARRANGEGLOBAL,BST_CHECKED);

				if(m_bSortAscendingGlobal)
					CheckDlgButton(hDlg,IDC_SORTASCENDINGGLOBAL,BST_CHECKED);

				switch(m_ViewModeGlobal)
				{
					case VM_THUMBNAILS:
						ItemToCheck = IDC_DEFAULT_THUMBNAILS;
						break;

					case VM_TILES:
						ItemToCheck = IDC_DEFAULT_TILES;
						break;

					case VM_ICONS:
						ItemToCheck = IDC_DEFAULT_ICONS;
						break;

					case VM_SMALLICONS:
						ItemToCheck = IDC_DEFAULT_SMALLICONS;
						break;

					case VM_LIST:
						ItemToCheck = IDC_DEFAULT_LIST;
						break;

					case VM_DETAILS:
						ItemToCheck = IDC_DEFAULT_DETAILS;
						break;

					default:
						ItemToCheck = IDC_DEFAULT_ICONS;
						m_ViewModeGlobal = VM_ICONS;
						break;
				}
				CheckDlgButton(hDlg,ItemToCheck,BST_CHECKED);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			/* With buttons, check if the item
			was selected first (as changes in focus
			may cause this message to be sent). */
			case IDC_DEFAULT_THUMBNAILS:
			case IDC_DEFAULT_TILES:
			case IDC_DEFAULT_ICONS:
			case IDC_DEFAULT_SMALLICONS:
			case IDC_DEFAULT_LIST:
			case IDC_DEFAULT_DETAILS:
				if(IsDlgButtonChecked(hDlg,LOWORD(wParam)) == BST_CHECKED)
					PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_SHOWHIDDENGLOBAL:
			case IDC_AUTOARRANGEGLOBAL:
			case IDC_SORTASCENDINGGLOBAL:
			case IDC_SHOWINGROUPSGLOBAL:
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;

			case IDC_BUTTON_DEFAULTCOLUMNS:
				DialogBoxParam(g_hLanguageModule,MAKEINTRESOURCE(IDD_SETDEFAULTCOLUMNS),
					hDlg,SetDefaultColumnsProcStub,(LPARAM)this);
				break;
			}
			break;

		case WM_NOTIFY:
			{
				NMHDR	*nmhdr = NULL;
				nmhdr = (NMHDR *)lParam;

				switch(nmhdr->code)
				{
				case PSN_APPLY:
					{
						m_bShowHiddenGlobal = (IsDlgButtonChecked(hDlg,IDC_SHOWHIDDENGLOBAL)
							== BST_CHECKED);

						m_bShowInGroupsGlobal = (IsDlgButtonChecked(hDlg,IDC_SHOWINGROUPSGLOBAL)
							== BST_CHECKED);

						m_bAutoArrangeGlobal = (IsDlgButtonChecked(hDlg,IDC_AUTOARRANGEGLOBAL)
							== BST_CHECKED);

						m_bSortAscendingGlobal = (IsDlgButtonChecked(hDlg,IDC_SORTASCENDINGGLOBAL)
							== BST_CHECKED);

						if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_THUMBNAILS) == BST_CHECKED)
							m_ViewModeGlobal = VM_THUMBNAILS;
						else if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_TILES) == BST_CHECKED)
							m_ViewModeGlobal = VM_TILES;
						else if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_ICONS) == BST_CHECKED)
							m_ViewModeGlobal = VM_ICONS;
						else if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_SMALLICONS) == BST_CHECKED)
							m_ViewModeGlobal = VM_SMALLICONS;
						else if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_LIST) == BST_CHECKED)
							m_ViewModeGlobal = VM_LIST;
						else if(IsDlgButtonChecked(hDlg,IDC_DEFAULT_DETAILS) == BST_CHECKED)
							m_ViewModeGlobal = VM_DETAILS;
					}
					break;
				}
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,0);
			break;
	}

	return 0;
}

INT_PTR CALLBACK TabSettingsProcStub(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	static CContainer *pContainer;

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				PROPSHEETPAGE *ppsp;

				ppsp = (PROPSHEETPAGE *)lParam;
				pContainer = (CContainer *)ppsp->lParam;
			}
			break;
	}

	return pContainer->TabSettingsProc(hDlg,uMsg,wParam,lParam);
}

INT_PTR CALLBACK CContainer::TabSettingsProc(HWND hDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				if(m_bForceSameTabWidth)
					CheckDlgButton(hDlg,IDC_TABS_SAMEWIDTH,BST_CHECKED);
				if(m_bConfirmCloseTabs)
					CheckDlgButton(hDlg,IDC_TABS_CLOSECONFIRMATION,BST_CHECKED);
				if(m_bOpenNewTabNextToCurrent)
					CheckDlgButton(hDlg,IDC_TABS_OPENNEXTTOCURRENT,BST_CHECKED);
				if(m_bAlwaysOpenNewTab)
					CheckDlgButton(hDlg,IDC_SETTINGS_CHECK_ALWAYSNEWTAB,BST_CHECKED);
				if(m_bDoubleClickTabClose)
					CheckDlgButton(hDlg,IDC_TABS_DOUBLECLICKCLOSE,BST_CHECKED);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
			case IDC_TABS_SAMEWIDTH:
			case IDC_TABS_CLOSECONFIRMATION:
			case IDC_TABS_OPENNEXTTOCURRENT:
			case IDC_SETTINGS_CHECK_ALWAYSNEWTAB:
			case IDC_TABS_DOUBLECLICKCLOSE:
				PropSheet_Changed(g_hOptionsPropertyDialog,hDlg);
				break;
			}
			break;

		case WM_NOTIFY:
			{
				NMHDR	*nmhdr = NULL;
				nmhdr = (NMHDR *)lParam;

				switch(nmhdr->code)
				{
				case PSN_APPLY:
					{
						m_bForceSameTabWidth = (IsDlgButtonChecked(hDlg,IDC_TABS_SAMEWIDTH)
							== BST_CHECKED);

						m_bConfirmCloseTabs = (IsDlgButtonChecked(hDlg,IDC_TABS_CLOSECONFIRMATION)
							== BST_CHECKED);

						m_bOpenNewTabNextToCurrent = (IsDlgButtonChecked(hDlg,IDC_TABS_OPENNEXTTOCURRENT)
							== BST_CHECKED);

						m_bAlwaysOpenNewTab = (IsDlgButtonChecked(hDlg,IDC_SETTINGS_CHECK_ALWAYSNEWTAB)
							== BST_CHECKED);

						m_bDoubleClickTabClose = (IsDlgButtonChecked(hDlg,IDC_TABS_DOUBLECLICKCLOSE)
							== BST_CHECKED);

						AddWindowStyle(m_hTabCtrl,TCS_FIXEDWIDTH,m_bForceSameTabWidth);
					}
					break;
				}
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg,0);
			break;
	}

	return 0;
}

void CContainer::OnDefaultSettingsNewTabDir(HWND hDlg)
{
	BROWSEINFO bi;
	PIDLIST_ABSOLUTE pidl = NULL;
	HWND hEdit;
	TCHAR szDisplayName[MAX_PATH];
	TCHAR szHelper[256];
	TCHAR szNewTabDir[MAX_PATH];
	TCHAR szVirtualParsingPath[MAX_PATH];
	HRESULT hr;

	/* Load the dialog helper message. */
	LoadString(g_hLanguageModule,IDS_DEFAULTSETTINGS_NEWTAB,
		szHelper,SIZEOF_ARRAY(szHelper));

	GetDlgItemText(hDlg,IDC_DEFAULT_NEWTABDIR_EDIT,szNewTabDir,
		SIZEOF_ARRAY(szNewTabDir));

	hr = DecodeFriendlyPath(szNewTabDir,szVirtualParsingPath);

	if(SUCCEEDED(hr))
		StringCchCopy(g_szNewTabDirectory,SIZEOF_ARRAY(g_szNewTabDirectory),
		szVirtualParsingPath);
	else
		StringCchCopy(g_szNewTabDirectory,SIZEOF_ARRAY(g_szNewTabDirectory),
		szNewTabDir);

	CoInitializeEx(NULL,COINIT_APARTMENTTHREADED);

	bi.hwndOwner		= hDlg;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= szDisplayName;
	bi.lpszTitle		= szHelper;
	bi.ulFlags			= BIF_NEWDIALOGSTYLE;
	bi.lpfn				= NewTabDirectoryBrowseCallbackProc;

	pidl = SHBrowseForFolder(&bi);

	CoUninitialize();

	if(pidl != NULL)
	{
		hEdit = GetDlgItem(hDlg,IDC_DEFAULT_NEWTABDIR_EDIT);

		DefaultSettingsSetNewTabDir(hEdit,pidl);

		CoTaskMemFree(pidl);
	}
}

int CALLBACK NewTabDirectoryBrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)g_szNewTabDirectory);
		break;
	}

	return 0;
}

void CContainer::DefaultSettingsSetNewTabDir(HWND hEdit,TCHAR *szPath)
{
	LPITEMIDLIST	pidl = NULL;
	HRESULT			hr;

	hr = GetIdlFromParsingName(szPath,&pidl);

	if(SUCCEEDED(hr))
	{
		DefaultSettingsSetNewTabDir(hEdit,pidl);

		CoTaskMemFree(pidl);
	}
}

void CContainer::DefaultSettingsSetNewTabDir(HWND hEdit,LPITEMIDLIST pidl)
{
	SFGAOF			Attributes;
	DWORD			uNameFlags;
	TCHAR			szNewTabDir[MAX_PATH];

	Attributes = SFGAO_FILESYSTEM;

	/* Check if the specified folder is real or virtual. */
	GetItemAttributes(pidl,&Attributes);

	if(Attributes & SFGAO_FILESYSTEM)
		uNameFlags = SHGDN_FORPARSING;
	else
		uNameFlags = SHGDN_INFOLDER;

	GetDisplayName(pidl,szNewTabDir,uNameFlags);

	SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)szNewTabDir);
}

void CContainer::AddLanguages(HWND hDlg)
{
	HWND			hLanguageComboBox;
	WIN32_FIND_DATA	wfd;
	HANDLE			hFindFile;
	TCHAR			szImageDirectory[MAX_PATH];
	TCHAR			szNamePattern[MAX_PATH];
	WORD			wLanguage;
	int				iIndex = 1;
	int				iSel = 0;

	hLanguageComboBox = GetDlgItem(hDlg,IDC_OPTIONS_LANGUAGE);

	/* English will always be added to the combox, and will
	always be the first item. */
	SendMessage(hLanguageComboBox,CB_ADDSTRING,0,(LPARAM)_T("English"));
	SendMessage(hLanguageComboBox,CB_SETITEMDATA,0,9);

	GetCurrentProcessImageName(szImageDirectory,SIZEOF_ARRAY(szImageDirectory));
	PathRemoveFileSpec(szImageDirectory);
	StringCchCopy(szNamePattern,SIZEOF_ARRAY(szNamePattern),szImageDirectory);
	PathAppend(szNamePattern,_T("Explorer++??.dll"));

	hFindFile = FindFirstFile(szNamePattern,&wfd);

	/* Enumerate all the possible language DLL's. */
	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		wLanguage = AddLanguageToComboBox(hLanguageComboBox,
			szImageDirectory,wfd.cFileName);

		if(wLanguage == m_Language)
			iSel = iIndex;

		iIndex++;

		while(FindNextFile(hFindFile,&wfd) != 0)
		{
			wLanguage = AddLanguageToComboBox(hLanguageComboBox,
			szImageDirectory,wfd.cFileName);

			if(wLanguage == m_Language)
				iSel = iIndex;

			iIndex++;
		}

		FindClose(hFindFile);
	}

	/* Now, select the current language. */
	SendMessage(hLanguageComboBox,CB_SETCURSEL,iSel,0);
}

WORD CContainer::AddLanguageToComboBox(HWND hComboBox,
TCHAR *szImageDirectory,TCHAR *szFileName)
{
	TCHAR			szFullFileName[MAX_PATH];
	TCHAR			szLanguageName[32];
	LANGANDCODEPAGE	*plcp = NULL;
	DWORD			dwLen;
	DWORD			dwHandle;
	WORD			wRet = 0;
	UINT			uLen;
	void			*pTranslateInfo = NULL;
	int				iIndex;

	StringCchCopy(szFullFileName,SIZEOF_ARRAY(szFullFileName),szImageDirectory);
	PathAppend(szFullFileName,szFileName);

	dwLen = GetFileVersionInfoSize(szFullFileName,&dwHandle);

	if(dwLen > 0)
	{
		pTranslateInfo = malloc(dwLen);

		if(pTranslateInfo != NULL)
		{
			GetFileVersionInfo(szFullFileName,NULL,dwLen,pTranslateInfo);
			VerQueryValue(pTranslateInfo,_T("\\VarFileInfo\\Translation"),
				(LPVOID *)&plcp,&uLen);

			if(uLen >= sizeof(LANGANDCODEPAGE))
			{
				if(plcp[0].wLanguage == LANG_SINHALA)
				{
					StringCchCopy(szLanguageName,SIZEOF_ARRAY(szLanguageName),
						_T("Sinhala"));
				}
				else
				{
					GetLocaleInfo(plcp[0].wLanguage,LOCALE_SNATIVELANGNAME,
						szLanguageName,SIZEOF_ARRAY(szLanguageName));
				}

				iIndex = (int)SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)szLanguageName);

				if(iIndex != CB_ERR)
				{
					/* Associate the language identifier with the item. */
					SendMessage(hComboBox,CB_SETITEMDATA,iIndex,PRIMARYLANGID(plcp[0].wLanguage));
				}

				wRet = PRIMARYLANGID(plcp[0].wLanguage);
			}

			free(pTranslateInfo);
		}
	}

	return wRet;
}

int CContainer::GetLanguageIDFromIndex(HWND hDlg,int iIndex)
{
	HWND	hComboBox;
	int		iLanguage;

	hComboBox = GetDlgItem(hDlg,IDC_OPTIONS_LANGUAGE);

	iLanguage = (int)SendMessage(hComboBox,CB_GETITEMDATA,iIndex,0);

	return iLanguage;
}

void CContainer::SetInfoTipWindowStates(HWND hDlg)
{
	HWND	hCheckSystemInfoTips;
	HWND	hCheckCustomInfoTips;
	BOOL	bEnable;

	hCheckSystemInfoTips = GetDlgItem(hDlg,IDC_OPTIONS_RADIO_SYSTEMINFOTIPS);
	hCheckCustomInfoTips = GetDlgItem(hDlg,IDC_OPTIONS_RADIO_CUSTOMINFOTIPS);

	bEnable = (IsDlgButtonChecked(hDlg,IDC_OPTIONS_CHECK_SHOWINFOTIPS)
		== BST_CHECKED);

	EnableWindow(hCheckSystemInfoTips,bEnable);
	EnableWindow(hCheckCustomInfoTips,bEnable);
}